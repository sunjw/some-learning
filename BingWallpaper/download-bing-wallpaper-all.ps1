$MktArr = @("en-us", "zh-cn", "ja-jp", "fr-fr", "it-it", "en-gb", "es-es")

$FirstMkt = $true
foreach ($MktItr in $MktArr) {
    $ArgNoPromote = "--no-promote"
    if ($FirstMkt) {
        $ArgNoPromote = ""
        $FirstMkt = $false
    }
    $ArgMkt=" --mkt $MktItr"

    $DownloadArgs="$ArgMkt $ArgNoPromote"
    # Write-Host $DownloadArgs

    # python3 bing_wallpaper.py $DownloadArgs
    Start-Process "python3" -ArgumentList "bing_wallpaper.py $DownloadArgs" -NoNewWindow -PassThru -Wait
}
