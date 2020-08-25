#ifndef _HTTP_CORE_H_
#define _HTTP_CORE_H_
/*
 * HTTP 简单操作内核
 * 基于 libcurl 的 easy functions
 * 线程不安全!!!
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
     * 执行 HTTP GET 通信
     * strUrl: string, 要访问的地址
	 * bUtf8: bool, 访问的是不是 uft8 编码的
     * return: string, Response 字符串
	 */
	inline int DoGet(std::string& strUrl, std::string& strRet, bool bUtf8 = true)
    { return DoHttp(strUrl, NULL, strRet, bUtf8); }

    /**
     * 执行 HTTP POST 通信
     * strUrl: string, 要访问的地址
     * pMapData: MapStrStr*, POST 数据
	 * bUtf8: bool, 访问的是不是 uft8 编码的
     * return: Response 字符串
	 */
	inline int DoPost(std::string& strUrl, MapStrStr* pMapData, std::string& strRet, bool bUtf8 = true)
    { return DoHttp(strUrl, pMapData, strRet, bUtf8); }
	
	/**
     * 执行 HTTP POST 上传文件
     * strUrl: string, 要访问的地址
     * pMapData: MapStrStr*, POST 数据
	 * strFileMapKey: string, 文件名在 pMapData 中的 key
	 * bUtf8: bool, 访问的是不是 uft8 编码的
     * return: Response 字符串
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
     * 将 MapStrStr 类型的参数转换成字符串
     * pMapData: MapStrStr*, POST 数据
     * return: string, POST 字符串
	 */
	std::string MapToString(MapStrStr* pMapData);
	/*
     * 执行 HTTP 通信
     * strUrl: string, 要访问的地址
     * pMapData: MapStrStr*, POST 数据
     * return: string, Response 字符串
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
