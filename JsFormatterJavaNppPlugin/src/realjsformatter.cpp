/* realjsformatter.cpp
   2010-12-16

Copyright (c) 2010-2013 SUN Junwen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <cstdlib>
#include <string>
#include <cstring>
#include <iostream>
#include <ctime>

#include "realjsformatter.h"

using namespace std;

RealJSFormatter::RealJSFormatter():
	m_chIndent('\t'),
	m_nChPerInd(1),
	m_bSkipCR(false),
	m_bPutCR(false),
	m_bNLBracket(false),
	m_bIndentInEmpty(false)
{
	Init();
}

RealJSFormatter::RealJSFormatter(char chIndent, int nChPerInd):
	m_chIndent(chIndent),
	m_nChPerInd(nChPerInd),
	m_bSkipCR(false),
	m_bPutCR(false),
	m_bNLBracket(false),
	m_bIndentInEmpty(false)
{
	Init();
}

RealJSFormatter::RealJSFormatter(bool bSkipCR, bool bPutCR):
	m_chIndent('\t'),
	m_nChPerInd(1),
	m_bSkipCR(bSkipCR),
	m_bPutCR(bPutCR),
	m_bNLBracket(false),
	m_bIndentInEmpty(false)
{
	Init();
}

RealJSFormatter::RealJSFormatter(char chIndent, int nChPerInd, 
								 bool bSkipCR, bool bPutCR, 
								 bool bNLBracket, bool bIndentInEmpty):
	m_chIndent(chIndent),
	m_nChPerInd(nChPerInd),
	m_bSkipCR(bSkipCR),
	m_bPutCR(bPutCR),
	m_bNLBracket(bNLBracket),
	m_bIndentInEmpty(bIndentInEmpty)
{
	Init();
}

string RealJSFormatter::Trim(const string& str)
{
	std::string ret(str);
	ret = ret.erase(ret.find_last_not_of(" \r\n\t") + 1);
	return ret.erase(0, ret.find_first_not_of(" \r\n\t"));
}

string RealJSFormatter::TrimSpace(const string& str)
{
	std::string ret(str);
	ret = ret.erase(ret.find_last_not_of(" \t") + 1);
	return ret.erase(0, ret.find_first_not_of(" \t"));
}

string RealJSFormatter::TrimRightSpace(const string& str)
{
	std::string ret(str);
	return ret.erase(ret.find_last_not_of(" \t") + 1);
}

void RealJSFormatter::StringReplace(string& strBase, const string& strSrc, const string& strDes)
{
	string::size_type pos = 0;
	string::size_type srcLen = strSrc.size();
	string::size_type desLen = strDes.size();
	pos = strBase.find(strSrc, pos);
	while((pos != string::npos))
	{
		strBase.replace(pos, srcLen, strDes);
		pos = strBase.find(strSrc, pos + desLen);
	}
}

void RealJSFormatter::Init()
{
	m_initIndent = "";

	m_debugOutput = false;
	m_tokenCount = 0;

	m_lineBuffer = "";

	m_nIndents = 0;
	m_nLineIndents = 0;
	m_bNewLine = false;
	m_bBlockStmt = true;
	m_bAssign = false;
	m_bEmptyBracket = false;
	m_bCommentPut = false;

	m_blockMap[string("if")] = JS_IF;
	m_blockMap[string("else")] = JS_ELSE;
	m_blockMap[string("for")] = JS_FOR;
	m_blockMap[string("do")] = JS_DO;
	m_blockMap[string("while")] = JS_WHILE;
	m_blockMap[string("switch")] = JS_SWITCH;
	m_blockMap[string("case")] = JS_CASE;
	m_blockMap[string("default")] = JS_CASE;
	m_blockMap[string("try")] = JS_TRY;
	m_blockMap[string("finally")] = JS_TRY; // ��ͬ�� try
	m_blockMap[string("catch")] = JS_CATCH;
	m_blockMap[string("=")] = JS_ASSIGN;
	m_blockMap[string("function")] = JS_FUNCTION;
	m_blockMap[string("{")] = JS_BLOCK;
	m_blockMap[string("(")] = JS_BRACKET;
	m_blockMap[string("[")] = JS_SQUARE;

	m_specKeywordSet.insert("if");
	m_specKeywordSet.insert("for");
	m_specKeywordSet.insert("while");
	m_specKeywordSet.insert("switch");
	m_specKeywordSet.insert("catch");
	m_specKeywordSet.insert("function");
	m_specKeywordSet.insert("with");
	m_specKeywordSet.insert("return");
}

void RealJSFormatter::PutToken(const string& token,
		const string& leftStyle,
		const string& rightStyle)
{
	// debug
	/*size_t length = token.size();
	for(size_t i = 0; i < length; ++i)
		PutChar(token[i]);
	PutChar('\n');*/
	// debug
	PutString(leftStyle);
	PutString(token);
	PutString(rightStyle);
	if(!(m_bCommentPut && m_bNewLine))
		m_bCommentPut = false; // ���һ���ᷢ����ע��֮����κ��������
}

