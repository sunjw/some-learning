#include "SimpleHttp.h"
#include "strhelper.h"
#include "urlencode.h"

using namespace sunjwbase;

bool SimpleHttp::m_bUtf8 = true;

SimpleHttp::SimpleHttp(unsigned long lTimeout)
:m_headerList(NULL)
{
	Init(lTimeout);
}

SimpleHttp::~SimpleHttp()
{
	Clear();
}

void SimpleHttp::Init(unsigned long lTimeout)
{
	m_lTimeout = lTimeout;
	curl_slist_free_all(m_headerList);
	m_pCurl = curl_easy_init();
	if(m_pCurl)
	{
		curl_easy_setopt(m_pCurl, CURLOPT_COOKIEFILE, ""); /* just to start the cookie engine */
		curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1); // 跟踪入 loaction
		curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, lTimeout); // 设置超时
		//m_headerList = curl_slist_append(m_headerList, "Accept-charset: GB2312,utf-8;q=0.7,*;q=0.7");
		//curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_headerList); // 设置 Accept-charset
        
		curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, &SimpleHttp::ProcessData);
		curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void*)&m_strBuffer); // 传入指针
	}
}

void SimpleHttp::Clear()
{
	if(m_pCurl)
	{
		curl_easy_cleanup(m_pCurl);
		curl_slist_free_all(m_headerList);
		m_headerList = NULL;
	}
}

void SimpleHttp::Reset()
{
	Clear();
	Init(m_lTimeout);
}

size_t SimpleHttp::ProcessData(void* ptr, size_t size, size_t nmemb, void* userdata)
{
	std::string str;
	std::string* pstrContent = (std::string*)userdata;
	if(size == 1)
	{
		// 应该是 char
		char* pchar = (char*)ptr;
		str.append(pchar, pchar + nmemb);
        
        pstrContent->append(str);
	}
    
	return size * nmemb;
}

std::string SimpleHttp::MapToString(MapStrStr* pMapData)
{
	if(!pMapData)
		return std::string("");
    
	std::string strRet("");
    
	MapStrStr::iterator itr = pMapData->begin();
	while(itr != pMapData->end())
	{
		strRet.append(strtrim(itr->first));
		strRet.append("=");
		strRet.append(urlencode(strtrim(itr->second)));
		++itr;
		if(itr != pMapData->end())
		{
			strRet.append("&");
		}
	}
    
	return strRet;
}

int SimpleHttp::DoHttp(std::string& strUrl, MapStrStr* pMapData, std::string& strRet, bool bUtf8)
{
	std::string strPost;
    
	SimpleHttp::m_bUtf8 = bUtf8;
	if(pMapData)
	{
		// POST
		strPost = MapToString(pMapData);
		//curl_easy_setopt(m_pCurl, CURLOPT_HTTPPOST, 1);
		curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, strPost.c_str());
		curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, strPost.size());
	}
	else
	{
		// GET
		curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, 0);
		curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, 0);
		curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1);
	}
    
	int retCode = SendRequest(strUrl);
    if(m_bUtf8)
        strRet = asciiconv(m_strBuffer); // convert utf8 string to ascii(native) code string
    else
        strRet = m_strBuffer;
	return retCode;
}

int SimpleHttp::DoPostFile(std::string& strUrl,
                           MapStrStr* pMapData,
                           std::string& strFileMapKey,
                           std::string& strRet,
                           bool bUtf8)
{
	if(!pMapData || pMapData->find(strFileMapKey) == pMapData->end())
	{
		return -1;
	}
    
	SimpleHttp::m_bUtf8 = bUtf8;
	// 准备 POST 数据
	struct curl_httppost* pPostBegin = NULL;
    struct curl_httppost* pPostEnd = NULL;
	std::string strKey;
	MapStrStr::iterator itr = pMapData->begin();
	while(1)
	{
		strKey = itr->first;
		if(strKey.compare(strFileMapKey) == 0)
		{
			// 是文件
			curl_formadd(&pPostBegin, &pPostEnd,
                         CURLFORM_COPYNAME, strKey.c_str(),
                         CURLFORM_FILE, (*pMapData)[strKey].c_str(),
                         CURLFORM_END);
		}
		else
		{
			// 普通 Post
			std::string strValue = itr->second;
			strValue = urlencode(strValue);
            
			curl_formadd(&pPostBegin, &pPostEnd,
                         CURLFORM_COPYNAME, strKey.c_str(),
                         CURLFORM_COPYCONTENTS, strValue.c_str(),
                         CURLFORM_END);
		}
        
		++itr;
		if(itr == pMapData->end())
			break;
	}
    
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPPOST, pPostBegin);
    
	int retCode = SendRequest(strUrl);
    
	curl_formfree(pPostBegin);
	strRet = m_strBuffer;
	return retCode;
}

int SimpleHttp::SendRequest(std::string& strUrl)
{
	curl_easy_setopt(m_pCurl, CURLOPT_URL, strUrl.c_str());
	m_strBuffer.clear();
	return curl_easy_perform(m_pCurl);
}
