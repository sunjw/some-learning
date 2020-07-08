package org.sunjw.js;

import java.io.IOException;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.TreeMap;
import java.util.TreeSet;

import org.apache.commons.lang.StringUtils;
import org.sunjw.js.util.CodeBuffer;

/**
 * JsFormatter <br>
 * A JavaScript formatter(beautify) in Java. <br>
 * Based on realjsformatter.h & realjsformatter.cpp.<br>
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
 * @date 2013-2-6
 * @version 0.9
 * 
 */
public abstract class JsFormatter extends JsParser {

	public boolean debugOutput;

	private long mStartTime;
	private long mEndTime;
	private long mDuration;

	private int mNLineIndents;
	private StringBuffer mLineBuffer;

	private Set<String> mSpecKeywordSet; // 后面要跟着括号的关键字集合
	private Map<String, Character> mBlockMap;
	private Stack<Character> mBlockStack;
	private int mNIndents; // 缩进数量，不用计算 blockStack，效果不好

	// 使用栈是为了解决在判断条件中出现循环的问题
	private Stack<Boolean> mBrcNeedStack; // if 之类的后面的括号

	private boolean mBNewLine; // 准备换行的标志
	private boolean mBBlockStmt; // block 真正开始了
	private boolean mBAssign;
	private boolean mBEmptyBracket; // 空 {}

	private boolean mBCommentPut; // 刚刚输出了注释

	private String mInitIndent; // 起始缩进
	private char mChIndent; // 作为缩进的字符
	private int mNChPerInd; // 每个缩进缩进字符个数

	@SuppressWarnings("unused")
	private boolean mBSkipCR; // 读取时跳过 \r
	private boolean mBPutCR; // 使用 \r\n 作为换行

	private boolean mBNLBracket; // { 之前是否换行
	private boolean mBIndentInEmpty; // 是否保持空行的缩进, JSLint 不推荐

	public JsFormatter() {
		mChIndent = '\t';
		mNChPerInd = 1;
		mBSkipCR = false;
		mBPutCR = false;
		mBNLBracket = false;
		mBIndentInEmpty = false;

		init();
	}

	public JsFormatter(char chIndent, int nChPerInd) {
		mChIndent = chIndent;
		mNChPerInd = nChPerInd;
		mBSkipCR = false;
		mBPutCR = false;
		mBNLBracket = false;
		mBIndentInEmpty = false;

		init();
	}

	public JsFormatter(boolean skipCR, boolean putCR) {
		mChIndent = '\t';
		mNChPerInd = 1;
		mBSkipCR = skipCR;
		mBPutCR = putCR;
		mBNLBracket = false;
		mBIndentInEmpty = false;

		init();
	}

	public JsFormatter(char chIndent, int nChPerInd, boolean skipCR,
			boolean putCR, boolean NLBracket, boolean indentInEmpty) {
		mChIndent = chIndent;
		mNChPerInd = nChPerInd;
		mBSkipCR = skipCR;
		mBPutCR = putCR;
		mBNLBracket = NLBracket;
		mBIndentInEmpty = indentInEmpty;

		init();
	}

	/**
	 * 对于 StringUtils.strip(str, " \r\n\t") 的包装
	 * 
	 * @param str
	 * @return
	 */
	public static String trimString(String str) {
		return StringUtils.strip(str, " \r\n\t");
	}

	/**
	 * 对于 StringUtils.strip(str, " \t") 的包装
	 * 
	 * @param str
	 * @return
	 */
	public static String trimSpace(String str) {
		return StringUtils.strip(str, " \t");
	}

	/**
	 * 对于 StringUtils.stripEnd(str, " \t") 的包装
	 * 
	 * @param str
	 * @return
	 */
	public static String trimRightSpace(String str) {
		return StringUtils.stripEnd(str, " \t");
	}

	/**
	 * 对于 StringUtils.replace(str, src, des) 的包装
	 * 
	 * @param str
	 * @param src
	 * @param des
	 * @return
	 */
	public static String stringReplace(String str, String src, String des) {
		return StringUtils.replace(str, src, des);
	}

