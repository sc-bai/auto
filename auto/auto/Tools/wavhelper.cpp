#include "stdafx.h"
#include "wavhelper.h"

/*
	weather:
			0-19   s0 0000 X s0 1919  总共20*20个 是可以互转的
			S10000~S18181  晴到少云 ~ 小雷阵雨转多云
			s20000~----    文件配置
 
	temp1:	T00600.wav-T00649.wav  0-49°C
			T00651.wav-T00699.wav  -1 - -49°C
			T01700.wav-T01710.wav  -50 - -60°C

	temp2:	T00800.wav-T00849.wav  到0-49°C
			T00851.wav-T00899.wav  到-1 - -49°C
			T00900.wav-T00910.wav  到-50 - -60°C

	wind1:  f70000.wav-f70013.wav  风向
	wind2:	f80000.wav-f80013.wav  转风向

	windlv1:f50000.wav - f50009.wav (一到二级 -- 十到十一级)
			f50010.wav - f50019.wav (一级 -- 十到)
			f50020.wav			   （空）
	windlv2:f60000.wav - f60009.wav (转 一到二级 -- 十到十一级)
			f60010.wav - f60019.wav (转 一级 -- 十到)
			f60020.wav			   （空）

	Precipitation: jsgl00.wav-jsgl10.wav 0%-100%(0 用kk0002.wav)
*/

WavHelper::WavHelper()
{
	m_strWavDir = PathHelper::Instance()->GetWavDir();
	return;
	// 客户要求生成的文件也在wav中
	/*
	m_strWavDir = PathHelper::Instance()->GetWavDir() + L"build\\";
	if (_waccess(m_strWavDir.c_str(), 0) != 0) {
		::CreateDirectory(m_strWavDir.c_str(), nullptr);
	}
	*/
}

bool WavHelper::BuildWavWithOnceCall(std::vector<ContentListItem>& itemlist)
{
	m_vecParam.clear();
	std::vector<std::wstring> OneWavFilesList;
	for (auto& item : itemlist) {
		OneWavFilesList.clear();
		
		if (!GetWavFilesFromIndexWithCheck(item, OneWavFilesList)) {
			continue;
		}

		m_vecParam.push_back(L"buildname##");
		for (auto& file : OneWavFilesList) {
			m_vecParam.push_back(file);
		}
	}

	if (m_vecParam.empty()) {
		return false;
	}
	m_vecParam.push_back(L"buildname##"); // 结束的时候再添加一个
	// start exe
	std::wstring strExecFilePath = PathHelper::Instance()->GetCurrentDir() + L"wavcat2.0.exe ";
	std::wstring strCmd;
	for (auto& item : m_vecParam) {
		strCmd += item;
		strCmd += L" ";
	}
	OutputDebugString(L"[weather]start wav:");
	OutputDebugString(strCmd.c_str());
	OutputDebugString(L"\n");

	//StartWavCatExe(strExecFilePath, strCmd);
	std::wstring strCmdFile = L"cmd.txt";
	if (!WriteCmdFile(strCmdFile, strCmd)) {
		return false;
	}

	StartWavCatExe(strExecFilePath, strCmdFile);
	return true;

}

bool WavHelper::BuildAndCatWavAll(std::vector<ContentListItem>& itemlist)
{
	for (auto & item : itemlist)	{
		BuildAndCatWav(item);
	}
	return true;
}

bool WavHelper::GetWavFileFromWeatherIndex(std::wstring strIndex, std::wstring strIndex2, vector<std::wstring>& vec)
{
	int nIndex1 = _wtoi(strIndex.c_str());
	int nIndex2 = _wtoi(strIndex2.c_str());
	wchar_t sz[24] = { 0 };
	// 0 - 19   s0 0000 ~ s0 1919  总共20 * 20个 是可以互转的
	if (nIndex1 < 19 && nIndex2 < 19) {
		wsprintf(sz, L"S0%02d%02d.wav", nIndex1, nIndex2);
	}
	// S10000 S18181  晴到少云 --- 小雷阵雨转多云
	else if (nIndex1 > 19 && nIndex1 <= 101) {  // nindex1 = nindex2
		wsprintf(sz, L"S1%02d%02d.wav", nIndex1 - 20, nIndex2 - 20);
	}
	else if (nIndex1 == 19 || nIndex2 == 19) {
		//kk0002.wav
		vec.push_back(m_strWavDir + L"kk0002.wav");
		return true;
	}

	// s20000 文件配置
	else {
		int nIndexTmp = nIndex2 - 102;
		if (nIndexTmp < 0 && nIndexTmp > CItemInit::Instance()->m_scWeatherInfoEx.size()) {
			return false;
		}
		vec.push_back(m_strWavDir + CItemInit::Instance()->m_scWeatherInfoEx[nIndexTmp].strWavName);
		return true;
	}
	
	vec.push_back(m_strWavDir + sz);
	return true;
}

