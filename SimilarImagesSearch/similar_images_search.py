import json
import os
import sys

import log_util

logger = log_util.logger


def main():
    logger.info('SimilarImagesSearch go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 similar_images_search.py [images directory]')
        return

    images_dir_path = os.path.realpath(sys.argv[1])
    logger.info('images_dir_path=[%s]', images_dir_path)


if __name__ == '__main__':
    main()
