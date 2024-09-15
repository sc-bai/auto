#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "rechelper.h"

RECHelper::RECHelper()
{
}

bool RECHelper::ModifyRecFileAll(std::vector<ContentListItem>& vec)
{
	for (auto &item : vec)	{
		ModifyRecFileEx(item);
	}
	return true;
}
/*
bool RECHelper::ModifyRecFile(ContentListItem& index)
{
	m_strRecFile = PathHelper::Instance()->GetRecDir();
	std::wstring strOutModifyFile = L"m_" + index.strRecName;
	strOutModifyFile = m_strRecFile + strOutModifyFile;

	m_strRecFile += index.strRecName;
	if (_waccess(m_strRecFile.c_str(), 0) != 0) {
		OutputDebugString(L"[weather] rec file not exist ");
		OutputDebugString(m_strRecFile.c_str());
		return false;
	}
	
	std::ifstream file(m_strRecFile.c_str(), std::ios::binary);
	FILE* fp = _wfopen(strOutModifyFile.c_str(), L"wb");
	if (fp == nullptr) return false;

	if (file.is_open()) {
		file.seekg(0, file.end);
		auto fos = file.tellg();
		unsigned long long filesize = fos;
		char* buffer = new char[filesize];
		memset(buffer, 0, filesize);
		file.seekg(0);
		file.read(buffer, filesize);

		ModifyRecBuffer(index, (unsigned char*)buffer, filesize);
		
		fwrite(buffer, 1, filesize, fp);
		fclose(fp);
		
		file.close();
		delete[] buffer;
	}
#ifndef _DEBUG
	if (_waccess(strOutModifyFile.c_str(), 0) == 0) {
		::DeleteFile(m_strRecFile.c_str());
		_wrename(strOutModifyFile.c_str(), m_strRecFile.c_str());
	}
#endif // _DEBUG
	
	//::DeleteFile(m_strRecFile.c_str());
	//::CopyFile(strOutModifyFile.c_str(), m_strRecFile.c_str(), FALSE);

	return true;
}
*/
bool RECHelper::ModifyRecFile(ContentListItem& index, std::wstring strRecModifyFileName)
{
	m_strRecFile = PathHelper::Instance()->GetRecDir();
	std::wstring strOutModifyFile = L"m_" + strRecModifyFileName;
	strOutModifyFile = m_strRecFile + strOutModifyFile;

	m_strRecFile += strRecModifyFileName;
	if (_waccess(m_strRecFile.c_str(), 0) != 0) {
		OutputDebugString(L"[weather] rec file not exist ");
		OutputDebugString(m_strRecFile.c_str());
		return false;
	}

	std::ifstream file(m_strRecFile.c_str(), std::ios::binary);
	FILE* fp = _wfopen(strOutModifyFile.c_str(), L"wb");
	if (fp == nullptr) return false;

	if (file.is_open()) {
		file.seekg(0, file.end);
		auto fos = file.tellg();
		unsigned long long filesize = fos;
		char* buffer = new char[filesize];
		memset(buffer, 0, filesize);
		file.seekg(0);
		file.read(buffer, filesize);

		ModifyRecBuffer(index, (unsigned char*)buffer, filesize);

		fwrite(buffer, 1, filesize, fp);
		fclose(fp);

		file.close();
		delete[] buffer;
	}
#ifndef _DEBUG
	if (_waccess(strOutModifyFile.c_str(), 0) == 0) {
		::DeleteFile(m_strRecFile.c_str());
		_wrename(strOutModifyFile.c_str(), m_strRecFile.c_str());
	}
#endif // _DEBUG

	//::DeleteFile(m_strRecFile.c_str());
	//::CopyFile(strOutModifyFile.c_str(), m_strRecFile.c_str(), FALSE);

	return true;
}

bool RECHelper::ModifyRecFileEx(ContentListItem& index)
{
	auto strRecNames = tool::CodeHelper::vStringSplit(index.strRecName, L"\\");
	if (strRecNames.size() == 0) {
		return false;
	}

	for (int i=0;i<strRecNames.size();i++){
		ModifyRecFile(index, strRecNames[i]);
	}
	return true;
}

/*
	修改日期rec
*/
void RECHelper::ModifyRecFileWithDate(std::vector<DateModifyItem>& vec)
{
	for (auto& item : vec)	{
		if (_waccess(item.strRecFile.c_str(), 0) == 0) {
			ModifySingleRecDateFile(item);
		}
	}
}

