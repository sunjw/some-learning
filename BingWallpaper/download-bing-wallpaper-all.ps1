$MktArr = @("en-us", "zh-cn", "ja-jp", "fr-fr", "it-it", "en-gb", "es-es", "de-de")
$NoPromoteString = "--no-promote"

$CurDir = Get-Location
Set-Location (Split-Path -Parent $MyInvocation.MyCommand.Path)

$NoPromoteScriptArg = $args[0]
$AllNoPromote = $false
if ($NoPromoteScriptArg -eq $NoPromoteString) {
    $AllNoPromote = $true
}
# Write-Output $AllNoPromote

$FirstMkt = $true
foreach ($MktItr in $MktArr) {
    $ArgNoPromote = $NoPromoteString
    if ($FirstMkt) {
        $ArgNoPromote = ""
        $FirstMkt = $false
    }
    if ($AllNoPromote -eq $true) {
        $ArgNoPromote = $NoPromoteString
    }
    $ArgMkt=" --mkt $MktItr"

    $DownloadArgs="$ArgMkt $ArgNoPromote"
    # Write-Host $DownloadArgs

    # python3 bing_wallpaper.py $DownloadArgs
    Start-Process "python3" -ArgumentList "bing_wallpaper.py $DownloadArgs" -NoNewWindow -PassThru -Wait
}

Set-Location $CurDir
