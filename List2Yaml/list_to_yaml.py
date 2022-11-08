import json
import os
import sys

import comm_util
import log_util

logger = log_util.logger


def main():
    logger.info('List2Yaml go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 list_to_yaml.py <some-list.txt>')
        return

    list_txt_path = os.path.realpath(sys.argv[1])
    logger.info('list_txt_path=[%s]', list_txt_path)

    list_txt_content = comm_util.read_file_text(list_txt_path)
    #logger.info(es_cat_json)


if __name__ == '__main__':
    main()
