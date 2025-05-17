#include "TTSHelper.h"
#include <stdlib.h>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include <errno.h>

#include "include/qtts.h"
#include "include/msp_cmn.h"
#include "include/msp_errors.h"
#include "Net/HvNetManager.h"
#include "Net/HvWebsocketManager.h"
 
/* Ĭ��wav��Ƶͷ������ */
wave_pcm_hdr default_wav_hdr =
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

TTSHelper::TTSHelper()
{

}

TTSHelper::~TTSHelper()
{
	uninit();
}

int TTSHelper::init()
{
	int         ret = MSP_SUCCESS;
	const char* login_params = "appid = 205250fa, work_dir = .";//��¼����,appid��msc���,��������Ķ�
	/*
	* rdn:           �ϳ���Ƶ���ַ�����ʽ
	* volume:        �ϳ���Ƶ������
	* pitch:         �ϳ���Ƶ������
	* speed:         �ϳ���Ƶ��Ӧ������
	* voice_name:    �ϳɷ�����
	* sample_rate:   �ϳ���Ƶ������
	* text_encoding: �ϳ��ı������ʽ
	*
	*/
	
	/* �û���¼ */
	ret = MSPLogin(NULL, NULL, login_params);
	if (MSP_SUCCESS != ret)
	{
		printf("MSPLogin failed, error code: %d.\n", ret);
		return ret;
	}
	return ret;
}

int TTSHelper::do_tts(std::string strText, std::vector<std::string> strBuildFilePath, std::vector<std::string> voice_params)
{
	if (strBuildFilePath.size() != voice_params.size() || strBuildFilePath.empty()|| voice_params.empty()) {
		return -1;
	}
	int ret = -1;
	for (int i=0;i<strBuildFilePath.size();i++)
	{
		ret = do_tts_once_http(strText, strBuildFilePath[i], voice_params[i]);
		if (ret == -1) {
			printf("�ϳ�ʧ��");
		}
	}
	return 0;
}

/*
	����0 �ɹ�
*/
int TTSHelper::do_tts_once(std::string strText, std::string strBuildFilePath, std::string voice_params)
{
	const char* src_text = strText.c_str();
	const char* des_path = strBuildFilePath.c_str();

	char param[1024] = { 0 };
	sprintf_s(param, "voice_name = %s, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 0", voice_params.c_str());
	int  ret = -1;
	FILE* fp = NULL;
	const char* sessionID = NULL;
	unsigned int audio_len = 0;
	wave_pcm_hdr wav_hdr = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

	if (NULL == src_text || NULL == des_path)
	{
		return ret;
	}
	fp = fopen(des_path, "wb");
	if (NULL == fp)
	{
		printf("open %s error.\n", des_path);
		return ret;
	}
	/* ��ʼ�ϳ� */
	sessionID = QTTSSessionBegin(param, &ret);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionBegin failed, error code: %d.\n", ret);
		fclose(fp);
		return ret;
	}
	ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSTextPut failed, error code: %d.\n", ret);
		QTTSSessionEnd(sessionID, "TextPutError");
		fclose(fp);
		return ret;
	}
	printf("���ںϳ� ...\n");
	fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp); //���wav��Ƶͷ��ʹ�ò�����Ϊ16000
	while (1)
	{
		/* ��ȡ�ϳ���Ƶ */
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (MSP_SUCCESS != ret)
			break;
		if (NULL != data)
		{
			fwrite(data, audio_len, 1, fp);
			wav_hdr.data_size += audio_len; //����data_size��С
		}
		if (MSP_TTS_FLAG_DATA_END == synth_status)
			break;
		printf(">");
		Sleep(150); //��ֹƵ��ռ��CPU
	}
	printf("\n");
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSAudioGet failed, error code: %d.\n", ret);
		QTTSSessionEnd(sessionID, "AudioGetError");
		fclose(fp);
		return ret;
	}
	/* ����wav�ļ�ͷ���ݵĴ�С */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

	/* ��������������д���ļ�ͷ��,��Ƶ�ļ�Ϊwav��ʽ */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8, sizeof(wav_hdr.size_8), 1, fp); //д��size_8��ֵ
	fseek(fp, 40, 0); //���ļ�ָ��ƫ�Ƶ��洢data_sizeֵ��λ��
	fwrite(&wav_hdr.data_size, sizeof(wav_hdr.data_size), 1, fp); //д��data_size��ֵ
	fclose(fp);
	fp = NULL;
	/* �ϳ���� */
	ret = QTTSSessionEnd(sessionID, "Normal");
	if (MSP_SUCCESS != ret)
	{
		printf("QTTSSessionEnd failed, error code: %d.\n", ret);
	}

	return ret;
}

#include <QDebug>
// http
int TTSHelper::do_tts_once_http(std::string strText, std::string strBuildFilePath, std::string voice_params)
{
	/*std::string url = HvNetManager::instance()->assemble_auth_url("wss://tts-api.xfyun.cn/v2/tts", "53964d67bd0a5dfa0ccd9a0c69f540a2", "NTA3YTUwZThkMzk4YTk4YzQwMTY2ZGNk");

	std::string respon;
	HvNetManager::instance()->HttpRequestionSync(url, "", respon);
	qDebug() << "respon:" << respon.c_str();
	return 0;
	*/

	HVWebSocket::instance()->sendMsg(strText);

	return 0;
}

void TTSHelper::uninit()
{
	MSPLogout(); //�˳���¼
}
