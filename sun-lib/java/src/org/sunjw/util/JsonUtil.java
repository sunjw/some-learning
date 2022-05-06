package org.sunjw.util;

import com.google.gson.*;

import java.util.List;

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

    public static String toString(JsonObject jsonObject) {
        Gson gson = new Gson();
        return gson.toJson(jsonObject);
    }

    public static String toPrettyString(JsonObject jsonObject) {
        Gson gsonPretty = new GsonBuilder().setPrettyPrinting().create();
        return gsonPretty.toJson(jsonObject);
    }

    public static JsonElement parseJsonString(String jsonString) {
        return JsonParser.parseString(jsonString);
    }

    public static JsonArray convertStringListToJsonArray(List<String> stringList) {
        JsonArray jsonArray = new JsonArray();
        for (String item : stringList) {
            jsonArray.add(item);
        }
        return jsonArray;
    }
}