std::vector<std::string> RECHelper::Spliter4To2(std::string str)
{
	std::vector<std::string> vec;
	if (str.size() != 4) {
		return vec;
	}
	
	std::string temp = str;
	temp = str.substr(0, 2);
	vec.push_back(temp);
	temp = str.substr(2, 2);
	vec.push_back(temp);
	return vec;
}

// 根据index 获取要修改的tga后两位
std::vector<std::string> RECHelper::GetWeatherModifyTgaFromItem(ContentListItem& index)
{
	std::vector<std::string> vec;
	int nIndex1 = _wtoi(index.strWeather.c_str());
	int nIndex2 = _wtoi(index.strWeatherEx.c_str());
	// big than 19, index1 == index2
	if (nIndex1 == nIndex2) {
		// one tga or two tga
		if (nIndex1 <= 19) {
			vec.push_back(CItemInit::Instance()->m_tag_weather[nIndex1]);
			goto exit;
		}
		else if (nIndex1 <= 101) {
			auto res = Spliter4To2(CItemInit::Instance()->m_tag_weather[nIndex1]);
			if (res.size() == 2) {
				if (res[0] == res[1]) {
					vec.push_back(res[0]);
				}
				else {
					vec.push_back(res[0]);
					vec.push_back(res[1]);
				}
			}
		}else {
			int nIndexTmp = nIndex1 - 102;
			std::string strTemp = tool::CodeHelper::WStr2Str(CItemInit::Instance()->m_scWeatherInfoEx[nIndexTmp].strCode);
			auto res = Spliter4To2(strTemp);
			if (res.size() == 2) {
				if (res[0] == res[1]) {
					vec.push_back(res[0]);
				}
				else {
					vec.push_back(res[0]);
					vec.push_back(res[1]);
				}
			}
		}
	}
	else {
		vec.push_back(CItemInit::Instance()->m_tag_weather[nIndex1]);
		vec.push_back(CItemInit::Instance()->m_tag_weather[nIndex2]);
	}

exit:
	return vec;
}

std::vector<std::string> RECHelper::GetWindAndLevelModifyTgaFromItem(ContentListItem& index)
{
	std::vector<std::string> vec; // eg: [11] or [11,12]
	std::string strTemp;
	strTemp.resize(2);
	int nIndex1 = _wtoi(index.strWind.c_str());
	int nIndex2 = _wtoi(index.strWindLv.c_str());
	int nIndexEx1 = _wtoi(index.strWindEx.c_str());
	int nIndexEx2 = _wtoi(index.strWindLvEx.c_str());

	strTemp[0] = CItemInit::Instance()->m_sz_tga_wind[nIndex1];
	strTemp[1] = CItemInit::Instance()->m_sz_tga_windlv[nIndex2];
	vec.push_back(strTemp);
	if (nIndex1 == nIndexEx1 && nIndex2 == nIndexEx2) {
		return vec;
	}
	strTemp[0] = CItemInit::Instance()->m_sz_tga_wind[nIndexEx1];
	strTemp[1] = CItemInit::Instance()->m_sz_tga_windlv[nIndexEx2];
	vec.push_back(strTemp);

	return vec;
}

// s2: 3个tga
// 2个-> 修改 第0,1个
// 1个->修改 第2个
void RECHelper::HandleWeatherIndex(unsigned char* buf, std::vector<std::string> &mdfvec, int nIndexTga, int ntotalgta) {
	// normal ntotalgta =3;
	int iTotalModifySize = mdfvec.size();
	if (iTotalModifySize == 0) {
		return;
	}
	if (nIndexTga == 0) {
		if (iTotalModifySize == 2) {
			buf[0] = mdfvec[0][0];
			buf[1] = mdfvec[0][1];
		}
		else if (iTotalModifySize == 1) {
			buf[0] = 'k';
			buf[1] = 'k';
		}
	}
	else if (nIndexTga == 1) {
		if (iTotalModifySize == 2) {
			buf[0] = mdfvec[1][0];
			buf[1] = mdfvec[1][1];
		}
		else if (iTotalModifySize == 1) {
			buf[0] = 'k';
			buf[1] = 'k';
		}
	}
	else if (nIndexTga == 2) {
		if (iTotalModifySize == 2) {
			buf[0] = 'k';
			buf[1] = 'k';
		}
		else if (iTotalModifySize == 1) {
			buf[0] = mdfvec[0][0];
			buf[1] = mdfvec[0][1];
		}
	}

	return;
}

