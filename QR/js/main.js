const utils = require('./utils');
const QREnc = require('./qrenc');
const QRDec = require('./qrdec');

$(function () {
    utils.log('init QRFUNC=[%s]', QRFUNC);
    if (QRFUNC === 'QRENC') {
        utils.log('QREncPage start...');
        (new QREnc.QREncPage()).init();
    } else if (QRFUNC === 'QRDEC') {
        utils.log('QRDecPage start...');
        (new QRDec.QRDecPage()).init();
    }
});
