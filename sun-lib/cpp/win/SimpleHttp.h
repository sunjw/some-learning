#ifndef _HTTP_CORE_H_
#define _HTTP_CORE_H_
/*
 * HTTP �򵥲����ں�
 * ���� libcurl �� easy functions
 * �̲߳���ȫ!!!
 * Sun Junwen 2011.04.16
 */
#pragma comment(lib, "libcurl.lib")

#include <string>
#include <map>

#include "curl/curl.h"

class SimpleHttp
{
public:
	typedef std::map<std::string, std::string> MapStrStr;

	SimpleHttp(unsigned long lTimeout = 5);
	~SimpleHttp();

	/**
     * ִ�� HTTP GET ͨ��
     * strUrl: string, Ҫ���ʵĵ�ַ
	 * bUtf8: bool, ���ʵ��ǲ��� uft8 �����
     * return: string, Response �ַ���
	 */
	inline int DoGet(std::string& strUrl, std::string& strRet, bool bUtf8 = true)
    { return DoHttp(strUrl, NULL, strRet, bUtf8); }

    /**
     * ִ�� HTTP POST ͨ��
     * strUrl: string, Ҫ���ʵĵ�ַ
     * pMapData: MapStrStr*, POST ����
	 * bUtf8: bool, ���ʵ��ǲ��� uft8 �����
     * return: Response �ַ���
	 */
	inline int DoPost(std::string& strUrl, MapStrStr* pMapData, std::string& strRet, bool bUtf8 = true)
    { return DoHttp(strUrl, pMapData, strRet, bUtf8); }
	
	/**
     * ִ�� HTTP POST �ϴ��ļ�
     * strUrl: string, Ҫ���ʵĵ�ַ
     * pMapData: MapStrStr*, POST ����
	 * strFileMapKey: string, �ļ����� pMapData �е� key
	 * bUtf8: bool, ���ʵ��ǲ��� uft8 �����
     * return: Response �ַ���
	 */
	int DoPostFile(std::string& strUrl, 
		MapStrStr* pMapData, 
		std::string& strFileMapKey, 
		std::string& strRet,
		bool bUtf8 = true);

	void Reset();

protected:
	void Init(unsigned long lTimeout);
	void Clear();

	static size_t ProcessData(void* ptr, size_t size, size_t nmemb, void* userdata);
	/**
     * �� MapStrStr ���͵Ĳ���ת�����ַ���
     * pMapData: MapStrStr*, POST ����
     * return: string, POST �ַ���
	 */
	std::string MapToString(MapStrStr* pMapData);
	/*
     * ִ�� HTTP ͨ��
     * strUrl: string, Ҫ���ʵĵ�ַ
     * pMapData: MapStrStr*, POST ����
     * return: string, Response �ַ���
	 */
	int DoHttp(std::string& strUrl, MapStrStr* pMapData, std::string& strRet, bool bUtf8);
	
	int SendRequest(std::string& strUrl);

	CURL* m_pCurl;
	curl_slist* m_headerList;
	std::string m_strBuffer;
	static bool m_bUtf8;
	unsigned long m_lTimeout;
};

#endif
