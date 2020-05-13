const base64url = require('base64url');
const crypto = require('crypto');

class JWTObject {
    constructor(...args) {
        this.header = null;
        this.payload = null;
        this.sign == null;

        if (args.length == 1) {
            var jwtParts = args[0].split('.');
            if (jwtParts.length == 3) {
                this.header = jwtParts[0];
                this.payload = jwtParts[1];
                this.sign = jwtParts[2];
            }
        }

        if (args.length == 3) {
            this.header = args[0];
            this.payload = args[1];
            this.sign = args[2];
        }
    }

    toString() {
        return this.header + '.' + this.payload + '.' + this.sign;
    }
}

function base64UrlEncode(input) {
    return base64url(input);
}

function base64UrlEncodeFromBase64(input) {
    return base64url.fromBase64(input);
}

function base64UrlDecodeToString(input) {
    return base64url.decode(input);
}

function hmacSHA256Base64Url(input, secret) {
    if (!input || !secret) {
        return null;
    }
    var result = crypto.createHmac('SHA256', secret).update(input).digest('base64');
    result = base64UrlEncodeFromBase64(result);
    return result;
}

function generateJwtHmacSHA256(header, payload, secret) {
    return generateJwtObjectHmacSHA256(header, payload, secret).toString();
}

function generateJwtObjectHmacSHA256(header, payload, secret) {
    if (!header || !payload || !secret) {
        return null;
    }

    var headerBase64 = base64UrlEncode(header);
    var payloadBase64 = base64UrlEncode(payload);

    var sign = hmacSHA256Base64Url(headerBase64 + '.' + payloadBase64, secret);

    return new JWTObject(headerBase64, payloadBase64, sign);
}

function verifyJwtHmacSHA256(jwt, secret) {
    return verifyJwtObjectHmacSHA256(new JWTObject(jwt), secret);
}

function verifyJwtObjectHmacSHA256(jwtObject, secret) {
    var signCalc = hmacSHA256Base64Url(jwtObject.header + '.' + jwtObject.payload, secret);
    if (signCalc === jwtObject.sign) {
        return true;
    }
    return false;
}

exports.JWTObject = JWTObject;
exports.base64UrlEncode = base64UrlEncode;
exports.base64UrlEncodeFromBase64 = base64UrlEncodeFromBase64;
exports.base64UrlDecodeToString = base64UrlDecodeToString;
exports.hmacSHA256Base64Url = hmacSHA256Base64Url;
exports.generateJwtHmacSHA256 = generateJwtHmacSHA256;
exports.generateJwtObjectHmacSHA256 = generateJwtObjectHmacSHA256;
exports.verifyJwtHmacSHA256 = verifyJwtHmacSHA256;
exports.verifyJwtObjectHmacSHA256 = verifyJwtObjectHmacSHA256;
