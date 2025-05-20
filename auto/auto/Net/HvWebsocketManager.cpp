#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include "HvWebsocketManager.h"
#include "hv/hthread.h"
#include <QDebug>
using namespace hv;
using namespace std;
#include "TTSHelper.h"

// notify : 使用libhv的时候会偶发出现接受不到数据问题
#define USER_QWEBSOCKET // 使用qt的qwebsocket  

/* 默认wav音频头部数据 */
wave_pcm_hdr default_wav_hdr_ex =
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0
};

HVWebSocket::HVWebSocket()
	: QObject(nullptr)
{
#ifdef USER_QWEBSOCKET
	m_ClientSocket = new QWebSocket();
	connect(m_ClientSocket, &QWebSocket::connected, this, &HVWebSocket::onConnectedSlotQt);
	connect(m_ClientSocket, &QWebSocket::disconnected, this, &HVWebSocket::onDisConnectSlotQt);
	connect(m_ClientSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
		[=](QAbstractSocket::SocketError error) {
			qDebug() << error << ",error string:" << m_ClientSocket->errorString();
		});
	connect(m_ClientSocket, &QWebSocket::textMessageReceived, this, [&](const QString& msg) {
		std::string strText = msg.toStdString();
		onMessage(strText);
		});
#else
	reconn.min_delay = 1000;
	reconn.max_delay = 10000;
	reconn.delay_policy = 2;
	//m_wc.setReconnect(&reconn);

	m_wc.onopen = std::bind(&HVWebSocket::openWebSocket, this);
	m_wc.onmessage = std::bind(&HVWebSocket::onMessage, this, std::placeholders::_1);
	m_wc.onclose = std::bind(&HVWebSocket::onClose, this);
#endif // USER_QWEBSOCKET
		 
	OpenWebSkt();
}

HVWebSocket::~HVWebSocket()
{

}

void HVWebSocket::sendMsg(const std::string Msg, std::string strBuildFilePath, std::string voice_params)
{
	qDebug() << "send msg start:"<<QDateTime::currentDateTime().toString("hh:mm:ss");
	if (bConnected == false) {
		OpenWebSkt();
		Sleep(4000);
	}
	if (bConnected) {
		qDebug() << "send msg real-----" << QDateTime::currentDateTime().toString("hh:mm:ss");;

		WebSocketReq req;
		req.common.app_id = "205250fa";
		req.business.vcn = voice_params;
		req.data.text = CodeTool::EncodeBase64((unsigned char*)Msg.c_str(), Msg.size());

		nlohmann::json j = req;
		std::string req_json = j.dump(4);
#ifdef USER_QWEBSOCKET
		QString send_text = QString::fromStdString(req_json);
		m_ClientSocket->sendTextMessage(send_text);
#else
		m_wc.send(req_json);
#endif // USER_QWEBSOCKET
	}
}

void HVWebSocket::onConnectedSlotQt()
{
	qDebug() << "onConnectedSlot" << QDateTime::currentDateTime().toString("hh:mm:ss");;
	bConnected = true;
}

void HVWebSocket::onDisConnectSlotQt()
{
	qDebug() << "onDisConnectSlot";
	bConnected = false;
}

void HVWebSocket::OpenWebSkt()
{
#ifdef USER_QWEBSOCKET
	std::string url = assemble_auth_url("ws://tts-api.xfyun.cn/v2/tts", "53964d67bd0a5dfa0ccd9a0c69f540a2", "NTA3YTUwZThkMzk4YTk4YzQwMTY2ZGNk");

	m_ClientSocket->open(QUrl(QString::fromStdString(url))); // qwebsocket
#else
	std::string url = assemble_auth_url("wss://tts-api.xfyun.cn/v2/tts", "53964d67bd0a5dfa0ccd9a0c69f540a2", "NTA3YTUwZThkMzk4YTk4YzQwMTY2ZGNk");
	int code = m_wc.open(url.c_str());
	qDebug() << "open websocket url:code:" << code;
#endif
}

