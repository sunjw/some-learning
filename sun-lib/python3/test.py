import os
import platform

import log_util
import comm_util
import csv_util

logger = log_util.logger


def get_test_csv_file_path():
    return 'test.csv'


def run_target_test(function_name, test_func):
    test_func()
    logger.info('Test target: %s, result: PASS', function_name)


def build_test_datas():
    test_datas = []

    test_data = {}
    test_data['id'] = 1
    test_data['title'] = 'abcdefg'
    test_data['value'] = 66.88
    test_datas.append(test_data)

    test_data = {}
    test_data['id'] = 2
    test_data['title'] = 'jkhskjdfskdafk kjhsakfhkdsafh'
    test_data['value'] = 4267.1254
    test_datas.append(test_data)

    test_data = {}
    test_data['id'] = 3
    test_data['title'] = 'bsa mf"bms da"nf'
    test_data['value'] = 2684.51054
    test_datas.append(test_data)

    test_data = {}
    test_data['id'] = 4
    test_data['title'] = 'uiwyr,iewqyr'
    test_data['value'] = 781942.54485
    test_datas.append(test_data)

    test_data = {}
    test_data['id'] = 5
    test_data['title'] = 'ui  w"yr,ie"w sad\'fs\'afq"yr'
    test_data['value'] = 17654.321
    test_datas.append(test_data)

    return test_datas


def stringify_row(row_data):
    return {
        'id': str(row_data['id']),
        'title': str(row_data['title']),
        'value': str(row_data['value'])
    }


def test_timestamp_utils():
    cur_timestamp = 0
    cur_timestamp_millis = 0

    def run_get_cur_timestamp():
        nonlocal cur_timestamp
        cur_timestamp = comm_util.get_cur_timestamp()
        assert isinstance(cur_timestamp, int)

    run_target_test('comm_util.get_cur_timestamp', run_get_cur_timestamp)

    def run_get_cur_timestamp_millis():
        nonlocal cur_timestamp_millis
        cur_timestamp_millis = comm_util.get_cur_timestamp_millis()
        assert isinstance(cur_timestamp_millis, int)

    run_target_test('comm_util.get_cur_timestamp_millis', run_get_cur_timestamp_millis)

    assert cur_timestamp_millis >= cur_timestamp * 1000
    assert cur_timestamp_millis < (cur_timestamp + 2) * 1000


def test_common_string_utils():
    def run_find_strings_in_string():
        assert comm_util.find_strings_in_string(['abc', 'xyz'], '123abc456') is True
        assert comm_util.find_strings_in_string(['abc', 'xyz'], '123456') is False

    run_target_test('comm_util.find_strings_in_string', run_find_strings_in_string)

    def run_pprint_dict_to_string():
        pretty_text = comm_util.pprint_dict_to_string({'name': '测试', 'value': 1})
        assert '"name": "测试"' in pretty_text
        assert '  "value": 1' in pretty_text

    run_target_test('comm_util.pprint_dict_to_string', run_pprint_dict_to_string)

    def run_bytes_to_str():
        assert comm_util.bytes_to_str('hello'.encode('utf-8')) == 'hello'

    run_target_test('comm_util.bytes_to_str', run_bytes_to_str)

    def run_is_ipv4():
        assert comm_util.is_ipv4('127.0.0.1') is True
        assert comm_util.is_ipv4('256.1.1.1') is False
        assert comm_util.is_ipv4('192.168.1') is False

    run_target_test('comm_util.is_ipv4', run_is_ipv4)


def test_platform_utils():
    system_name = platform.system()

    def run_is_windows_sys():
        assert comm_util.is_windows_sys() == (system_name == 'Windows')

    run_target_test('comm_util.is_windows_sys', run_is_windows_sys)

    def run_is_macos_sys():
        assert comm_util.is_macos_sys() == (system_name == 'Darwin')

    run_target_test('comm_util.is_macos_sys', run_is_macos_sys)

    def run_is_linux_sys():
        assert comm_util.is_linux_sys() == (system_name == 'Linux')

    run_target_test('comm_util.is_linux_sys', run_is_linux_sys)

    assert sum([
        comm_util.is_windows_sys(),
        comm_util.is_macos_sys(),
        comm_util.is_linux_sys()
    ]) == 1

    def run_get_machine():
        machine_name = comm_util.get_machine()
        assert isinstance(machine_name, str)
        assert len(machine_name) > 0

    run_target_test('comm_util.get_machine', run_get_machine)

    sample_path = 'folder/sub/file.txt'
    fixed_path = ''

    def run_fix_win_path():
        nonlocal fixed_path
        fixed_path = comm_util.fix_win_path(sample_path)
        if comm_util.is_windows_sys():
            assert fixed_path == 'folder\\sub\\file.txt'
        else:
            assert fixed_path == sample_path

    run_target_test('comm_util.fix_win_path', run_fix_win_path)

    if comm_util.is_windows_sys():
        def run_fix_win_long_path_windows():
            long_path = comm_util.fix_win_long_path(sample_path)
            assert long_path.startswith('\\\\?\\')

        run_target_test('comm_util.fix_win_long_path', run_fix_win_long_path_windows)
    else:
        def run_fix_win_long_path_other():
            assert comm_util.fix_win_long_path(sample_path) == sample_path

        run_target_test('comm_util.fix_win_long_path', run_fix_win_long_path_other)


