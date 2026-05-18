import os
import sys
from itertools import combinations

import numpy as np
from PIL import Image

import log_util

logger = log_util.logger

SUPPORTED_EXTENSIONS = {'.jpg', '.jpeg', '.png', '.bmp', '.gif', '.webp', '.tiff'}
DEFAULT_SIMILARITY_THRESHOLD = 90
HASH_SIZE = 8


def _iter_image_paths(images_dir_path):
    for root, _, file_names in os.walk(images_dir_path):
        for file_name in sorted(file_names):
            file_extension = os.path.splitext(file_name)[1].lower()
            if file_extension in SUPPORTED_EXTENSIONS:
                yield os.path.join(root, file_name)


def _average_hash(image, hash_size=HASH_SIZE):
    image_gray = image.convert('L').resize((hash_size, hash_size), Image.Resampling.LANCZOS)
    pixels = np.asarray(image_gray, dtype=np.float32)
    return pixels >= pixels.mean()


def _difference_hash(image, hash_size=HASH_SIZE):
    image_gray = image.convert('L').resize((hash_size + 1, hash_size), Image.Resampling.LANCZOS)
    pixels = np.asarray(image_gray, dtype=np.float32)
    return pixels[:, 1:] >= pixels[:, :-1]


def _hamming_distance(hash_a, hash_b):
    return int(np.count_nonzero(hash_a != hash_b))


def _get_similarity(image_path_a, image_path_b):
    with Image.open(image_path_a) as image_a, Image.open(image_path_b) as image_b:
        ahash_a = _average_hash(image_a)
        ahash_b = _average_hash(image_b)
        dhash_a = _difference_hash(image_a)
        dhash_b = _difference_hash(image_b)

    hash_bits = HASH_SIZE * HASH_SIZE
    ahash_distance = _hamming_distance(ahash_a, ahash_b)
    dhash_distance = _hamming_distance(dhash_a, dhash_b)
    best_distance = min(ahash_distance, dhash_distance)
    similarity = round((1 - best_distance / hash_bits) * 100)

    return similarity, ahash_distance, dhash_distance


def go(images_dir_path, similarity_threshold=DEFAULT_SIMILARITY_THRESHOLD):
    image_paths = list(_iter_image_paths(images_dir_path))
    result_list = []
    for image_from, image_to in combinations(image_paths, 2):
        similarity, ahash_distance, dhash_distance = _get_similarity(image_from, image_to)
        if similarity >= similarity_threshold:
            result_list.append({
                'image_from': image_from,
                'image_to': image_to,
                'similarity': similarity,
                'ahash_distance': ahash_distance,
                'dhash_distance': dhash_distance,
            })

    result_list_sorted = sorted(result_list, key=lambda d_val: d_val['similarity'], reverse=True)
    return result_list_sorted


def main():
    logger.info('SimilarImagesSearch go!')

    if len(sys.argv) not in (2, 3):
        logger.error('Usage: python3 similar_images_search.py [images directory] [similarity threshold, default=90]')
        return

    images_dir_path = os.path.realpath(sys.argv[1])
    similarity_threshold = DEFAULT_SIMILARITY_THRESHOLD
    if len(sys.argv) == 3:
        similarity_threshold = int(sys.argv[2])

    logger.info('go with images_dir_path=[%s], similarity_threshold=[%s]', images_dir_path, similarity_threshold)
    result_list = go(images_dir_path, similarity_threshold=similarity_threshold)

    if not result_list:
        logger.info('No similar images found.')
        return

    idx = 1
    for result_itr in result_list:
        logger.info('%d: %d%% (aHash=%d, dHash=%d)\n%s\n%s\n------',
                    idx, result_itr['similarity'],
                    result_itr['ahash_distance'], result_itr['dhash_distance'],
                    result_itr['image_from'], result_itr['image_to'])
        idx = idx + 1


if __name__ == '__main__':
    main()