void HVWebSocket::openWebSocket()
{
	qDebug() << "openWebSocket success.";
	bConnected = true;
}

void HVWebSocket::onMessage(const std::string& msg)
{
	qDebug() << "onMessage:"<< msg.c_str();
	try {
		json parsed = json::parse(msg);
		WebSocketResp resp = parsed.get<WebSocketResp>();
		
		if (resp.code != 0) {
			qDebug() << "tts recv msg code = 0;";
			return;
		}
		if (resp.data.audio.empty()) return;

		qDebug() << "resp.code:" << resp.code << ",resp.message:" << resp.message.c_str() << ",resp.sid" << resp.sid.c_str();

		if (resp.data.status == 1) {
			m_full_wav_file_data += CodeTool::DecodeBase64(resp.data.audio.c_str(), resp.data.audio.length());
			return;
		}
		if (resp.data.status == 2) {
			m_full_wav_file_data += CodeTool::DecodeBase64(resp.data.audio.c_str(), resp.data.audio.length());
		}
		write_pcm_to_wav_file("text.wav", m_full_wav_file_data);
		m_full_wav_file_data = "";
#ifdef USER_QWEBSOCKET
		m_ClientSocket->abort();
#else
		m_wc.close();
#endif // USER_QWEBSOCKET

	}
	catch (...)
	{

	}

}

void HVWebSocket::onClose()
{
	qDebug() << "onClose:";
	bConnected = false;
}

std::string HVWebSocket::assemble_auth_url(const std::string& hosturl, const std::string& api_key, const std::string& api_secret)
{
	// Parse URL components (simplified parsing)
	size_t protocol_end = hosturl.find("://");
	size_t host_start = protocol_end != string::npos ? protocol_end + 3 : 0;
	size_t path_start = hosturl.find('/', host_start);

	string host = hosturl.substr(host_start, path_start - host_start);
	string path = path_start != string::npos ? hosturl.substr(path_start) : "/";

	// Generate RFC1123 formatted date
	std::string date_buf = get_rfc1123_time();

	//std::string date_buf = "Sun, 18 May 2025 14:33:57 GMT";

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

	qDebug() << "sign_string:" << sign_string.c_str();


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
		{"authorization", authorization},
		{"date", date_buf},
		{"host", host}
	};

	// Build final URL
	string query;
	for (auto& item : params) {
		std::string key = item.first;
		std::string value = item.second;
		if (!query.empty()) query += "&";
		query += url_encode(key) + "=" + url_encode(value);
	}

	return hosturl + "?" + (query);
}

/*
	pcm 写wav文件
*/
void HVWebSocket::write_pcm_to_wav_file(std::string wav_path, std::string& pcm_data)
{
	if (wav_path.empty() || pcm_data.empty()) return;

	const char* src_text = pcm_data.c_str();
	const char* des_path = wav_path.c_str();
	wave_pcm_hdr wav_hdr = default_wav_hdr_ex;

	FILE* fp = NULL;
	fp = fopen(des_path, "wb");
	if (NULL == fp)
	{
		printf("open %s error.\n", des_path);
		qDebug() << "open file error:" << des_path;
		return ;
	}
	fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp); //添加wav音频头，使用采样率为16000

	fwrite(src_text, pcm_data.length(), 1, fp);
	wav_hdr.data_size = pcm_data.length(); //计算data_size大小
	/* 修正wav文件头数据的大小 */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

	/* 将修正过的数据写回文件头部,音频文件为wav格式 */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8, sizeof(wav_hdr.size_8), 1, fp); //写入size_8的值
	fseek(fp, 40, 0); //将文件指针偏移到存储data_size值的位置
	fwrite(&wav_hdr.data_size, sizeof(wav_hdr.data_size), 1, fp); //写入data_size的值
	fclose(fp);
	fp = NULL;
	qDebug() << "write file over.";
}

