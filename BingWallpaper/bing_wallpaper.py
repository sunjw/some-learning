import json
import os
import requests
import sys
from urllib.request import urlopen

import comm_util
import log_util

logger = log_util.logger


def get_bing_wallpaper(download_count):
    bing_base_url = 'https://www.bing.com'
    bing_wallpaper_meta_path = '/HPImageArchive.aspx?format=js&idx=0&n=10&nc=1612409408851&pid=hp&FORM=BEHPTB&uhd=1&uhdwidth=3840&uhdheight=2160';

    wallpaper_list = []

    bing_wallpaper_meta_json_path = bing_base_url + bing_wallpaper_meta_path
    logger.info('bing_wallpaper_meta_json_path=[%s]', bing_wallpaper_meta_json_path)
    resp = urlopen(bing_wallpaper_meta_json_path)
    str_body = resp.read().decode('utf-8')
    json_body = json.loads(str_body)
    # logger.debug('json_body\n%s', comm_util.pprint_dict_to_string(json_body))

    images_list = json_body['images']
    images_list_len = len(images_list)
    if images_list_len < download_count:
        logger.error('No enough images, images_list_len=%d, download_count=%d', images_list_len, download_count)
        return wallpaper_list
    else:
        logger.info('images_list_len=%d', images_list_len)

    i = 0
    while i < download_count:
        image_data = {}
        image_url = images_list[i]['url']
        idx = image_url.find('&')
        if idx > -1:
            image_url = image_url[:idx]

        name_key = 'id='
        image_name = image_url
        idx = image_name.find(name_key)
        if idx == -1:
            logger.error('No name found, image_url=[%s]', image_url)
            return wallpaper_list
        image_name = image_name[idx + len(name_key):]

        image_url = bing_base_url + image_url
        image_data['url'] = image_url
        image_data['name'] = image_name
        # logger.info('image_data\n%s', comm_util.pprint_dict_to_string(image_data))

        wallpaper_list.append(image_data)

        i = i + 1

    logger.info('wallpaper_list\n%s', comm_util.pprint_dict_to_string(wallpaper_list))
    return wallpaper_list


def download_wallpaper_list(wallpaper_list):
    wallpaper_dir = 'wallpaper'

    wallpaper_list_len = len(wallpaper_list)
    logger.info('wallpaper_list_len=%d', wallpaper_list_len)

    if not os.path.isdir(wallpaper_dir):
        logger.info('Make directory [%s]', wallpaper_dir)
        os.mkdir(wallpaper_dir)
    else:
        logger.info('[%s] exists.', wallpaper_dir)

    for wallpaper_data in wallpaper_list:
        # logger.debug('wallpaper_data\n%s', comm_util.pprint_dict_to_string(wallpaper_data))
        image_url = wallpaper_data['url']
        image_name = wallpaper_data['name']

        image_path = os.path.join(wallpaper_dir, image_name)
        if os.path.exists(image_path):
            logger.info('[%s] exists, ignore.', image_name)
            continue

        logger.info('Download [%s]', image_url)
        with open(image_path, 'wb') as handle:
            response = requests.get(image_url, stream=True)

            if not response.ok:
                logger.error('%s', response)

            for block in response.iter_content(1024):
                if not block:
                    logger.info('Download finished.')
                    break
                handle.write(block)
            logger.info('Download finished.')

        image_file_size = os.path.getsize(image_path)
        logger.info('[%s], image_file_size=%d', image_name, image_file_size)


def main():
    logger.info('BingWallpaper go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 bing_wallpaper.py [download count]')
        return

    download_count = int(sys.argv[1])
    logger.info('download_count=%d', download_count)
    wallpaper_list = get_bing_wallpaper(download_count)
    download_wallpaper_list(wallpaper_list)


if __name__ == '__main__':
    main()
