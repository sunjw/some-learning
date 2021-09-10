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


def calculate_distance_by(data_items, by_col):
    data_distances = []
    data_count = len(data_items)
    logger.info('data_count=%d, by_col=[%s]', data_count, by_col)

    logger.info('calculate_distance_by...')
    comb_count = 0
    for i in range(data_count):
        for j in range(i + 1, data_count):
            data_distance_obj = {}
            data1 = data_items[i]
            data2 = data_items[j]
            data1_by_col = data1[by_col]
            data2_by_col = data2[by_col]
            if not data1_by_col and not data2_by_col:
                data_distance = 0
            elif not data1_by_col:
                data_distance = len(data2_by_col)
            elif not data2_by_col:
                data_distance = len(data1_by_col)
            else:
                data_distance = distance(data1_by_col, data2_by_col)
            data_distance_obj['data1_id'] = data1['id']
            data_distance_obj['data2_id'] = data2['id']
            data_distance_obj['distance'] = data_distance
            data_distances.append(data_distance_obj)
            comb_count = comb_count + 1

    # data_distances_count = len(data_distances)
    # logger.debug('data_distances_count=%d', data_distances_count)

    logger.info('sort data_distances...')
    data_distances.sort(key=lambda item: item['distance'])

    return data_distances


def find_data_pair_in_test_distances(test_distances, test_data1, test_data2, by_col):
    data1_by = 'data1_%s' % (by_col)
    data2_by = 'data2_%s' % (by_col)
    for test_distance in test_distances:
        if test_distance[data1_by] == test_data1 and test_distance[data2_by] == test_data2:
            return True
    return False


def do_distance_by(bookmarks, by_col):
    simple_bookmarks = extract_bookmarks(bookmarks)
    bookmark_distances = calculate_distance_by(simple_bookmarks, by_col)
    bookmark_distances_count = len(bookmark_distances)
    logger.info('got bookmark_distances, len=%d', bookmark_distances_count)

    test_distances = []
    test_distance_threshold = 1
    for i in range(bookmark_distances_count):
        bookmark_distance = bookmark_distances[i]
        if bookmark_distance['distance'] <= test_distance_threshold:
            test_distance = {}
            test_distance['distance'] = bookmark_distance['distance']
            test_distance['data1_id'] = bookmark_distance['data1_id']
            test_distance['data1_title'] = bookmarks[bookmark_distance['data1_id']]['title']
            test_distance['data1_url'] = bookmarks[bookmark_distance['data1_id']]['url']
            test_distance['data2_id'] = bookmark_distance['data2_id']
            test_distance['data2_title'] = bookmarks[bookmark_distance['data2_id']]['title']
            test_distance['data2_url'] = bookmarks[bookmark_distance['data2_id']]['url']

            if not test_distance['data1_title']:
                continue
            if not test_distance['data2_title']:
                continue

            data1_by = 'data1_%s' % (by_col)
            data2_by = 'data2_%s' % (by_col)
            if find_data_pair_in_test_distances(test_distances,
                                                test_distance[data1_by],
                                                test_distance[data2_by],
                                                by_col):
                continue

            test_distances.append(test_distance)
        else:
            break

    test_distance_count = len(test_distances)
    logger.info('test_distance_threshold=%d, test_distance_count=%d', test_distance_threshold, test_distance_count)

    csv_file_name = 'distance-%s-%d.csv' % (by_col, test_distance_threshold)
    csv_util.write_dict_to_csv(test_distances, csv_file_name)


def do_distance_by_url(bookmarks):
    do_distance_by(bookmarks, 'url')


def do_distance_by_title(bookmarks):
    do_distance_by(bookmarks, 'title')


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
    # do_distance_by_title(bookmarks)


if __name__ == '__main__':
    main()
