package org.sunjw.util;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import javax.xml.bind.DatatypeConverter;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.security.GeneralSecurityException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.Normalizer;
import java.util.Base64;
import java.util.Random;
import java.util.concurrent.ThreadLocalRandom;

public class CommUtil {

    public static boolean isStringNullOrEmpty(String string) {
        if (string == null) {
            return true;
        }
        return string.isEmpty();
    }

    public static String[] splitStringWithWhitespace(String string) {
        return string.split("\\s+");
    }

    public static long getCurrentMillis() {
        return System.currentTimeMillis();
    }

    public static long getCurrentSeconds() {
        return (getCurrentMillis() / 1000L);
    }

    public static int generateRandomInt(int min, int max) {
        int randomNum = ThreadLocalRandom.current().nextInt(min, max + 1);
        return randomNum;
    }

    public static String generateRandomString(int length) {
        String SOMECHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
        StringBuilder prefix = new StringBuilder();
        Random rand = new Random();
        while (prefix.length() < length) {
            int index = (int) (rand.nextFloat() * SOMECHARS.length());
            prefix.append(SOMECHARS.charAt(index));
        }
        String randStr = prefix.toString();
        return randStr;
    }

    public static String getMD5(String message) throws NoSuchAlgorithmException, UnsupportedEncodingException {
        byte[] bytesOfMessage = message.getBytes("UTF-8");
        MessageDigest md = MessageDigest.getInstance("MD5");
        byte[] md5Digest = md.digest(bytesOfMessage);
        String md5Hash = DatatypeConverter.printHexBinary(md5Digest).toLowerCase();
        return md5Hash;
    }

    public static String getHMAC256(String key, String input) throws GeneralSecurityException, UnsupportedEncodingException {
        Mac sha256_HMAC = null;
        String hash = null;
        sha256_HMAC = Mac.getInstance("HmacSHA256");
        SecretKeySpec secret_key = new SecretKeySpec(key.getBytes(), "HmacSHA256");
        sha256_HMAC.init(secret_key);
        Base64.Encoder encoder = Base64.getEncoder();

        hash = new String(encoder.encode(sha256_HMAC.doFinal(input.getBytes("UTF-8"))));

        return hash;
    }

    public static boolean checkHostIsIp(String host) {
        for (int i = 0; i < host.length(); ++i) {
            char ch = host.charAt(i);
            if (ch == '.' || (ch >= '0' && ch <= '9')) {
                continue;
            }
            return false;
        }
        return true;
    }

    public static int runCmd(String[] cmdArgs) throws InterruptedException, IOException {
        ProcessBuilder processBuilder = new ProcessBuilder(cmdArgs);
        Process proc = processBuilder.start();

        return proc.waitFor();
    }

    public static String runCmdWithOutput(String[] cmdArgs) throws IOException {
        ProcessBuilder processBuilder = new ProcessBuilder(cmdArgs);
        Process proc = processBuilder.start();

        BufferedReader stdInput = new BufferedReader(new
                InputStreamReader(proc.getInputStream()));

        // Read the output from the command.
        StringBuilder sb = new StringBuilder();
        String line = null;
        while ((line = stdInput.readLine()) != null) {
            sb.append(line);
            sb.append("\n");
        }

        return sb.toString();
    }

    public static long getCurrentThreadId() {
        return Thread.currentThread().getId();
    }

    public static byte[] convertIntTo4Bytes(int intValue) {
        return ByteBuffer.allocate(4).putInt(intValue).array();
    }

    public static int convert4BytesToInt(byte[] intBytes) {
        return ByteBuffer.wrap(intBytes).getInt();
    }

    public static String normalizeString(String input) {
        String normalized = Normalizer.normalize(input, Normalizer.Form.NFD);
        String accentRemoved = normalized.replaceAll("\\p{InCombiningDiacriticalMarks}+", "");
        return accentRemoved;
    }

}