void RECHelper::HandleWindAndLevelIndex(unsigned char* buf, std::vector<std::string>& mdfvec, int nIndexTga, int ntotalgta)
{
	int iTotalModifySize = mdfvec.size();
	if (iTotalModifySize == 0) {
		return;
	}
	if (nIndexTga == 0) {
		if (iTotalModifySize == 2) {
			buf[0] = mdfvec[0][0];
			buf[1] = mdfvec[0][1];
		}
		else if (iTotalModifySize == 1) {
			buf[0] = 'k';
			buf[1] = 'k';
		}
	}
	else if (nIndexTga == 1) {
		if (iTotalModifySize == 2) {
			buf[0] = mdfvec[1][0];
			buf[1] = mdfvec[1][1];
		}
		else if (iTotalModifySize == 1) {
			buf[0] = 'k';
			buf[1] = 'k';
		}
	}
	else if (nIndexTga == 2) {
		if (iTotalModifySize == 2) {
			buf[0] = 'k';
			buf[1] = 'k';
		}
		else if (iTotalModifySize == 1) {
			buf[0] = mdfvec[0][0];
			buf[1] = mdfvec[0][1];
		}
	}
	return;
}

void RECHelper::HandleTempIndex(unsigned char* buf, ContentListItem& index, int nflag, char cfalg)
{
	// fill buf with ten types (unuse type fill with 0x20)
	int iTotilUseTypes = 0;
	
	int nIndex1 = _wtoi(index.strTemp.c_str());
	int nIndex2 = _wtoi(index.strTempEx.c_str());

	std::wstring wstr1 = CItemInit::Instance()->g_scTempName[nIndex1];
	std::wstring wstr2 = CItemInit::Instance()->g_scTempName[nIndex2];
	// 去掉度
	wstr1 = wstr1.substr(0, wstr1.length() - 1);
	wstr2 = wstr2.substr(0, wstr2.length() - 1);
	std::string strTemp1 = tool::CodeHelper::WStr2Str(wstr1);
	std::string strTemp2 = tool::CodeHelper::WStr2Str(wstr2);
	
	iTotilUseTypes += strTemp1.length();
	memcpy(buf, strTemp1.c_str(), strTemp1.length());

	if (strTemp1 == strTemp2) { // add °C
		buf[iTotilUseTypes++] = 0xa1;
		buf[iTotilUseTypes++] = 0xe6;
	}
	else {
		if (nflag == 1) {	// 1个标记位的时候
			buf[iTotilUseTypes++] = cfalg;
		}
		else if (nflag == 2) {
			buf[iTotilUseTypes++] = 0xa1;
			buf[iTotilUseTypes++] = 0xab;
		}
		memcpy(&buf[iTotilUseTypes], strTemp2.c_str(), strTemp2.length());
		iTotilUseTypes += strTemp2.length();
		// add °C
		buf[iTotilUseTypes++] = 0xa1;
		buf[iTotilUseTypes++] = 0xe6;
	}
	
	for (int i=0;i<10-iTotilUseTypes;i++){
		buf[iTotilUseTypes + i] = 0x20;
	}
}

void RECHelper::HandleTempIndexSignal(unsigned char* buf, ContentListItem& index, int nflag, char cfalg)
{
	int iTotilUseTypes = 0;

	int nIndex1 = _wtoi(index.strTemp.c_str());
	int nIndex2 = _wtoi(index.strTempEx.c_str());

	std::wstring wstr1 = CItemInit::Instance()->g_scTempName[nIndex1];
	std::wstring wstr2 = CItemInit::Instance()->g_scTempName[nIndex2];
	// 去掉度
	wstr1 = wstr1.substr(0, wstr1.length() - 1);
	wstr2 = wstr2.substr(0, wstr2.length() - 1);
	std::string strTemp1 = tool::CodeHelper::WStr2Str(wstr1);
	std::string strTemp2 = tool::CodeHelper::WStr2Str(wstr2);

	if (strTemp1 != strTemp2) {
		iTotilUseTypes += strTemp1.length();
		memcpy(buf, strTemp1.c_str(), strTemp1.length());
		buf[iTotilUseTypes++] = '\\'; // 添加右斜杠 
		memcpy(buf + iTotilUseTypes, strTemp2.c_str(), strTemp2.length());
		iTotilUseTypes += strTemp2.length();
		buf[iTotilUseTypes++] = 0xa1;
		buf[iTotilUseTypes++] = 0xe6;
	}
	else {
		iTotilUseTypes += strTemp1.length();
		memcpy(buf, strTemp1.c_str(), strTemp1.length());
		buf[iTotilUseTypes++] = 0xa1;
		buf[iTotilUseTypes++] = 0xe6;
	}

	for (int i = 0; i < 10 - iTotilUseTypes; i++) {
		buf[iTotilUseTypes + i] = 0x20;
	}
}

