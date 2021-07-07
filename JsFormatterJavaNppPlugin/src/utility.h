#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <string>
#include "strhelper.h"

using namespace std;
using namespace sunjwbase;

struct StruOptions
{
	bool bPutCR; // �Ƿ���� \r\n
	char chIndent; // �����õ��ַ�
	int nChPerInd; // ÿ������ʹ�ü����ַ�
	bool bNLBracket; // { ֮ǰ�Ƿ���
	bool bKeepTopComt; // �Ƿ���ͷ����ע��
	bool bIndentInEmpty; // �Ƿ��������е�����
};

const string keyPutCR("Put CR");
const string keyChIndent("Indent char");
const string keyChPerInd("Chars per indent");
const string keyNLBracket("New line before {");
const string keyKeepTopComt("Kepp top comment");
const string keyIndentInEmpty("Indent in empty");

tstring GetConfigFilePath(HWND nppHandle);

void loadOption(HWND nppHandle, StruOptions& struOptions);
void loadDefaultOption(StruOptions& struOptions);

void saveOption(HWND nppHandle, StruOptions struOptions);

#endif
