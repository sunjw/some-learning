import os
import sys

import log_util

logger = log_util.logger


def main():
    logger.info('SQLiteEditDistance go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 sqlite_edit_distance.py [input.sqlite]')
        return

    sqlite_db_path = os.path.realpath(sys.argv[1])
    logger.info('sqlite_db_path=[%s]', sqlite_db_path)


if __name__ == '__main__':
    main()
