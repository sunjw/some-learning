const dayjs = require('dayjs');
const utc = require('dayjs/plugin/utc');
dayjs.extend(utc);

const utils = require('./utils');
const npmUtils = require('./npmUtils');

utils.log('go...');

let prev = '2018-04-04T10:45:00.000Z';
let laters = ['2021-04-04T10:46:00.000Z', '2019-04-04T10:44:00.000Z', '2018-04-04T23:59:00.000Z',
    '2018-05-04T09:45:00.000Z'];

for (let later of laters) {
    let prevTime = dayjs(prev);
    utils.log('prevTime, unix=[%d], utc=[%s], local=[%s]', prevTime.unix(), prevTime.utc().format('YYYY-MM-DD HH:mm'), prevTime.format('YYYY-MM-DD HH:mm'));

    let laterTime = dayjs(later);
    utils.log('laterTime, unix=[%d], utc=[%s], local=[%s]', laterTime.unix(), laterTime.utc().format('YYYY-MM-DD HH:mm'), laterTime.format('YYYY-MM-DD HH:mm'));

    let diffTime = npmUtils.diffTimestamp(prevTime.unix(), laterTime.unix());
    utils.log('diff, %d years %d months %d days %d hours %d minutes %d seconds.',
        diffTime['year'], diffTime['month'], diffTime['day'],
        diffTime['hour'], diffTime['minute'], diffTime['second']);

    let prevTimeAdded = prevTime.clone();
    prevTimeAdded = prevTimeAdded.add(diffTime['year'], 'year');
    prevTimeAdded = prevTimeAdded.add(diffTime['month'], 'month');
    prevTimeAdded = prevTimeAdded.add(diffTime['day'], 'day');
    prevTimeAdded = prevTimeAdded.add(diffTime['hour'], 'hour');
    prevTimeAdded = prevTimeAdded.add(diffTime['minute'], 'minute');
    prevTimeAdded = prevTimeAdded.add(diffTime['second'], 'second');
    utils.log('prevTimeAdded, unix=[%d], equal [%s]', prevTimeAdded.unix(), (prevTimeAdded.unix() == laterTime.unix()).toString());

    utils.log('');
}

let full = true;
let count = 1000000;

let baseTime = dayjs('2012-10-12T13:33:43.000Z');
let baseTimestamp = baseTime.unix();
for (let i = 0; i < count; i++) {
    let laterTimestamp = baseTimestamp + utils.getRandomInt(31536000 * 10 /* 10 years */);
    let laterTime = dayjs.unix(laterTimestamp);

    let diffTime = npmUtils.diffTimestamp(baseTime.unix(), laterTime.unix());
    let prevTimeAdded = baseTime.clone();
    prevTimeAdded = prevTimeAdded.add(diffTime['year'], 'year');
    prevTimeAdded = prevTimeAdded.add(diffTime['month'], 'month');
    prevTimeAdded = prevTimeAdded.add(diffTime['day'], 'day');
    prevTimeAdded = prevTimeAdded.add(diffTime['hour'], 'hour');
    prevTimeAdded = prevTimeAdded.add(diffTime['minute'], 'minute');
    prevTimeAdded = prevTimeAdded.add(diffTime['second'], 'second');

    if (prevTimeAdded.unix() != laterTime.unix()) {
        full = false;
        utils.log('%d wrong!', i);
        utils.log('baseTime, unix=[%d], utc=[%s], local=[%s]', baseTime.unix(), baseTime.utc().format('YYYY-MM-DD HH:mm'), baseTime.format('YYYY-MM-DD HH:mm'));
        utils.log('laterTime, unix=[%d], utc=[%s], local=[%s]', laterTime.unix(), laterTime.utc().format('YYYY-MM-DD HH:mm'), laterTime.format('YYYY-MM-DD HH:mm'));
        utils.log('diff, %d years %d months %d days %d hours %d minutes %d seconds.',
            diffTime['year'], diffTime['month'], diffTime['day'],
            diffTime['hour'], diffTime['minute'], diffTime['second']);
        break;
    }
}
if (full) {
    utils.log('%d equal!', count);
}

utils.log('done!');