void RealJSFormatter::PutString(const string& str)
{
	size_t length = str.size();
	//char topStack = m_blockStack.top();
	for(size_t i = 0; i < length; ++i)
	{
		if(m_bNewLine && (m_bCommentPut ||
			((m_bNLBracket || str[i] != '{') && str[i] != ',' && str[i] != ';')))
		{
			// ���к��治�ǽ����� {,; ��������
			PutLineBuffer(); // ����л���

			m_lineBuffer = "";
			m_bNewLine = false;
			m_nIndents = m_nIndents < 0 ? 0 : m_nIndents; // ��������
			m_nLineIndents = m_nIndents;
			if(str[i] == '{' || str[i] == ',' || str[i] == ';') // �н�β��ע�ͣ�ʹ��{,;���ò�����
				--m_nLineIndents;
		}

		if(m_bNewLine && !m_bCommentPut &&
			((!m_bNLBracket && str[i] == '{') || str[i] == ',' || str[i] == ';'))
			m_bNewLine = false;

		if(str[i] == '\n')
			m_bNewLine = true;
		else
			m_lineBuffer += str[i];
	}
}

void RealJSFormatter::PutLineBuffer()
{
	string line;
	line.append(TrimRightSpace(m_lineBuffer));
	
	if(line != "" || m_bIndentInEmpty) // Fix "JSLint unexpect space" bug
	{
		for(size_t i = 0; i < m_initIndent.length(); ++i)
			PutChar(m_initIndent[i]); // �����Ԥ����

		for(int c = 0; c < m_nLineIndents; ++c)
			for(int c2 = 0; c2 < m_nChPerInd; ++c2)
				PutChar(m_chIndent); // �������
	}
	
	// ���ϻ���
	if(m_bPutCR)
		line.append("\r"); //PutChar('\r');
	line.append("\n"); //PutChar('\n');

	// ��� line
	for(size_t i = 0; i < line.length(); ++i)
		PutChar(line[i]);
}

void RealJSFormatter::PopMultiBlock(char previousStackTop)
{
	if(m_tokenB.code == ";") // ��� m_tokenB �� ;����������������������
		return;

	if(!((previousStackTop == JS_IF && m_tokenB.code == "else") ||
		(previousStackTop == JS_DO && m_tokenB.code == "while") ||
		(previousStackTop == JS_TRY && m_tokenB.code == "catch")))
	{
		char topStack;// = m_blockStack.top();
		if(!GetStackTop(m_blockStack, topStack))
			return;
		// ; �����ܿ��ܽ������ if, do, while, for, try, catch
		while(topStack == JS_IF || topStack == JS_FOR || topStack == JS_WHILE ||
			topStack == JS_DO || topStack == JS_ELSE || topStack == JS_TRY || topStack == JS_CATCH)
		{
			if(topStack == JS_IF || topStack == JS_FOR ||
				topStack == JS_WHILE || topStack == JS_CATCH ||
				topStack == JS_ELSE || topStack == JS_TRY)
			{
				m_blockStack.pop();
				--m_nIndents;
			}
			else if(topStack == JS_DO)
			{
				--m_nIndents;
			}

			if((topStack == JS_IF && m_tokenB.code == "else") ||
				(topStack == JS_DO && m_tokenB.code == "while") ||
				(topStack == JS_TRY && m_tokenB.code == "catch"))
				break; // ֱ���ոս���һ�� if...else, do...while, try...catch
			//topStack = m_blockStack.top();
			if(!GetStackTop(m_blockStack, topStack))
				break;
		}
	}
}