bool WavHelper::GetWavFileFromTempIndex(ContentListItem& item, vector<std::wstring>& vec)
{
	auto findtempfun = [&](int Index) -> std::wstring {
		wchar_t wsz[24] = { 0 };
		if (Index <= 49) {
			Index = 49 - Index;
			wsprintf(wsz, L"T006%02d.wav", Index);
		}else if (Index > 49 && Index <= 98) {
			Index++; // T00650 不取
			wsprintf(wsz, L"T006%02d.wav", Index);
		}else if (Index > 98) {
			Index -= 99; 
			wsprintf(wsz, L"T017%02d.wav", Index);
		}
		return wsz;
	};

	auto findtemp2fun = [&](int Index) -> std::wstring {
		wchar_t wsz[24] = { 0 };
		if (Index <= 49) {
			Index = 49 - Index;
			wsprintf(wsz, L"T008%02d.wav", Index);
		}else if (Index > 49 && Index <= 98) {
			Index++; // T00850 不取
			wsprintf(wsz, L"T008%02d.wav", Index);
		}else if (Index > 98) {
			Index -= 99;
			wsprintf(wsz, L"T009%02d.wav", Index);
		}
		return wsz;
	};

	auto findtempexfun = [&](int Index) -> std::wstring {
		wchar_t wsz[24] = { 0 };
		if (Index <= 49) {
			Index = 49 - Index;
			wsprintf(wsz, L"T007%02d.wav", Index);
		}else if (Index > 49 && Index <= 98) {
			Index++; // T00750 不取
			wsprintf(wsz, L"T007%02d.wav", Index);
		}else if (Index > 98) {
			Index -= 99;
			wsprintf(wsz, L"T018%02d.wav", Index);
		}
		return wsz;
	};

	// 最低最高温度现在有个合成的语音。
	// NT3435.WAV  34到35度
	// NT5308.WAV  零下3到8度
	// NT5352.WAV  零下3到零下2度
	// temp  49 ... 0 ... -60
	// index 0  ... 49 ... 109
	auto findtempexfunwithminmax = [&](int Indexmin, int indexmax) -> std::wstring {
		wchar_t wsz[24] = { 0 };

		int minTemp = 0; 
		int maxTemp = 0;
		// 正数或者0
		if (Indexmin <= 49) {
			minTemp = 49 - Indexmin;
		}
		// 负数
		else if (Indexmin > 49 ) {
			minTemp = 49 - Indexmin;
			minTemp = 50 + abs(minTemp);
		}

		// 正数或者0
		if (indexmax <= 49) {
			maxTemp = 49 - indexmax;
		}
		// 负数
		else if (indexmax > 49) {
			maxTemp = 49 - indexmax;
			maxTemp = 50 + abs(maxTemp);
		}
		wsprintf(wsz, L"NT%02d%02d.wav", minTemp, maxTemp);
		return wsz;
	};

	int nIndex = _wtoi(item.strTemp.c_str());
	int nIndex2 = _wtoi(item.strTempEx.c_str());
	int nTemp = nIndex;
	std::wstring strTemp;
	

	// 看是否是新增已存在的情况（一条完整的最低最高语句） 2024 8 11
	if (nIndex != nIndex2) {
		strTemp = findtempexfunwithminmax(nIndex, nIndex2);
		if (_waccess(std::wstring(m_strWavDir + strTemp).c_str(), 0) == 0) {
			vec.push_back(m_strWavDir + strTemp);
			return true;
		}
	}

	int nMost = _wtoi(item.strTempMost.c_str());
	if (nMost == 1) { // 最低温度wd001.wav
		vec.push_back(m_strWavDir + L"wd001.wav");
		strTemp = findtempexfun(nIndex);
		vec.push_back(m_strWavDir + strTemp);
		return true;
	}
	else if (nMost == 2) { // 最高温度wd000.wav
		vec.push_back(m_strWavDir + L"wd000.wav");
		strTemp = findtempexfun(nIndex2);
		vec.push_back(m_strWavDir + strTemp);
		return true;
	}
	else if (nMost == 3) {	// 最低 最高温度
		vec.push_back(m_strWavDir + L"wd001.wav");
		strTemp = findtempexfun(nIndex);
		vec.push_back(m_strWavDir + strTemp);

		vec.push_back(m_strWavDir + L"wd000.wav");
		strTemp = findtempexfun(nIndex2);
		vec.push_back(m_strWavDir + strTemp);
		return true;
	}
	// 温度相等
	if (nIndex == nIndex2) {
		strTemp = findtempexfun(nIndex);
		vec.push_back(m_strWavDir + strTemp);
		return true;
	}

	strTemp = findtempfun(nIndex);
	vec.push_back(m_strWavDir + strTemp);
	strTemp = findtemp2fun(nIndex2);
	vec.push_back(m_strWavDir + strTemp);
	return true;
}

