const utils = require('./utils');
const QREnc = require('./qrenc');

$(function () {
    utils.log('init, QREncPage start...');
    const page = new QREnc.QREncPage();
    page.init();
});
