#include "stdafx.h"
#include "Tools/Tools.h"

#include <iostream>
#include <locale>
#include <codecvt>
#include <string>
#include <atlconv.h>

std::wstring tool::FileHelp::GetIniKeyValue(std::wstring strFilePath, std::wstring strNode, std::wstring strKey, std::wstring strValue /*= L""*/)
{
	wchar_t sz[128] = { 0 };
	if (GetPrivateProfileString(strNode.c_str(), strKey.c_str(), strValue.c_str(), sz, 128, strFilePath.c_str())) {
		std::string strTmp = tool::CodeHelper::UnicodeToUtf8(std::wstring(sz));
		return tool::CodeHelper::Utf8ToUnicode(strTmp);
	}
	return strValue;
}

bool tool::FileHelp::WriteIniKeyValue(std::wstring strFilePath, std::wstring strNode, std::wstring strKey, std::wstring strValue)
{
	bool bRet = ::WritePrivateProfileString(strNode.c_str(), strKey.c_str(), strValue.c_str(), strFilePath.c_str());
	if (!bRet) {
		OutputDebugString(L"[weather]error-WriteIniKeyValue false\n");
	}
	return bRet;
}

bool tool::FileHelp::ReadConfigIniFile(std::wstring strFilePath, std::vector<ContentListItem>& vec)
{
	if (_waccess(strFilePath.c_str(), 0) != 0) {
		return false;
	}
	try {
		char sz[128] = { 0 };
		std::wstring strValue;
		int nTotilNum = 0;
		std::wstring strNodeIndex;
		strValue = GetIniKeyValue(strFilePath, L"count", L"total", L"0");
		nTotilNum = _wtoi(strValue.c_str());
		for (int i = 1; i <= nTotilNum; i++)
		{
			ContentListItem item;
			strNodeIndex = std::to_wstring(i);
			item.strWeather = GetIniKeyValue(strFilePath, strNodeIndex, L"weather");
			item.strWeatherEx = GetIniKeyValue(strFilePath, strNodeIndex, L"weatherex");
			item.strTemp = GetIniKeyValue(strFilePath, strNodeIndex, L"temp");
			item.strTempEx = GetIniKeyValue(strFilePath, strNodeIndex, L"tempex");
			item.strTempMost = GetIniKeyValue(strFilePath, strNodeIndex, L"tempmost");
			item.strWind = GetIniKeyValue(strFilePath, strNodeIndex, L"wind");
			item.strWindEx = GetIniKeyValue(strFilePath, strNodeIndex, L"windex");
			item.strWindLv = GetIniKeyValue(strFilePath, strNodeIndex, L"windlv");
			item.strWindLvEx = GetIniKeyValue(strFilePath, strNodeIndex, L"windlvex");
			item.strPrecipitation = GetIniKeyValue(strFilePath, strNodeIndex, L"precipitation");
			//item.strLocation = GetIniKeyValue(strFilePath, strNodeIndex, L"location");
			GetPrivateProfileStringA(tool::CodeHelper::WStr2Str(strNodeIndex).c_str(), "location", "", sz, 128, tool::CodeHelper::WStr2Str(strFilePath).c_str());
			item.strLocation = tool::CodeHelper::Utf8ToUnicode(sz);

			memset(sz, 0, 128);
			GetPrivateProfileStringA(tool::CodeHelper::WStr2Str(strNodeIndex).c_str(), "rec", "", sz, 128, tool::CodeHelper::WStr2Str(strFilePath).c_str());
			item.strRecName = tool::CodeHelper::Utf8ToUnicode(sz);

			//item.strRecName = GetIniKeyValue(strFilePath, strNodeIndex, L"rec");
			item.strWavName = GetIniKeyValue(strFilePath, strNodeIndex, L"wav");

			vec.push_back(item);
		}

		return true;
	}
	catch(...){
		return false;
	}
}

bool tool::FileHelp::ReadTempConfigFle(std::wstring strFilePath, std::vector<TempItem>& vec)
{
	vec.clear();
	if (_waccess(strFilePath.c_str(), 0) != 0) {
		return false;
	}
	try {
		char sz[128] = { 0 };
		std::wstring strValue;
		int nTotilNum = 0;
		std::wstring strNodeIndex;
		strValue = GetIniKeyValue(strFilePath, L"count", L"total", L"0");
		nTotilNum = _wtoi(strValue.c_str());
		for (int i = 1; i <= nTotilNum; i++){
			TempItem item;
			strNodeIndex = std::to_wstring(i);
			item.strIndex = GetIniKeyValue(strFilePath, strNodeIndex, L"index");
			item.strTemp1 = GetIniKeyValue(strFilePath, strNodeIndex, L"temp");
			item.strTemp2 = GetIniKeyValue(strFilePath, strNodeIndex, L"tempex");
			vec.push_back(item);
		}

		return true;
	}
	catch (...) {
		return false;
	}
}

