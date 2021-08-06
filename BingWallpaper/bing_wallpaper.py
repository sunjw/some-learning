import argparse
import hashlib
import json
import os
import requests
import shlex
import sys
from urllib.request import urlopen

import comm_util
import log_util

logger = log_util.logger

BING_BASE_URL = 'https://www.bing.com'
BING_WALLPAPER_META_PATH = '/HPImageArchive.aspx?format=js&idx=0&n=10&nc=1612409408851&pid=hp&FORM=BEHPTB&uhd=1&uhdwidth=3840&uhdheight=2160'

WALLPAPER_DIR = 'wallpaper'
OPEN_WALLPAPER_DIR = True


def gen_bing_wallpaper_json_url():
    bing_wallpaper_meta_json_path = BING_BASE_URL + BING_WALLPAPER_META_PATH
    logger.info('bing_wallpaper_meta_json_path=[%s]', bing_wallpaper_meta_json_path)
    return bing_wallpaper_meta_json_path


def get_bing_wallpaper_json_over_ssh(ssh_cmd):
    logger.info('Get Bing wallpaper json over ssh, ssh_cmd=[%s]', ssh_cmd)
    bing_wallpaper_meta_json_path = gen_bing_wallpaper_json_url()

    remote_curl_cmd = 'curl -s "%s"' % (bing_wallpaper_meta_json_path)
    remote_cmd_list = shlex.split(ssh_cmd)
    remote_cmd_list.append(remote_curl_cmd)
    # logger.debug('remote_cmd_list\n%s', comm_util.pprint_dict_to_string(remote_cmd_list))

    returncode, stdout, stderr = comm_util.call_command_with_output(remote_cmd_list)
    logger.debug('stdout\n%s', stdout)
    wallpaper_json_body = json.loads(stdout)
    # logger.debug('wallpaper_json_body\n%s', comm_util.pprint_dict_to_string(wallpaper_json_body))

    return wallpaper_json_body


def get_bing_wallpaper_json():
    logger.info('Get Bing wallpaper json')
    bing_wallpaper_meta_json_path = gen_bing_wallpaper_json_url()
    resp = urlopen(bing_wallpaper_meta_json_path)
    str_body = resp.read().decode('utf-8')
    wallpaper_json_body = json.loads(str_body)
    # logger.debug('wallpaper_json_body\n%s', comm_util.pprint_dict_to_string(wallpaper_json_body))

    return wallpaper_json_body


def get_bing_wallpaper(download_count, over_ssh=False, ssh_cmd=None):
    wallpaper_list = []
    wallpaper_json_body = {}
    if not over_ssh:
        wallpaper_json_body = get_bing_wallpaper_json()
    else:
        wallpaper_json_body = get_bing_wallpaper_json_over_ssh(ssh_cmd)

    images_list = wallpaper_json_body['images']
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

        image_url = BING_BASE_URL + image_url
        image_data['url'] = image_url
        image_data['name'] = image_name
        # logger.info('image_data\n%s', comm_util.pprint_dict_to_string(image_data))

        wallpaper_list.append(image_data)

        i = i + 1

    logger.info('wallpaper_list\n%s', comm_util.pprint_dict_to_string(wallpaper_list))
    return wallpaper_list


def open_wallpaper_dir(wallpaper_dir):
    logger.info('Open [%s]', wallpaper_dir)
    if comm_util.is_windows():
        comm_util.call_command(['explorer', wallpaper_dir])
    if comm_util.is_macos():
        comm_util.call_command(['open', wallpaper_dir])


def clean_the_same_wallpaper(wallpaper_dir):
    files_in_wallpaper_md5 = {}
    files_in_wallpaper_dir = comm_util.list_file(wallpaper_dir)
    files_in_wallpaper_dir.sort()
    for file_name in files_in_wallpaper_dir:
        file_path = os.path.join(wallpaper_dir, file_name)
        file_md5 = hashlib.md5(open(file_path, 'rb').read()).hexdigest()
        files_in_wallpaper_md5[file_name] = file_md5
    logger.info('files_in_wallpaper_md5\n%s', comm_util.pprint_dict_to_string(files_in_wallpaper_md5))

    files_in_wallpaper_dir_count = len(files_in_wallpaper_dir)
    for i in range(files_in_wallpaper_dir_count):
        file_name_1 = files_in_wallpaper_dir[i]
        file_md5_1 = files_in_wallpaper_md5[file_name_1]
        for j in range(i + 1, files_in_wallpaper_dir_count):
            file_name_2 = files_in_wallpaper_dir[j]
            file_md5_2 = files_in_wallpaper_md5[file_name_2]
            if file_md5_1 == file_md5_2 and file_name_1 != file_name_2:
                file_path_2 = os.path.join(wallpaper_dir, file_name_2)
                if os.path.exists(file_path_2):
                    logger.info('found the same wallpapers [%s, %s]', file_name_1, file_name_2)
                    logger.info('delete [%s]', file_name_2)
                    os.remove(os.path.join(file_path_2))


def download_wallpaper_list(wallpaper_list):
    wallpaper_dir = WALLPAPER_DIR

    wallpaper_list_len = len(wallpaper_list)
    logger.info('wallpaper_list_len=%d', wallpaper_list_len)

    if not os.path.isdir(wallpaper_dir):
        logger.info('Make directory [%s]', wallpaper_dir)
        os.mkdir(wallpaper_dir)
    else:
        logger.info('[%s] exists.', wallpaper_dir)

    if OPEN_WALLPAPER_DIR:
        open_wallpaper_dir(wallpaper_dir)

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

    clean_the_same_wallpaper(wallpaper_dir)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('download_count', metavar='N', type=int,
                        help='download count')
    parser.add_argument('--ssh', dest='ssh_cmd', required=False,
                        help='get bing wallpaper json over ssh')

    args = vars(parser.parse_args())
    logger.debug('args\n%s', comm_util.pprint_dict_to_string(args))

    logger.info('BingWallpaper go!')

    download_count = args['download_count']
    logger.info('download_count=%d', download_count)

    wallpaper_list = []
    ssh_cmd = args['ssh_cmd']
    if ssh_cmd == None:
        wallpaper_list = get_bing_wallpaper(download_count)
    else:
        wallpaper_list = get_bing_wallpaper(download_count, True, ssh_cmd)

    download_wallpaper_list(wallpaper_list)


if __name__ == '__main__':
    main()
