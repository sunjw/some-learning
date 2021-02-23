import os
import sqlite3
import sys

from Levenshtein import distance

import log_util

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


def convert_bookmarks_to_urls(bookmarks):
    urls = []
    for bookmark_id in bookmarks:
        bookmark = bookmarks[bookmark_id]
        if not bookmark['url']:
            continue
        url = {}
        url['id'] = bookmark['id']
        url['title'] = bookmark['title']
        url['url'] = bookmark['url']
        urls.append(url)
        # logger.debug('%d: {id: %d, title: "%s", url: "%s"}', row_count, bookmark['id'], bookmark['title'], bookmark['url'])

    return urls


def calculate_urls_distance(urls):
    url_distances = []
    url_count = len(urls)
    logger.info('url_count=%d', url_count)
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
    logger.info('comb_count=%d', comb_count)

    return url_distances


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

    urls = convert_bookmarks_to_urls(bookmarks)
    url_distances = calculate_urls_distance(urls)


if __name__ == '__main__':
    main()
