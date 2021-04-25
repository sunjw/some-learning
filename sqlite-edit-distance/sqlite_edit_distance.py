import os
import sqlite3
import sys

from Levenshtein import distance

import log_util
import csv_util

logger = log_util.logger


def read_bookmarks_from_sqlite(sqlite_db_path):
    bookmarks = {}
    conn = sqlite3.connect(sqlite_db_path)
    cursor = conn.cursor()
    row_count = 0
    for row in cursor.execute(
            'select moz_bookmarks.*, moz_places.url from moz_bookmarks left join moz_places on moz_bookmarks.fk=moz_places.id'):
        bookmark = {}
        col_count = 0
        for col in cursor.description:
            col_name = col[0]
            bookmark[col_name] = row[col_count]
            col_count = col_count + 1

        # logger.debug('%d: {id: %d, title: "%s", url: "%s"}', row_count, bookmark['id'], bookmark['title'], bookmark['url'])
        bookmarks[bookmark['id']] = bookmark
        row_count = row_count + 1

    return bookmarks


def extract_bookmarks(bookmarks):
    simple_bookmarks = []
    for bookmark_id in bookmarks:
        bookmark = bookmarks[bookmark_id]
        if not bookmark['url']:
            continue
        simple_bookmark = {}
        simple_bookmark['id'] = bookmark['id']
        simple_bookmark['title'] = bookmark['title']
        simple_bookmark['url'] = bookmark['url']
        simple_bookmarks.append(simple_bookmark)
        # logger.debug('%d: {id: %d, title: "%s", url: "%s"}', row_count, bookmark['id'], bookmark['title'], bookmark['url'])

    return simple_bookmarks


def calculate_distance_by(urls):
    url_distances = []
    url_count = len(urls)
    logger.info('url_count=%d', url_count)

    logger.info('calculate_urls_distance...')
    comb_count = 0
    for i in range(url_count):
        for j in range(i + 1, url_count):
            url_distance_obj = {}
            url1 = urls[i]
            url2 = urls[j]
            url_distance = distance(url1['url'], url2['url'])
            url_distance_obj['url1_id'] = url1['id']
            url_distance_obj['url2_id'] = url2['id']
            url_distance_obj['distance'] = url_distance
            url_distances.append(url_distance_obj)
            comb_count = comb_count + 1

    url_distances_count = len(url_distances)
    # logger.debug('url_distances_count=%d', url_distances_count)

    logger.info('sort url_distances...')
    url_distances.sort(key=lambda item: item['distance'])

    return url_distances


def find_data_pair_in_test_distances(test_distances, url1, url2):
    for test_distance in test_distances:
        if test_distance['url1_url'] == url1 and test_distance['url2_url'] == url2:
            return True
    return False


def do_distance_by(bookmarks, by_col):
    simple_bookmarks = extract_bookmarks(bookmarks)
    bookmark_distances = calculate_distance_by(simple_bookmarks)
    bookmark_distances_count = len(bookmark_distances)
    logger.info('got bookmark_distances, len=%d', bookmark_distances_count)

    test_distances = []
    test_distance_threshold = 1
    for i in range(bookmark_distances_count):
        bookmark_distance = bookmark_distances[i]
        if bookmark_distance['distance'] <= test_distance_threshold:
            test_distance = {}
            test_distance['distance'] = bookmark_distance['distance']
            test_distance['url1_id'] = bookmark_distance['url1_id']
            test_distance['url1_title'] = bookmarks[bookmark_distance['url1_id']]['title']
            test_distance['url1_url'] = bookmarks[bookmark_distance['url1_id']]['url']
            test_distance['url2_id'] = bookmark_distance['url2_id']
            test_distance['url2_title'] = bookmarks[bookmark_distance['url2_id']]['title']
            test_distance['url2_url'] = bookmarks[bookmark_distance['url2_id']]['url']

            if not test_distance['url1_title']:
                continue
            if not test_distance['url2_title']:
                continue

            if find_data_pair_in_test_distances(test_distances,
                                                test_distance['url1_url'],
                                                test_distance['url2_url']):
                continue

            test_distances.append(test_distance)
        else:
            break

    test_distance_count = len(test_distances)
    logger.info('test_distance_threshold=%d, test_distance_count=%d', test_distance_threshold, test_distance_count)

    csv_file_name = 'distance-url-%d.csv' % (test_distance_threshold)
    csv_util.write_dict_to_csv(test_distances, csv_file_name)


def do_distance_by_url(bookmarks):
    do_distance_by(bookmarks, 'url')


def main():
    logger.info('SQLiteEditDistance go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 sqlite_edit_distance.py [input.sqlite]')
        return

    sqlite_db_path = os.path.realpath(sys.argv[1])
    logger.info('sqlite_db_path=[%s]', sqlite_db_path)

    bookmarks = read_bookmarks_from_sqlite(sqlite_db_path)
    # row_count = 0
    # for bookmark_id in bookmarks:
    #     bookmark = bookmarks[bookmark_id]
    #     logger.debug('%d: {id: %d, title: "%s", url: "%s"}', row_count, bookmark['id'], bookmark['title'],
    #                  bookmark['url'])
    #     row_count = row_count + 1

    do_distance_by_url(bookmarks)


if __name__ == '__main__':
    main()