bool tool::FileHelp::WriteTempConfigFle(std::wstring strFilePath, std::vector<TempItem>& vecIndex)
{
	if (_waccess(strFilePath.c_str(), 0) == 0) {
		DeleteFile(strFilePath.c_str());
		std::ofstream file(strFilePath.c_str());
		if (file.is_open()) {
			file.close();
		}
	}
	if (vecIndex.size() == 0) {
		return false;
	}

	try {
		wchar_t sz[128] = { 0 };
		wsprintf(sz, L"%d", vecIndex.size());
		WriteIniKeyValue(strFilePath, L"count", L"total", sz);
		for (int i = 1; i <= vecIndex.size(); i++) {
			ZeroMemory(sz, sizeof(sz));
			wsprintf(sz, L"%d", i);
			WriteIniKeyValue(strFilePath, sz, L"index", vecIndex[i - 1].strIndex);
			WriteIniKeyValue(strFilePath, sz, L"temp", vecIndex[i - 1].strTemp1);
			WriteIniKeyValue(strFilePath, sz, L"tempex", vecIndex[i - 1].strTemp2);
		}
	}
	catch (...) {
		return false;
	}

	return true;
}

/*
	读取日期修改ini
*/
bool tool::FileHelp::ReadDateRecConfig(std::wstring strFilePath, std::vector<DateModifyItem>& vec)
{
	if (_waccess(strFilePath.c_str(), 0) != 0) {
		return false;
	}
	try {
		char sz[128] = { 0 };
		std::wstring strValue;
		int nTotilNum = 0;
		std::wstring strNodeIndex;
		strValue = GetIniKeyValue(strFilePath, L"count", L"total", L"0");
		nTotilNum = _wtoi(strValue.c_str());
		DateModifyItem item;
		std::wstring strDataTmp;
		for (int i = 0; i < nTotilNum; i++) {
			item.Clean();
			strNodeIndex = std::to_wstring(i);
			strDataTmp = GetIniKeyValue(strFilePath, strNodeIndex, L"date");
			item.nAddDate = _wtoi(strDataTmp.c_str());
			strDataTmp = GetIniKeyValue(strFilePath, strNodeIndex, L"file");
			item.strRecFile = PathHelper::Instance()->GetRecDir() + strDataTmp;
			item.strTmpRecFile = PathHelper::Instance()->GetRecDir() + L"m_" + strDataTmp;
			vec.push_back(item);
		}

		return true;
	}
	catch (...) {
		return false;
	}

	return true;
}

bool tool::FileHelp::WriteConfigIniFile(std::wstring strFilePath, std::vector<ContentListItem>& vecIndex)
{
	if (_waccess(strFilePath.c_str(), 0) != 0 || vecIndex.size() == 0) {
		return false;
	}
	try {
	wchar_t sz[128] = { 0 };
	wsprintf(sz, L"%d", vecIndex.size());
	WriteIniKeyValue(strFilePath, L"count", L"total", sz);
	for (int i=1; i<=vecIndex.size(); i++)	{
		ZeroMemory(sz, sizeof(sz));
		wsprintf(sz, L"%d", i);
		WriteIniKeyValue(strFilePath, sz, L"weather", vecIndex[i-1].strWeather);
		WriteIniKeyValue(strFilePath, sz, L"weatherex", vecIndex[i - 1].strWeatherEx);
		WriteIniKeyValue(strFilePath, sz, L"temp", vecIndex[i - 1].strTemp);
		WriteIniKeyValue(strFilePath, sz, L"tempex", vecIndex[i - 1].strTempEx);
		WriteIniKeyValue(strFilePath, sz, L"wind", vecIndex[i - 1].strWind);
		WriteIniKeyValue(strFilePath, sz, L"windex", vecIndex[i - 1].strWindEx);
		WriteIniKeyValue(strFilePath, sz, L"windlv", vecIndex[i - 1].strWindLv);
		WriteIniKeyValue(strFilePath, sz, L"windlvex", vecIndex[i - 1].strWindLvEx);
		WriteIniKeyValue(strFilePath, sz, L"precipitation", vecIndex[i - 1].strPrecipitation);
		::WritePrivateProfileStringA(tool::CodeHelper::UnicodeToUtf8(sz).c_str(), "location", tool::CodeHelper::UnicodeToUtf8(vecIndex[i - 1].strLocation).c_str(), tool::CodeHelper::UnicodeToUtf8(strFilePath).c_str());
		//WriteIniKeyValue(strFilePath, sz, L"location", vecIndex[i - 1].strLocation);
		WriteIniKeyValue(strFilePath, sz, L"rec", vecIndex[i - 1].strRecName);
		WriteIniKeyValue(strFilePath, sz, L"wav", vecIndex[i - 1].strWavName);
	}
	}
	catch (...) {
		return false;
	}

	return true;
}