	private void init() {
		mInitIndent = new String("");

		debugOutput = false;

		mTokenCount = 0;

		mLineBuffer = new StringBuffer("");

		mNIndents = 0;
		mNLineIndents = 0;
		mBNewLine = false;
		mBBlockStmt = true;
		mBAssign = false;
		mBEmptyBracket = false;
		mBCommentPut = false;

		mBlockStack = new Stack<Character>();
		mBrcNeedStack = new Stack<Boolean>();

		mBlockMap = new TreeMap<String, Character>();
		mBlockMap.put("if", JS_IF);
		mBlockMap.put("else", JS_ELSE);
		mBlockMap.put("for", JS_FOR);
		mBlockMap.put("do", JS_DO);
		mBlockMap.put("while", JS_WHILE);
		mBlockMap.put("switch", JS_SWITCH);
		mBlockMap.put("case", JS_CASE);
		mBlockMap.put("default", JS_CASE);
		mBlockMap.put("try", JS_TRY);
		mBlockMap.put("finally", JS_TRY); // 等同于 try
		mBlockMap.put("catch", JS_CATCH);
		mBlockMap.put("=", JS_ASSIGN);
		mBlockMap.put("function", JS_FUNCTION);
		mBlockMap.put("{", JS_BLOCK);
		mBlockMap.put("(", JS_BRACKET);
		mBlockMap.put("[", JS_SQUARE);

		mSpecKeywordSet = new TreeSet<String>();
		mSpecKeywordSet.add("if");
		mSpecKeywordSet.add("for");
		mSpecKeywordSet.add("while");
		mSpecKeywordSet.add("switch");
		mSpecKeywordSet.add("catch");
		mSpecKeywordSet.add("function");
		mSpecKeywordSet.add("with");
		mSpecKeywordSet.add("return");
	}

	/**
	 * 派生类需要实现如何输出字符
	 * 
	 * @param ch
	 * @throws IOException
	 */
	protected abstract void putChar(char ch) throws IOException;

	private void putToken(String token) throws IOException {
		putToken(token, "", "");
	}

	private void putToken(String token, String leftStyle) throws IOException {
		putToken(token, leftStyle, "");
	}

	private void putToken(String token, String leftStyle, String rightStyle)
			throws IOException {
		putString(leftStyle);
		putString(token);
		putString(rightStyle);
		if (!(mBCommentPut && mBNewLine))
			mBCommentPut = false; // 这个一定会发生在注释之后的任何输出后面
	}

	private void putString(String str) throws IOException {
		int length = str.length();
		for (int i = 0; i < length; ++i) {
			if (mBNewLine
					&& (mBCommentPut || ((mBNLBracket || str.charAt(i) != '{')
							&& str.charAt(i) != ',' && str.charAt(i) != ';'))) {
				// 换行后面不是紧跟着 {,; 才真正换
				putLineBuffer(); // 输出行缓冲

				mLineBuffer = new StringBuffer("");
				mBNewLine = false;
				mNIndents = mNIndents < 0 ? 0 : mNIndents; // 出错修正
				mNLineIndents = mNIndents;
				if (str.charAt(i) == '{' || str.charAt(i) == ','
						|| str.charAt(i) == ';') // 行结尾是注释，使得{,;不得不换行
					--mNLineIndents;
			}

			if (mBNewLine
					&& !mBCommentPut
					&& ((!mBNLBracket && str.charAt(i) == '{')
							|| str.charAt(i) == ',' || str.charAt(i) == ';'))
				mBNewLine = false;

			if (str.charAt(i) == '\n')
				mBNewLine = true;
			else
				mLineBuffer.append(str.charAt(i));
		}
	}

	private void putLineBuffer() throws IOException {
		CodeBuffer line = new CodeBuffer("");
		line.append(trimRightSpace(mLineBuffer.toString()));

		if (!line.equals("") || mBIndentInEmpty) // Fix "JSLint unexpect space"
													// bug
		{
			for (int i = 0; i < mInitIndent.length(); ++i)
				putChar(mInitIndent.charAt(i)); // 先输出预缩进

			for (int c = 0; c < mNLineIndents; ++c)
				for (int c2 = 0; c2 < mNChPerInd; ++c2)
					putChar(mChIndent); // 输出缩进
		}

		// 加上换行
		if (mBPutCR)
			line.append("\r"); // PutChar('\r');
		line.append("\n"); // PutChar('\n');

		// 输出 line
		for (int i = 0; i < line.length(); ++i)
			putChar(line.charAt(i));
	}

