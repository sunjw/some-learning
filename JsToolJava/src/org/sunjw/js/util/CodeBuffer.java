package org.sunjw.js.util;

import java.io.Serializable;

/**
 * TokenBuffer <br>
 * A wrapper of StringBuffer, provides equals like String.equals().<br>
 * <br>
 * Copyright (c) 2012-
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * 
 * @author Sun Junwen
 * @date 2013-2-5
 * @version 0.9
 * 
 */
public class CodeBuffer implements Serializable, CharSequence {

	private static final long serialVersionUID = 4063766941723794799L;

	private StringBuffer mValue;

	public CodeBuffer() {
		mValue = new StringBuffer();
	}

	public CodeBuffer(String string) {
		mValue = new StringBuffer(string);
	}

	public CodeBuffer(char ch) {
		mValue = new StringBuffer();
		mValue.append(ch);
	}

	/**
	 * Wrapper of StringBuffer.append(boolean b)
	 * 
	 * @param b
	 * @return
	 */
	public StringBuffer append(boolean b) {
		return mValue.append(b);
	}

	/**
	 * Wrapper of StringBuffer.append(char c)
	 * 
	 * @param c
	 * @return
	 */
	public StringBuffer append(char c) {
		return mValue.append(c);
	}

	/**
	 * Wrapper of StringBuffer.append(char[] str)
	 * 
	 * @param str
	 * @return
	 */
	public StringBuffer append(char[] str) {
		return mValue.append(str);
	}

	/**
	 * Wrapper of StringBuffer.append(char[] str, int offset, int len)
	 * 
	 * @param str
	 * @param offset
	 * @param len
	 * @return
	 */
	public StringBuffer append(char[] str, int offset, int len) {
		return mValue.append(str, offset, len);
	}

	/**
	 * Wrapper of StringBuffer.append(CharSequence s)
	 * 
	 * @param s
	 * @return
	 */
	public StringBuffer append(CharSequence s) {
		return mValue.append(s);
	}

	/**
	 * Wrapper of StringBuffer.append(CharSequence s, int start, int end)
	 * 
	 * @param s
	 * @param start
	 * @param end
	 * @return
	 */
	public StringBuffer append(CharSequence s, int start, int end) {
		return mValue.append(s, start, end);
	}

	/**
	 * Wrapper of StringBuffer.append(double d)
	 * 
	 * @param d
	 * @return
	 */
	public StringBuffer append(double d) {
		return mValue.append(d);
	}

	/**
	 * Wrapper of StringBuffer.append(float f)
	 * 
	 * @param f
	 * @return
	 */
	public StringBuffer append(float f) {
		return mValue.append(f);
	}

	/**
	 * Wrapper of StringBuffer.append(int i)
	 * 
	 * @param i
	 * @return
	 */
	public StringBuffer append(int i) {
		return mValue.append(i);
	}

	/**
	 * Wrapper of StringBuffer.append(long lng)
	 * 
	 * @param lng
	 * @return
	 */
	public StringBuffer append(long lng) {
		return mValue.append(lng);
	}

	/**
	 * Wrapper of StringBuffer.append(Object obj)
	 * 
	 * @param obj
	 * @return
	 */
	public StringBuffer append(Object obj) {
		return mValue.append(obj);
	}

	/**
	 * Wrapper of StringBuffer.append(String str)
	 * 
	 * @param str
	 * @return
	 */
	public StringBuffer append(String str) {
		return mValue.append(str);
	}

	/**
	 * Behavior as String.equals()
	 */
	@Override
	public boolean equals(Object anObject) {
		if (this == anObject) {
			return true;
		}
		if (anObject instanceof String) {
			String anotherString = (String) anObject;
			int n = mValue.length();
			if (n == anotherString.length()) {
				int i = 0;
				int j = 0;
				while (n-- != 0) {
					if (mValue.charAt(i++) != anotherString.charAt(j++))
						return false;
				}
				return true;
			}
		}
		return false;
	}

	/**
	 * Wrapper of StringBuffer.charAt(int index)
	 */
	@Override
	public char charAt(int index) {
		return mValue.charAt(index);
	}

	/**
	 * Wrapper of StringBuffer.length()
	 */
	@Override
	public int length() {
		return mValue.toString().length();
	}

	/**
	 * Wrapper of StringBuffer.subSequence(int start, int end)
	 */
	@Override
	public CharSequence subSequence(int start, int end) {
		return mValue.subSequence(start, end);
	}

	/**
	 * Wrapper of StringBuffer.toString()
	 */
	@Override
	public String toString() {
		return mValue.toString();
	}
}
