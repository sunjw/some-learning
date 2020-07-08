package org.sunjw.js;

import java.io.IOException;
import java.util.Stack;

/**
 * JsonParser <br>
 * A Json parser in Java. <br>
 * Based on jsonparser.h & jsonparser.cpp.<br>
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
 * @date 2013-1-29
 * @version 0.9
 * 
 */
public abstract class JsonParser extends JsParser {

	private int mNRecuLevel; // 块递归层次
	private Stack<Character> mBlockStack;

	private long mStartTime;
	private long mEndTime;
	private long mDuration;

	public boolean debugOutput;

	public JsonParser() {
		mNRecuLevel = 0;
		mBlockStack = new Stack<Character>();

		debugOutput = false;
		mStartTime = (long) 0.0;
		mEndTime = (long) 0.0;
		mDuration = (long) 0.0;
	}

	public void go(JsonValue jsonValue) throws IOException {
		recursiveProc(jsonValue);
	}

	private void recursiveProc(JsonValue jsonValue) throws IOException {
		// initial job
		if (mNRecuLevel == 0) {
			mStartTime = System.currentTimeMillis();
			jsonValue.setValueType(JsonValue.VALUE_TYPE.UNKNOWN_VALUE);
		}

		++mNRecuLevel;
		// initial job

		char stackTop = JS_EMPTY;
		stackTop = getStackTop(mBlockStack, stackTop);

		String key = new String(), strValue = new String();
		long keyLine = -1, valLine = -1;
		boolean bGetKey = false;
		boolean bGetSplitor = false;

		while (getToken()) // 获得下一个 m_tokenA 和 m_tokenB
		{
			// JsonParser 忽略换行, 其它的解析器可能不要忽略
			if (mTokenA.code.equals("\r\n") || mTokenA.code.equals("\n")
					|| mTokenA.type == COMMENT_TYPE_1
					|| mTokenA.type == COMMENT_TYPE_2) {
				continue;
			}

			/*
			 * 至此，读取完成 mTokenA.code 和 mTokenB 已经合并多个换行 已经识别负数 已经识别正则表达式
			 */
			if (mTokenA.code.equals("{")) {
				mBlockStack.push(JS_BLOCK);
				long blockLine = mTokenA.line;

				if (stackTop == JS_EMPTY) {
					jsonValue.setValueType(JsonValue.VALUE_TYPE.MAP_VALUE);
					recursiveProc(jsonValue);
				} else {
					JsonValue innerValue = new JsonValue();
					innerValue.setValueType(JsonValue.VALUE_TYPE.MAP_VALUE);

					recursiveProc(innerValue);

					if (stackTop == JS_SQUARE) {
						innerValue.line = blockLine;
						jsonValue.arrayPut(innerValue);
					} else if (stackTop == JS_BLOCK) {
						bGetKey = false;
						bGetSplitor = false;
						innerValue.line = keyLine;
						jsonValue.mapPut(key, innerValue);
					}
				}

				continue;
			}

			if (mTokenA.code.equals("}")) {
				bGetKey = false;
				bGetSplitor = false;

				mBlockStack.pop();
				--mNRecuLevel;

				return;
			}

			if (mTokenA.code.equals("[")) {
				mBlockStack.push(JS_SQUARE);
				long squareLine = mTokenA.line;

				if (stackTop == JS_EMPTY) {
					jsonValue.setValueType(JsonValue.VALUE_TYPE.ARRAY_VALUE);
					recursiveProc(jsonValue);
				} else {
					JsonValue innerValue = new JsonValue();
					innerValue.setValueType(JsonValue.VALUE_TYPE.ARRAY_VALUE);

					recursiveProc(innerValue);

					if (stackTop == JS_SQUARE) {
						innerValue.line = squareLine;
						jsonValue.arrayPut(innerValue);
					} else if (stackTop == JS_BLOCK) {
						bGetKey = false;
						bGetSplitor = false;
						innerValue.line = keyLine;
						jsonValue.mapPut(key, innerValue);
					}
				}

				continue;
			}

			if (mTokenA.code.equals("]")) {
				mBlockStack.pop();
				--mNRecuLevel;

				return;
			}

			if (stackTop == JS_BLOCK) {
				if (!bGetKey && !mTokenA.code.equals(",")) {
					key = mTokenA.code.toString();
					keyLine = mTokenA.line;

					if (key.charAt(0) == '\'')
						key = key.substring(1, key.length() - 1); // strtrim(key,
																	// String("'"));
					else if (key.charAt(0) == '"')
						key = key.substring(1, key.length() - 1); // strtrim(key,
																	// String("\""));

					bGetKey = true;
					continue;
				}

				if (bGetKey && !bGetSplitor && mTokenA.code.equals(":")) {
					bGetSplitor = true;
					continue;
				}

				if (bGetKey && bGetSplitor) {
					strValue = readStrValue();
					valLine = mTokenA.line;

					JsonValue jValue = new JsonValue();
					genStrJsonValue(jValue, strValue);

					jValue.line = keyLine;
					jsonValue.mapPut(key, jValue);

					bGetKey = false;
					bGetSplitor = false;
				}
			}

			if (stackTop == JS_SQUARE) {
				if (!mTokenA.code.equals(",")) {
					strValue = readStrValue();
					valLine = mTokenA.line;

					JsonValue jValue = new JsonValue();
					genStrJsonValue(jValue, strValue);

					jValue.line = valLine;
					jsonValue.arrayPut(jValue);
				}
			}
		}

		// finished job
		if (mNRecuLevel == 1) {
			// FlushLineBuffer();
			mEndTime = System.currentTimeMillis();
			mDuration = mEndTime - mStartTime;
			if (debugOutput) {
				System.out.println("Processed tokens: " + mTokenCount);
				System.out.println("Time used: " + mDuration + "ms");
				System.out.println(mTokenCount / mDuration + " tokens/msecond");
			}
		}
		// finished job
	}