void RealJSFormatter::Go()
{
	m_blockStack.push(' ');
	m_brcNeedStack.push(true);

	bool bHaveNewLine;
	char tokenAFirst;
	char tokenBFirst;

	m_startClock = clock();

	while(GetToken())
	{
		bHaveNewLine = false; // bHaveNewLine ��ʾ���潫Ҫ���У�m_bNewLine ��ʾ�Ѿ�������
		tokenAFirst = m_tokenA.code[0];
		tokenBFirst = m_tokenB.code.size() ? m_tokenB.code[0] : 0;
		if(tokenBFirst == '\r')
			tokenBFirst = '\n';
		if(tokenBFirst == '\n' || m_tokenB.type == COMMENT_TYPE_1)
			bHaveNewLine = true;

		if(!m_bBlockStmt && m_tokenA.code != "{" && m_tokenA.code != "\n"
			&& m_tokenA.type != COMMENT_TYPE_1 && m_tokenA.type != COMMENT_TYPE_2)
			m_bBlockStmt = true;

		/*
		 * �ο� m_tokenB ������ m_tokenA
		 * �������� m_tokenA
		 * ��һ��ѭ��ʱ�Զ����� m_tokenB ���� m_tokenA
		 */
		//PutToken(m_tokenA);
		switch(m_tokenA.type)
		{
		case REGULAR_TYPE:
			PutToken(m_tokenA.code); // ������ʽֱ�������ǰ��û���κ���ʽ
			break;
		case COMMENT_TYPE_1:
		case COMMENT_TYPE_2:
			if(m_tokenA.code[1] == '*')
			{
				// ����ע��
				if(!bHaveNewLine)
					PutToken(m_tokenA.code, string(""), string("\n")); // ��Ҫ����
				else
					PutToken(m_tokenA.code);
			}
			else
			{
				// ����ע��
				PutToken(m_tokenA.code); // �϶��ỻ�е�
			}
			m_bCommentPut = true;
			break;
		case OPER_TYPE:
			ProcessOper(bHaveNewLine, tokenAFirst, tokenBFirst);

			break;
		case STRING_TYPE:
			ProcessString(bHaveNewLine, tokenAFirst, tokenBFirst);
			break;
		}
	}

	m_lineBuffer = Trim(m_lineBuffer);
	if(m_lineBuffer.length())
		PutLineBuffer();

	m_endClock = clock();
	m_duration = (double)(m_endClock - m_startClock) / CLOCKS_PER_SEC;
	if(m_debugOutput)
	{
		cout << "Processed tokens: " << m_tokenCount << endl;
		cout << "Time used: " << m_duration << "s" << endl;
		cout << m_tokenCount / m_duration << " tokens/second" << endl;
	}
}

