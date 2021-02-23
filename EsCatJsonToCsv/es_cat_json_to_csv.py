import json
import os
import sys

import comm_util
import csv_util
import log_util

logger = log_util.logger


def main():
    logger.info('EsCatJsonToCsv go!')

    if len(sys.argv) != 3:
        logger.error('Usage: python3 es_cat_json_to_csv.py [input.json] [output.csv]')
        return

    es_cat_json_path = os.path.realpath(sys.argv[1])
    to_csv_path = os.path.realpath(sys.argv[2])
    logger.info('es_cat_json_path=[%s], to_csv_path=[%s]', es_cat_json_path, to_csv_path)

    es_cat_json_content = comm_util.read_file_text(es_cat_json_path)
    es_cat_json = json.loads(es_cat_json_content)
    #logger.info(es_cat_json)

    csv_util.write_dict_to_csv(es_cat_json, to_csv_path)


if __name__ == '__main__':
    main()
