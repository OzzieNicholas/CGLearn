# 获取当前脚本文件的目录
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# 切换到该目录
Set-Location -Path $scriptDir

# FTP 相关信息
$ftpUrl = "ftp://admin:111111@build-server.d5next.com:41657/build/sketch/dependency/config.zip"
$downloadPath = "../../tmp"
$localZipFile = "${downloadPath}/config.zip"
$buildPath = "../../loongstc_bin"
$binDebugDir = Join-Path -Path $buildPath -ChildPath "bin_debug"
$binReleaseDir = Join-Path -Path $buildPath -ChildPath "bin_release"
$debugConfigPath = Join-Path -Path $binDebugDir -ChildPath "config"
$releaseConfigPath = Join-Path -Path $binReleaseDir -ChildPath "config"

# 检查是binDebugDir否已经存在config目录
if (Test-Path -Path $debugConfigPath) {
    if (Test-Path -Path $releaseConfigPath) {
        Write-Host "config exists, skip download"
        exit
    }
}

# 检查并创建目标目录
if (-not (Test-Path -Path $downloadPath)) {
    New-Item -ItemType Directory -Path $downloadPath
}
if (-not (Test-Path -Path $binDebugDir)) {
    New-Item -ItemType Directory -Path $binDebugDir
}
if (-not (Test-Path -Path $binReleaseDir)) {
    New-Item -ItemType Directory -Path $binReleaseDir
}

# 使用 curl 下载文件
curl.exe ${ftpUrl} -o ${localZipFile}

# 解压文件
if (-not (Test-Path -Path $debugConfigPath)) {
    Expand-Archive -Path $localZipFile -DestinationPath $binDebugDir
}

if (-not (Test-Path -Path $releaseConfigPath)) {
    Expand-Archive -Path $localZipFile -DestinationPath $binReleaseDir
}