void RealJSFormatter::ProcessOper(bool bHaveNewLine, char tokenAFirst, char tokenBFirst)
{
	char topStack;// = m_blockStack.top();
	GetStackTop(m_blockStack, topStack);
	string strRight(" ");

	if(m_tokenA.code == "(" || m_tokenA.code == ")" ||
		m_tokenA.code == "[" || m_tokenA.code == "]" ||
		m_tokenA.code == "!" || m_tokenA.code == "!!" ||
		m_tokenA.code == "~" || m_tokenA.code == "^" ||
		m_tokenA.code == ".")
	{
		// ()[]!. ����ǰ��û����ʽ�������
		if((m_tokenA.code == ")" || m_tokenA.code == "]") &&
			(topStack == JS_ASSIGN || topStack == JS_HELPER))
		{
			if(topStack == JS_ASSIGN)
				--m_nIndents;
			m_blockStack.pop();
		}
		GetStackTop(m_blockStack, topStack);
		if((m_tokenA.code == ")" && topStack == JS_BRACKET) ||
			(m_tokenA.code == "]" && topStack == JS_SQUARE))
		{
			// )] ��Ҫ��ջ����������
			m_blockStack.pop();
			--m_nIndents;
			GetStackTop(m_blockStack, topStack);
			//topStack = m_blockStack.top();
			if(topStack == JS_ASSIGN || topStack == JS_HELPER)
				m_blockStack.pop();
		}

		GetStackTop(m_blockStack, topStack);
		if(m_tokenA.code == ")" && !m_brcNeedStack.top() &&
			(topStack == JS_IF || topStack == JS_FOR || topStack == JS_WHILE ||
			topStack == JS_SWITCH || topStack == JS_CATCH))
		{
			// ջ���� if, for, while, switch, catch ���ڵȴ� )��֮������������
			// ����Ŀո������Ŀո������� { �ģ�m_bNLBracket Ϊ true ����Ҫ�ո���
			string rightDeco = m_tokenB.code != ";" ? strRight : "";
			if(!bHaveNewLine)
				rightDeco.append("\n");
			PutToken(m_tokenA.code, string(""), rightDeco);
			//bBracket = true;
			m_brcNeedStack.pop();
			m_bBlockStmt = false; // �ȴ� statment
			if(StackTopEq(m_blockStack, JS_WHILE)) //m_blockStack.top() == JS_WHILE
			{
				m_blockStack.pop();
				if(StackTopEq(m_blockStack, JS_DO)) // m_blockStack.top() == JS_DO
				{
					// ���� do...while
					m_blockStack.pop();

					PopMultiBlock(JS_WHILE);
				}
				else
				{
					m_blockStack.push(JS_WHILE);
					++m_nIndents;
				}
			}
			else
				++m_nIndents;
		}
		else if(m_tokenA.code == ")" && (m_tokenB.code == "{" || bHaveNewLine))
			PutToken(m_tokenA.code, string(""), strRight); // { ���߻���֮ǰ�����ո�
		else
			PutToken(m_tokenA.code); // �������

		if(m_tokenA.code == "(" || m_tokenA.code == "[")
		{
			// ([ ��ջ����������
			GetStackTop(m_blockStack, topStack);
			//topStack = m_blockStack.top();
			if(topStack == JS_ASSIGN)
			{
				if(!m_bAssign)
					--m_nIndents;
				else
					m_blockStack.push(JS_HELPER);
			}
			m_blockStack.push(m_blockMap[m_tokenA.code]);
			++m_nIndents;
		}

		return;
	}

	if(m_tokenA.code == ";")
	{
		GetStackTop(m_blockStack, topStack);
		//topStack = m_blockStack.top();
		if(topStack == JS_ASSIGN)
		{
			--m_nIndents;
			m_blockStack.pop();
		}

		GetStackTop(m_blockStack, topStack);
		//topStack = m_blockStack.top();

		// ; ���� if, else, while, for, try, catch
		if(topStack == JS_IF || topStack == JS_FOR ||
			topStack == JS_WHILE || topStack == JS_CATCH)
		{
			m_blockStack.pop();
			--m_nIndents;
			// ����� } ��ͬ���Ĵ���
			PopMultiBlock(topStack);
		}
		if(topStack == JS_ELSE || topStack == JS_TRY)
		{
			m_blockStack.pop();
			--m_nIndents;
			PopMultiBlock(topStack);
		}
		if(topStack == JS_DO)
		{
			--m_nIndents;
			PopMultiBlock(topStack);
		}
		// do �ڶ�ȡ�� while ����޸ļ���
		// ���� do{} Ҳһ��

		//if(m_blockStack.top() == 't')
			//m_blockStack.pop(); // ; Ҳ����������������ʱ��ѹ�� t

		GetStackTop(m_blockStack, topStack);
		//topStack = m_blockStack.top();
		if(topStack != JS_BRACKET && !bHaveNewLine)
			PutToken(m_tokenA.code, string(""), strRight.append("\n")); // ������� () ��� ; �ͻ���
		else if(topStack == JS_BRACKET || m_tokenB.type == COMMENT_TYPE_1)
			PutToken(m_tokenA.code, string(""), strRight); // (; ) �ո�
		else
			PutToken(m_tokenA.code);

		return; // ;
	}

	if(m_tokenA.code == ",")
	{
		if(StackTopEq(m_blockStack, JS_ASSIGN)) // m_blockStack.top() == JS_ASSIGN
		{
			--m_nIndents;
			m_blockStack.pop();
		}
		if(StackTopEq(m_blockStack, JS_BLOCK) && !bHaveNewLine)
			PutToken(m_tokenA.code, string(""), strRight.append("\n")); // ����� {} ���
		else
			PutToken(m_tokenA.code, string(""), strRight);

		return; // ,
	}

	if(m_tokenA.code == "{")
	{
		GetStackTop(m_blockStack, topStack);
		if(topStack == JS_IF || topStack == JS_FOR ||
			topStack == JS_WHILE || topStack == JS_DO ||
			topStack == JS_ELSE || topStack == JS_SWITCH ||
			topStack == JS_TRY || topStack == JS_CATCH ||
			topStack == JS_ASSIGN)
		{
			if(!m_bBlockStmt || topStack == JS_ASSIGN)//(topStack == JS_ASSIGN && !m_bAssign))
			{
				//m_blockStack.pop(); // �����Ǹ������������� } ʱһ��
				--m_nIndents;
				m_bBlockStmt = true;
			}
			else
			{
				m_blockStack.push(JS_HELPER); // ѹ��һ�� JS_HELPER ͳһ״̬
			}
		}

		// ����({...}) �ж�һ������
		bool bPrevFunc = (topStack == JS_FUNCTION);
		char fixTopStack = topStack;
		if(bPrevFunc)
		{
			m_blockStack.pop(); // ���� JS_FUNCTION
			GetStackTop(m_blockStack, fixTopStack);
		}

		if(fixTopStack == JS_BRACKET)
		{
			--m_nIndents; // ({ ����һ������
		}

		if(bPrevFunc)
		{
			m_blockStack.push(JS_FUNCTION); // ѹ�� JS_FUNCTION
		}

		m_blockStack.push(m_blockMap[m_tokenA.code]); // ��ջ����������
		++m_nIndents;

		/*
		 * { ֮��Ŀո�����֮ǰ�ķ���׼���õ�
		 * ����Ϊ�˽�� { ������ʱ��ǰ����һ���ո������
		 * ��Ϊ�㷨ֻ����󣬲�����ǰ��
		 */
		if(m_tokenB.code == "}")
		{
			// �� {}
			m_bEmptyBracket = true;
			if(m_bNewLine == false && m_bNLBracket &&
				(topStack == JS_IF || topStack == JS_FOR ||
				topStack == JS_WHILE || topStack == JS_SWITCH ||
				topStack == JS_CATCH || topStack == JS_FUNCTION))
			{
				PutToken(m_tokenA.code, string(" ")); // ��Щ����£�ǰ�油һ���ո�
			}
			else
			{
				PutToken(m_tokenA.code);
			}
		}
		else
		{
			string strLeft = (m_bNLBracket && !m_bNewLine) ? string("\n") : string("");	
			if(!bHaveNewLine) // ��Ҫ����
				PutToken(m_tokenA.code, strLeft, strRight.append("\n"));
			else
				PutToken(m_tokenA.code, strLeft, strRight);
		}

		return; // {
	}

	if(m_tokenA.code == "}")
	{
		//topStack = m_blockStack.top();

		// �����Ĳ��ԣ�} һֱ���� {
		// ���������ٿ���ʹ�� {} ֮������ȷ��
		while(GetStackTop(m_blockStack, topStack))
		{
			if(topStack == JS_BLOCK)
				break;

			m_blockStack.pop();

			switch(topStack)
			{
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
				--m_nIndents;
				break;
			}

			/*if(!GetStackTop(m_blockStack, topStack))
				break;*/
			//topStack = m_blockStack.top();
		}

		if(topStack == JS_BLOCK)
		{
			// ��ջ����С����
			m_blockStack.pop();
			--m_nIndents;
			bool bGetTop = GetStackTop(m_blockStack, topStack);// = m_blockStack.top();

			if(bGetTop)
			{
				switch(topStack)
				{
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
					m_blockStack.pop();
					break;
				case JS_DO:
					// �����Ѿ�����do ���� while
					break;
				}
			}
			//topStack = m_blockStack.top();
		}

		string leftStyle("");
		if(!m_bNewLine)
			leftStyle = "\n";
		if(m_bEmptyBracket)
		{
			leftStyle = "";
			strRight.append("\n");
			m_bEmptyBracket = false;
		}

		if((!bHaveNewLine && m_tokenB.code != ";" && m_tokenB.code != ",")
			&& (m_bNLBracket || !((topStack == JS_DO && m_tokenB.code == "while") ||
			(topStack == JS_IF && m_tokenB.code == "else") ||
			(topStack == JS_TRY && m_tokenB.code == "catch") ||
			m_tokenB.code == ")")))
			PutToken(m_tokenA.code, leftStyle, strRight.append("\n")); // һЩ�������
		else if(m_tokenB.type == STRING_TYPE || m_tokenB.type == COMMENT_TYPE_1)
			PutToken(m_tokenA.code, leftStyle, strRight); // Ϊ else ׼���Ŀո�
		else
			PutToken(m_tokenA.code, leftStyle); // }, }; })
		// ע�� ) ��Ҫ�����ʱ���� ,; ȡ��ǰ��Ļ���

		//char tmpTopStack;
		//GetStackTop(m_blockStack, tmpTopStack);
		if(topStack != JS_ASSIGN && StackTopEq(m_blockStack, JS_BRACKET))
			++m_nIndents;

		PopMultiBlock(topStack);

		return; // }
	}

	if(m_tokenA.code == "++" || m_tokenA.code == "--" ||
		m_tokenA.code == "\n" || m_tokenA.code == "\r\n")
	{
		PutToken(m_tokenA.code);
		return;
	}

	if(m_tokenA.code == ":" && StackTopEq(m_blockStack, JS_CASE)) //m_blockStack.top() == JS_CASE
	{
		// case, default
		if(!bHaveNewLine)
			PutToken(m_tokenA.code, string(""), strRight.append("\n"));
		else
			PutToken(m_tokenA.code, string(""), strRight);
		m_blockStack.pop();
		return;
	}

	if(m_tokenA.code == "::" || m_tokenA.code == "->")
	{
		PutToken(m_tokenA.code);
		return;
	}

	if(StackTopEq(m_blockStack, JS_ASSIGN)) // m_blockStack.top() == JS_ASSIGN
		m_bAssign = true;

	if(m_tokenA.code == "=" && !StackTopEq(m_blockStack, JS_ASSIGN)) // m_blockStack.top() != JS_ASSIGN)
	{
		m_blockStack.push(m_blockMap[m_tokenA.code]);
		++m_nIndents;
		m_bAssign = false;
	}

	PutToken(m_tokenA.code, string(" "), string(" ")); // ʣ��Ĳ��������� �ո�oper�ո�
}