bool WavHelper::GetWavFileFromWindIndex(std::wstring strIndex1, std::wstring strIndex2, vector<std::wstring>& vec)
{
	wchar_t sz[24] = { 0 };
	wchar_t szEx[24] = { 0 };
	bool bEq = false;
	int nIndex = _wtoi(strIndex1.c_str());
	int nIndex2 = _wtoi(strIndex2.c_str());

	bEq = (nIndex == nIndex2) ? true : false;

	if (nIndex < 14) {
		wsprintf(sz, L"f700%02d.wav", nIndex);
		vec.push_back(m_strWavDir + sz);
	}
	else {
		nIndex -= 14;
		if (nIndex<0 || nIndex>CItemInit::Instance()->m_scWindInfoEx.size()) {
			return false;
		}
		vec.push_back(m_strWavDir + CItemInit::Instance()->m_scWindInfoEx[nIndex].strWavName);
	}

	if (bEq) {
		return true;
	}

	if (nIndex2 < 14) {
		wsprintf(szEx, L"f800%02d.wav", nIndex2);
		vec.push_back(m_strWavDir + szEx);
	}
	else {
		nIndex2 -= 14;
		if (nIndex2 < 0 || nIndex2 > CItemInit::Instance()->m_scWindInfoEx.size()) {
			return false;
		}
		vec.push_back(m_strWavDir + CItemInit::Instance()->m_scWindInfoEx[nIndex2].strWavNameEx);
	}

	return true;
}

bool WavHelper::GetWavFileFromWindlvIndex(std::wstring strIndex1, std::wstring strIndex2, vector<std::wstring>& vec)
{
	wchar_t sz[24] = { 0 };
	int nIndex1 = _wtoi(strIndex1.c_str());
	wchar_t szex[24] = { 0 };
	int nIndex2 = _wtoi(strIndex2.c_str());

	if (nIndex1 < 21) {
		wsprintf(sz, L"f500%02d.wav", nIndex1);
		vec.push_back(m_strWavDir + sz);
	}
	else {
		nIndex1 -= 21;
		if (nIndex1<0 || nIndex1>CItemInit::Instance()->m_scWindLvInfoEx.size()) {
			return false;
		}
		vec.push_back(m_strWavDir + CItemInit::Instance()->m_scWindLvInfoEx[nIndex1].strWavName);
	}

	if (nIndex1 == nIndex2) {
		return true;
	}

	if (nIndex2 < 21) {
		wsprintf(szex, L"f600%02d.wav", nIndex2);
		vec.push_back(m_strWavDir + szex);
	}
	else {
		nIndex2 -= 21;
		if (nIndex2<0 || nIndex2>CItemInit::Instance()->m_scWindLvInfoEx.size()) {
			return false;
		}
		vec.push_back(m_strWavDir + CItemInit::Instance()->m_scWindLvInfoEx[nIndex2].strWavNameEx);
	}

	return true;
}
/*
* 
* //temp1:	T00600.wav - T00649.wav  0 - 49
//			T00651.wav - T00699.wav - 1 - -49
//			T01700.wav-T01711.wav  -50 - -60
//
//temp2 :	T00800.wav - T00849.wav  到0 - 49°C
//			T00851.wav - T00899.wav  到 -1 - -49°C
//			T00900.wav - T00910.wav  到 -50 - -60°C

//			t00700.wav - t00749.wav			0-49°C  
//			T00751.wav - T00799.wav		- 1 - -49°C
*/
// jsgl00.wav-jsgl10.wav 0%-100%(0 用kk0002.wav)
std::wstring WavHelper::GetWavFileFromPrecipitationIndex(std::wstring strIndex)
{
	int nIndex = _wtoi(strIndex.c_str());
	wchar_t sz[24] = { 0 };
	if (nIndex == 0) {
		//return L"kk0002.wav";
		return L"jsgl00.wav";
	}
	wsprintf(sz, L"jsgl%02d.wav", nIndex);
	return sz;
}

