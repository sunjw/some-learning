#!/bin/bash

## declare an array variable
declare -a mkt_arr=("en-us" "zh-cn" "ja-jp" "fr-fr" "it-it" "xx-xx")

first_mkt=true
## now loop through the above array
for mkt_itr in "${mkt_arr[@]}"
do
    arg_no_promote="--no-promote"
    if [ "$first_mkt" = true ] ; then
        arg_no_promote=""
        first_mkt=false
    fi
    arg_mkt=" --mkt $mkt_itr"

    download_args="$arg_mkt $arg_no_promote"
    # echo "$download_args"

    python3 bing_wallpaper.py $download_args
done
