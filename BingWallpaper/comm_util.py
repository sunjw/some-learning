import datetime
import io
import json
import math
import os
import re
import time


def get_cur_timestamp():
    return math.floor(time.mktime(datetime.datetime.today().timetuple()))


def read_file_text(file_path):
    file_content = ''
    with open(file_path, encoding='utf-8') as f:
        file_content = f.read()
    return file_content


def write_file_text(file_path, file_content, eol=None):
    with io.open(file_path, 'w', newline=eol, encoding='utf-8') as file:
        file.write(file_content)


def read_file_binary(file_path):
    if not os.path.exists(file_path):
        return ''
    file_content = open(file_path, 'rb').read()
    return file_content


def write_file_binary(file_path, file_content):
    file_obj = open(file_path, 'wb')
    file_obj.write(file_content)
    file_obj.close()


def pprint_dict_to_string(a_dict):
    return json.dumps(a_dict, indent=2, ensure_ascii=False)


def is_ipv4(input_str):
    compile_ip = re.compile(
        '^(1\d{2}|2[0-4]\d|25[0-5]|[1-9]\d|[1-9])\.(1\d{2}|2[0-4]\d|25[0-5]|[1-9]\d|\d)\.(1\d{2}|2[0-4]\d|25[0-5]|[1-9]\d|\d)\.(1\d{2}|2[0-4]\d|25[0-5]|[1-9]\d|\d)$')
    if compile_ip.match(input_str):
        return True
    else:
        return False