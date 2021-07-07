/* jsparser.cpp
   2012-3-11
   Version: 0.9.8

Copyright (c) 2012- SUN Junwen

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
#include "jsparser.h"

using namespace std;

JSParser::JSParser()
{
	Init();
}

void JSParser::Init()
{
	m_lineCount = 1; // �кŴ� 1 ��ʼ
	m_tokenCount = 0;

	m_strBeforeReg = "(,=:[!&|?+{};\n";

	m_bRegular = false;
	m_bPosNeg = false;

	m_bGetTokenInit = false;
}

bool JSParser::IsNormalChar(int ch)
{
	// һ���ַ�
	return ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') ||
		(ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '$' ||
		ch > 126 || ch < 0);
}

bool JSParser::IsNumChar(int ch)
{
	// ���ֺ�.
	return ((ch >= '0' && ch <= '9') || ch == '.');
}

bool JSParser::IsBlankChar(int ch)
{
	// �հ��ַ�
	return (ch == ' ' || ch == '\t' || ch == '\r');
}

bool JSParser::IsSingleOper(int ch)
{
	// ���ַ�����
	return (ch == '.' || ch == '(' || ch == ')' ||
		ch == '[' || ch == ']' || ch == '{' || ch == '}' ||
		ch == ',' || ch == ';' || ch == '~' ||
		ch == '\n');
}

bool JSParser::IsQuote(int ch)
{
	// ����
	return (ch == '\'' || ch == '\"');
}

bool JSParser::IsComment()
{
	// ע��
	return (m_charA == '/' && (m_charB == '/' || m_charB == '*'));
}

void JSParser::GetTokenRaw()
{
	if(!m_bGetTokenInit)
	{
		m_charB = GetChar();
	}

	// normal procedure
	if(!m_bRegular && !m_bPosNeg)
	{
		m_tokenB.code = "";
		m_tokenB.type = STRING_TYPE;
		m_tokenB.line = m_lineCount;
	}
	else if(m_bRegular)
	{
		//m_tokenB.push_back('/');
		m_tokenB.type = REGULAR_TYPE; // ����
	}
	else
	{
		m_tokenB.type = STRING_TYPE; // ������
	}

	bool bQuote = false;
	bool bComment = false;
	bool bRegularFlags = false;
	bool bFirst = true;
	bool bNum = false; // �ǲ�������
	bool bLineBegin = false;
	char chQuote; // ��¼�������� ' �� "
	char chComment; // ע������ / �� *

	while(1)
	{
		m_charA = m_charB;
		if(m_charA == 0)
			return;
	
		if(m_charA == '\n')
			++m_lineCount;

		do
		{
			m_charB = GetChar();
		} while(m_charB == '\r');

		/*
		 * �ο� m_charB ������ m_charA
		 * �������� m_charA
		 * ��һ��ѭ��ʱ�Զ����� m_charB ���� m_charA
		 */

		// ������Ҫ�� token �����ж�
		if(m_bRegular)
		{
			// ����״̬ȫ�������ֱ�� /
			m_tokenB.code.push_back(m_charA);

			if(m_charA == '\\' && (m_charB == '/' || m_charB == '\\')) // ת���ַ�
			{
				m_tokenB.code.push_back(m_charB);
				m_charB = GetChar();
			}

			if(m_charA == '/') // ������ܽ���
			{
				if(!bRegularFlags && IsNormalChar(m_charB))
				{
					// ����� flags ����
					bRegularFlags = true;
				}
				else
				{
					// �������
					m_bRegular = false;
					return;
				}
			}

			if(bRegularFlags && !IsNormalChar(m_charB))
			{
				// �������
				bRegularFlags = false;
				m_bRegular = false;
				return;
			}

			continue;
		}

		if(bQuote)
		{
			// ����״̬��ȫ�������ֱ�����Ž���
			m_tokenB.code.push_back(m_charA);

			if(m_charA == '\\' && (m_charB == chQuote || m_charB == '\\')) // ת���ַ�
			{
				m_tokenB.code.push_back(m_charB);
				m_charB = GetChar();
			}

			if(m_charA == chQuote) // ���Ž���
				return;

			continue;
		}

		if(bComment)
		{
			// ע��״̬��ȫ�����
			if(m_tokenB.type == COMMENT_TYPE_2)
			{
				// /*...*/ÿ��ǰ���\t, ' '��Ҫɾ��
				if(bLineBegin && (m_charA == '\t' || m_charA == ' '))
					continue;
				else if(bLineBegin && m_charA == '*')
					m_tokenB.code.push_back(' ');

				bLineBegin = false;

				if(m_charA == '\n')
					bLineBegin = true;
			}
			m_tokenB.code.push_back(m_charA);

			if(chComment == '*')
			{
				// ֱ�� */
				m_tokenB.type = COMMENT_TYPE_2;
				if(m_charA == '*' && m_charB == '/')
				{
					m_tokenB.code.push_back(m_charB);
					m_charB = GetChar();
					return;
				}
			}
			else
			{
				// ֱ������
				m_tokenB.type = COMMENT_TYPE_1;
				if(m_charA == '\n')
					return;
			}

			continue;
		}

		if(IsNormalChar(m_charA))
		{
			m_tokenB.type = STRING_TYPE;
			m_tokenB.code.push_back(m_charA);

			// ������� 82e-2, 442e+6, 555E-6 ������
			// ��Ϊ����������������ֻ�ܷ������µı����ʽ
			bool bNumOld = bNum;
			if(bFirst || bNumOld) // ֻ��֮ǰ�����ֲŸı�״̬
			{
				bNum = IsNumChar(m_charA);
				bFirst = false;
			}
			if(bNumOld && !bNum && (m_charA == 'e' || m_charA == 'E') &&
				(m_charB == '-' || m_charB == '+' || IsNumChar(m_charB)))
			{
				bNum = true;
				if(m_charB == '-' || m_charB == '+')
				{
					m_tokenB.code.push_back(m_charB);
					m_charB = GetChar();
				}
			}

			if(!IsNormalChar(m_charB)) // loop until m_charB is not normal char
			{
				m_bPosNeg = false;
				return;
			}
		}
		else
		{
			if(IsBlankChar(m_charA))
				continue; // ���Կհ��ַ�

			if(IsQuote(m_charA))
			{
				// ����
				bQuote= true;
				chQuote = m_charA;

				m_tokenB.type = STRING_TYPE;
				m_tokenB.code.push_back(m_charA);
				continue;
			}

			if(IsComment())
			{
				// ע��
				bComment = true;
				chComment = m_charB;

				//m_tokenBType = COMMENT_TYPE;
				m_tokenB.code.push_back(m_charA);
				continue;
			}

			if( IsSingleOper(m_charA) ||
				IsNormalChar(m_charB) || IsBlankChar(m_charB) || IsQuote(m_charB))
			{
				m_tokenB.type = OPER_TYPE;
				m_tokenB.code = m_charA; // ���ַ�����
				return;
			}

			// ���ַ�����
			if((m_charB == '=' || m_charB == m_charA) || 
				(m_charA == '-' && m_charB == '>'))
			{
				// ��ȷ�Ƕ��ַ�����
				m_tokenB.type = OPER_TYPE;
				m_tokenB.code.push_back(m_charA);
				m_tokenB.code.push_back(m_charB);
				m_charB = GetChar();
				if((m_tokenB.code == "==" || m_tokenB.code == "!=" ||
					m_tokenB.code == "<<" || m_tokenB.code == ">>") && m_charB == '=')
				{
					// ���ַ� ===, !==, <<=, >>=
					m_tokenB.code.push_back(m_charB);
					m_charB = GetChar();
				}
				else if(m_tokenB.code == ">>" && m_charB == '>')
				{
					// >>>, >>>=
					m_tokenB.code.push_back(m_charB);
					m_charB = GetChar();
					if(m_charB == '=') // >>>=
					{
						m_tokenB.code.push_back(m_charB);
						m_charB = GetChar();
					}
				}
				return;
			}
			else
			{
				// ���ǵ��ַ���
				m_tokenB.type = OPER_TYPE;
				m_tokenB.code = m_charA; // ���ַ�����
				return;
			}

			// What? How could we come here?
			m_charA = 0;
			return;
		}
	}
}

