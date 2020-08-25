#ifndef _URL_ENCODE_H_
#define _URL_ENCODE_H_
#include <string>

// 用 urlencode 编码字符串
std::string urlencode(const std::string& c);
// 内部方法
std::string char2hex(char dec);

#endif