	private void popMultiBlock(char previousStackTop) {
		if (mTokenB.code.equals(";")) // 如果 m_tokenB 是 ;，弹出多个块的任务留给它
			return;

		if (!((previousStackTop == JS_IF && mTokenB.code.equals("else"))
				|| (previousStackTop == JS_DO && mTokenB.code.equals("while")) || (previousStackTop == JS_TRY && mTokenB.code
				.equals("catch")))) {
			Character topStack = null;// = m_blockStack.top();
			if ((topStack = getStackTop(mBlockStack, null)) == null)
				return;
			// ; 还可能可能结束多个 if, do, while, for, try, catch
			while (topStack == JS_IF || topStack == JS_FOR
					|| topStack == JS_WHILE || topStack == JS_DO
					|| topStack == JS_ELSE || topStack == JS_TRY
					|| topStack == JS_CATCH) {
				if (topStack == JS_IF || topStack == JS_FOR
						|| topStack == JS_WHILE || topStack == JS_CATCH
						|| topStack == JS_ELSE || topStack == JS_TRY) {
					mBlockStack.pop();
					--mNIndents;
				} else if (topStack == JS_DO) {
					--mNIndents;
				}

				if ((topStack == JS_IF && mTokenB.code.equals("else"))
						|| (topStack == JS_DO && mTokenB.code.equals("while"))
						|| (topStack == JS_TRY && mTokenB.code.equals("catch")))
					break; // 直到刚刚结束一个 if...else, do...while, try...catch
				// topStack = m_blockStack.top();
				if ((topStack = getStackTop(mBlockStack, null)) == null)
					break;
			}
		}
	}

	public void go() throws IOException {
		mBlockStack.push(' ');
		mBrcNeedStack.push(true);

		boolean bHaveNewLine;
		char tokenAFirst;
		char tokenBFirst;

		mStartTime = System.currentTimeMillis();

		while (getToken()) {
			bHaveNewLine = false; // bHaveNewLine 表示后面将要换行，m_bNewLine 表示已经换行了
			tokenAFirst = mTokenA.code.charAt(0);
			tokenBFirst = (mTokenB.code.length() > 0) ? mTokenB.code.charAt(0)
					: 0;
			if (tokenBFirst == '\r')
				tokenBFirst = '\n';
			if (tokenBFirst == '\n' || mTokenB.type == COMMENT_TYPE_1)
				bHaveNewLine = true;

			if (!mBBlockStmt && !mTokenA.code.equals("{")
					&& !mTokenA.code.equals("\n")
					&& mTokenA.type != COMMENT_TYPE_1
					&& mTokenA.type != COMMENT_TYPE_2)
				mBBlockStmt = true;

			/*
			 * 参考 m_tokenB 来处理 m_tokenA 输出或不输出 m_tokenA 下一次循环时自动会用 m_tokenB 覆盖
			 * m_tokenA
			 */
			// PutToken(m_tokenA);
			switch (mTokenA.type) {
			case REGULAR_TYPE:
				putToken(mTokenA.code.toString()); // 正则表达式直接输出，前后没有任何样式
				break;
			case COMMENT_TYPE_1:
			case COMMENT_TYPE_2:
				if (mTokenA.code.charAt(1) == '*') {
					// 多行注释
					if (!bHaveNewLine)
						putToken(mTokenA.code.toString(), "", "\n"); // 需要换行
					else
						putToken(mTokenA.code.toString());
				} else {
					// 单行注释
					putToken(mTokenA.code.toString()); // 肯定会换行的
				}
				mBCommentPut = true;
				break;
			case OPER_TYPE:
				processOper(bHaveNewLine, tokenAFirst, tokenBFirst);

				break;
			case STRING_TYPE:
				processString(bHaveNewLine, tokenAFirst, tokenBFirst);
				break;
			}
		}

		mLineBuffer = new StringBuffer(trimString(mLineBuffer.toString()));
		if (mLineBuffer.length() > 0)
			putLineBuffer();

		mEndTime = System.currentTimeMillis();
		mDuration = mEndTime - mStartTime;
		if (debugOutput) {
			System.out.println("Processed tokens: " + mTokenCount);
			System.out.println("Time used: " + mDuration + "ms");
			System.out.println((mTokenCount / mDuration) + " tokens/ms");
		}
	}