//void RECHelper::HandleJSGVIndex(unsigned char* buff) {
//	buff[-3] = '1';
//	buff[-2] = '0';
//	buff[-1] = '0';
//}

void RECHelper::HandleJSGVIndex(ContentListItem& contentindex, unsigned char* buff)
{
	int index = _wtoi(contentindex.strPrecipitation.c_str());
	std::wstring str = CItemInit::Instance()->g_scPrecipitationName[index];
	std::string strTemp = tool::CodeHelper::WStr2Str(str);
	strTemp = strTemp.substr(0, strTemp.length() - 1); // 去掉%
	if (strTemp.size() == 1) { // 0%
		buff[-3] = ' ';
		buff[-2] = ' ';
		buff[-1] = '0';
	}
	else if (strTemp.size() == 2) {
		buff[-3] = ' ';
		buff[-2] = strTemp[0];
		buff[-1] = strTemp[1];
	}
	else if (strTemp.size() == 3) {
		buff[-3] = '1';
		buff[-2] = '0';
		buff[-1] = '0';
	}
}

void RECHelper::ModifySingleRecDateFile(DateModifyItem& item)
{
	std::ifstream file(item.strRecFile.c_str(), std::ios::binary);

	FILE* fp = _wfopen(item.strTmpRecFile.c_str(), L"wb");
	if (fp == nullptr) return;

	if (file.is_open()) {
		file.seekg(0, file.end);
		auto fos = file.tellg();
		unsigned long long filesize = fos;
		char* buffer = new char[filesize];
		memset(buffer, 0, filesize);
		file.seekg(0);
		file.read(buffer, filesize);

		RealModifyDateFunction(item, (unsigned char*)buffer, filesize);

		fwrite(buffer, 1, filesize, fp);
		fclose(fp);
		file.close();
		delete[] buffer;

		if (_waccess(item.strTmpRecFile.c_str(), 0) == 0) {
			::DeleteFile(item.strRecFile.c_str());
			_wrename(item.strTmpRecFile.c_str(), item.strRecFile.c_str());
		}
	}
}

void RECHelper::RealModifyDateFunction(DateModifyItem& item, unsigned char* buf, int filesize)
{
	std::wstring strDate;
	std::string strDateUtf;
	for (int i = 0; i < filesize - 0xe; i++)
	{
		if (buf[i] == 0x0e) {
			int a = 0; 
			a++;
		}
		// 年=c4 ea 月= d4 c2
		if (buf[i] == 0x0e && buf[i + 5] == 0xc4 && buf[i + 6] == 0xea && buf[i + 9] == 0xd4 && buf[i + 10] == 0xc2) {
			strDate = tool::TimeTool::AddDay(item.nAddDate);
			strDateUtf = tool::CodeHelper::WStr2Str(strDate);
			memcpy(buf + i + 1, strDateUtf.c_str(), strDateUtf.length());
			break;
		}
	}
}

