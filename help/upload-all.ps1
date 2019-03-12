Add-Type -AssemblyName 'System.Net.Http'

$uriPattern = "http://192.168.0.{0}/"
$path = "data\www\"
$timeout = 5
$configFile = "help\upload-config.json"

$uris = 100..140 | foreach { [String]::Format($uriPattern, $_) }

if (Test-Path $configFile -PathType Leaf) {
    $config = Get-Content -Path $configFile -Raw | ConvertFrom-Json
} else {
    $config = @{}
}

workflow Invoke-URLRequest {
    param ([String[]]$uris, [int]$timeout)
    $myoutput=@()
    foreach -parallel ($uri in $uris) {
        try {
            $x = Invoke-RestMethod -Uri ($uri+"dir") -Method Get -TimeoutSec $timeout
            $Workflow:myoutput += @{Uri=$uri; Files=$x}
        }
        catch {
        }
    }
    Return $myoutput
}

Invoke-URLRequest -uris $uris -timeout $timeout | Foreach-Object {
    $loggedIn = $false
    $hostUri = $_.Uri
    $currentFiles = $_.Files

    Get-ChildItem $path | 
    Foreach-Object {
        $file = [System.IO.Path]::GetFileName($_.FullName)
        $lastWrite = $_.LastWriteTimeUtc.Ticks
        $size = $_.length

        $uri = ($hostUri + $file)
        if (-Not ($config.PSobject.Properties.name -match $uri)) { $config.Add($uri, @{}) }
    
        $lastUpdate = $config.$uri.LastUpdate
        if ($null -eq $lastUpdate) { $lastUpdate = 0; }
    
        $wwwfile = ("/www/" + $file)
        $currentSize = $currentFiles.$wwwfile

        $toUpload = $false
        if (-not $toUpload) { $toUpload = ($null -eq $currentSize) } # if not exists
        if (-not $toUpload) { $toUpload = ($size -ne $currentSize) } # if size is different
        if (-not $toUpload) { $toUpload = ($lastUpdate -lt $lastWrite) } # if config date lower than local file date
        
        if (-not $loggedIn -and $toUpload) {
            $r = Invoke-RestMethod -Uri ($hostUri+"run") -Method Put -Body "LOGIN esplight"
            $loggedIn = ($r -eq "OK")
        }

        if ($loggedIn -and $toUpload) {
            Write-Host(("Uploading " + $file + " to " + $hostUri))

            try {
                $client = New-Object System.Net.Http.HttpClient
                $content = New-Object System.Net.Http.MultipartFormDataContent
                $stringHeader = New-Object System.Net.Http.Headers.ContentDispositionHeaderValue("form-data")
                $stringHeader.Name = "filename"
                $formContent = New-Object System.Net.Http.StringContent($file)
                $formContent.Headers.ContentDisposition = $stringHeader
                $fileStream = [System.IO.File]::OpenRead(($_.FullName))
                $fileContent = New-Object System.Net.Http.StreamContent($fileStream)
                $fileHeader = New-Object System.Net.Http.Headers.ContentDispositionHeaderValue("form-data")
                $fileHeader.Name = "file"
                $fileHeader.FileName = $file
                $fileContent.Headers.ContentDisposition = $fileHeader
                $fileContent.Headers.ContentType = [System.Net.Http.Headers.MediaTypeHeaderValue]::Parse("application/octet-stream")
                $content.Add($formContent, "filename", $file)
                $content.Add($fileContent, "file", $file)
                $result = $client.PostAsync(($hostUri+"upload"), $content).Result
                $r = $result.EnsureSuccessStatusCode()

                $config.$uri.LastUpdate = $lastWrite
                $config | ConvertTo-Json | Set-Content -Path $configFile
            } catch {
                Write-Error $_
            } finally {
                if ($null -ne $client) { $client.Dispose() }
                if ($null -ne $content) { $content.Dispose() }
                if ($null -ne $fileStream) { $fileStream.Dispose() }
                if ($null -ne $fileContent) { $fileContent.Dispose() }
                if ($null -ne $formContent) { $formContent.Dispose() }
            }
        }
    }
}
