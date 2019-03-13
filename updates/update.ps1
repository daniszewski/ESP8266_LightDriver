# CONFIG
$uriPattern = "http://192.168.0.{0}/"
$uriRange = 201..210
$path = "data\www\"
$src = "src"
$firmware = ".pioenvs\nodemcuv2\firmware.bin"
$timeout = 5
$configFile = "updates\update-config.json"
$defaultPwd = "esplight"

# IMPLEMENTATION

Add-Type -AssemblyName 'System.Net.Http'

Function Upload-File {
    param( [String]$Url, [String]$FilePath )
    
    $file = [System.IO.Path]::GetFileName($FilePath)

    try {
        $client = New-Object System.Net.Http.HttpClient
        $content = New-Object System.Net.Http.MultipartFormDataContent
        $stringHeader = New-Object System.Net.Http.Headers.ContentDispositionHeaderValue("form-data")
        $stringHeader.Name = "filename"
        $formContent = New-Object System.Net.Http.StringContent($file)
        $formContent.Headers.ContentDisposition = $stringHeader
        $fileStream = [System.IO.File]::OpenRead(($FilePath))
        $fileContent = New-Object System.Net.Http.StreamContent($fileStream)
        $fileHeader = New-Object System.Net.Http.Headers.ContentDispositionHeaderValue("form-data")
        $fileHeader.Name = "file"
        $fileHeader.FileName = $file
        $fileContent.Headers.ContentDisposition = $fileHeader
        $fileContent.Headers.ContentType = [System.Net.Http.Headers.MediaTypeHeaderValue]::Parse("application/octet-stream")
        $content.Add($formContent, "filename", $file)
        $content.Add($fileContent, "file", $file)
        $result = $client.PostAsync($Url, $content).Result
        if ($null -ne $result) { $r = $result.EnsureSuccessStatusCode() }
        return $true
    } catch {
        Write-Host $_.Exception.Message
        return $false
    } finally {
        if ($null -ne $client) { $client.Dispose() }
        if ($null -ne $content) { $content.Dispose() }
        if ($null -ne $fileStream) { $fileStream.Dispose() }
        if ($null -ne $fileContent) { $fileContent.Dispose() }
        if ($null -ne $formContent) { $formContent.Dispose() }
    }
}

Function Get-FolderHash
{
    param ($folder)
    $files = dir $folder -Recurse |? { -not $_.psiscontainer }
    $allBytes = new-object System.Collections.Generic.List[byte]
    foreach ($file in $files)
    {
        $allBytes.AddRange([System.IO.File]::ReadAllBytes($file.FullName))
        $allBytes.AddRange([System.Text.Encoding]::UTF8.GetBytes($file.Name))
    }
    $hasher = [System.Security.Cryptography.MD5]::Create()
    $ret = [string]::Join("",$($hasher.ComputeHash($allBytes.ToArray()) | %{"{0:x2}" -f $_}))
    return $ret
}

function Format-Json([Parameter(Mandatory, ValueFromPipeline)][String] $json) {
    $indent = 0;
    ($json -Split '\n' |
    % {
        if ($_ -match '[\}\]]') { $indent-- }
        $line = (' ' * $indent * 2) + $_.TrimStart().Replace(':  ', ': ')
        if ($_ -match '[\{\[]') { $indent++ }
        $line
    }) -Join "`n"
}

workflow Invoke-URLRequest {
    param ([String[]]$uris, [int]$timeout)
    $myoutput=@()
    foreach -parallel ($uri in $uris) {
        try {
            $x = Invoke-RestMethod -Uri ($uri+"dir/www") -Method Get -TimeoutSec $timeout
            $Workflow:myoutput += @{Uri=$uri; Files=$x}
        }
        catch {
        }
    }
    Return $myoutput
}

$uris = $uriRange | foreach { [String]::Format($uriPattern, $_) }
$sourcesCrc = Get-FolderHash $src
$firmware = (dir $firmware).FullName

if (Test-Path $configFile -PathType Leaf) {
    $config = Get-Content -Path $configFile -Raw | ConvertFrom-Json
} else {
    $config = @{}
}

Invoke-URLRequest -uris $uris -timeout $timeout | Foreach-Object {
    $hostUri = $_.Uri
    $currentFiles = $_.Files
    $loggedIn = $false

    if ($null -eq $config.$hostUri) {
        $config.Add($hostUri, @{})
        # default config per node
        $config.$hostUri.Config = @{}
        $config.$hostUri.Files = @{}
        $config.$hostUri.Config.Password = $defaultPwd
    }
    
    $password = $config.$hostUri.Config.Password

    # WWW files update
    Get-ChildItem $path | Foreach-Object {
        $file = [System.IO.Path]::GetFileName($_.FullName)
        $lastWrite = $_.LastWriteTimeUtc.Ticks
        $size = $_.length

        $lastUpdate = $config.$hostUri.Files.$file
        if ($null -eq $lastUpdate) { $lastUpdate = 0; }
    
        $wwwfile = ("/www/" + $file)
        $currentSize = $currentFiles.$wwwfile

        $toUpload = $false
        if (-not $toUpload) { $toUpload = ($null -eq $currentSize) } # if not exists
        if (-not $toUpload) { $toUpload = ($size -ne $currentSize) } # if size is different
        if (-not $toUpload) { $toUpload = ($lastUpdate -lt $lastWrite) } # if config date lower than local file date
        
        if (-not $loggedIn-and $toUpload) { $loggedIn = "OK" -eq (Invoke-RestMethod -Uri ($hostUri+"run") -Method Put -Body ("LOGIN " + $password)) }

        if ($loggedIn -and $toUpload) {
            Write-Host(("Uploading " + $file + " to " + $hostUri))
            if (Upload-File ($hostUri+"upload")  $_.FullName) {
                $config.$hostUri.Files.$file = $lastWrite
                $config | ConvertTo-Json | Format-Json | Set-Content -Path $configFile
            }
        }
    }

    # Firmware update
    # This should be the last step because the node will restart after this
    if (($config.$hostUri.SourcesCrc -ne $sourcesCrc)) {
        if (-not $loggedIn) { $loggedIn = "OK" -eq (Invoke-RestMethod -Uri ($hostUri+"run") -Method Put -Body ("LOGIN " + $password)) }
        if ($loggedIn) {
            Write-Host(("Updating firmware in " + $hostUri))
            if(Upload-File ($hostUri+"update") $firmware) {
                $config.$hostUri.SourcesCrc = $sourcesCrc
                $config | ConvertTo-Json | Format-Json | Set-Content -Path $configFile
            }
            Write-Host("Done")
        }
    }
}
