const log4js = require('log4js');
const jwtUtil = require('./jwt_util');

log4js.configure({
    appenders: {
        out: {
            type: 'stdout',
            layout: {
                type: 'pattern',
                pattern: '%d{yyyy-MM-dd hh:mm:ss.SSS} %p [%f{1}.%l] - %m'
            }
        }
    },
    categories: {
        default: {
            appenders: ['out'],
            level: 'debug',
            enableCallStack: true
        }
    }
});

const logger = log4js.getLogger();

function main() {
    logger.info('main started.');

    var secret = 'secretkey';
    var header = '{"alg":"HS256","typ":"JWT"}';
    var payload = '{"loggedInAs":"admin","iat":1422779638}';

    var headerBase64 = jwtUtil.base64UrlEncode(header);
    var payloadBase64 = jwtUtil.base64UrlEncode(payload);
    logger.info('headerBase64=%s', headerBase64);
    logger.info('payloadBase64=%s', payloadBase64);

    var sign = jwtUtil.hmacSHA256Base64Url(headerBase64 + '.' + payloadBase64, secret);
    logger.info('sign=%s', sign);

    var headerDecode = jwtUtil.base64UrlDecodeToString(headerBase64);
    var payloadDecode = jwtUtil.base64UrlDecodeToString(payloadBase64);
    logger.info('headerDecode=%s', headerDecode);
    logger.info('payloadDecode=%s', payloadDecode);

    var jwtHmacSHA256Obj = jwtUtil.generateJwtObjectHmacSHA256(header, payload, secret);
    logger.info('jwtHmacSHA256Obj=%s', jwtHmacSHA256Obj);

    var jwtHmacSHA256 = jwtUtil.generateJwtHmacSHA256(header, payload, secret);
    logger.info('jwtHmacSHA256=%s', jwtHmacSHA256);

    var jwtObjEqualsString = true;
    if (!jwtHmacSHA256Obj || !jwtHmacSHA256) {
        jwtObjEqualsString = false;
    }
    if (jwtHmacSHA256Obj.toString() !== jwtHmacSHA256) {
        jwtObjEqualsString = false;
    }

    if (jwtObjEqualsString) {
        logger.info('jwtHmacSHA256Obj == jwtHmacSHA256');
    } else {
        logger.error('jwtHmacSHA256Obj != jwtHmacSHA256');
    }

    logger.info('jwtHmacSHA256Obj verify=%s', jwtUtil.verifyJwtObjectHmacSHA256(jwtHmacSHA256Obj, secret));
    logger.info('jwtHmacSHA256 verify=%s', jwtUtil.verifyJwtHmacSHA256(jwtHmacSHA256, secret));

    var testSecret = '_qUJSDZMX%sw=8DVPXQRTesxPh^+PkFa';
    jwtHmacSHA256 = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.' +
        'eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.' +
        '_njXJMMpxRK10OW47Tpu_iPgG8tbkFbsun2Brl1O6zI';
    var jwtObject = new jwtUtil.JWTObject(jwtHmacSHA256);
    logger.info('Test verify=%s', jwtUtil.verifyJwtObjectHmacSHA256(jwtObject, testSecret));
    logger.info('Test payload=%s', jwtUtil.base64UrlDecodeToString(jwtObject.payload));
}

main();
