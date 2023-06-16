import argparse
import os

import comm_util
import log_util

logger = log_util.logger

OUTPUT_FILE_POSTFIX = '-grep'


def get_file_list(source_dir, file_pattern):
    file_list = []
    all_files = comm_util.list_file(source_dir)
    logger.debug('all_files\n%s', comm_util.pprint_dict_to_string(all_files))
    for a_file in all_files:
        a_file_path = os.path.join(source_dir, a_file)
        if file_pattern is None:
            file_list.append(a_file_path)
        else:
            file_name_lower = a_file.lower()
            if file_pattern in file_name_lower:
                file_list.append(a_file_path)

    return file_list


def grep_in_file_list(file_list, grep, output_dir):
    logger.info('grep_in_file_list, grep=[%s]', grep)
    grep = grep.lower()
    for a_file in file_list:
        file_name_full = os.path.basename(a_file)
        file_name, file_extension = os.path.splitext(file_name_full)
        output_file_name_full = file_name + OUTPUT_FILE_POSTFIX + file_extension
        output_file_path = os.path.join(output_dir, output_file_name_full)
        logger.info('grep_in_file_list, [%s] => [%s]', a_file, output_file_path)

        output_file_content = ''
        with open(a_file, encoding='utf-8') as input_file:
            for a_line in input_file:
                a_line = a_line.lower()
                if grep in a_line:
                    output_file_content = output_file_content + a_line
        comm_util.write_file_text(output_file_path, output_file_content)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--source', dest='source_dir', required=True,
                        help='source directory')
    parser.add_argument('--pattern', dest='file_pattern', required=False,
                        help='source file pattern, ignore case, like ".csv"')
    parser.add_argument('--grep', dest='grep', required=True,
                        help='text grep pattern, ignore case')
    parser.add_argument('--output', dest='output_dir', required=False,
                        help='output directory, default is output to source directory')

    args = vars(parser.parse_args())
    # logger.debug('args\n%s', comm_util.pprint_dict_to_string(args))

    logger.info('GrepEx go!')

    source_dir = args['source_dir']
    file_pattern = args['file_pattern']
    grep = args['grep']
    output_dir = args['output_dir']

    source_dir = os.path.realpath(source_dir)
    if output_dir is None:
        output_dir = source_dir
    else:
        output_dir = os.path.realpath(output_dir)

    if file_pattern is not None:
        file_pattern = file_pattern.strip()
        if file_pattern == '':
            file_pattern = None
    if file_pattern is not None:
        file_pattern = file_pattern.lower()

    logger.info('source_dir=[%s], file_pattern=[%s], grep=[%s], output_dir=[%s]',
                source_dir, file_pattern, grep, output_dir)

    file_list = get_file_list(source_dir, file_pattern)
    logger.debug('file_list\n%s', comm_util.pprint_dict_to_string(file_list))

    if not os.path.exists(output_dir):
        logger.info('makedirs, path=[%s]', output_dir)
        os.makedirs(output_dir, exist_ok=True)

    grep_in_file_list(file_list, grep, output_dir)


if __name__ == '__main__':
    main()
