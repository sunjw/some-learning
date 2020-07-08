package org.sunjw.js;

import java.io.IOException;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Stack;

import org.sunjw.js.util.CodeBuffer;

/**
 * JsParser <br>
 * A JavaScript code parser in Java. <br>
 * Based on jsparser.h & jsparser.cpp.<br>
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
public abstract class JsParser {
	protected static final int STRING_TYPE = 0;
	protected static final int OPER_TYPE = 1;
	protected static final int REGULAR_TYPE = 2;
	protected static final int COMMENT_TYPE_1 = 9; // 单行注释
	protected static final int COMMENT_TYPE_2 = 10; // 多行注释

	protected static final char JS_IF = 'i';
	protected static final char JS_ELSE = 'e';
	protected static final char JS_FOR = 'f';
	protected static final char JS_DO = 'd';
	protected static final char JS_WHILE = 'w';
	protected static final char JS_SWITCH = 's';
	protected static final char JS_CASE = 'c';
	protected static final char JS_TRY = 'r';
	protected static final char JS_CATCH = 'h';
	protected static final char JS_FUNCTION = 'n';
	protected static final char JS_ASSIGN = '=';
	protected static final char JS_BLOCK = '{';
	protected static final char JS_BRACKET = '(';
	protected static final char JS_SQUARE = '[';
	protected static final char JS_HELPER = '\\';
	protected static final char JS_EMPTY = 0;

	protected class Token {
		CodeBuffer code;
		int type;
		long line;

		public Token() {
			code = new CodeBuffer();
			type = STRING_TYPE;
			line = -1;
		}

		public Token(Token anotherToken) {
			if (this == anotherToken)
				return;

			code = new CodeBuffer(anotherToken.code.toString());
			type = anotherToken.type;
			line = anotherToken.line;
		}
	}

	protected char mCharA;
	protected char mCharB;
	protected Token mTokenA;
	protected Token mTokenB;

	protected long mLineCount;
	protected long mTokenCount;

	private String mStrBeforeReg; // 判断正则时，正则前面可以出现的字符

	private boolean mBRegular; // tokenB 实际是正则 GetToken 用到的两个成员状态
	private boolean mBPosNeg; // tokenB 实际是正负数
	private Queue<Token> mTokenBQueue;

	private boolean mBGetTokenInit; // 是否是第一次执行 GetToken

	/**
	 * Get the top element from a stack.
	 * 
	 * @param stk
	 * @param ret
	 * @return
	 */
	public static <T> T getStackTop(Stack<T> stk, T empty) {
		if (stk.size() == 0)
			return empty;
		// ret = stk.peek();
		// return true;
		return stk.peek();
	}

	/**
	 * Is the top element of a stack equals to specified object.
	 * 
	 * @param stk
	 * @param eq
	 * @return
	 */
	public static <T> boolean isStackTopEq(Stack<T> stk, T object) {
		if (stk.size() == 0)
			return false;
		return (object == stk.peek());
	}

	public JsParser() {
		init();
	}

	private void init() {
		mTokenA = new Token();
		mTokenB = new Token();

		mTokenCount = 0;
		mLineCount = 1;

		mStrBeforeReg = "(,=:[!&|?+{};\n";

		mBRegular = false;
		mBPosNeg = false;
		mBGetTokenInit = false;

		mTokenBQueue = new LinkedList<Token>();
	}

	/**
	 * Is ch a normal character.
	 * 
	 * @param ch
	 * @return
	 */
	protected boolean isNormalChar(char ch) {
		return ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')
				|| (ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '$'
				|| ch > 126 || ch < 0);
	}

	/**
	 * Is ch a number or '.'.
	 * 
	 * @param ch
	 * @return
	 */
	protected boolean isNumChar(char ch) {
		return ((ch >= '0' && ch <= '9') || ch == '.');
	}

	/**
	 * Is ch a blank character.
	 * 
	 * @param ch
	 * @return
	 */
	protected boolean isBlankChar(char ch) {
		return (ch == ' ' || ch == '\t' || ch == '\r');
	}

	/**
	 * Is ch a operator which only has one character.
	 * 
	 * @param ch
	 * @return
	 */
	protected boolean isSingleOper(char ch) {
		return (ch == '.' || ch == '(' || ch == ')' || ch == '[' || ch == ']'
				|| ch == '{' || ch == '}' || ch == ',' || ch == ';'
				|| ch == '~' || ch == '\n');
	}

	/**
	 * Is ch a quote character.
	 * 
	 * @param ch
	 * @return
	 */
	protected boolean isQuote(char ch) {
		return (ch == '\'' || ch == '\"');
	}

	/**
	 * If there is a comment.
	 * 
	 * @return
	 */
	private boolean isComment() {
		return (mCharA == '/' && (mCharB == '/' || mCharB == '*'));
	}

	/**
	 * How JsParser get char from an input source. Must be implemented by a
	 * derived class.
	 * 
	 * @return
	 * @throws IOException
	 */
	protected abstract char getChar() throws IOException;

	/**
	 * Inner function to process character input.
	 * 
	 * @throws IOException
	 */
	private void getTokenRaw() throws IOException {
		if (!mBGetTokenInit) {
			mCharB = getChar();
		}

		// normal procedure
		if (!mBRegular && !mBPosNeg) {
			mTokenB = new Token();
			mTokenB.type = STRING_TYPE;
			mTokenB.line = mLineCount;
		} else if (mBRegular) {
			mTokenB.type = REGULAR_TYPE; // 正则
			// m_tokenB.push_back('/');
		} else {
			mTokenB.type = STRING_TYPE; // 正负数
		}

		boolean bQuote = false;
		boolean bComment = false;
		boolean bRegularFlags = false;
		boolean bFirst = true;
		boolean bNum = false; // 是不是数字
		boolean bLineBegin = false;
		char chQuote = '\0'; // 记录引号类型 ' 或 "
		char chComment = '\0'; // 注释类型 / 或 *

		while (true) {
			mCharA = mCharB;
			if (mCharA == 0)
				return;

			if (mCharA == '\n')
				++mLineCount;

			do {
				mCharB = getChar();
			} while (mCharB == '\r');

			/*
			 * 参考 m_charB 来处理 m_charA 输出或不输出 m_charA 下一次循环时自动会用 m_charB 覆盖
			 * m_charA
			 */

			// 正则需要在 token 级别判断
			if (mBRegular) {
				// 正则状态全部输出，直到 /
				mTokenB.code.append(mCharA);

				if (mCharA == '\\' && (mCharB == '/' || mCharB == '\\')) // 转义字符
				{
					mTokenB.code.append(mCharB);
					mCharB = getChar();
				}

				if (mCharA == '/') // 正则可能结束
				{
					if (!bRegularFlags && isNormalChar(mCharB)) {
						// 正则的 flags 部分
						bRegularFlags = true;
					} else {
						// 正则结束
						mBRegular = false;
						return;
					}
				}

				if (bRegularFlags && !isNormalChar(mCharB)) {
					// 正则结束
					bRegularFlags = false;
					mBRegular = false;
					return;
				}

				continue;
			}

			if (bQuote) {
				// 引号状态，全部输出，直到引号结束
				mTokenB.code.append(mCharA);

				if (mCharA == '\\' && (mCharB == chQuote || mCharB == '\\')) // 转义字符
				{
					mTokenB.code.append(mCharB);
					mCharB = getChar();
				}

				if (mCharA == chQuote) // 引号结束
					return;

				continue;
			}

			if (bComment) {
				// 注释状态，全部输出
				if (mTokenB.type == COMMENT_TYPE_2) {
					// /*...*/每行前面的\t, ' '都要删掉
					if (bLineBegin && (mCharA == '\t' || mCharA == ' '))
						continue;
					else if (bLineBegin && mCharA == '*')
						mTokenB.code.append(' ');

					bLineBegin = false;

					if (mCharA == '\n')
						bLineBegin = true;
				}
				mTokenB.code.append(mCharA);

				if (chComment == '*') {
					// 直到 */
					mTokenB.type = COMMENT_TYPE_2;
					if (mCharA == '*' && mCharB == '/') {
						mTokenB.code.append(mCharB);
						mCharB = getChar();
						return;
					}
				} else {
					// 直到换行
					mTokenB.type = COMMENT_TYPE_1;
					if (mCharA == '\n')
						return;
				}

				continue;
			}

			if (isNormalChar(mCharA)) {
				mTokenB.type = STRING_TYPE;
				mTokenB.code.append(mCharA);

				// 解决类似 82e-2, 442e+6, 555E-6 的问题
				// 因为这是立即数，所以只能符合以下的表达形式
				boolean bNumOld = bNum;
				if (bFirst || bNumOld) // 只有之前是数字才改变状态
				{
					bNum = isNumChar(mCharA);
					bFirst = false;
				}
				if (bNumOld
						&& !bNum
						&& (mCharA == 'e' || mCharA == 'E')
						&& (mCharB == '-' || mCharB == '+' || isNumChar(mCharB))) {
					bNum = true;
					if (mCharB == '-' || mCharB == '+') {
						mTokenB.code.append(mCharB);
						mCharB = getChar();
					}
				}

				if (!isNormalChar(mCharB)) // loop until m_charB is not normal
											// char
				{
					mBPosNeg = false;
					return;
				}
			} else {
				if (isBlankChar(mCharA))
					continue; // 忽略空白字符

				if (isQuote(mCharA)) {
					// 引号
					bQuote = true;
					chQuote = mCharA;

					mTokenB.type = STRING_TYPE;
					mTokenB.code.append(mCharA);
					continue;
				}

				if (isComment()) {
					// 注释
					bComment = true;
					chComment = mCharB;

					// m_tokenBType = COMMENT_TYPE;
					mTokenB.code.append(mCharA);
					continue;
				}

				if (isSingleOper(mCharA) || isNormalChar(mCharB)
						|| isBlankChar(mCharB) || isQuote(mCharB)) {
					mTokenB = new Token(); // 单字符符号
					mTokenB.type = OPER_TYPE;
					mTokenB.code.append(mCharA);
					return;
				}

				// 多字符符号
				if ((mCharB == '=' || mCharB == mCharA)
						|| (mCharA == '-' && mCharB == '>')) {
					// 的确是多字符符号
					mTokenB.type = OPER_TYPE;
					mTokenB.code.append(mCharA);
					mTokenB.code.append(mCharB);
					mCharB = getChar();
					if ((mTokenB.code.equals("==") || mTokenB.code.equals("!=")
							|| mTokenB.code.equals("<<") || mTokenB.code
								.equals(">>")) && mCharB == '=') {
						// 三字符 ===, !==, <<=, >>=
						mTokenB.code.append(mCharB);
						mCharB = getChar();
					} else if (mTokenB.code.equals(">>") && mCharB == '>') {
						// >>>, >>>=
						mTokenB.code.append(mCharB);
						mCharB = getChar();
						if (mCharB == '=') // >>>=
						{
							mTokenB.code.append(mCharB);
							mCharB = getChar();
						}
					}
					return;
				} else {
					// 还是单字符的
					mTokenB = new Token(); // 单字符符号
					mTokenB.type = OPER_TYPE;
					mTokenB.code.append(mCharA);
					return;
				}

				// What? How could we come here?
				// OK, javac told me that we cann't reach here.
				// mCharA = '\0';
				// return;
			}
		}
	}

	protected boolean getToken() throws IOException {
		if (!mBGetTokenInit) {
			// 第一次多调用一次 GetTokenRaw
			getTokenRaw();
			mBGetTokenInit = true;
		}

		prepareRegular(); // 判断正则
		preparePosNeg(); // 判断正负数

		++mTokenCount;
		mTokenA = new Token(mTokenB);

		if (mTokenBQueue.size() == 0) {
			getTokenRaw();
			prepareTokenB(); // 看看是不是要跳过换行
		} else {
			// 有排队的换行
			Token temp;
			temp = mTokenBQueue.poll();
			// mTokenBQueue.pop();
			mTokenB = new Token(temp);
		}

		return (mCharA != 0 || mTokenA.code.length() != 0);
	}

	/**
	 * 预处理正则表达式
	 * 
	 * @throws IOException
	 */
	private void prepareRegular() throws IOException {
		/*
		 * 先处理一下正则 m_tokenB[0] == /，且 m_tokenB 不是注释 m_tokenA 不是 STRING (除了
		 * m_tokenA == return) 而且 m_tokenA 的最后一个字符是下面这些
		 */
		// size_t last = m_tokenA.size() > 0 ? m_tokenA.size() - 1 : 0;
		char tokenALast = mTokenA.code.length() > 0 ? mTokenA.code
				.charAt(mTokenA.code.length() - 1) : 0;
		char tokenBFirst = mTokenB.code.length() > 0 ? mTokenB.code.charAt(0)
				: 0;
		if (tokenBFirst == '/'
				&& mTokenB.type != COMMENT_TYPE_1
				&& mTokenB.type != COMMENT_TYPE_2
				&& ((mTokenA.type != STRING_TYPE && mStrBeforeReg
						.indexOf(tokenALast) != -1) || mTokenA.code
						.equals("return"))) {
			mBRegular = true;
			getTokenRaw(); // 把正则内容加到 m_tokenB
		}
	}

	/**
	 * 预处理正负号
	 * 
	 * @throws IOException
	 */
	private void preparePosNeg() throws IOException {
		/*
		 * 如果 m_tokenB 是 -,+ 号 而且 m_tokenA 不是字符串型也不是正则表达式 而且 m_tokenA 不是 ++, --,
		 * ], ) 而且 m_charB 是一个 NormalChar 那么 m_tokenB 实际上是一个正负数
		 */
		if (mTokenB.type == OPER_TYPE
				&& (mTokenB.code.equals("-") || mTokenB.code.equals("+"))
				&& (mTokenA.type != STRING_TYPE || mTokenA.code
						.equals("return")) && mTokenA.type != REGULAR_TYPE
				&& !mTokenA.code.equals("++") && !mTokenA.code.equals("--")
				&& !mTokenA.code.equals("]") && !mTokenA.code.equals(")")
				&& isNormalChar(mCharB)) {
			// m_tokenB 实际上是正负数
			mBPosNeg = true;
			getTokenRaw();
		}
	}

	/**
	 * 预处理 TokenB
	 * 
	 * @throws IOException
	 */
	private void prepareTokenB() throws IOException {
		// char stackTop = m_blockStack.top();

		/*
		 * 跳过 else, while, catch, ',', ';', ')', { 之前的换行
		 * 如果最后读到的不是上面那几个，再把去掉的换行补上
		 */
		int c = 0;
		while (mTokenB.code.equals("\n") || mTokenB.code.equals("\r\n")) {
			++c;
			getTokenRaw();
		}

		if (!mTokenB.code.equals("else") && !mTokenB.code.equals("while")
				&& !mTokenB.code.equals("catch") && !mTokenB.code.equals(",")
				&& !mTokenB.code.equals(";") && !mTokenB.code.equals(")")) {
			// 将去掉的换行压入队列，先处理
			if (mTokenA.code.equals("{") && mTokenB.code.equals("}"))
				return; // 空 {}

			Token temp = new Token();
			c = c > 2 ? 2 : c;
			for (; c > 0; --c) {
				temp.code = new CodeBuffer("\n");
				temp.type = OPER_TYPE;
				mTokenBQueue.offer(temp);
			}

			temp = new Token(mTokenB);
			mTokenBQueue.offer(temp);
			temp = mTokenBQueue.poll();
			// mTokenBQueue.pop();
			mTokenB = new Token(temp);
		}
	}
}
