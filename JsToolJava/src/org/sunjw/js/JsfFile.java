package org.sunjw.js;

import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class JsfFile extends JsFormatter {

	private FileReader mFr;
	private FileWriter mFw;

	public JsfFile(FileReader fr, FileWriter fw, char chIndent, int nChPerInd,
			boolean bNLBracket, boolean bIndentInEmpty) {
		super(chIndent, nChPerInd, false, true, bNLBracket, bIndentInEmpty);
		mFr = fr;
		mFw = fw;
	}

	@Override
	protected char getChar() throws IOException {
		int iCh = mFr.read();
		if (iCh == -1)
			return 0;
		char ch = (char) iCh;
		return ch;
	}

	@Override
	protected void putChar(char ch) throws IOException {
		mFw.write(ch);
	}

}