	private void processOper(boolean bHaveNewLine, char tokenAFirst,
			char tokenBFirst) throws IOException {
		char topStack = 0;// = m_blockStack.top();
		topStack = getStackTop(mBlockStack, topStack);
		String strRight = new String(" ");

		if (mTokenA.code.equals("(") || mTokenA.code.equals(")")
				|| mTokenA.code.equals("[") || mTokenA.code.equals("]")
				|| mTokenA.code.equals("!") || mTokenA.code.equals("!!")
				|| mTokenA.code.equals("~") || mTokenA.code.equals("^")
				|| mTokenA.code.equals(".")) {
			// ()[]!. 都是前后没有样式的运算符
			if ((mTokenA.code.equals(")") || mTokenA.code.equals("]"))
					&& (topStack == JS_ASSIGN || topStack == JS_HELPER)) {
				if (topStack == JS_ASSIGN)
					--mNIndents;
				mBlockStack.pop();
			}
			topStack = getStackTop(mBlockStack, topStack);
			if ((mTokenA.code.equals(")") && topStack == JS_BRACKET)
					|| (mTokenA.code.equals("]") && topStack == JS_SQUARE)) {
				// )] 需要弹栈，减少缩进
				mBlockStack.pop();
				--mNIndents;
				topStack = getStackTop(mBlockStack, topStack);
				// topStack = m_blockStack.top();
				if (topStack == JS_ASSIGN || topStack == JS_HELPER)
					mBlockStack.pop();
			}

			topStack = getStackTop(mBlockStack, topStack);
			if (mTokenA.code.equals(")")
					&& !mBrcNeedStack.peek()
					&& (topStack == JS_IF || topStack == JS_FOR
							|| topStack == JS_WHILE || topStack == JS_SWITCH || topStack == JS_CATCH)) {
				// 栈顶的 if, for, while, switch, catch 正在等待 )，之后换行增加缩进
				// 这里的空格和下面的空格是留给 { 的，m_bNLBracket 为 true 则不需要空格了
				String rightDeco = (!mTokenB.code.equals(";")) ? strRight : "";
				if (!bHaveNewLine)
					rightDeco += "\n";
				putToken(mTokenA.code.toString(), "", rightDeco);
				// bBracket = true;
				mBrcNeedStack.pop();
				mBBlockStmt = false; // 等待 statment
				if (isStackTopEq(mBlockStack, JS_WHILE)) // m_blockStack.top()
															// == JS_WHILE
				{
					mBlockStack.pop();
					if (isStackTopEq(mBlockStack, JS_DO)) // m_blockStack.top()
															// == JS_DO
					{
						// 结束 do...while
						mBlockStack.pop();

						popMultiBlock(JS_WHILE);
					} else {
						mBlockStack.push(JS_WHILE);
						++mNIndents;
					}
				} else
					++mNIndents;
			} else if (mTokenA.code.equals(")")
					&& (mTokenB.code.equals("{") || bHaveNewLine))
				putToken(mTokenA.code.toString(), "", strRight); // { 或者换行之前留个空格
			else
				putToken(mTokenA.code.toString()); // 正常输出

			if (mTokenA.code.equals("(") || mTokenA.code.equals("[")) {
				// ([ 入栈，增加缩进
				topStack = getStackTop(mBlockStack, topStack);
				// topStack = m_blockStack.top();
				if (topStack == JS_ASSIGN) {
					if (!mBAssign)
						--mNIndents;
					else
						mBlockStack.push(JS_HELPER);
				}
				mBlockStack.push(mBlockMap.get(mTokenA.code.toString()));
				++mNIndents;
			}

			return;
		}

		if (mTokenA.code.equals(";")) {
			topStack = getStackTop(mBlockStack, topStack);
			// topStack = m_blockStack.top();
			if (topStack == JS_ASSIGN) {
				--mNIndents;
				mBlockStack.pop();
			}

			topStack = getStackTop(mBlockStack, topStack);
			// topStack = m_blockStack.top();

			// ; 结束 if, else, while, for, try, catch
			if (topStack == JS_IF || topStack == JS_FOR || topStack == JS_WHILE
					|| topStack == JS_CATCH) {
				mBlockStack.pop();
				--mNIndents;
				// 下面的 } 有同样的处理
				popMultiBlock(topStack);
			}
			if (topStack == JS_ELSE || topStack == JS_TRY) {
				mBlockStack.pop();
				--mNIndents;
				popMultiBlock(topStack);
			}
			if (topStack == JS_DO) {
				--mNIndents;
				popMultiBlock(topStack);
			}
			// do 在读取到 while 后才修改计数
			// 对于 do{} 也一样

			// if(m_blockStack.top() == 't')
			// m_blockStack.pop(); // ; 也结束变量声明，暂时不压入 t

			topStack = getStackTop(mBlockStack, topStack);
			// topStack = m_blockStack.top();
			if (topStack != JS_BRACKET && !bHaveNewLine)
				putToken(mTokenA.code.toString(), "", (strRight += "\n")); // 如果不是
																			// ()
																			// 里的
																			// ;
																			// 就换行
			else if (topStack == JS_BRACKET || mTokenB.type == COMMENT_TYPE_1)
				putToken(mTokenA.code.toString(), "", strRight); // (; ) 空格
			else
				putToken(mTokenA.code.toString());

			return; // ;
		}

		if (mTokenA.code.equals(",")) {
			if (isStackTopEq(mBlockStack, JS_ASSIGN)) // m_blockStack.top() ==
														// JS_ASSIGN
			{
				--mNIndents;
				mBlockStack.pop();
			}
			if (isStackTopEq(mBlockStack, JS_BLOCK) && !bHaveNewLine)
				putToken(mTokenA.code.toString(), "", (strRight += "\n")); // 如果是
																			// {}
																			// 里的
			else
				putToken(mTokenA.code.toString(), "", strRight);

			return; // ,
		}

		if (mTokenA.code.equals("{")) {
			topStack = getStackTop(mBlockStack, topStack);
			if (topStack == JS_IF || topStack == JS_FOR || topStack == JS_WHILE
					|| topStack == JS_DO || topStack == JS_ELSE
					|| topStack == JS_SWITCH || topStack == JS_TRY
					|| topStack == JS_CATCH || topStack == JS_ASSIGN) {
				if (!mBBlockStmt || topStack == JS_ASSIGN)// (topStack ==
															// JS_ASSIGN &&
															// !m_bAssign))
				{
					// m_blockStack.pop(); // 不把那个弹出来，遇到 } 时一起弹
					--mNIndents;
					mBBlockStmt = true;
				} else {
					mBlockStack.push(JS_HELPER); // 压入一个 JS_HELPER 统一状态
				}
			}

			// 修正({...}) 中多一次缩进
			boolean bPrevFunc = (topStack == JS_FUNCTION);
			char fixTopStack = topStack;
			if (bPrevFunc) {
				mBlockStack.pop(); // 弹掉 JS_FUNCTION
				fixTopStack = getStackTop(mBlockStack, fixTopStack);
			}

			if (fixTopStack == JS_BRACKET) {
				--mNIndents; // ({ 减掉一个缩进
			}

			if (bPrevFunc) {
				mBlockStack.push(JS_FUNCTION); // 压回 JS_FUNCTION
			}

			mBlockStack.push(mBlockMap.get(mTokenA.code.toString())); // 入栈，增加缩进
			++mNIndents;

			/*
			 * { 之间的空格都是由之前的符号准备好的 这是为了解决 { 在新行时，前面会多一个空格的问题 因为算法只能向后，不能向前看
			 */
			if (mTokenB.code.equals("}")) {
				// 空 {}
				mBEmptyBracket = true;
				if (mBNewLine == false
						&& mBNLBracket
						&& (topStack == JS_IF || topStack == JS_FOR
								|| topStack == JS_WHILE
								|| topStack == JS_SWITCH
								|| topStack == JS_CATCH || topStack == JS_FUNCTION)) {
					putToken(mTokenA.code.toString(), " "); // 这些情况下，前面补一个空格
				} else {
					putToken(mTokenA.code.toString());
				}
			} else {
				String strLeft = (mBNLBracket && !mBNewLine) ? new String("\n")
						: new String("");
				if (!bHaveNewLine) // 需要换行
					putToken(mTokenA.code.toString(), strLeft,
							(strRight += "\n"));
				else
					putToken(mTokenA.code.toString(), strLeft, strRight);
			}

			return; // {
		}

		if (mTokenA.code.equals("}")) {
			// topStack = m_blockStack.top();

			// 激进的策略，} 一直弹到 {
			// 这样做至少可以使得 {} 之后是正确的
			while ((topStack = getStackTop(mBlockStack, (char) 0)) != 0) {
				if (topStack == JS_BLOCK)
					break;

				mBlockStack.pop();

				switch (topStack) {
				case JS_IF:
				case JS_FOR:
				case JS_WHILE:
				case JS_CATCH:
				case JS_DO:
				case JS_ELSE:
				case JS_TRY:
				case JS_SWITCH:
				case JS_ASSIGN:
				case JS_FUNCTION:
				case JS_HELPER:
					--mNIndents;
					break;
				}

				/*
				 * if(!GetStackTop(m_blockStack, topStack)) break;
				 */
				// topStack = m_blockStack.top();
			}

			if (topStack == JS_BLOCK) {
				// 弹栈，减小缩进
				mBlockStack.pop();
				--mNIndents;
				topStack = getStackTop(mBlockStack, (char) 0);
				boolean bGetTop = (topStack != 0);// = m_blockStack.top();

				if (bGetTop) {
					switch (topStack) {
					case JS_IF:
					case JS_FOR:
					case JS_WHILE:
					case JS_CATCH:
					case JS_ELSE:
					case JS_TRY:
					case JS_SWITCH:
					case JS_ASSIGN:
					case JS_FUNCTION:
					case JS_HELPER:
						mBlockStack.pop();
						break;
					case JS_DO:
						// 缩进已经处理，do 留给 while
						break;
					}
				}
				// topStack = m_blockStack.top();
			}

			String leftStyle = new String("");
			if (!mBNewLine)
				leftStyle = "\n";
			if (mBEmptyBracket) {
				leftStyle = "";
				strRight += "\n";
				mBEmptyBracket = false;
			}

			if ((!bHaveNewLine && !mTokenB.code.equals(";") && !mTokenB.code
					.equals(","))
					&& (mBNLBracket || !((topStack == JS_DO && mTokenB.code
							.equals("while"))
							|| (topStack == JS_IF && mTokenB.code
									.equals("else"))
							|| (topStack == JS_TRY && mTokenB.code
									.equals("catch")) || mTokenB.code
								.equals(")"))))
				putToken(mTokenA.code.toString(), leftStyle, (strRight += "\n")); // 一些情况换行
			else if (mTokenB.type == STRING_TYPE
					|| mTokenB.type == COMMENT_TYPE_1)
				putToken(mTokenA.code.toString(), leftStyle, strRight); // 为
																		// else
																		// 准备的空格
			else
				putToken(mTokenA.code.toString(), leftStyle); // }, }; })
			// 注意 ) 不要在输出时仿照 ,; 取消前面的换行

			// char tmpTopStack;
			// GetStackTop(m_blockStack, tmpTopStack);
			if (topStack != JS_ASSIGN && isStackTopEq(mBlockStack, JS_BRACKET))
				++mNIndents;

			popMultiBlock(topStack);

			return; // }
		}

		if (mTokenA.code.equals("++") || mTokenA.code.equals("--")
				|| mTokenA.code.equals("\n") || mTokenA.code.equals("\r\n")) {
			putToken(mTokenA.code.toString());
			return;
		}

		if (mTokenA.code.equals(":") && isStackTopEq(mBlockStack, JS_CASE)) // m_blockStack.top()
		// ==
		// JS_CASE
		{
			// case, default
			if (!bHaveNewLine)
				putToken(mTokenA.code.toString(), "", (strRight += "\n"));
			else
				putToken(mTokenA.code.toString(), "", strRight);
			mBlockStack.pop();
			return;
		}

		if (mTokenA.code.equals("::") || mTokenA.code.equals("->")) {
			putToken(mTokenA.code.toString());
			return;
		}

		if (isStackTopEq(mBlockStack, JS_ASSIGN)) // m_blockStack.top() ==
													// JS_ASSIGN
			mBAssign = true;

		if (mTokenA.code.equals("=") && !isStackTopEq(mBlockStack, JS_ASSIGN)) // m_blockStack.top()
																				// !=
																				// JS_ASSIGN)
		{
			mBlockStack.push(mBlockMap.get(mTokenA.code.toString()));
			++mNIndents;
			mBAssign = false;
		}

		putToken(mTokenA.code.toString(), " ", " "); // 剩余的操作符都是 空格oper空格
	}

