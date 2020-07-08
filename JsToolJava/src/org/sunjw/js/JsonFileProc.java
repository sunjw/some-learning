package org.sunjw.js;

import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class JsonFileProc extends JsonParser {

	private String mFileName;
	private FileReader mFr;

	public JsonFileProc(String fileName) {
		mFileName = fileName;
	}

	@Override
	protected char getChar() throws IOException {
		int iCh = mFr.read();
		if (iCh == -1)
			return 0;
		char ch = (char) iCh;
		return ch;
	}

	public void getJsonValue(JsonValue jsonValue) throws IOException {
		mFr = new FileReader(mFileName);
		if (mFr.ready()) {
			go(jsonValue);
		}
		mFr.close();
	}

	public void save(JsonValue jsonValue) throws IOException {
		FileWriter fw = new FileWriter(mFileName);
		fw.write(jsonValue.toString());
		fw.close();
	}
}
