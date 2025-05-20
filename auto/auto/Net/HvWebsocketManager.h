#pragma once

#include <vector>
#include <map>
#include <iomanip>
#include <string>
#include <QObject>

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <Tools/CodeTool.h>
#include <QWebSocket>
#include "hv/WebSocketClient.h"
#include "hv/WebSocketChannel.h"
#include "hv/requests.h"
#include "hv/axios.h"
#include "Tools/CodeTool.h"

using namespace hv;
using namespace std;
#include "hv/hthread.h" // import hv_gettid

#include "hv/json.hpp"


struct WebSocketReqComm {
	std::string app_id;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WebSocketReqComm, app_id)
};

struct WebSocketReqBusiness {
	std::string aue{"raw"};
	std::string auf{"audio/L16;rate=16000"};
	std::string vcn;
	int speed{50};
	int volume{50};
	int pitch{50};
	std::string tte{ "UTF8" };
	std::string rdn{ "1" };

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WebSocketReqBusiness, aue,auf, vcn, speed, volume,pitch, tte, rdn )
};

struct WebSocketReqData {
	std::string text;
	int status{2};

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WebSocketReqData, text, status)
};

struct WebSocketReq {
	WebSocketReqComm common;
	WebSocketReqBusiness business;
	WebSocketReqData data;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WebSocketReq, common, business, data)
};

struct WebSocketRespData {
	std::string audio;
	std::string ced;
	int status;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WebSocketRespData, audio, ced, status)
};
struct WebSocketResp {
	int code;
	std::string message;
	std::string sid;
	WebSocketRespData data;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(WebSocketResp, code, message, sid, data)
};


class HVWebSocket : public QObject
{
	Q_OBJECT

public:
	static HVWebSocket* instance() {
		static HVWebSocket h;
		return &h;
	}
	explicit HVWebSocket();
	~HVWebSocket();

	void sendMsg(const std::string Msg, std::string strBuildFilePath, std::string voice_params);

	//接收消息回调，用于将数据发送到界面
	std::function<void(std::string)> m_pMsgCallBack;

private slots:
	void onConnectedSlotQt();
	void onDisConnectSlotQt();
private:
	void OpenWebSkt();
	void openWebSocket();

	void onMessage(const std::string& msg);

	void onClose();

	std::string assemble_auth_url(const std::string& hosturl,
		const std::string& api_key,
		const std::string& api_secret);
private:
	std::string get_rfc1123_time() {
		std::time_t now = std::time(nullptr);
		std::tm tm = *std::gmtime(&now);
		char buffer[128] = {0};
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
		unsigned char digest[EVP_MAX_MD_SIZE] = {0};
		unsigned int len = 0;

		HMAC(EVP_sha256(),
			key.data(), key.length(),
			reinterpret_cast<const unsigned char*>(data.data()), data.length(),
			digest, &len);

		return CodeTool::EncodeBase64(digest, len);
	}

	void write_pcm_to_wav_file(std::string wav_path, std::string& pcm_data);
	


private:

	QWebSocket* m_ClientSocket = nullptr;

	WebSocketClient m_wc;
	reconn_setting_t reconn;

	//用于通信
	WebSocketChannelPtr m_channelPtr;

	//定时器回调接口
	std::function<void(TimerID)> m_pTimerCallBack;

	bool bConnected = false;

	std::string m_full_wav_file_data;

};