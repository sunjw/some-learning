import os
import sys

import comm_util
import log_util

logger = log_util.logger


def list_to_yaml(list_txt_path, list_yaml_path):
    logger.info('list_to_yaml, convert list txt to yaml.')


def generate_yaml_path(list_txt_path):
    list_txt_path = os.path.realpath(list_txt_path)
    list_dir = os.path.dirname(list_txt_path)
    list_filename = os.path.basename(list_txt_path)
    list_filename, txt_extension = os.path.splitext(list_filename)
    list_yaml_path = os.path.join(list_dir, list_filename + '.yml')
    # logger.info('generate_yaml_path, list_dir=[%s], list_yaml_path=[%s]', list_dir, list_yaml_path)
    return list_yaml_path


def main():
    logger.info('List2Yaml go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 list_to_yaml.py <some-list.txt>')
        return

    list_txt_path = os.path.realpath(sys.argv[1])
    list_yaml_path = generate_yaml_path(list_txt_path)
    logger.info('List2Yaml, [%s] -> [%s]', list_txt_path, list_yaml_path)
    list_to_yaml(list_txt_path, list_yaml_path)


if __name__ == '__main__':
    main()
