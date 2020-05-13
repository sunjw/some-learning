package org.sunjw.jwt;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

public class TestMain {

    private static Logger logger = LoggerFactory.getLogger(TestMain.class);

    public static void main(String[] args) {
        logger.info("main started.");

        String secret = "secretkey";
        String header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
        String payload = "{\"loggedInAs\":\"admin\",\"iat\":1422779638}";

        String headerBase64 = JWTUtil.base64UrlEncode(header);
        String payloadBase64 = JWTUtil.base64UrlEncode(payload);
        logger.info("headerBase64={}", headerBase64);
        logger.info("payloadBase64={}", payloadBase64);

        try {
            String sign = JWTUtil.hmacSHA256Base64Url(headerBase64 + "." + payloadBase64, secret);
            logger.info("sign={}", sign);
        } catch (NoSuchAlgorithmException e) {
            logger.error("JWTUtil.HmacSHA256Base64Url, NoSuchAlgorithmException", e);
        } catch (InvalidKeyException e) {
            logger.error("JWTUtil.InvalidKeyException, NoSuchAlgorithmException", e);
        }

        String headerDecode = JWTUtil.base64UrlDecodeToString(headerBase64);
        String payloadDecode = JWTUtil.base64UrlDecodeToString(payloadBase64);
        logger.info("headerDecode={}", headerDecode);
        logger.info("payloadDecode={}", payloadDecode);

        JWTUtil.JWTObject jwtHmacSHA256Obj = null;
        try {
            jwtHmacSHA256Obj = JWTUtil.generateJwtObjectHmacSHA256(header, payload, secret);
            logger.info("jwtHmacSHA256Obj={}", jwtHmacSHA256Obj);
        } catch (InvalidKeyException e) {
            logger.error("JWTUtil.generateJwtObjectHmacSHA256, NoSuchAlgorithmException", e);
        } catch (NoSuchAlgorithmException e) {
            logger.error("JWTUtil.generateJwtObjectHmacSHA256, NoSuchAlgorithmException", e);
        }

        String jwtHmacSHA256 = null;
        try {
            jwtHmacSHA256 = JWTUtil.generateJwtHmacSHA256(header, payload, secret);
            logger.info("jwtHmacSHA256={}", jwtHmacSHA256);
        } catch (InvalidKeyException e) {
            logger.error("JWTUtil.generateJwtHmacSHA256, NoSuchAlgorithmException", e);
        } catch (NoSuchAlgorithmException e) {
            logger.error("JWTUtil.generateJwtHmacSHA256, NoSuchAlgorithmException", e);
        }

        boolean jwtObjEqualsString = true;
        if (jwtHmacSHA256Obj == null || jwtHmacSHA256 == null) {
            jwtObjEqualsString = false;
        }
        if (jwtHmacSHA256Obj != null && !jwtHmacSHA256Obj.toString().equals(jwtHmacSHA256)) {
            jwtObjEqualsString = false;
        }

        if (jwtObjEqualsString) {
            logger.info("jwtHmacSHA256Obj == jwtHmacSHA256");
        } else {
            logger.error("jwtHmacSHA256Obj != jwtHmacSHA256");
        }

        try {
            logger.info("jwtHmacSHA256Obj verify={}", JWTUtil.verifyJwtObjectHmacSHA256(jwtHmacSHA256Obj, secret));
        } catch (NoSuchAlgorithmException e) {
            logger.error("JWTUtil.verifyJwtObjectHmacSHA256, NoSuchAlgorithmException", e);
        } catch (InvalidKeyException e) {
            logger.error("JWTUtil.InvalidKeyException, NoSuchAlgorithmException", e);
        }

        try {
            logger.info("jwtHmacSHA256 verify={}", JWTUtil.verifyJwtHmacSHA256(jwtHmacSHA256, secret));
        } catch (NoSuchAlgorithmException e) {
            logger.error("JWTUtil.verifyJwtHmacSHA256, NoSuchAlgorithmException", e);
        } catch (InvalidKeyException e) {
            logger.error("JWTUtil.verifyJwtHmacSHA256, NoSuchAlgorithmException", e);
        }

        try {
            String testSecret = "_qUJSDZMX%sw=8DVPXQRTesxPh^+PkFa";
            jwtHmacSHA256 = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9." +
                    "eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ." +
                    "_njXJMMpxRK10OW47Tpu_iPgG8tbkFbsun2Brl1O6zI";
            JWTUtil.JWTObject jwtObject = new JWTUtil.JWTObject(jwtHmacSHA256);
            logger.info("Test verify={}", JWTUtil.verifyJwtObjectHmacSHA256(jwtObject, testSecret));
            logger.info("Test payload={}", JWTUtil.base64UrlDecodeToString(jwtObject.payload));
        } catch (NoSuchAlgorithmException e) {
            logger.error("JWTUtil.verifyJwtHmacSHA256, NoSuchAlgorithmException", e);
        } catch (InvalidKeyException e) {
            logger.error("JWTUtil.verifyJwtHmacSHA256, NoSuchAlgorithmException", e);
        }
    }

}
