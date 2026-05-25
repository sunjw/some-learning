import logging

from logging.handlers import RotatingFileHandler

logPath = './log/runtime.log'
logger = logging.getLogger('RunTime')
logging.basicConfig(
    format = '%(asctime)s.%(msecs)03d %(levelname)s [%(filename)s line:%(lineno)d] - %(message)s',
    datefmt = '%Y-%m-%d %H:%M:%S',
    level = logging.DEBUG,
    handlers = [
        #RotatingFileHandler(logPath, maxBytes=10000000, backupCount=10),
        logging.StreamHandler()
    ])
