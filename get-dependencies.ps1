# Test and set path to 7-Zip
$globalPaths = $env:Path -split ';'
ForEach ($line in $globalPaths) {
    If (!(Test-Path ($line + "\7z.exe"))) {
        $path = Join-Path -Path ${env:ProgramFiles} `
                          -ChildPath "7-Zip\7z.exe"
    }
}

# Getting DirectX SDK March 2009 from archive
If (!(Test-Path "sdk\dxsdk_mar2009")) {
    Invoke-WebRequest -Uri "https://github.com/ixray-team/ixray-1.0-stsoc/releases/download/r0.1/sdk-directxsdk-mar2009.zip" `
                      -OutFile "directxsdk-mar2009.zip"
    Start-Process -FilePath $path `
                  -ArgumentList "x directxsdk-mar2009.zip" `
                  -NoNewWindow -Wait
    Remove-Item "directxsdk-mar2009.zip"
}
