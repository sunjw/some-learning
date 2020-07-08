package org.sunjw.test;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import org.sunjw.js.JsfString;

public class JsfTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length != 2) {
			System.out.println("Usage: jsf [input file] [output file]");
			return;
		}

		String inputFile = args[0];
		String outputFile = args[1];

		FileReader fr;
		FileWriter fw;
		try {
			fr = new FileReader(inputFile);
			fw = new FileWriter(outputFile);

			// Read from file.
			String source = readWholeFile(fr);

			// JsfString
			StringBuffer result = new StringBuffer();
			JsfString jsf = new JsfString(source, result, ' ', 4, false, false,
					false);
			jsf.debugOutput = true;
			jsf.go();

			// Write to file.
			writeWholeFile(fw, result.toString());

			fr.close();
			fw.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private static String readWholeFile(FileReader fr) throws IOException {
		StringBuffer sb = new StringBuffer();

		int c = 0;
		while ((c = fr.read()) != -1) {
			sb.append((char) c);
		}

		return sb.toString();
	}

	private static void writeWholeFile(FileWriter fw, String text)
			throws IOException {
		fw.write(text);
	}
}
