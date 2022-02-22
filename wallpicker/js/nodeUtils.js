const crypto = require('crypto');

function hashMd5(strOrBuffer) {
    let md5 = crypto.createHash('md5');
    md5.update(strOrBuffer);
    return md5.digest('hex');
}

exports.hashMd5 = hashMd5;