def test_file_utils(csv_file_name):
    def run_list_file():
        files = comm_util.list_file('.')
        assert 'test.py' in files
        assert 'test.csv' in files
        assert '__pycache__' not in files

    run_target_test('comm_util.list_file', run_list_file)

    text_content = 'alpha\nbeta\n'

    def run_write_file_text():
        comm_util.write_file_text(csv_file_name, text_content)

    run_target_test('comm_util.write_file_text', run_write_file_text)

    def run_read_file_text():
        assert comm_util.read_file_text(csv_file_name) == text_content

    run_target_test('comm_util.read_file_text', run_read_file_text)

    binary_content = b'\x00\x01abc123'

    def run_write_file_binary():
        comm_util.write_file_binary(csv_file_name, binary_content)

    run_target_test('comm_util.write_file_binary', run_write_file_binary)

    def run_read_file_binary():
        assert comm_util.read_file_binary(csv_file_name) == binary_content

    run_target_test('comm_util.read_file_binary', run_read_file_binary)

    def run_md5_str():
        assert comm_util.md5_str('hello') == '5d41402abc4b2a76b9719d911017c592'

    run_target_test('comm_util.md5_str', run_md5_str)

    def run_md5_file():
        assert comm_util.md5_file(csv_file_name) == '7fa354cb06588d405657d6101bf60923'

    run_target_test('comm_util.md5_file', run_md5_file)

    def run_remove_file():
        comm_util.remove_file(csv_file_name)

    run_target_test('comm_util.remove_file', run_remove_file)

    assert os.path.exists(csv_file_name) is False

    def run_read_file_binary_after_remove():
        assert comm_util.read_file_binary(csv_file_name) == ''

    run_target_test('comm_util.read_file_binary', run_read_file_binary_after_remove)


def test_csv_round_trip(csv_file_name):
    test_datas = build_test_datas()

    def run_write_dict_to_csv_from_list():
        csv_util.write_dict_to_csv(test_datas, csv_file_name)

    run_target_test('csv_util.write_dict_to_csv', run_write_dict_to_csv_from_list)

    def run_read_file_text_for_csv():
        csv_content = comm_util.read_file_text(csv_file_name)
        assert 'id,title,value\n' in csv_content
        assert '"uiwyr,iewqyr"' in csv_content
        assert '"ui  w""yr,ie""w sad\'fs\'afq""yr"' in csv_content

    run_target_test('comm_util.read_file_text', run_read_file_text_for_csv)

    expected_rows = [stringify_row(row_data) for row_data in test_datas]

    def run_read_csv_to_array():
        actual_rows = csv_util.read_csv_to_array(csv_file_name)
        assert actual_rows == expected_rows

    run_target_test('csv_util.read_csv_to_array', run_read_csv_to_array)

    expected_dict = {}
    for row_data in test_datas:
        expected_dict[str(row_data['id'])] = stringify_row(row_data)

    def run_read_csv_to_dict():
        actual_dict = csv_util.read_csv_to_dict(csv_file_name, 'id')
        assert actual_dict == expected_dict

    run_target_test('csv_util.read_csv_to_dict', run_read_csv_to_dict)


def test_csv_write_from_dict(csv_file_name):
    test_datas = build_test_datas()
    csv_dict = {}
    for row_data in test_datas:
        csv_dict[str(row_data['id'])] = row_data

    def run_write_dict_to_csv_from_dict():
        csv_util.write_dict_to_csv(csv_dict, csv_file_name)

    run_target_test('csv_util.write_dict_to_csv', run_write_dict_to_csv_from_dict)

    expected_dict = {}
    for row_data in test_datas:
        expected_dict[str(row_data['id'])] = stringify_row(row_data)

    def run_read_csv_to_dict_after_dict_write():
        actual_dict = csv_util.read_csv_to_dict(csv_file_name, 'id')
        assert actual_dict == expected_dict

    run_target_test('csv_util.read_csv_to_dict', run_read_csv_to_dict_after_dict_write)


def main():
    logger.info('Test go!')

    csv_file_name = get_test_csv_file_path()
    original_content = None
    if os.path.exists(csv_file_name):
        original_content = comm_util.read_file_text(csv_file_name)

    try:
        test_timestamp_utils()
        test_common_string_utils()
        test_platform_utils()
        test_file_utils(csv_file_name)
        test_csv_round_trip(csv_file_name)
        test_csv_write_from_dict(csv_file_name)
    finally:
        if original_content is None:
            comm_util.remove_file(csv_file_name)
        else:
            comm_util.write_file_text(csv_file_name, original_content)

    comm_util.log('All tests passed.')


if __name__ == '__main__':
    main()
