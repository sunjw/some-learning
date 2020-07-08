package org.sunjw.test;

import java.io.IOException;

import org.sunjw.js.JsonFileProc;
import org.sunjw.js.JsonValue;

public class JsonTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length != 2) {
			System.out.println("Usage: jsformatter [input file] [output file]");
			return;
		}

		String inputFile = args[0];
		// String outputFile = args[1];

		JsonFileProc jfp = new JsonFileProc(inputFile);
		JsonValue jsonValue = new JsonValue();
		try {
			jfp.getJsonValue(jsonValue);
			jfp.save(jsonValue);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