void RECHelper::ModifyRecBuffer(ContentListItem& index, unsigned char* recbuf, int nSize) {
	if (recbuf == nullptr || nSize <= 0) {
		return;
	}
	int nWeatherTgaSize = 0, nWeathTgaIndex = 0;
	auto weatherModifyData = GetWeatherModifyTgaFromItem(index); // .size == 1 or 2

	int nWindAndLevelTgaSize = 0, nWindTgaIndex = 0;
	auto WindAndLevelModifyData = GetWindAndLevelModifyTgaFromItem(index); // .size == 1 or 2

	for (int i = 5; i < nSize - 5; i++) {
		if (((recbuf[i] == 'l') || (recbuf[i] == 'L')) &&
			((recbuf[i + 1] == 'i') || (recbuf[i + 1] == 'I')) &&
			((recbuf[i + 2] == 'b') || (recbuf[i + 2] == 'B')) &&
			((recbuf[i + 4] == 's') || (recbuf[i + 4] == 'S')))	{
			nWeatherTgaSize++;
		}
		if (((recbuf[i] == 'l') || (recbuf[i] == 'L')) &&
			((recbuf[i + 1] == 'i') || (recbuf[i + 1] == 'I')) &&
			((recbuf[i + 2] == 'b') || (recbuf[i + 2] == 'B')) &&
			((recbuf[i + 4] == 'f') || (recbuf[i + 4] == 'F'))) {
			nWindAndLevelTgaSize++;
		}
		// 处理单温度  a1 e6 -> °C  温度是固定10个字节
		if (((recbuf[i] >= '0') && (recbuf[i] <= '9')) && recbuf[i + 1] == 0xa1 && recbuf[i + 2] == 0xe6)	{
			int nTemp = i;
			bool bRight = true;
			if (recbuf[i - 1] == 0x0a)  nTemp = i;		// eg 1°C				占一位
			else if (recbuf[i - 2] == 0x0a) nTemp = i - 1;	// eg 12°C 或者 -1°C		占两位
			else if (recbuf[i - 3] == 0x0a) nTemp = i - 2;	// eg -12°C				// 占三位
			else {
				bRight = false;
			}
			if (bRight) {
				HandleTempIndexSignal(&recbuf[nTemp], index, 1, recbuf[i + 1]);
				continue;
			}
		}


		// 处理温度区间
		if (((recbuf[i] >= '0') && (recbuf[i] <= '9')) &&
			((recbuf[i + 1] == '\\') || (recbuf[i + 1] == '/') ||
				(recbuf[i + 1] == '-') || (recbuf[i + 1] == '~')) &&
			(((recbuf[i + 2] >= '0') && (recbuf[i + 2] <= '9')) || (recbuf[i + 2] == '-'))) {
			int nTemp = i;
			if (recbuf[i - 1] == 0x0a)  nTemp = i;		// eg 1/8
			if (recbuf[i - 2] == 0x0a) nTemp = i - 1;
			if (recbuf[i - 3] == 0x0a) nTemp = i - 2;
			HandleTempIndex(&recbuf[nTemp], index, 1, recbuf[i + 1]);
		}
		if (((recbuf[i] >= '0') && (recbuf[i] <= '9')) &&
			(((recbuf[i + 1] == 0xa1) && (recbuf[i + 2] == 0xab)) ||
				((recbuf[i + 1] == 0xa1) && (recbuf[i + 2] == 0xaa))) &&
			(((recbuf[i + 3] >= '0') && (recbuf[i + 3] <= '9')) || (recbuf[i + 3] == '-')))
		{
			int nTemp = i;
			if (recbuf[i - 1] == 0x0a)  nTemp = i;		// eg 1~8
			if (recbuf[i - 2] == 0x0a) nTemp = i - 1;
			if (recbuf[i - 3] == 0x0a) nTemp = i - 2;
			HandleTempIndex(&recbuf[nTemp], index, 2, recbuf[i + 1]);
		}

		if ((recbuf[i] == 0x25)) {

			HandleJSGVIndex(index, &recbuf[i]);
		}
	}

	
	for (int i = 5; i < nSize - 5; i++) {
		if (((recbuf[i] == 'l') || (recbuf[i] == 'L')) &&
			((recbuf[i + 1] == 'i') || (recbuf[i + 1] == 'I')) &&
			((recbuf[i + 2] == 'b') || (recbuf[i + 2] == 'B')) &&
			((recbuf[i + 4] == 's') || (recbuf[i + 4] == 'S')))
		{
			// handle lib/s weather
			HandleWeatherIndex(&recbuf[i + 6], weatherModifyData, nWeathTgaIndex, nWeatherTgaSize);
			nWeathTgaIndex++;
		}

		if (((recbuf[i] == 'l') || (recbuf[i] == 'L')) &&
			((recbuf[i + 1] == 'i') || (recbuf[i + 1] == 'I')) &&
			((recbuf[i + 2] == 'b') || (recbuf[i + 2] == 'B')) &&
			((recbuf[i + 4] == 'f') || (recbuf[i + 4] == 'F'))) {
			nWindAndLevelTgaSize++;
			HandleWindAndLevelIndex(&recbuf[i + 6], WindAndLevelModifyData, nWindTgaIndex, nWindAndLevelTgaSize);
			nWindTgaIndex++;
		}
	}
	
}

int main11() {
	std::string filePath = "elva01.rec";
	std::string filePathm = "elva01_m.rec";
	std::ifstream file(filePath, std::ios::binary);
	FILE* fp = fopen(filePathm.c_str(), "wb");

	if (file.is_open()) {
		file.seekg(0, file.end);
		auto fos = file.tellg();
		unsigned long long filesize = fos;
		char* buffer = new char[filesize];
		memset(buffer, 0, filesize);
		file.seekg(0);
		file.read(buffer, filesize);

		//ModifyRec(buffer, filesize);
		if (fp) {
			fwrite(buffer, 1, filesize, fp);

			/*for (int i=0;i<filesize;i++)
			{
				fwrite(buffer, 1, filesize, fp);
			}*/
			fclose(fp);
			std::cout << "success。\n";
		}
		else {
			std::cout << "error。\n";
		}
		file.close();
		delete[] buffer;
	}
	else {
		std::cout << "err" << std::endl;
	}

	return 0;
}
