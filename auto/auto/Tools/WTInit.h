#pragma once

#define MAX_TEMP 49
#define MIN_TEMP (-60)

class CItemInit {
public:
	static CItemInit* Instance() {
		static CItemInit p;
		return &p;
	}
	void AppInit();
public:
	std::vector<std::wstring> g_scWeatherName;
	std::vector<std::wstring> g_scWindName;
	std::vector<std::wstring> g_scTempName;
	std::vector<std::wstring> g_scWindLv;
	std::vector<std::wstring> g_scPrecipitationName;

 // ÍØÕ¹µÄlist item
	std::vector<ItemExtern> m_scWeatherInfoEx;	
	std::vector<ItemExtern> m_scWindInfoEx;		
	std::vector<ItemExtern> m_scWindLvInfoEx;	

	std::vector<std::string> m_tag_weather;

	char m_sz_tga_wind[100] = { 0 };
	char m_sz_tga_windlv[100] = { 0 };

// temp modify
	std::vector<TempItem> m_scTempModify;
};
