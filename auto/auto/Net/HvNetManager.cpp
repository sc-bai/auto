#include<string>
#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include<vector>
#include "HvNetManager.h"
#include "hv/hthread.h"
#include <QDebug>
using namespace hv;
using namespace std;

//utf8 转 Unicode
std::wstring Utf82Unicode(const std::string& utf8string)
{
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (widesize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<wchar_t> resultstring(widesize);
	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);
	if (convresult != widesize)
	{
		throw std::exception("La falla!");
	}
	return std::wstring(&resultstring[0]);
}

//unicode 转为 ascii
std::string WideByte2Acsi(std::wstring& wstrcode)
{
	int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
	if (asciisize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (asciisize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<char> resultstring(asciisize);
	int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);

	if (convresult != asciisize)
	{
		throw std::exception("La falla!");
	}

	return std::string(&resultstring[0]);
}

//utf-8 转 ascii
std::string UTF_82ASCII(std::string& strUtf8Code)
{
	std::string strRet("");
	//先把 utf8 转为 unicode
	std::wstring wstr = Utf82Unicode(strUtf8Code);
	//最后把 unicode 转为 ascii
	strRet = WideByte2Acsi(wstr);
	return strRet;
}

//ascii 转 Unicode
std::wstring Acsi2WideByte(std::string& strascii)
{
	int widesize = MultiByteToWideChar(CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (widesize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<wchar_t> resultstring(widesize);
	int convresult = MultiByteToWideChar(CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);
	if (convresult != widesize)
	{
		throw std::exception("La falla!");
	}

	return std::wstring(&resultstring[0]);
}

//Unicode 转 Utf8
std::string Unicode2Utf8(const std::wstring& widestring)
{
	int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
	if (utf8size == 0)
	{
		throw std::exception("Error in conversion.");
	}

	std::vector<char> resultstring(utf8size);

	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);

	if (convresult != utf8size)
	{
		throw std::exception("La falla!");
	}

	return std::string(&resultstring[0]);
}

//ascii 转 Utf8
std::string ASCII2UTF_8(std::string& strAsciiCode)
{
	std::string strRet("");
	//先把 ascii 转为 unicode
	std::wstring wstr = Acsi2WideByte(strAsciiCode);
	//最后把 unicode 转为 utf8
	strRet = Unicode2Utf8(wstr);
	return strRet;
}

/*
	同步请求http服务
	nMethod 0delete 1：get 3:post
*/
bool HvNetManager::HttpRequestionSync(std::string strUrl, std::string strBody, std::string& strRespon, HTTP_METHOD nMethod, int nWaitTime /*= 5*/)
{
	for (int i=0;i<2;i++)
	{
		std::shared_ptr<HttpRequest> req = std::make_shared<HttpRequest>();
		req->method = (http_method)nMethod;
		req->url = strUrl;
		req->headers["Connection"] = "keep-alive";
		//auto& item = req->headers["Connection"];
		//item = "keep-alive";
		req->headers["Content-Type"] = "application/json";
		req->body = strBody;
		req->timeout = nWaitTime;

		qDebug() << "http req: url:" << strUrl.c_str() << ", body:" << strBody.c_str() << ", methed(0:delete 1:get 3:post):" << (int)nMethod;
		HttpResponse resp;
		HttpClient sync_client;
		int ret = sync_client.send(req.get(), &resp);
		if (ret != 0) {
			//LOG_PRINT_E("http req failed.url:%s, method:%d, body:%s", strUrl.c_str(), nMethod, strBody.c_str());
			//LOG_PRINT_E("http req failed info.code:%d, msg:%s", resp.status_code, resp.status_message());
			// return false;
			qDebug() << "ret:"<<ret;
			continue;
		}
		else {
			//LOG_PRINT_N("resp: code[%d],len[%d],data[%s]\r\n", resp.status_code, resp.body.size(), resp.body.c_str());
			qDebug() << "resp: code:" << resp.status_code << "respbody:" << resp.body.c_str();

			if (resp.status_code == 200) {
				strRespon = resp.body;
				return true;
			}
		}
	}
	return false;
}
