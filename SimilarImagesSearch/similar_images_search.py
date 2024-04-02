import os
import sys
import difPy

import log_util
import comm_util

logger = log_util.logger


def go(images_dir_path):
    dif = difPy.build(images_dir_path, px_size=200)
    search = difPy.search(dif, similarity=75)
    logger.info(comm_util.pprint_dict_to_string(search.result))


def main():
    logger.info('SimilarImagesSearch go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 similar_images_search.py [images directory]')
        return

    images_dir_path = os.path.realpath(sys.argv[1])
    logger.info('go with images_dir_path=[%s]', images_dir_path)
    go(images_dir_path)


if __name__ == '__main__':
    main()