bool tool::FileHelp::ReadConfigIniFile(std::wstring strFilePath, std::vector<ItemExtern>& vec)
{
	if (_waccess(strFilePath.c_str(), 0) != 0) {
		return false;
	}
	try {
		char sz[128] = { 0 };
		std::wstring strValue;
		int nTotilNum = 0;
		std::wstring strNodeIndex;
		strValue = GetIniKeyValue(strFilePath, L"count", L"total", L"0");
		nTotilNum = _wtoi(strValue.c_str());
		for (int i = 1; i <= nTotilNum; i++){
			ItemExtern item;
			strNodeIndex = std::to_wstring(i);
			GetPrivateProfileStringA(tool::CodeHelper::WStr2Str(strNodeIndex).c_str(), "name", "", sz, 128, tool::CodeHelper::WStr2Str(strFilePath).c_str());
			item.strName = tool::CodeHelper::Utf8ToUnicode(sz);
			item.strCode = GetIniKeyValue(strFilePath, strNodeIndex, L"code");
			item.strWavName = GetIniKeyValue(strFilePath, strNodeIndex, L"wav");
			item.strWavNameEx = GetIniKeyValue(strFilePath, strNodeIndex, L"wavex");
			vec.push_back(item);
		}
		return true;
	}
	catch (...) {
		return false;
	}
}

