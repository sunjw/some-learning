import os
import sqlite3
import sys

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


def main():
    logger.info('SQLiteEditDistance go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 sqlite_edit_distance.py [input.sqlite]')
        return

    sqlite_db_path = os.path.realpath(sys.argv[1])
    logger.info('sqlite_db_path=[%s]', sqlite_db_path)

    bookmarks = read_bookmarks_from_sqlite(sqlite_db_path)
    row_count = 0
    for bookmark_id in bookmarks:
        bookmark = bookmarks[bookmark_id]
        logger.debug('%d: {id: %d, title: "%s", url: "%s"}', row_count, bookmark['id'], bookmark['title'],
                     bookmark['url'])
        row_count = row_count + 1


if __name__ == '__main__':
    main()
