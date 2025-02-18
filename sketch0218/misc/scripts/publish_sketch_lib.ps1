param (
    [switch]$NoNous,
    [switch]$NoRebuild
)

# 获取当前脚本文件的目录
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# 切换到该目录
Set-Location -Path $scriptDir

# FTP 相关信息
$ftpUrl = "ftp://admin:111111@build-server.d5next.com:41657/build/sketch/dependency/externals.zip"
$downloadPath = "../../tmp"
$localZipPath = "${downloadPath}/externals.zip"
$extractPath = "../.."
$exernalPath = Join-Path -Path $extractPath -ChildPath "externals"

if ( -not (Test-Path -Path $exernalPath)) {

    if (-not (Test-Path -Path $downloadPath)) {
        New-Item -ItemType Directory -Path $downloadPath
    }

    # 使用 curl 下载文件
    curl.exe ${ftpUrl} -o ${localZipPath}

    # 解压文件
    Expand-Archive -Path $localZipPath -DestinationPath $extractPath
}
else {
    Write-Host "externals exists, skip download"
}

# 编译项目
$solutionPath = "../../loong/sketch.sln"
$msbuildPath = "C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe"
$buildConfig = "Release"  # 或 "Debug"
$platform = "x64"         # 或 "x86"
$defineConstants = "SIM_UI_INPUT=0"

if(-not $NoRebuild)
{
& $msbuildPath $solutionPath `
    /target:sketch `
    /p:Configuration=$buildConfig `
    /p:Platform=$platform `
    /p:DefineConstants=$defineConstants
}

# 源目录和目标目录
$libPath = "../../sketch_lib"
if ($NoNous) {
    $libPath = "../../sketch_lib_no_nous"
}

if (-not (Test-Path -Path $libPath)) {
    New-Item -ItemType Directory -Path $libPath
}

## copy headers
$projctPath = "../../loong"
$sketchPath = Join-Path -Path $projctPath -ChildPath "sketch"
$includePath = Join-Path -Path $sketchPath -ChildPath "include"
Copy-Item -Path $includePath -Destination $libPath -Recurse -Force

# copy libs
$libDebugSource = Join-Path -Path $projctPath -ChildPath "x64/Debug"
$libReleaseSource = Join-Path -Path $projctPath -ChildPath "x64/Release"
$libTarget = Join-Path -Path $libPath -ChildPath "lib"
if (-not (Test-Path -Path "$libTarget/debug")) {
    New-Item -ItemType Directory -Path "$libTarget/debug"
}
if (-not (Test-Path -Path "$libTarget/release")) {
    New-Item -ItemType Directory -Path "$libTarget/release"
}

Copy-Item -Path "$libDebugSource/sketch.lib" -Destination "$libTarget/debug" -Force
Copy-Item -Path "$libReleaseSource/sketch.lib" -Destination "$libTarget/release" -Force

$binDebugDir = Join-Path -Path $libPath -ChildPath "bin/debug"
$binReleaseDir = Join-Path -Path $libPath -ChildPath "bin/release"

# 检查并创建目标目录
if (-not (Test-Path -Path $binDebugDir)) {
    New-Item -ItemType Directory -Path $binDebugDir
}
if (-not (Test-Path -Path $binReleaseDir)) {
    New-Item -ItemType Directory -Path $binReleaseDir
}

Copy-Item -Path "$libDebugSource/sketch.dll" -Destination $binDebugDir -Force
Copy-Item -Path "$libReleaseSource/sketch.dll" -Destination $binReleaseDir -Force

## copy dependcies
# 遍历子目录
$subDirs = Get-ChildItem -Path $exernalPath -Directory

foreach ($subDir in $subDirs) { 
    
    if ($NoNous -and $subDir.Name -eq "nous") {
        continue
    }
    
    $relateSubPath = Join-Path -Path $exernalPath -ChildPath $subDir
    #copy headers
    $includeSource = Join-Path -Path $relateSubPath -ChildPath "include"
    if (-not (Test-Path -Path "$libPath/include")) {
        New-Item -ItemType Directory -Path "$libPath/include"
    }

    if (Test-Path -Path $includeSource) {
        Copy-Item -Path "$includeSource/*" -Destination "$libPath/include" -Recurse -Force
        Write-Host "Copy $includeSource to $libPath"
    }

    #copy libs
    $libSource = Join-Path -Path $relateSubPath -ChildPath "lib"
    if (-not (Test-Path -Path "$libPath/lib/debug")) {
        New-Item -ItemType Directory -Path "$libPath/lib/debug"
    }
    if (-not (Test-Path -Path "$libPath/lib/release")) {
        New-Item -ItemType Directory -Path "$libPath/lib/release"
    }

    if (Test-Path -Path $libSource) {
        Copy-Item -Path "$libSource/Debug/*" -Destination "$libPath/lib/debug" -Recurse -Force
        Copy-Item -Path "$libSource/Release/*" -Destination "$libPath/lib/release" -Recurse -Force
        Write-Host "Copy $libSource to $libPath"
    }

    # 拷贝 bin/debug 下的文件
    $debugSource = Join-Path -Path $relateSubPath -ChildPath "bin/Debug"
    if (Test-Path -Path $debugSource) {
        Copy-Item -Path "$debugSource/*" -Destination $binDebugDir -Recurse -Force
        Write-Host "Copy $debugSource to $binDebugDir"
    }

    # 拷贝 bin/release 下的文件
    $releaseSource = Join-Path -Path $relateSubPath -ChildPath "bin/Release"
    if (Test-Path -Path $releaseSource) {
        Copy-Item -Path "$releaseSource/*" -Destination $binReleaseDir -Recurse -Force
        Write-Host "Copy $releaseSource to $binReleaseDir"
    }
}