import json
import os
import sys

import log_util

logger = log_util.logger


def get_bing_wallpaper(count):
    bing_wallpaper_meta_url = 'https://cn.bing.com/HPImageArchive.aspx?format=js&idx=0&n=10&nc=1612409408851&pid=hp&FORM=BEHPTB&uhd=1&uhdwidth=3840&uhdheight=2160';


def main():
    logger.info('BingWallpaper go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 bing_wallpaper.py [download count]')
        return

    download_count = int(sys.argv[1])
    logger.info('download_count=%d', download_count)


if __name__ == '__main__':
    main()
