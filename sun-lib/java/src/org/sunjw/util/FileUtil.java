package org.sunjw.util;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.gson.JsonSyntaxException;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;

public class FileUtil {

    public static String readFile(String path) throws IOException {
        byte[] contentBytes = Files.readAllBytes(Paths.get(path));
        return new String(contentBytes, StandardCharsets.UTF_8);
    }

    public static void saveFile(String path, String content) throws IOException {
        byte[] contentBytes = content.getBytes();
        Files.write(Paths.get(path), contentBytes);
    }

    public static JsonElement readJsonFile(String jsonPath) throws IOException, JsonSyntaxException {
        File jsonFile = new File(jsonPath);
        if (!jsonFile.exists() || !jsonFile.isFile()) {
            return null;
        }
        String jsonData = null;
        jsonData = FileUtil.readFile(jsonFile.getAbsolutePath()).trim();
        if (jsonData == null) {
            return null;
        }

        JsonElement jsonElement = JsonParser.parseString(jsonData);

        return jsonElement;
    }

    public static void saveJsonFile(String jsonPath, JsonObject jsonObject) throws IOException {
        Gson gsonPrettyPrint = new GsonBuilder().setPrettyPrinting().create();
        String jsonString = gsonPrettyPrint.toJson(jsonObject);
        Writer out = new BufferedWriter(new OutputStreamWriter(
                new FileOutputStream(jsonPath), "UTF-8"));
        try {
            out.write(jsonString);
        } finally {
            out.close();
        }
    }
}
