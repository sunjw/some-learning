import os
import sys
import difPy

import log_util
import comm_util

logger = log_util.logger


def go(images_dir_path):
    dif = difPy.build(images_dir_path, px_size=512)
    search = difPy.search(dif, similarity=50)
    result_dict = search.result
    # logger.info(comm_util.pprint_dict_to_string(result_dict))
    result_list = []
    for itr_key, itr_val in result_dict.items():
        image_from = itr_key
        for image_to in itr_val:
            image_similar = {
                'image_from': image_from,
                'image_to': image_to[0],
                'similarity': image_to[1]
            }
            result_list.append(image_similar)

    # sort
    result_list_sorted = sorted(result_list, key=lambda d_val: d_val['similarity'])
    return result_list_sorted


def main():
    logger.info('SimilarImagesSearch go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 similar_images_search.py [images directory]')
        return

    images_dir_path = os.path.realpath(sys.argv[1])
    logger.info('go with images_dir_path=[%s]', images_dir_path)
    result_list = go(images_dir_path)
    # logger.info(result_list)
    idx = 1
    for result_itr in result_list:
        logger.info('%d: %d\n%s\n%s\n------',
                    idx, result_itr['similarity'],
                    result_itr['image_from'], result_itr['image_to'])
        idx = idx + 1


if __name__ == '__main__':
    main()
