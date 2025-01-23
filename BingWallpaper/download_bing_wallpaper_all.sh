#!/bin/bash

## declare an array variable
declare -a mkt_arr=("en-us" "zh-cn" "ja-jp" "fr-fr" "it-it" "en-gb" "es-es" "de-de")
no_promote_string="--no-promote"

cur_dir="$PWD"
cd "$(dirname "$0")"

no_promote_script_arg="$1"
all_no_promote=false
if [ "$no_promote_script_arg" == "$no_promote_string" ] ; then
    all_no_promote=true
fi
# echo "$all_no_promote"

first_mkt=true
## now loop through the above array
for mkt_itr in "${mkt_arr[@]}"
do
    arg_no_promote="$no_promote_string"
    if [ "$first_mkt" = true ] ; then
        arg_no_promote=""
        first_mkt=false
    fi
    if [ "$all_no_promote" = true ] ; then
        arg_no_promote="$no_promote_string"
    fi
    arg_mkt=" --mkt $mkt_itr"

    download_args="$arg_mkt $arg_no_promote"
    # echo "$download_args"

    python3 bing_wallpaper.py $download_args
done

cd $cur_dir
