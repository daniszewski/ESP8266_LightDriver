Add-Type -AssemblyName 'System.Net.Http'

$uri = "http://192.168.0.116/"
$path = "data\www\"

Write-Host("Login...")
Invoke-RestMethod -Uri ($uri+"run") -Method Put -Body "LOGIN esplight"

Get-ChildItem $path | 
Foreach-Object {
    $file = [System.IO.Path]::GetFileName($_.FullName)

    Write-Host(("Uploading " + $file))

    Try {
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
        $result = $client.PostAsync($($uri+"upload"), $content).Result
        $r = $result.EnsureSuccessStatusCode()
    }
    Catch {
        Write-Error $_
        exit 1
    }
    Finally {
        if ($client -ne $null) { $client.Dispose() }
        if ($content -ne $null) { $content.Dispose() }
        if ($fileStream -ne $null) { $fileStream.Dispose() }
        if ($fileContent -ne $null) { $fileContent.Dispose() }
        if ($formContent -ne $null) { $formContent.Dispose() }
    }
}