import comm_util
import log_util

logger = log_util.logger


def read_csv_internal(file_path, key_col):
    logger.info('read_csv_internal, read [%s] by key_col=%s.' % (file_path, key_col))
    csv_content = comm_util.read_file_text(file_path)

    return_array = False
    if key_col == 0 or key_col is None:
        return_array = True

    csv_headers = []
    csv_datas = {}
    if return_array:
        csv_datas = []
    i = 0
    for csv_line in csv_content.splitlines():
        i = i + 1
        csv_line_array = csv_line.split(',')
        if i == 1:
            # first line
            csv_headers.extend(csv_line_array)
            continue

        # data lines
        csv_data_line = {}
        j = 0
        while j < len(csv_headers):
            csv_data_line[csv_headers[j]] = csv_line_array[j]
            j = j + 1
        if return_array:
            csv_datas.append(csv_data_line)
        else:
            csv_datas[csv_data_line[key_col]] = csv_data_line

    return csv_datas


def read_csv_to_dict(file_path, key_col):
    logger.info('read_csv_to_dict, read [%s] by key_col=%s.' % (file_path, key_col))

    return read_csv_internal(file_path, key_col)


def read_csv_to_array(file_path):
    logger.info('read_csv_to_array, read [%s].' % file_path)

    return read_csv_internal(file_path, 0)


def write_dict_to_csv(dict_data, file_path):
    logger.info('write_dict_to_csv, write [%s].' % file_path)
    is_list = isinstance(dict_data, list)
    file_content = ''
    header = True
    for itr_key in dict_data:
        if not is_list:
            itr_dict = dict_data[itr_key]
        else:
            itr_dict = itr_key

        if header:
            # output header
            header = False
            for itr_header in itr_dict:
                file_content = file_content + itr_header + ','
            file_content = file_content + '\n'

        for itr_header in itr_dict:
            file_content = file_content + str(itr_dict[itr_header]) + ','
        file_content = file_content + '\n'

    comm_util.write_file_text(file_path, file_content)
