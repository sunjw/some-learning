/* realjsformatter.h
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
#ifndef _REAL_JSFORMATTER_H_
#define _REAL_JSFORMATTER_H_
#include <ctime>
#include <string>
#include <stack>
#include <queue>
#include <map>
#include <set>

#include "jsparser.h"

using namespace std;

class RealJSFormatter: public JSParser
{
public:
	typedef stack<char> CharStack;
	typedef stack<bool> BoolStack;
	typedef queue<JSParser::Token> TokenQueue;
	typedef map<string, char> StrCharMap;
	typedef set<string> StrSet;

	RealJSFormatter();
	RealJSFormatter(char chIndent, int nChPerInd);
	RealJSFormatter(bool bSkipCR, bool bPutCR);
	RealJSFormatter(char chIndent, int nChPerInd, 
		bool bSkipCR, bool bPutCR, 
		bool bNLBracket, bool bIndentInEmpty);

	virtual ~RealJSFormatter()
	{}

	inline void SetInitIndent(const string& initIndent)
	{ m_initIndent = initIndent; }

	void Go();

	static string Trim(const string& str);
	static string TrimSpace(const string& str);
	static string TrimRightSpace(const string& str);
	void StringReplace(string& strBase, const string& strSrc, const string& strDes);

	bool m_debugOutput;

private:
	void Init();

	virtual void PutChar(int ch) = 0;

	void PopMultiBlock(char previousStackTop);
	void ProcessOper(bool bHaveNewLine, char tokenAFirst, char tokenBFirst);
	void ProcessString(bool bHaveNewLine, char tokenAFirst, char tokenBFirst);

	void PutToken(const string& token,
		const string& leftStyle = string(""),
		const string& rightStyle = string("")); // Put a token out with style
	void PutString(const string& str);
	void PutLineBuffer();

	clock_t m_startClock;
	clock_t m_endClock;
	double m_duration;

	int m_nLineIndents;
	string m_lineBuffer;

	StrSet m_specKeywordSet; // ����Ҫ�������ŵĹؼ��ּ���
	StrCharMap m_blockMap;
	CharStack m_blockStack;
	int m_nIndents; // �������������ü��� blockStack��Ч������

	// ʹ��ջ��Ϊ�˽�����ж������г���ѭ��������
	BoolStack m_brcNeedStack; // if ֮��ĺ��������

	bool m_bNewLine; // ׼�����еı�־
	bool m_bBlockStmt; // block ������ʼ��
	bool m_bAssign;
	bool m_bEmptyBracket; // �� {}

	bool m_bCommentPut; // �ո������ע��

	string m_initIndent; // ��ʼ����
	char m_chIndent; // ��Ϊ�������ַ�
	int m_nChPerInd; // ÿ�����������ַ�����

	bool m_bSkipCR; // ��ȡʱ���� \r
	bool m_bPutCR; // ʹ�� \r\n ��Ϊ����

	bool m_bNLBracket; // { ֮ǰ�Ƿ���
	bool m_bIndentInEmpty; // �Ƿ񱣳ֿ��е�����, JSLint ���Ƽ�

private:
	// ��ֹ����
	RealJSFormatter(const RealJSFormatter&);
	RealJSFormatter& operator=(const RealJSFormatter&);
};

#endif