void RealJSFormatter::ProcessString(bool bHaveNewLine, char tokenAFirst, char tokenBFirst)
{
	if(m_tokenA.code == "case" || m_tokenA.code == "default")
	{
		// case, default ��������һ��
		--m_nIndents;
		string rightDeco = m_tokenA.code != "default" ? string(" ") : string();
		PutToken(m_tokenA.code, string(""), rightDeco);
		++m_nIndents;
		m_blockStack.push(m_blockMap[m_tokenA.code]);
		return;
	}

	if(m_tokenA.code == "do" ||
		(m_tokenA.code == "else" && m_tokenB.code != "if") ||
		m_tokenA.code == "try" || m_tokenA.code == "finally")
	{
		// do, else (NOT else if), try
		PutToken(m_tokenA.code);

		m_blockStack.push(m_blockMap[m_tokenA.code]);
		++m_nIndents; // ���� ()��ֱ������
		m_bBlockStmt = false; // �ȴ� block �ڲ��� statment

		PutString(string(" "));
		if((m_tokenB.type == STRING_TYPE || m_bNLBracket) && !bHaveNewLine)
			PutString(string("\n"));

		return;
	}

	if(m_tokenA.code == "function")
	{
		if(StackTopEq(m_blockStack, JS_ASSIGN)) // m_blockStack.top() == JS_ASSIGN)
		{
			--m_nIndents;
			m_blockStack.pop();
		}
		m_blockStack.push(m_blockMap[m_tokenA.code]); // �� function Ҳѹ��ջ������ } ����
	}

	if(StackTopEq(m_blockStack, JS_ASSIGN)) //m_blockStack.top() == JS_ASSIGN
		m_bAssign = true;

	if(m_tokenB.type == STRING_TYPE || 
		m_tokenB.type == COMMENT_TYPE_1 ||
		m_tokenB.type == COMMENT_TYPE_2 ||
		m_tokenB.code == "{")
	{
		PutToken(m_tokenA.code, string(""), string(" "));

		//if(m_blockStack.top() != 't' && IsType(m_tokenA))
			//m_blockStack.push('t'); // ��������
		return;
	}

	if(m_specKeywordSet.find(m_tokenA.code) != m_specKeywordSet.end() &&
		m_tokenB.code != ";")
		PutToken(m_tokenA.code, string(""), string(" "));
	else
		PutToken(m_tokenA.code);

	if(m_tokenA.code == "if" || m_tokenA.code == "for" ||
		m_tokenA.code == "while" || m_tokenA.code == "catch")
	{
		// �ȴ� ()��() ��������ܼ�����
		m_brcNeedStack.push(false);
		m_blockStack.push(m_blockMap[m_tokenA.code]);

	}

	if(m_tokenA.code == "switch")
	{
		//bBracket = false;
		m_brcNeedStack.push(false);
		m_blockStack.push(m_blockMap[m_tokenA.code]);
	}
}

