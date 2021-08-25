package org.sunjw.util;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;

public class JsonUtil {

    public static JsonObject getObject(JsonObject jsonObject, String memberName) {
        if (jsonObject == null || !jsonObject.has(memberName)) {
            return null;
        }
        return jsonObject.get(memberName).getAsJsonObject();
    }

    public static JsonArray getArray(JsonObject jsonObject, String memberName) {
        if (jsonObject == null || !jsonObject.has(memberName)) {
            return null;
        }
        return jsonObject.get(memberName).getAsJsonArray();
    }

    public static String getString(JsonObject jsonObject, String memberName, String defaultValue) {
        if (jsonObject == null || !jsonObject.has(memberName)) {
            return defaultValue;
        }
        return jsonObject.get(memberName).getAsString();
    }

    public static int getInt(JsonObject jsonObject, String memberName, int defaultValue) {
        if (jsonObject == null || !jsonObject.has(memberName)) {
            return defaultValue;
        }
        return jsonObject.get(memberName).getAsInt();
    }

    public static long getLong(JsonObject jsonObject, String memberName, long defaultValue) {
        if (jsonObject == null || !jsonObject.has(memberName)) {
            return defaultValue;
        }
        return jsonObject.get(memberName).getAsLong();
    }
}
