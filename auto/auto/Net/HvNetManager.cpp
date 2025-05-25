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
	文本翻译成其他语言
	中文(简体)	cn	  x4_lingxiaoshan_profnews  x4_chaoge
	维吾尔语	uy   x2_ugsp_dilare
	藏语	ti  x2_BoCn_YangJin
*/
bool HvNetManager::TransText(std::string strFrom, std::string to, std::string strSrcText, std::string& strDesText)
{
	HttpReqBody req;
	req.common.app_id = "205250fa";
	req.business.from = strFrom;
	req.business.to = to;
	//req.data.text = strSrcText;
	req.data.text = CodeTool::EncodeBase64((unsigned char*)strSrcText.c_str(), strSrcText.size());

	nlohmann::json j = req;
	std::string str_json_body = j.dump();

	// Generate RFC1123 formatted date
	std::string date_buf = get_rfc1123_time();

	qDebug() << "data_buf-1:" << date_buf.c_str();
	std::string signature = build_signature("https://ntrans.xfyun.cn/v2/ots", str_json_body, "53964d67bd0a5dfa0ccd9a0c69f540a2", "NTA3YTUwZThkMzk4YTk4YzQwMTY2ZGNk", date_buf);

	std::string respon;
	bool bret = HttpRequestionSync("https://ntrans.xfyun.cn/v2/ots", str_json_body, signature, date_buf, respon, HTTP_METHOD::http_post);

	if (bret) {
		nlohmann::json parsed = nlohmann::json::parse(respon);
		HttpResp resp = parsed.get<HttpResp>();
		if (resp.code == 0) {
			strDesText = resp.data.result.trans_result.dst;
			return true;
		}
	}

	return false;
}

/*
	同步请求http服务
	nMethod 0delete 1：get 3:post
*/
bool HvNetManager::HttpRequestionSync(std::string strUrl, std::string strBody,std::string Authorization, std::string& strDate, std::string& strRespon, HTTP_METHOD nMethod, int nWaitTime /*= 5*/)
{
	for (int i=0;i<1;i++)
	{
		std::shared_ptr<HttpRequest> req = std::make_shared<HttpRequest>();
		req->method = (http_method)nMethod;
		req->url = strUrl;
		req->headers["Accept"] = "application/json,version=1.0";
		req->headers["Content-Type"] = "application/json";
		req->headers["Date"] = strDate;

		std::string strDigest = sha256(strBody);
		strDigest = CodeTool::EncodeBase64((unsigned char*)strDigest.c_str(), strDigest.size());
		req->headers["Digest"] = "SHA-256=" +strDigest;
		req->headers["Authorization"] = Authorization;


		// Parse URL components(simplified parsing)
		size_t protocol_end = strUrl.find("://");
		size_t host_start = protocol_end != string::npos ? protocol_end + 3 : 0;
		size_t path_start = strUrl.find('/', host_start);

		string host = strUrl.substr(host_start, path_start - host_start);
		string path = path_start != string::npos ? strUrl.substr(path_start) : "/";
		req->headers["Host"] = host;

		req->body = strBody;
		req->timeout = nWaitTime;

		qDebug() << "http req: url:" << strUrl.c_str() << ", body:" << strBody.c_str() << ", methed(0:delete 1:get 3:post):" << (int)nMethod;
		HttpResponse resp;
		HttpClient sync_client;
		int ret = sync_client.send(req.get(), &resp);
		if (ret != 0) {
			qDebug() << "ret:"<<ret;
			continue;
		}
		else {
			qDebug() << "resp: code:" << resp.status_code << "respbody:" << resp.body.c_str();

			if (resp.status_code == 200) {
				strRespon = resp.body;
				return true;
			}
		}
	}
	return false;
}