std::vector<std::wstring> WavHelper::GetWavFilesFromIndex(ContentListItem& item)
{
	bool bRet = false;
	std::vector<std::wstring> buildvec;
	std::wstring strTmp;

	// 23.7.5 修改多个rec文件
	auto strRecNames = tool::CodeHelper::vStringSplit(item.strRecName, L"\\");
	if (strRecNames.size() == 0) {
		return buildvec;
	}
	std::wstring strResultFile = strRecNames[0]; // new wav name = rec.wav
	
	int index = strResultFile.find(L".rec");
	if (index == -1) {
		index = strResultFile.find(L".REC");
		if (index == -1) {
			return buildvec;
		}
	}
		
	strResultFile = strResultFile.substr(0, strResultFile.length() - 4) + L".wav";

	// the first file is build wav file
	buildvec.push_back(m_strWavDir + strResultFile);
	// wav of locatoion
	buildvec.push_back(m_strWavDir + item.strWavName);

	if (!GetWavFileFromWeatherIndex(item.strWeather, item.strWeatherEx, buildvec)) {
		goto exit;
	}
	if (!GetWavFileFromTempIndex(item, buildvec)) {
		goto exit;
	}
	if (!GetWavFileFromWindIndex(item.strWind, item.strWindEx, buildvec)) {
		goto exit;
	}
	if (!GetWavFileFromWindlvIndex(item.strWindLv, item.strWindLvEx, buildvec)) {
		goto exit;
	}

	strTmp = GetWavFileFromPrecipitationIndex(item.strPrecipitation);
	if (strTmp.empty()) {
		goto exit;
	}
	buildvec.push_back(m_strWavDir + strTmp);

	// feat : 每个语音后面新增这个语音 250429
	buildvec.push_back(m_strWavDir + L"kk0030.wav");

	bRet = true;
exit:
	if(bRet)
		return buildvec;

	std::vector<std::wstring> err;
	return err;
}

 bool WavHelper::GetWavFilesFromIndexWithCheck(ContentListItem& item, std::vector<std::wstring>& vecFiles)
{
	bool bRet = false;
	auto files = GetWavFilesFromIndex(item);
	if (files.size() == 0) {
		OutputDebugString(L"[weather]get wav files from index size=0\n");
		return false;
	}

	int i = 0;
	for (auto& itemfile : files) {
		if (i == 0) {
			i++;
			continue;// the first file is build file, so it will not exist
		}

		if (_waccess(itemfile.c_str(), 0) != 0) {
			OutputDebugString(L"[weather]get wav file not exist");
			OutputDebugString(itemfile.c_str());
			OutputDebugString(L"\n");

			MessageBox(nullptr, std::wstring(itemfile + L"不存在").c_str(), L"提示",  MB_OK);
			return false;
		}
	}
	vecFiles = files;
	return true;
}


bool WavHelper::BuildAndCatWav(ContentListItem& item)
{
	try{
	bool bRet = false;
	auto files = GetWavFilesFromIndex(item);
	if (files.size() == 0) {
		OutputDebugString(L"[weather]get wav files from index size=0\n");
		return false;
	}

	int i = 0;
	for (auto &itemfile : files){
		if (i == 0) {
			i++;
			continue;// the first file is build file, so it will not exist
		}
		
		if (_waccess(itemfile.c_str(), 0) != 0) {
			OutputDebugString(L"[weather]get wav file not exist");
			OutputDebugString(itemfile.c_str());
			OutputDebugString(L"\n");
			return false;
		}
	}

	std::wstring strExecFilePath = PathHelper::Instance()->GetCurrentDir() + L"wavcat2.0.exe ";
	std::wstring strCmd;
	for (auto &item : files) {
		strCmd += item;
		strCmd += L" ";
	}
	OutputDebugString(L"[weather]start wav:");
	OutputDebugString(strCmd.c_str());
	OutputDebugString(L"\n");

	StartWavCatExe(strExecFilePath, strCmd);
	return true;
	}
	catch (...) {
		return false;
	}
}

void WavHelper::StartWavCatExe(std::wstring strExecPathFile, std::wstring strCmd)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(STARTUPINFOW);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_HIDE;//SW_SHOW;SW_HIDE;

	ZeroMemory(&processInfo, sizeof(processInfo));

	if (CreateProcess(
		strExecPathFile.c_str(),        
		(LPWSTR)strCmd.c_str(),         
		NULL,							
		NULL,                     
		FALSE,                    
		0,                        
		NULL,                     
		NULL,                     
		&startupInfo,
		&processInfo)) {

		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
	else {
		OutputDebugString(L"[weather]wav exe createprocess error");
	}
}

bool WavHelper::WriteCmdFile(std::wstring strFile, std::wstring strContent)
{
	wofstream ofs;
	ofs.open(strFile.c_str(), ios::out | ios::trunc);

	if (ofs.is_open()) {
		ofs << strContent.c_str();
		ofs.close();
		return true;
	}

	return false;
}

