import logging

logging.basicConfig(
    # Timezone, no msecs.
    format = '%(asctime)s %(name)s %(levelname)s [%(filename)s:%(lineno)d] - %(message)s',
    # msecs, not using %z below.
    #format = '%(asctime)s.%(msecs)03d %(name)s %(levelname)s [%(filename)s:%(lineno)d] - %(message)s',
    datefmt = '%Y-%m-%d %H:%M:%S%z',
    level = logging.DEBUG,
    handlers = [
        logging.StreamHandler()
])

logger = logging.getLogger('TestLogPython')


def main():
    logger.info('main start...')
    logger.info('main end.')


if __name__ == '__main__':
    main()
