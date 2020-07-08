package org.sunjw.jni;

import java.io.IOException;

import org.sunjw.js.JsfString;

public class JsfJniWrapper {

	public String formatJs(String jscode, char chIndent, int nChPerInd,
			boolean bPutCR, boolean bNLBracket, boolean bIndentInEmpty) {
		StringBuffer sb = new StringBuffer();

		JsfString jsfstr = new JsfString(jscode, sb, chIndent, nChPerInd,
				bPutCR, bNLBracket, bIndentInEmpty);
		try {
			jsfstr.go();
		} catch (IOException e) {
			e.printStackTrace(System.out);
		}

		return sb.toString();
	}

}
