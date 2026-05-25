import argparse
import os

import comm_util
import log_util

logger = log_util.logger


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--url', dest='pihole_url', required=True,
                        help='url of pihole, for example http://192.168.1.1/pihole/admin/')
    parser.add_argument('--pswd', dest='pihole_password', required=True,
                        help='password for pihole')

    args = vars(parser.parse_args())
    # logger.debug('args\n%s', comm_util.pprint_dict_to_string(args))

    logger.info('PiHoleShot go!')

    pihole_url = args['pihole_url']
    pihole_password = args['pihole_password']



if __name__ == '__main__':
    main()