bool JSParser::GetToken()
{
	if(!m_bGetTokenInit)
	{
		// ��һ�ζ����һ�� GetTokenRaw
		GetTokenRaw();
		m_bGetTokenInit = true;
	}

	PrepareRegular(); // �ж�����
	PreparePosNeg(); // �ж�������

	++m_tokenCount;
	m_tokenA = m_tokenB;

	if(m_tokenBQueue.size() == 0)
	{
		GetTokenRaw();
		PrepareTokenB(); // �����ǲ���Ҫ��������
	}
	else
	{
		// ���ŶӵĻ���
		m_tokenB = m_tokenBQueue.front();
		m_tokenBQueue.pop();
	}

	return (m_charA != 0 || m_tokenA.code != "");
}

void JSParser::PrepareRegular()
{
	/*
	 * �ȴ���һ������
	 * m_tokenB[0] == /���� m_tokenB ����ע��
	 * m_tokenA ���� STRING (���� m_tokenA == return)
	 * ���� m_tokenA �����һ���ַ���������Щ
	*/
	//size_t last = m_tokenA.size() > 0 ? m_tokenA.size() - 1 : 0;
	char tokenALast = m_tokenA.code.size() > 0 ? m_tokenA.code[m_tokenA.code.size() - 1] : 0;
	char tokenBFirst = m_tokenB.code[0];
	if(tokenBFirst == '/' && m_tokenB.type != COMMENT_TYPE_1 &&
		m_tokenB.type != COMMENT_TYPE_2 &&
		((m_tokenA.type != STRING_TYPE && m_strBeforeReg.find(tokenALast) != string::npos) ||
			m_tokenA.code == "return"))
	{
		m_bRegular = true;
		GetTokenRaw(); // ���������ݼӵ� m_tokenB
	}
}