	private String readStrValue() throws IOException {
		String ret = mTokenA.code.toString();
		// fix decimal number value bug
		if (mTokenB.code.equals(".")) {
			// maybe it's a decimal
			String strDec = mTokenA.code.toString();
			getToken();
			strDec += ".";
			strDec += mTokenB.code.toString();
			ret = strDec;
			getToken();
		}

		return ret;
	}

	private void genStrJsonValue(JsonValue jsonValue, String value) {
		if (value.charAt(0) == '\'' || value.charAt(0) == '"') {
			if (value.charAt(0) == '\'')
				value = value.substring(1, value.length() - 1); // strtrim(value,
																// string("'"));
			else if (value.charAt(0) == '"')
				value = value.substring(1, value.length() - 1); // strtrim(value,
																// string("\""));

			/*
			 * STRING_VALUE 存入的时候会把周围的引号去掉 输出的时候统一成 "..." 所以要把里面的引号转义
			 */
			value = value.replaceAll("\\'", "'");
			value = value.replaceAll("\\\"", "\"");
			value = value.replaceAll("\"", "\\\"");

			jsonValue.setValueType(JsonValue.VALUE_TYPE.STRING_VALUE);
		} else if (isNumChar(value.charAt(0)) || value.charAt(0) == '-'
				|| value.charAt(0) == '+') {
			jsonValue.setValueType(JsonValue.VALUE_TYPE.NUMBER_VALUE);
		} else if (value.equals("true") || value.equals("false")) {
			jsonValue.setValueType(JsonValue.VALUE_TYPE.BOOL_VALUE);
		} else if (value.charAt(0) == '/') {
			jsonValue.setValueType(JsonValue.VALUE_TYPE.REGULAR_VALUE);
		} else {
			jsonValue.setValueType(JsonValue.VALUE_TYPE.UNKNOWN_VALUE);
		}

		jsonValue.setStrValue(value);
	}
}