std::string tool::CodeHelper::UnicodeToUtf8(const std::wstring& wstr)
{
	std::string ret;
	try {
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wcv;
		ret = wcv.to_bytes(wstr);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}

std::wstring tool::CodeHelper::Utf8ToUnicode(const std::string& str)
{
	std::wstring ret;
	try {
		std::wstring_convert<std::codecvt_utf8<wchar_t>> wcv;
		ret = wcv.from_bytes(str);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}


std::string tool::CodeHelper::WStr2Str(const std::wstring& wstr)
{
	USES_CONVERSION;
	return W2A(wstr.c_str());
}

std::wstring tool::CodeHelper::Str2WStr(const std::string& str)
{
	USES_CONVERSION;
	return A2W(str.c_str());
}

std::wstring tool::CodeHelper::trim(const std::wstring& str)
{
	std::wstring result = str;
	// 移除左侧空格
	result.erase(result.begin(), std::find_if(result.begin(), result.end(),
		[](wint_t ch) { return !std::isspace(ch, std::locale()); }));
	// 移除右侧空格
	result.erase(std::find_if(result.rbegin(), result.rend(),
		[](wint_t ch) { return !std::isspace(ch, std::locale()); }).base(), result.end());
	return result;
}

std::vector<std::wstring> tool::CodeHelper::vStringSplit(const std::wstring& s, const std::wstring& delim /*= ","*/)
{
	std::vector<std::wstring> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

void tool::ChinaStringHandle::EnglishToChinese(char* y_s, unsigned short* ll, int i) {
	if ((y_s[i] == 'a') || (y_s[i] == 'A')) { ll[i * 2] = 0xb0a1; ll[i * 2 + 1] = 0xb0c8; }
	if ((y_s[i] == 'b') || (y_s[i] == 'B')) { ll[i * 2] = 0xb0c8; ll[i * 2 + 1] = 0xb2c1; }
	if ((y_s[i] == 'c') || (y_s[i] == 'C')) { ll[i * 2] = 0xb2c1; ll[i * 2 + 1] = 0xb4ee; }
	if ((y_s[i] == 'd') || (y_s[i] == 'D')) { ll[i * 2] = 0xb4ee; ll[i * 2 + 1] = 0xb6ea; }
	if ((y_s[i] == 'e') || (y_s[i] == 'E')) { ll[i * 2] = 0xb6ea; ll[i * 2 + 1] = 0xb7a2; }
	if ((y_s[i] == 'f') || (y_s[i] == 'F')) { ll[i * 2] = 0xb7a2; ll[i * 2 + 1] = 0xb8c1; }
	if ((y_s[i] == 'g') || (y_s[i] == 'G')) { ll[i * 2] = 0xb8c1; ll[i * 2 + 1] = 0xb9fe; }
	if ((y_s[i] == 'h') || (y_s[i] == 'H')) { ll[i * 2] = 0xb9fe; ll[i * 2 + 1] = 0xbbf7; }
	if ((y_s[i] == 'i') || (y_s[i] == 'I')) { ll[i * 2] = 0xbbf7; ll[i * 2 + 1] = 0xbbf7; }
	if ((y_s[i] == 'j') || (y_s[i] == 'J')) { ll[i * 2] = 0xbbf7; ll[i * 2 + 1] = 0xbfa6; }
	if ((y_s[i] == 'k') || (y_s[i] == 'K')) { ll[i * 2] = 0xbfa6; ll[i * 2 + 1] = 0xc0ac; }
	if ((y_s[i] == 'l') || (y_s[i] == 'L')) { ll[i * 2] = 0xc0ac; ll[i * 2 + 1] = 0xc2e8; }
	if ((y_s[i] == 'm') || (y_s[i] == 'M')) { ll[i * 2] = 0xc2e8; ll[i * 2 + 1] = 0xc4c3; }
	if ((y_s[i] == 'n') || (y_s[i] == 'N')) { ll[i * 2] = 0xc4c3; ll[i * 2 + 1] = 0xc5b6; }
	if ((y_s[i] == 'o') || (y_s[i] == 'O')) { ll[i * 2] = 0xc5b6; ll[i * 2 + 1] = 0xc5be; }
	if ((y_s[i] == 'p') || (y_s[i] == 'P')) { ll[i * 2] = 0xc5be; ll[i * 2 + 1] = 0xc6da; }
	if ((y_s[i] == 'q') || (y_s[i] == 'Q')) { ll[i * 2] = 0xc6da; ll[i * 2 + 1] = 0xc8bb; }
	if ((y_s[i] == 'r') || (y_s[i] == 'R')) { ll[i * 2] = 0xc8bb; ll[i * 2 + 1] = 0xc8f6; }
	if ((y_s[i] == 's') || (y_s[i] == 'S')) { ll[i * 2] = 0xc8f6; ll[i * 2 + 1] = 0xcbfa; }
	if ((y_s[i] == 't') || (y_s[i] == 'T')) { ll[i * 2] = 0xcbfa; ll[i * 2 + 1] = 0xcdda; }
	if ((y_s[i] == 'w') || (y_s[i] == 'W')) { ll[i * 2] = 0xcdda; ll[i * 2 + 1] = 0xcef4; }
	if ((y_s[i] == 'x') || (y_s[i] == 'X')) { ll[i * 2] = 0xcef4; ll[i * 2 + 1] = 0xd1b9; }
	if ((y_s[i] == 'y') || (y_s[i] == 'Y')) { ll[i * 2] = 0xd1b9; ll[i * 2 + 1] = 0xd4d1; }
	if ((y_s[i] == 'z') || (y_s[i] == 'Z')) { ll[i * 2] = 0xd4d1; ll[i * 2 + 1] = 0xf9d7; }

}

int tool::ChinaStringHandle::Filter(char* chinastr, unsigned short* ll, int j)
{
	unsigned short ls, i, ls1, ls2;
	for (i = 0; i < j; i++)	{
		ls1 = (unsigned short)chinastr[i * 2 + 1] & 0xff;
		ls2 = (unsigned short)chinastr[i * 2 + 0] & 0xff;
		ls = (ls2 << 8) + ls1;

		if ((ls < ll[i * 2]) || (ls > ll[i * 2 + 1]))
			return 1;  //<=   >=
	}
	return 0;
}

#include <iomanip>
#include <sstream>
#include <iostream>
std::wstring tool::TimeTool::AddDay(int nDayAddNum)
{
	// 获取当前时间
	std::time_t now = std::time(nullptr);
	long seconds = now;
	seconds += nDayAddNum * 24 * 60 * 60; 
	now = seconds;
	// 转换为本地时间
	std::tm* local_time = std::localtime(&now);

	// 打印当前时间
	std::cout << "Current time is: "
		<< (local_time->tm_year + 1900) << "-"
		<< (local_time->tm_mon + 1) << "-"
		<< local_time->tm_mday << " "
		<< local_time->tm_hour << ":"
		<< local_time->tm_min << ":"
		<< local_time->tm_sec << std::endl;

	std::wstringstream wssDate;
	wssDate << std::setfill(L' ') << std::setw(4) << (local_time->tm_year + 1900)<< L"年"<< std::setfill(L' ') << std::setw(2) << (local_time->tm_mon + 1)<< L"月"<< std::setfill(L' ') << std::setw(2) << local_time->tm_mday << L"日";
	std::wstring wszDate = wssDate.str();

	return wszDate;
	/*
	wchar_t wszDate[MAX_PATH] = { 0 };
	wsprintf(wszDate, L"%04d年%02d月%02d日", local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday);
	*/
	return wszDate;
}
