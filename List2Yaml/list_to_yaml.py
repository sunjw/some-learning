import os
import re
import sys

import yaml

import comm_util
import log_util

logger = log_util.logger


def sort_yaml(list_yaml_path):
    list_yaml_content = comm_util.read_file_text(list_yaml_path)
    list_yaml_dict = yaml.safe_load(list_yaml_content)
    # print(list_yaml_dict)

    list_yaml_content_new = yaml.dump(list_yaml_dict, allow_unicode=True)
    comm_util.write_file_text(list_yaml_path, list_yaml_content_new)


def generate_yaml_path(list_txt_path):
    list_txt_path = os.path.realpath(list_txt_path)
    list_dir = os.path.dirname(list_txt_path)
    list_filename = os.path.basename(list_txt_path)
    list_filename, txt_extension = os.path.splitext(list_filename)
    list_yaml_path = os.path.join(list_dir, list_filename + '.yml')
    # logger.info('generate_yaml_path, list_dir=[%s], list_yaml_path=[%s]', list_dir, list_yaml_path)
    return list_yaml_path


def list_to_yaml(list_txt_path, list_yaml_path):
    logger.info('list_to_yaml, convert list txt to yaml.')

    list_txt_content = comm_util.read_file_text(list_txt_path)

    # replace tree symbols
    list_yaml_content = list_txt_content.replace(' ', ' ')
    list_yaml_content = list_yaml_content.replace('├', '│')
    list_yaml_content = list_yaml_content.replace('└', '│')
    list_yaml_content = list_yaml_content.replace('│', '')
    list_yaml_content = list_yaml_content.replace('── ', '- ')

    # line by line
    list_yaml_content_lines = list_yaml_content.splitlines()
    list_yaml_content_lines_len = len(list_yaml_content_lines)

    list_yaml_content_lines_new = []
    last_indent_count = 0
    for i in range(0, list_yaml_content_lines_len):
        list_yaml_line = list_yaml_content_lines[i]
        re_result = re.search(r'[^ ]', list_yaml_line)
        if re_result:
            space_count = re_result.start()
            # logger.info(space_count)
            indent_count = int(space_count / 3)
            if indent_count > last_indent_count:
                last_index = len(list_yaml_content_lines_new) - 1
                last_line = list_yaml_content_lines_new[last_index]
                last_line = last_line + ':'
                list_yaml_content_lines_new[last_index] = last_line

            indent_prefix = ''
            for j in range(0, indent_count):
                indent_prefix = indent_prefix + '  '
            list_yaml_line = list_yaml_line.lstrip()
            list_yaml_line = indent_prefix + list_yaml_line

            # fix 1st line
            if i == 0:
                list_yaml_line = list_yaml_line + ':'

            re_result_last_line = re.search(r'[0-9]* directories, [0-9]* files', list_yaml_line)
            if re_result_last_line:
                # logger.info('re_result_last_line')
                list_yaml_line = ''

            list_yaml_content_lines_new.append(list_yaml_line)

            last_indent_count = indent_count

    list_yaml_content = '\n'.join(list_yaml_content_lines_new)

    comm_util.write_file_text(list_yaml_path, list_yaml_content)


def main():
    logger.info('List2Yaml go!')

    if len(sys.argv) != 2:
        logger.error('Usage: python3 list_to_yaml.py <some-list.txt>')
        return

    list_txt_path = os.path.realpath(sys.argv[1])
    list_yaml_path = generate_yaml_path(list_txt_path)
    logger.info('List2Yaml, [%s] -> [%s]', list_txt_path, list_yaml_path)
    list_to_yaml(list_txt_path, list_yaml_path)

    sort_yaml(list_yaml_path)


if __name__ == '__main__':
    main()
