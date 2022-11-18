# Set title of the window
$Host.UI.RawUI.WindowTitle = "IX-Ray"

# Set path to 7-Zip
$path = Join-Path -Path ${env:ProgramFiles} -ChildPath "7-Zip\7z.exe"

# Getting DirectX SDK March 2009 from archive
If (!(Test-Path "sdk\dxsdk_mar2009")) {
    Invoke-WebRequest -Uri "https://github.com/ixray-team/ixray-1.0-stsoc/releases/download/r0.1/sdk-directxsdk-mar2009.7z" `
                      -OutFile "directxsdk-mar2009.7z"
    Start-Process -FilePath $path `
                  -ArgumentList "x directxsdk-mar2009.7z" `
                  -NoNewWindow -Wait
    Remove-Item "directxsdk-mar2009.7z"
}

# Getting another dependencies from Git
git clone --branch aug2021 --depth 1 https://github.com/microsoft/DirectXTex.git dep/DirectXTex

# Pause
Write-Host "Press any key to continue..."
$Host.UI.RawUI.ReadKey("NoEcho, IncludeKeyDown") | Out-Null
