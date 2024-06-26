Param(
    [Parameter(Mandatory=$false)]
    [Switch] $clean,

    [Parameter(Mandatory=$false)]
    [Switch] $log,

    [Parameter(Mandatory=$false)]
    [Switch] $useDebug,

    [Parameter(Mandatory=$false)]
    [Switch] $self,

    [Parameter(Mandatory=$false)]
    [Switch] $all,

    [Parameter(Mandatory=$false)]
    [String] $custom="",

    [Parameter(Mandatory=$false)]
    [String] $file="",

    [Parameter(Mandatory=$false)]
    [Switch] $help
)

if ($help -eq $true) {
    Write-Output "`"Copy`" - Builds and copies your mod to your quest, and also starts Beat Saber with optional logging"
    Write-Output "`n-- Arguments --`n"

    Write-Output "-Clean `t`t Performs a clean build (equvilant to running `"build -clean`")"
    Write-Output "-UseDebug `t Copies the debug version of the mod to your quest"
    Write-Output "-Log `t`t Logs Beat Saber using the `"Start-Logging`" command"

    Write-Output "`n-- Logging Arguments --`n"

    & $PSScriptRoot/start-logging.ps1 -help -excludeHeader

    exit
}

& $PSScriptRoot/build.ps1 -clean:$clean

if ($LASTEXITCODE -ne 0) {
    Write-Output "Failed to build, exiting..."
    exit $LASTEXITCODE
}

$version = (Get-Content "./qpm.json" -Raw | ConvertFrom-Json).info.version
$libname = "librapidjson-macros_" + $version.replace(".", "_") + ".so"

& adb shell rm /sdcard/Android/data/com.beatgames.beatsaber/files/mods/librapidjson-macros_*

if ($useDebug -eq $true) {
    & adb push build/debug/$libname /sdcard/Android/data/com.beatgames.beatsaber/files/mods/$libname
} else {
    & adb push build/$libname /sdcard/Android/data/com.beatgames.beatsaber/files/mods/$libname
}

& $PSScriptRoot/restart-game.ps1

if ($log -eq $true) {
    & adb logcat -c
    & $PSScriptRoot/start-logging.ps1 -self:$self -all:$all -custom:$custom -file:$file
}
