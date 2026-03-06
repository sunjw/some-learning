const utils = require('./utils');
const QREnc = require('./qrenc');

$(function () {
    utils.log('init QRFUNC=[%s]', QRFUNC);
    if (QRFUNC === 'QRENC') {
        utils.log('QREncPage start...');
        const page = new QREnc.QREncPage();
        page.init();
    }
});
