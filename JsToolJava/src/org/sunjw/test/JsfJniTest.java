package org.sunjw.test;

import org.sunjw.jni.JsfJniWrapper;

public class JsfJniTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		String testjs = "xx = {xxx};";
		JsfJniWrapper jjw = new JsfJniWrapper();
		String formattedjs = jjw.formatJs(testjs, '\t', 1, true, false, false);
		System.out.println(formattedjs);
	}

}
