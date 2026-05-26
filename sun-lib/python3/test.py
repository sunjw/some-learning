import os

import log_util
import comm_util
import csv_util

logger = log_util.logger


def get_test_csv_file_path():
    return os.path.join(os.path.dirname(__file__), 'test.csv')


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
    cur_timestamp = comm_util.get_cur_timestamp()
    cur_timestamp_millis = comm_util.get_cur_timestamp_millis()

    assert isinstance(cur_timestamp, int)
    assert isinstance(cur_timestamp_millis, int)
    assert cur_timestamp_millis >= cur_timestamp * 1000
    assert cur_timestamp_millis < (cur_timestamp + 2) * 1000


def test_csv_round_trip(csv_file_name):
    test_datas = build_test_datas()

    csv_util.write_dict_to_csv(test_datas, csv_file_name)

    csv_content = comm_util.read_file_text(csv_file_name)
    assert 'id,title,value\n' in csv_content
    assert '"uiwyr,iewqyr"' in csv_content
    assert '"ui  w""yr,ie""w sad\'fs\'afq""yr"' in csv_content

    actual_rows = csv_util.read_csv_to_array(csv_file_name)
    expected_rows = [stringify_row(row_data) for row_data in test_datas]
    assert actual_rows == expected_rows

    actual_dict = csv_util.read_csv_to_dict(csv_file_name, 'id')
    expected_dict = {}
    for row_data in test_datas:
        expected_dict[str(row_data['id'])] = stringify_row(row_data)
    assert actual_dict == expected_dict


def test_csv_write_from_dict(csv_file_name):
    test_datas = build_test_datas()
    csv_dict = {}
    for row_data in test_datas:
        csv_dict[str(row_data['id'])] = row_data

    csv_util.write_dict_to_csv(csv_dict, csv_file_name)

    actual_dict = csv_util.read_csv_to_dict(csv_file_name, 'id')
    expected_dict = {}
    for row_data in test_datas:
        expected_dict[str(row_data['id'])] = stringify_row(row_data)
    assert actual_dict == expected_dict


def main():
    logger.info('Test go!')

    csv_file_name = get_test_csv_file_path()
    original_content = None
    if os.path.exists(csv_file_name):
        original_content = comm_util.read_file_text(csv_file_name)

    try:
        test_timestamp_utils()
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
