const log4js = require('log4js');

log4js.addLayout('json', function (config) {
  return function (logEvent) {
    if (logEvent.startTime) {
      logEvent.timestamp = logEvent.startTime;
    }
    if (logEvent.columnNumber) {
      delete logEvent.columnNumber;
    }
    if (logEvent.callStack) {
      delete logEvent.callStack;
    }
    return JSON.stringify(logEvent);
  }
});

log4js.configure({
  appenders: {
    consolePattern: {
      type: 'console',
      layout: {
        type: 'pattern',
        pattern: '%d{yyyy-MM-dd hh:mm:ss.SSSO} %c %p [%f{1}:%l] - %m'
      }
    },
    consoleJson: {
      type: 'console',
      layout: {
        type: 'json',
        separator: ','
      }
    }
  },
  categories: {
    default: {
      appenders: ['consolePattern', 'consoleJson'],
      level: 'info',
      enableCallStack: true
    }
  }
});

const logger = log4js.getLogger('TestLogNode');

function main() {
  logger.info('main start...');
  logger.info('main end.');
}

main();
