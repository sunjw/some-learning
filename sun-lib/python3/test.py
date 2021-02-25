import log_util
import csv_util

logger = log_util.logger


def main():
    logger.info('Test go!')

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

    csv_file_name = 'test.csv'
    csv_util.write_dict_to_csv(test_datas, csv_file_name)


if __name__ == '__main__':
    main()
