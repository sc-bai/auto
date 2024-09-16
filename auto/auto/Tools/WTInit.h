#pragma once
#include <map>
#define MAX_TEMP 49
#define MIN_TEMP (-60)

/*
	连个天气和成
*/
struct stWeatherInfo
{
	std::wstring strWeather;
	std::wstring strWeatherEx;
};
class CItemInit {
public:
	static CItemInit* Instance() {
		static CItemInit p;
		return &p;
	}
	void AppInit();
public:
	std::map<std::wstring , stWeatherInfo> g_scWeatherMap;
	std::vector<std::wstring> g_scWeatherName;
	std::vector<std::wstring> g_scWindName;
	std::vector<std::wstring> g_scTempName;
	std::vector<std::wstring> g_scWindLv;
	std::vector<std::wstring> g_scPrecipitationName;

 // 拓展的list item
	std::vector<ItemExtern> m_scWeatherInfoEx;	
	std::vector<ItemExtern> m_scWindInfoEx;		
	std::vector<ItemExtern> m_scWindLvInfoEx;	

	std::vector<std::string> m_tag_weather;

	char m_sz_tga_wind[100] = { 0 };
	char m_sz_tga_windlv[100] = { 0 };

// temp modify
	std::vector<TempItem> m_scTempModify;
};
