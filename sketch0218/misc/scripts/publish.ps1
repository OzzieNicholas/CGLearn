$sourceFolder = "loongstc_bin/bin_release"
$zipFile = "sketch.zip"
$ftpUrl = "ftp://admin:111111@build-server.d5next.com:41657/build/sketch/build/$zipFile"

# 打包成 ZIP 文件
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory($sourceFolder, $zipFile)

# 上传到 FTP
curl.exe -T $zipFile $ftpUrl