void JSParser::PreparePosNeg()
{
	/*
	 * ��� m_tokenB �� -,+ ��
	 * ���� m_tokenA �����ַ�����Ҳ����������ʽ
	 * ���� m_tokenA ���� ++, --, ], )
	 * ���� m_charB ��һ�� NormalChar
	 * ��ô m_tokenB ʵ������һ��������
	 */
	if(m_tokenB.type == OPER_TYPE && (m_tokenB.code == "-" || m_tokenB.code == "+") &&
		(m_tokenA.type != STRING_TYPE || m_tokenA.code == "return") && m_tokenA.type != REGULAR_TYPE &&
		m_tokenA.code != "++" && m_tokenA.code != "--" &&
		m_tokenA.code != "]" && m_tokenA.code != ")" &&
		IsNormalChar(m_charB))
	{
		// m_tokenB ʵ������������
		m_bPosNeg = true;
		GetTokenRaw();
	}
}

void JSParser::PrepareTokenB()
{
	//char stackTop = m_blockStack.top();

	/*
	 * ���� else, while, catch, ',', ';', ')', { ֮ǰ�Ļ���
	 * ����������Ĳ��������Ǽ������ٰ�ȥ���Ļ��в���
	 */
	int c = 0;
	while(m_tokenB.code == "\n" || m_tokenB.code == "\r\n")
	{
		++c;
		GetTokenRaw();
	}

	if(m_tokenB.code != "else" && m_tokenB.code != "while" && m_tokenB.code != "catch" &&
		m_tokenB.code != "," && m_tokenB.code != ";" && m_tokenB.code != ")")
	{
		// ��ȥ���Ļ���ѹ����У��ȴ���
		if(m_tokenA.code == "{" && m_tokenB.code == "}")
			return; // �� {}

		Token temp;
		c = c > 2 ? 2 : c;
		for(; c > 0; --c)
		{
			temp.code = string("\n");
			temp.type = OPER_TYPE;
			m_tokenBQueue.push(temp);
		}
		temp = m_tokenB;
		m_tokenBQueue.push(temp);
		temp = m_tokenBQueue.front();
		m_tokenBQueue.pop();
		m_tokenB = temp;
	}
}