	private void processString(boolean bHaveNewLine, char tokenAFirst,
			char tokenBFirst) throws IOException {
		if (mTokenA.code.equals("case") || mTokenA.code.equals("default")) {
			// case, default 往里面缩一格
			--mNIndents;
			String rightDeco = !mTokenA.code.equals("default") ? new String(" ")
					: new String();
			putToken(mTokenA.code.toString(), "", rightDeco);
			++mNIndents;
			mBlockStack.push(mBlockMap.get(mTokenA.code.toString()));
			return;
		}

		if (mTokenA.code.equals("do")
				|| (mTokenA.code.equals("else") && !mTokenB.code.equals("if"))
				|| mTokenA.code.equals("try") || mTokenA.code.equals("finally")) {
			// do, else (NOT else if), try
			putToken(mTokenA.code.toString());

			mBlockStack.push(mBlockMap.get(mTokenA.code.toString()));
			++mNIndents; // 无需 ()，直接缩进
			mBBlockStmt = false; // 等待 block 内部的 statment

			putString(" ");
			if ((mTokenB.type == STRING_TYPE || mBNLBracket) && !bHaveNewLine)
				putString("\n");

			return;
		}

		if (mTokenA.code.equals("function")) {
			if (isStackTopEq(mBlockStack, JS_ASSIGN)) // m_blockStack.top() ==
														// JS_ASSIGN)
			{
				--mNIndents;
				mBlockStack.pop();
			}
			mBlockStack.push(mBlockMap.get(mTokenA.code.toString())); // 把
																		// function
																		// 也压入栈，遇到
			// } 弹掉
		}

		if (isStackTopEq(mBlockStack, JS_ASSIGN)) // m_blockStack.top() ==
													// JS_ASSIGN
			mBAssign = true;

		if (mTokenB.type == STRING_TYPE || mTokenB.type == COMMENT_TYPE_1
				|| mTokenB.type == COMMENT_TYPE_2 || mTokenB.code.equals("{")) {
			putToken(mTokenA.code.toString(), "", " ");

			// if(m_blockStack.top() != 't' && IsType(m_tokenA))
			// m_blockStack.push('t'); // 声明变量
			return;
		}

		if (mSpecKeywordSet.contains(mTokenA.code.toString())
				&& !mTokenB.code.equals(";"))
			putToken(mTokenA.code.toString(), "", " ");
		else
			putToken(mTokenA.code.toString());

		if (mTokenA.code.equals("if") || mTokenA.code.equals("for")
				|| mTokenA.code.equals("while") || mTokenA.code.equals("catch")) {
			// 等待 ()，() 到来后才能加缩进
			mBrcNeedStack.push(false);
			mBlockStack.push(mBlockMap.get(mTokenA.code.toString()));

		}

		if (mTokenA.code.equals("switch")) {
			// bBracket = false;
			mBrcNeedStack.push(false);
			mBlockStack.push(mBlockMap.get(mTokenA.code.toString()));
		}
	}
}
