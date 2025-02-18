# 获取当前脚本文件的目录
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# 切换到该目录
Set-Location -Path $scriptDir

# FTP 相关信息
$ftpUrl = "ftp://admin:111111@build-server.d5next.com:41657/build/sketch/dependency/externals.zip"
$nousUrl = "ftp://admin:111111@build-server.d5next.com:41657/build/sketch/dependency/nous.zip"
$downloadPath = "../../tmp"
$localZipPath = "${downloadPath}/externals.zip"
$nousZipPath = "${downloadPath}/nous.zip"
$extractPath = "../.."
$externalPath = Join-Path -Path $extractPath -ChildPath "externals"
$buildPath = "../../loongstc_bin"

if ( -not (Test-Path -Path $externalPath)) {

    if (-not (Test-Path -Path $downloadPath)) {
        New-Item -ItemType Directory -Path $downloadPath
    }

    # 使用 curl 下载文件
    curl.exe ${ftpUrl} -o ${localZipPath}
    curl.exe ${nousUrl} -o ${nousZipPath}

    # 解压文件
    Expand-Archive -Path $localZipPath -DestinationPath $extractPath
    Expand-Archive -Path $nousZipPath -DestinationPath $externalPath
}
else {
    Write-Host "externals exists, skip download"
}

# 源目录和目标目录
$binDebugDir = Join-Path -Path $buildPath -ChildPath "bin_debug"
$binReleaseDir = Join-Path -Path $buildPath -ChildPath "bin_release"

# 检查并创建目标目录
if (-not (Test-Path -Path $binDebugDir)) {
    New-Item -ItemType Directory -Path $binDebugDir
}
if (-not (Test-Path -Path $binReleaseDir)) {
    New-Item -ItemType Directory -Path $binReleaseDir
}

# 遍历子目录
$subDirs = Get-ChildItem -Path $externalPath -Directory

foreach ($subDir in $subDirs) {
   
    $relateSubPath = Join-Path -Path $externalPath -ChildPath $subDir
    # 拷贝 bin/debug 下的文件
    $debugSource = Join-Path -Path $relateSubPath -ChildPath "bin\Debug"
    if (Test-Path -Path $debugSource) {
        Copy-Item -Path "$debugSource\*" -Destination $binDebugDir -Recurse -Force
        Write-Host "Copy $debugSource to $binDebugDir"
    }

    # 拷贝 bin/release 下的文件
    $releaseSource = Join-Path -Path $relateSubPath -ChildPath "bin\Release"
    if (Test-Path -Path $releaseSource) {
        Copy-Item -Path "$releaseSource\*" -Destination $binReleaseDir -Recurse -Force
        Write-Host "Copy $releaseSource to $binReleaseDir"
    }
}