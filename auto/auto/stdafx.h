#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <time.h>
#include <fstream>
#include <tchar.h>

struct ContentListItem;
struct WHListItem;
struct ItemExtern;
struct TempItem;
struct DateModifyItem;

#include "Tools/PathHelper.h"
#include "Tools/Tools.h"
#include "Tools/WTInit.h"

enum class ModifyType {
	type_weather = 1,
	type_weatherex,
	type_temp,
	type_tempex,
	type_wind,
	type_windex,
	type_windlv,
	type_windlvex,
	type_precipitation
};

struct ContentListItem
{
	bool bChecked;
	std::wstring strWeather;
	std::wstring strWeatherEx;
	std::wstring strTemp;
	std::wstring strTempEx;
	std::wstring strTempMost;
	std::wstring strWind;
	std::wstring strWindEx;
	std::wstring strWindLv;
	std::wstring strWindLvEx;
	std::wstring strPrecipitation; // ��ˮ����
	std::wstring strLocation;
	std::wstring strRecName;
	std::wstring strWavName;

	ContentListItem(){
		Clean();
	}
	void Clean() {
		bChecked = false;
		strWeather = L"";
		strWeatherEx = L"";
		strTemp = L"";
		strTempEx = L"";
		strWind = L"";
		strWindEx = L"";
		strPrecipitation = L"";
		strLocation = L"";
		strRecName = L"";
		strWavName = L"";
	}
};

struct WHListItem
{
	std::wstring strNum;		// ʵ�ʱ�ʾ������ ��ini�е��������Ӧ
	std::wstring strName;
	std::wstring strWav;
	std::wstring strTga;

	WHListItem() {
		Clean();
	}
	void Clean() {
		strNum = L"";
		strName = L"";
		strWav = L"";
		strTga = L"";
	}
};

// list ��չ�б�
struct ItemExtern {
	std::wstring strName;
	std::wstring strCode;
	std::wstring strWavName;
	std::wstring strWavNameEx;	// �¶Ⱥͷ���ĵڶ���wav
	ItemExtern() {
		Clean();
	}
	void Clean() {
		strName = L"";
		strCode = L"";
		strWavName = L"";
		strWavNameEx = L"";
	}
};

struct TempItem
{
	std::wstring strIndex;
	std::wstring strTemp1;
	std::wstring strTemp2;
	TempItem() {
		Clean();
	}
	void Clean() {
		strIndex = L"";
		strTemp1 = L"";
		strTemp2 = L"";
	}
};


struct DateModifyItem
{
	int nAddDate; // 0��ǰ���� -1 ǰһ��  1 ���� 2���� ..
	std::wstring strRecFile;
	std::wstring strTmpRecFile;
	DateModifyItem() {
		Clean();
	}
	void Clean() {
		nAddDate = 0;
		strRecFile = L"";
		strTmpRecFile = L"";
	}
};

