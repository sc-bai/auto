#pragma once

#include <vector>
#include <map>
#include <iomanip>
#include <string>

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <Tools/CodeTool.h>

#include "hv/WebSocketClient.h"
#include "hv/WebSocketChannel.h"
#include "hv/requests.h"
#include "hv/axios.h"
#include "Tools/CodeTool.h"

using namespace hv;
using namespace std;
#include "hv/hthread.h" // import hv_gettid


enum class HTTP_METHOD {
	http_delete = 0,
	http_get = 1,
	http_post = 3,
};

class HvNetManager
{
public:
	static HvNetManager* instance() {
		static HvNetManager h;
		return &h;
	}
	
public:
	/*
		Õ¨≤Ω«Î«Û
	*/
	bool HttpRequestionSync(std::string strUrl, std::string strBody, std::string& strRespon, HTTP_METHOD nMethod = HTTP_METHOD::http_get, int nWaitTime = 5);

	std::string assemble_auth_url(const std::string& hosturl,
		const std::string& api_key,
		const std::string& api_secret) {
		// Parse URL components (simplified parsing)
		size_t protocol_end = hosturl.find("://");
		size_t host_start = protocol_end != string::npos ? protocol_end + 3 : 0;
		size_t path_start = hosturl.find('/', host_start);

		string host = hosturl.substr(host_start, path_start - host_start);
		string path = path_start != string::npos ? hosturl.substr(path_start) : "/";

		// Generate RFC1123 formatted date
		std::string date_buf = get_rfc1123_time();

		// Build signature string
		vector<string> sign_parts = {
			"host: " + host,
			"date: " + string(date_buf),
			"GET " + path + " HTTP/1.1"
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
			", headers=\"host date request-line\""
			", signature=\"" + signature + "\"";
		string authorization = CodeTool::EncodeBase64((unsigned char*)auth_params.c_str(), auth_params.size());

		// Build query parameters
		std::vector<std::pair<string, string>> params = {
			{"host", host},
			{"date", date_buf},
			{"authorization", authorization}
		};

		// Build final URL
		string query;
		/*
		for (const auto& [key, value] : params) {
			if (!query.empty()) query += "&";
			query += url_encode(key) + "=" + url_encode(value);
		}
		*/
		for (auto& item : params) {
			std::string key = item.first;
			std::string value = item.second;
			if (!query.empty()) query += "&";
			query += url_encode(key) + "=" + url_encode(value);
		}

		return hosturl + "?" + query;
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

	


	
private:
	HttpClient sync_client;
};
