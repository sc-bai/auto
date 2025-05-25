#pragma once

#include <vector>
#include <map>
#include <iomanip>
#include <string>
#include <QDebug>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>

#include "Tools/CodeTool.h"
#include "hv/json.hpp"
#include "hv/HttpClient.h"

using namespace hv;
using namespace std;
#include "hv/hthread.h" // import hv_gettid


enum class HTTP_METHOD {
	http_delete = 0,
	http_get = 1,
	http_post = 3,
};

struct HttpReqCommon {
	std::string app_id;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpReqCommon, app_id)
};

struct HttpReqBusiness {
	std::string from;
	std::string to;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpReqBusiness, from,to)
};

struct HttpReqData {
	std::string text;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpReqData, text)
};

struct HttpReqBody {
	HttpReqCommon common;
	HttpReqBusiness business;
	HttpReqData data;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpReqBody, common, business, data)
};


struct HttpRespDataResultTransResult {
	std::string dst;
	std::string src;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpRespDataResultTransResult, dst, src)
};

struct HttpRespDataResult {
	std::string from;
	std::string to;
	HttpRespDataResultTransResult trans_result;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpRespDataResult, from, to, trans_result)
};

struct HttpRespData {
	HttpRespDataResult result;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpRespData, result)
};

struct HttpResp {
	int code;
	std::string message;
	std::string sid;
	HttpRespData data;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpResp, code, message, sid, data)
};


class HvNetManager
{
public:
	static HvNetManager* instance() {
		static HvNetManager h;
		return &h;
	}
	
	bool TransText(std::string strFrom, std::string to, std::string strSrcText, std::string& strDesText);
	private:
	/*
		Õ¨≤Ω«Î«Û
	*/
	bool HttpRequestionSync(std::string hosturl, std::string strBody, std::string Authorization, std::string& strDate, std::string& strRespon, HTTP_METHOD nMethod = HTTP_METHOD::http_get, int nWaitTime = 5);

	std::string build_signature(const std::string hosturl, const std::string reqJson, const std::string& api_key,
		const std::string& api_secret, std::string& date_buf) {
	
		// Parse URL components(simplified parsing)
		size_t protocol_end = hosturl.find("://");
		size_t host_start = protocol_end != string::npos ? protocol_end + 3 : 0;
		size_t path_start = hosturl.find('/', host_start);

		string host = hosturl.substr(host_start, path_start - host_start);
		string path = path_start != string::npos ? hosturl.substr(path_start) : "/";

		
		std::string strDigest = sha256(reqJson);
		strDigest = CodeTool::EncodeBase64((unsigned char*)strDigest.c_str(), strDigest.size());

		// Build signature string
		vector<string> sign_parts = {
			"host: " + host,
			"date: " + string(date_buf),
			"POST " + path + " HTTP/1.1",
			"digest: SHA-256=" + strDigest
		};
		string sign_string;
		for (const auto& part : sign_parts) {
			sign_string += part + "\n";
		}
		if (!sign_string.empty()) {
			sign_string.pop_back(); // Remove last newline
		}

		// Calculate signature
		string signature = hmac_sha256_base64(api_secret, sign_string);

		// Build authorization header
		string auth_params = "api_key=\"" + api_key +
			"\", algorithm=\"hmac-sha256\""
			", headers=\"host date request-line digest\""
			", signature=\"" + signature + "\"";
			
		return auth_params;
	}
private:
	std::string get_rfc1123_time() {
		std::time_t now = std::time(nullptr);
		std::tm tm = *std::gmtime(&now);
		char buffer[128];
		std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &tm);
		return std::string(buffer);
	}

	std::string url_encode(const std::string& value) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << hex;

		for (char c : value) {
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}
			escaped << uppercase;
			escaped << '%' << setw(2) << int((unsigned char)c);
			escaped << nouppercase;
		}

		return escaped.str();
	}

	std::string hmac_sha256_base64(const std::string& key, const std::string& data) {
		unsigned char digest[EVP_MAX_MD_SIZE];
		unsigned int len;

		HMAC(EVP_sha256(),
			key.data(), key.length(),
			reinterpret_cast<const unsigned char*>(data.data()), data.length(),
			digest, &len);

		return CodeTool::EncodeBase64(digest, len);
	}

	std::string sha256(const std::string& str) {
		unsigned char hash[SHA256_DIGEST_LENGTH];
		EVP_MD_CTX* mdctx = EVP_MD_CTX_new();

		EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);
		EVP_DigestUpdate(mdctx, str.c_str(), str.size());
		EVP_DigestFinal_ex(mdctx, hash, NULL);
		EVP_MD_CTX_free(mdctx);

		std::string output;
		for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
			char buf[3];
			sprintf(buf, "%02x", hash[i]);
			output += buf;
		}
		return output;
	}


	
private:
	HttpClient sync_client;


};
