// npm install dayjs --save
const dayjs = require('dayjs');
const utc = require('dayjs/plugin/utc');
dayjs.extend(utc);
// npm install platform --save
const platform = require('platform');

function formatTimestampUTC(timestamp) {
    // timestamp is second
    return dayjs.unix(timestamp).utc().format('YYYY-MM-DD HH:mm');
}

function formatTimestampLocal(timestamp) {
    // timestamp is second
    return dayjs.unix(timestamp).format('YYYY-MM-DD HH:mm');
}

function diffTimestamp(timestamp1, timestamp2) {
    // all timestamps is second
    let prevTime = timestamp1;
    let laterTime = timestamp2;
    if (timestamp1 > timestamp2) {
        prevTime = timestamp2;
        laterTime = timestamp1;
    }
    prevTime = dayjs.unix(prevTime).utc();
    laterTime = dayjs.unix(laterTime).utc();
    let diffTimeObj = {
        'year': 0,
        'month': 0,
        'day': 0,
        'hour': 0,
        'minute': 0,
        'second': 0
    };
    // year
    diffTimeObj['year'] = laterTime.diff(prevTime, 'year');
    // month
    diffTimeObj['month'] = laterTime.diff(prevTime, 'month') % 12;
    // day
    let daysInMonthPrev = laterTime.subtract(1, 'month').daysInMonth();
    let dayPrev = prevTime.date();
    let dayLater = laterTime.date();
    let secondsInDayPrev = prevTime.hour() * 3600 + prevTime.minute() * 60 + prevTime.second();
    let secondsInDayLater = laterTime.hour() * 3600 + laterTime.minute() * 60 + laterTime.second();
    if (secondsInDayLater < secondsInDayPrev) {
        dayLater = laterTime.subtract(1, 'day').date();
    }
    if (dayLater >= dayPrev) {
        diffTimeObj['day'] = dayLater - dayPrev;
    } else {
        diffTimeObj['day'] = dayLater - dayPrev + daysInMonthPrev;
    }
    // hour
    diffTimeObj['hour'] = laterTime.diff(prevTime, 'hour') % 24;
    // minute
    diffTimeObj['minute'] = laterTime.diff(prevTime, 'minute') % 60;
    // second
    diffTimeObj['second'] = laterTime.diff(prevTime, 'second') % 60;
    return diffTimeObj;
}

function getPlatformOS() {
    return platform.os.toString();
}

function isMacOS() {
    let platformOS = getPlatformOS();
    return (platformOS.indexOf('macOS') > -1 || platformOS.indexOf('OS X') > -1);
}

function isiOS() {
    return (getPlatformOS().indexOf('iOS') > -1);
}

// exports
exports.formatTimestampUTC = formatTimestampUTC;
exports.formatTimestampLocal = formatTimestampLocal;
exports.diffTimestamp = diffTimestamp;
exports.getPlatformOS = getPlatformOS;
exports.isMacOS = isMacOS;
exports.isiOS = isiOS;
