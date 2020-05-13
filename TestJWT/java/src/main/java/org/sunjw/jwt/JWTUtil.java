package org.sunjw.jwt;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import java.nio.charset.StandardCharsets;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;

public class JWTUtil {

    public static class JWTObject {
        public String header = null;
        public String payload = null;
        public String sign = null;

        public JWTObject(String header, String payload, String sign) {
            this.header = header;
            this.payload = payload;
            this.sign = sign;
        }

        public JWTObject(String jwt) {
            if (jwt == null) {
                return;
            }
            String[] jwtParts = jwt.split("\\.");
            if (jwtParts.length == 3) {
                this.header = jwtParts[0];
                this.payload = jwtParts[1];
                this.sign = jwtParts[2];
            }
        }

        @Override
        public String toString() {
            return this.header + "." + this.payload + "." + this.sign;
        }
    }

    public static String base64UrlEncode(String src) {
        if (src == null) {
            return null;
        }
        return base64UrlEncode(src.getBytes(StandardCharsets.UTF_8));
    }

    public static String base64UrlEncode(byte[] src) {
        if (src == null) {
            return null;
        }
        return Base64.getUrlEncoder()
                .withoutPadding()
                .encodeToString(src);
    }

    public static byte[] base64UrlDecode(String src) {
        if (src == null) {
            return null;
        }
        return Base64.getUrlDecoder().decode(src);
    }

    public static String base64UrlDecodeToString(String src) {
        if (src == null) {
            return null;
        }
        return new String(base64UrlDecode(src));
    }

    public static String hmacSHA256Base64Url(String input, String secret) throws NoSuchAlgorithmException, InvalidKeyException {
        if (input == null || secret == null) {
            return null;
        }
        final String MAC_INSTANCE = "HmacSHA256";
        final String SECRET_KEY_ALG = "HmacSHA256";

        Mac sha256_HMAC = Mac.getInstance(MAC_INSTANCE);
        SecretKeySpec secret_key = new SecretKeySpec(secret.getBytes(), SECRET_KEY_ALG);
        sha256_HMAC.init(secret_key);
        byte[] hs256 = sha256_HMAC.doFinal(input.getBytes());
        String result = base64UrlEncode(hs256);

        return result;
    }

    public static String generateJwtHmacSHA256(String header, String payload, String secret) throws InvalidKeyException, NoSuchAlgorithmException {
        return generateJwtObjectHmacSHA256(header, payload, secret).toString();
    }

    public static JWTObject generateJwtObjectHmacSHA256(String header, String payload, String secret) throws NoSuchAlgorithmException, InvalidKeyException {
        if (header == null || payload == null || secret == null) {
            return null;
        }
        String headerBase64 = base64UrlEncode(header);
        String payloadBase64 = base64UrlEncode(payload);

        String sign = hmacSHA256Base64Url(headerBase64 + "." + payloadBase64, secret);

        return new JWTObject(headerBase64, payloadBase64, sign);
    }

    public static boolean verifyJwtHmacSHA256(String jwt, String secret) throws InvalidKeyException, NoSuchAlgorithmException {
        return verifyJwtObjectHmacSHA256(new JWTObject(jwt), secret);
    }

    public static boolean verifyJwtObjectHmacSHA256(JWTObject jwtObject, String secret) throws NoSuchAlgorithmException, InvalidKeyException {
        String signCalc = hmacSHA256Base64Url(jwtObject.header + "." + jwtObject.payload, secret);
        if (signCalc != null && signCalc.equals(jwtObject.sign)) {
            return true;
        }
        return false;
    }
}
