package org.sunjw.js;

import java.io.IOException;

public class JsfString extends JsFormatter {

	private String mInput;
	private StringBuffer mOutput;
	private int mGetPos;

	public JsfString(String input, StringBuffer output, char chIndent,
			int nChPerInd, boolean bPutCR, boolean bNLBracket,
			boolean bIndentInEmpty) {
		super(chIndent, nChPerInd, true, bPutCR, bNLBracket, bIndentInEmpty);
		mInput = input;
		mOutput = output;
		mGetPos = 0;
	}

	@Override
	protected char getChar() throws IOException {
		if (mGetPos < mInput.length())
			return mInput.charAt(mGetPos++);
		else
			return 0;
	}

	@Override
	protected void putChar(char ch) throws IOException {
		mOutput.append(ch);
	}

}
