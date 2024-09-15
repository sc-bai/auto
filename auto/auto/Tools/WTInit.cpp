#include "stdafx.h"
#include "WTInit.h"

void CItemInit::AppInit()
{
	g_scWeatherName.push_back(L"晴天");
	g_scWeatherName.push_back(L"多云");
	g_scWeatherName.push_back(L"阴天");
	g_scWeatherName.push_back(L"小雨");
	g_scWeatherName.push_back(L"中雨");
	g_scWeatherName.push_back(L"大雨");
	g_scWeatherName.push_back(L"暴雨");
	g_scWeatherName.push_back(L"雷阵雨");
	g_scWeatherName.push_back(L"冰雹");
	g_scWeatherName.push_back(L"冻雨");
	g_scWeatherName.push_back(L"雨夹雪");
	g_scWeatherName.push_back(L"小雪");
	g_scWeatherName.push_back(L"中雪");
	g_scWeatherName.push_back(L"大雪");
	g_scWeatherName.push_back(L"暴雪");
	g_scWeatherName.push_back(L"雾");
	g_scWeatherName.push_back(L"霜");
	g_scWeatherName.push_back(L"霾");
	g_scWeatherName.push_back(L"雾霾");
	g_scWeatherName.push_back(L"空");
	g_scWeatherName.push_back(L"晴到少云");
	g_scWeatherName.push_back(L"晴间多云");
	g_scWeatherName.push_back(L"晴到多云");
	g_scWeatherName.push_back(L"晴转多云到阴");
	g_scWeatherName.push_back(L"晴有霜");
	g_scWeatherName.push_back(L"晴有雾");
	g_scWeatherName.push_back(L"少云到多云");
	g_scWeatherName.push_back(L"多云间晴");
	g_scWeatherName.push_back(L"多云间阴");
	g_scWeatherName.push_back(L"多云到阴");
	g_scWeatherName.push_back(L"多云有小阵雨");
	g_scWeatherName.push_back(L"阴到多云");
	g_scWeatherName.push_back(L"阴天间多云");
	g_scWeatherName.push_back(L"阴有零星小雨");
	g_scWeatherName.push_back(L"阴有零星小雨转晴");
	g_scWeatherName.push_back(L"阴有零星小阵雨转晴");
	g_scWeatherName.push_back(L"阴有零星雨夹雪");
	g_scWeatherName.push_back(L"阴有小阵雨");
	g_scWeatherName.push_back(L"阴有小阵雨转晴");
	g_scWeatherName.push_back(L"阴有小阵雨夹雪");
	g_scWeatherName.push_back(L"阴有小到中雨部分地区有大雨");
	g_scWeatherName.push_back(L"阴有小雨");
	g_scWeatherName.push_back(L"阴有小雨部分地区有中雨");
	g_scWeatherName.push_back(L"阴有小雨转大雨");
	g_scWeatherName.push_back(L"阴有小雨转暴雨");
	g_scWeatherName.push_back(L"阴有小雨夹雪");
	g_scWeatherName.push_back(L"阴有小到中雨");
	g_scWeatherName.push_back(L"阴有小雷阵雨");
	g_scWeatherName.push_back(L"阴有小到中雷阵雨");
	g_scWeatherName.push_back(L"阴有小阵雨");
	g_scWeatherName.push_back(L"阴有中雨");
	g_scWeatherName.push_back(L"阴有中到大雨");
	g_scWeatherName.push_back(L"阴有中雨局部地区有大雨");
	g_scWeatherName.push_back(L"阴有中到大雨局部地区有暴雨");
	g_scWeatherName.push_back(L"阴有中雨转暴雨");
	g_scWeatherName.push_back(L"阴有中到大雷阵雨");
	g_scWeatherName.push_back(L"阴有大雨");
	g_scWeatherName.push_back(L"阴有大到暴雨");
	g_scWeatherName.push_back(L"阴有暴雨");
	g_scWeatherName.push_back(L"阴有雨夹雪");
	g_scWeatherName.push_back(L"阴有冻雨");
	g_scWeatherName.push_back(L"阴有大雨局部地区有暴雨");
	g_scWeatherName.push_back(L"阴有雨转雾");
	g_scWeatherName.push_back(L"阴有雾转晴");
	g_scWeatherName.push_back(L"阴有雷阵雨转晴");
	g_scWeatherName.push_back(L"阴有零星小雪");
	g_scWeatherName.push_back(L"阴有小雪");
	g_scWeatherName.push_back(L"阴有小到中雪");
	g_scWeatherName.push_back(L"阴有中雪");
	g_scWeatherName.push_back(L"阴有中到大雪");
	g_scWeatherName.push_back(L"阴有大雪");
	g_scWeatherName.push_back(L"阴有大到暴雪");
	g_scWeatherName.push_back(L"阴有暴雪");
	g_scWeatherName.push_back(L"阴有大雪局部地区有暴雪");
	g_scWeatherName.push_back(L"零星小雨");
	g_scWeatherName.push_back(L"小到中雨");
	g_scWeatherName.push_back(L"中到大雨");
	g_scWeatherName.push_back(L"大到暴雨");
	g_scWeatherName.push_back(L"阵雨");
	g_scWeatherName.push_back(L"小雷阵雨");
	g_scWeatherName.push_back(L"小到中雷阵雨");
	g_scWeatherName.push_back(L"中雷阵雨");
	g_scWeatherName.push_back(L"零星小雪");
	g_scWeatherName.push_back(L"小到中雪");
	g_scWeatherName.push_back(L"中到大雪");
	g_scWeatherName.push_back(L"大到暴雪");
	g_scWeatherName.push_back(L"零星雨夹雪");
	g_scWeatherName.push_back(L"小雨夹雪");
	g_scWeatherName.push_back(L"小到中雨夹雪");
	g_scWeatherName.push_back(L"中雨夹雪");
	g_scWeatherName.push_back(L"中到大雨夹雪");
	g_scWeatherName.push_back(L"暴雨到大暴雨");
	g_scWeatherName.push_back(L"特大暴雨");
	g_scWeatherName.push_back(L"零星小雨夹雪");
	g_scWeatherName.push_back(L"零星小阵雪");
	g_scWeatherName.push_back(L"零星小阵雨");
	g_scWeatherName.push_back(L"小阵雨夹雪");
	g_scWeatherName.push_back(L"小阵雪");
	g_scWeatherName.push_back(L"大雪局部地区有暴雪");
	g_scWeatherName.push_back(L"零星小雨转多云");
	g_scWeatherName.push_back(L"小阵雨转多云");
	g_scWeatherName.push_back(L"小雷阵雨转多云");

	if (tool::FileHelp::ReadConfigIniFile(std::wstring(PathHelper::Instance()->GetLibDir() + L"wavef10.ini"), m_scWeatherInfoEx)) {
		for (int i = 0; i < m_scWeatherInfoEx.size(); i++) {
			g_scWeatherName.push_back(m_scWeatherInfoEx[i].strName);
		}
	}


	g_scWindName.push_back(L"北风");
	g_scWindName.push_back(L"东北风");
	g_scWindName.push_back(L"东风");
	g_scWindName.push_back(L"东南风");
	g_scWindName.push_back(L"南风");
	g_scWindName.push_back(L"西南风");
	g_scWindName.push_back(L"西风");
	g_scWindName.push_back(L"西北风");
	g_scWindName.push_back(L"");
	g_scWindName.push_back(L"偏北风");
	g_scWindName.push_back(L"偏东风");
	g_scWindName.push_back(L"偏南风");
	g_scWindName.push_back(L"偏西风");
	g_scWindName.push_back(L"风力");

	if (tool::FileHelp::ReadConfigIniFile(std::wstring(PathHelper::Instance()->GetLibDir() + L"fx.ini"), m_scWindInfoEx)) {
		for (int i = 0; i < m_scWindInfoEx.size(); i++) {
			g_scWindName.push_back(m_scWindInfoEx[i].strName);
		}
	}

	wchar_t sz[24] = { 0 };
	for (int i = MAX_TEMP; i >= 0; i--) {
		ZeroMemory(sz, sizeof(sz));
		wsprintf(sz, L"%d度", i);
		g_scTempName.push_back(sz);
	}

	for (int i = -1; i >= MIN_TEMP; i--)
	{
		ZeroMemory(sz, sizeof(sz));
		wsprintf(sz, L"%d度", i);
		g_scTempName.push_back(sz);
	}

	g_scWindLv.push_back(L"一二级");
	g_scWindLv.push_back(L"二三级");
	g_scWindLv.push_back(L"三四级");
	g_scWindLv.push_back(L"四五级");
	g_scWindLv.push_back(L"五六级");
	g_scWindLv.push_back(L"六七级");
	g_scWindLv.push_back(L"七八级");
	g_scWindLv.push_back(L"八九级");
	g_scWindLv.push_back(L"九十级");
	g_scWindLv.push_back(L"十到十一级");
	g_scWindLv.push_back(L"一级");
	g_scWindLv.push_back(L"二级");
	g_scWindLv.push_back(L"三级");
	g_scWindLv.push_back(L"四级");
	g_scWindLv.push_back(L"五级");
	g_scWindLv.push_back(L"六级");
	g_scWindLv.push_back(L"七级");
	g_scWindLv.push_back(L"八级");
	g_scWindLv.push_back(L"九级");
	g_scWindLv.push_back(L"十级");
	g_scWindLv.push_back(L"空");

	if (tool::FileHelp::ReadConfigIniFile(std::wstring(PathHelper::Instance()->GetLibDir() + L"fd.ini"), m_scWindLvInfoEx)) {
		for (int i = 0; i < m_scWindLvInfoEx.size(); i++) {
			g_scWindLv.push_back(m_scWindLvInfoEx[i].strName);
		}
	}

	g_scPrecipitationName.push_back(L"0%");
	g_scPrecipitationName.push_back(L"10%");
	g_scPrecipitationName.push_back(L"20%");
	g_scPrecipitationName.push_back(L"30%");
	g_scPrecipitationName.push_back(L"40%");
	g_scPrecipitationName.push_back(L"50%");
	g_scPrecipitationName.push_back(L"60%");
	g_scPrecipitationName.push_back(L"70%");
	g_scPrecipitationName.push_back(L"80%");
	g_scPrecipitationName.push_back(L"90%");
	g_scPrecipitationName.push_back(L"100%");

	// tga
//00晴01多云02阴03小雨04 中雨05大雨06暴雨07雷阵雨08冰查10雨加雪11小雪12中雪13雪14暴雪15雾16霜17霾18雾霾
	char sztga_wh[24] = { 0 };
	for (int i=0;i<19;i++)	{
		ZeroMemory(sztga_wh, sizeof(sztga_wh));
		sprintf(sztga_wh, "%02d", i);
		m_tag_weather.push_back(sztga_wh);
	}
	m_tag_weather.push_back("kk");	//空
	m_tag_weather.push_back("0001");
	m_tag_weather.push_back("0001");
	m_tag_weather.push_back("0001");
	m_tag_weather.push_back("0102");
	m_tag_weather.push_back("0016");
	m_tag_weather.push_back("0015");
	m_tag_weather.push_back("0102");
	m_tag_weather.push_back("0100");
	m_tag_weather.push_back("0102");
	m_tag_weather.push_back("0102");
	m_tag_weather.push_back("0103");
	m_tag_weather.push_back("0201");
	m_tag_weather.push_back("0201");
	m_tag_weather.push_back("0203");
	m_tag_weather.push_back("0300");
	m_tag_weather.push_back("0300");
	m_tag_weather.push_back("0210");
	m_tag_weather.push_back("0203");
	m_tag_weather.push_back("0300");
	m_tag_weather.push_back("0210");
	m_tag_weather.push_back("0305");
	m_tag_weather.push_back("0203");
	m_tag_weather.push_back("0204");
	m_tag_weather.push_back("0205");
	m_tag_weather.push_back("0206");
	m_tag_weather.push_back("0210");
	m_tag_weather.push_back("0204");
	m_tag_weather.push_back("0207");
	m_tag_weather.push_back("0207");
	m_tag_weather.push_back("0203");
	m_tag_weather.push_back("0204");
	m_tag_weather.push_back("0205");
	m_tag_weather.push_back("0205");
	m_tag_weather.push_back("0205");
	m_tag_weather.push_back("0205");
	m_tag_weather.push_back("0205");
	m_tag_weather.push_back("0205");
	m_tag_weather.push_back("0205");
	m_tag_weather.push_back("0205");
	m_tag_weather.push_back("0210");
	m_tag_weather.push_back("0209");
	m_tag_weather.push_back("0206");
	m_tag_weather.push_back("0203");
	m_tag_weather.push_back("0201");
	m_tag_weather.push_back("0700");
	m_tag_weather.push_back("0211");
	m_tag_weather.push_back("0211");
	m_tag_weather.push_back("0212");
	m_tag_weather.push_back("0212");
	m_tag_weather.push_back("0212");
	m_tag_weather.push_back("0213");
	m_tag_weather.push_back("0214");
	m_tag_weather.push_back("0214");
	m_tag_weather.push_back("0214");
	m_tag_weather.push_back("0303");
	m_tag_weather.push_back("0304");
	m_tag_weather.push_back("0405");
	m_tag_weather.push_back("0506");
	m_tag_weather.push_back("0303");
	m_tag_weather.push_back("0707");
	m_tag_weather.push_back("0307");
	m_tag_weather.push_back("0707");
	m_tag_weather.push_back("1111");
	m_tag_weather.push_back("1112");
	m_tag_weather.push_back("1213");
	m_tag_weather.push_back("1314");
	m_tag_weather.push_back("1010");
	m_tag_weather.push_back("1010");
	m_tag_weather.push_back("1010");
	m_tag_weather.push_back("1010");
	m_tag_weather.push_back("0410");
	m_tag_weather.push_back("0506");
	m_tag_weather.push_back("0606");
	m_tag_weather.push_back("1010");
	m_tag_weather.push_back("1111");
	m_tag_weather.push_back("0303");
	m_tag_weather.push_back("1010");
	m_tag_weather.push_back("1111");
	m_tag_weather.push_back("1314");
	m_tag_weather.push_back("0302");
	m_tag_weather.push_back("0302");
	m_tag_weather.push_back("0702");

	m_sz_tga_wind[0] = '0';
	m_sz_tga_wind[1] = '1';
	m_sz_tga_wind[2] = '2';
	m_sz_tga_wind[3] = '3';
	m_sz_tga_wind[4] = '4';
	m_sz_tga_wind[5] = '5';
	m_sz_tga_wind[6] = '6';
	m_sz_tga_wind[7] = '7';
	m_sz_tga_wind[8] = 'k';
	m_sz_tga_wind[9] = '1';
	m_sz_tga_wind[10] = '3';
	m_sz_tga_wind[11] = '5';
	m_sz_tga_wind[12] = '7';
	m_sz_tga_wind[13] = 'k';

	std::string strWindInfoTemp;
	int index_wind = 14;
	for (int i=0;i< m_scWindInfoEx.size();i++)
	{
		strWindInfoTemp = tool::CodeHelper::WStr2Str(m_scWindInfoEx[i].strCode);
		m_sz_tga_wind[index_wind] = strWindInfoTemp[0];
		index_wind++;
	}

	m_sz_tga_windlv[0] = '0';
	m_sz_tga_windlv[1] = '1';
	m_sz_tga_windlv[2] = '1';
	m_sz_tga_windlv[3] = '2';
	m_sz_tga_windlv[4] = '3';
	m_sz_tga_windlv[5] = '3';
	m_sz_tga_windlv[6] = '4';
	m_sz_tga_windlv[7] = '4';
	m_sz_tga_windlv[8] = '5';
	m_sz_tga_windlv[9] = '5';
	m_sz_tga_windlv[10] = '0';
	m_sz_tga_windlv[11] = '1';
	m_sz_tga_windlv[12] = '1';
	m_sz_tga_windlv[13] = '2';
	m_sz_tga_windlv[14] = '2';
	m_sz_tga_windlv[15] = '3';
	m_sz_tga_windlv[16] = '4';
	m_sz_tga_windlv[17] = '4';
	m_sz_tga_windlv[18] = '5';
	m_sz_tga_windlv[19] = '5';
	m_sz_tga_windlv[20] = 'k';

	std::string strWindLvInfoTemp;
	int index_wind_lv = 21;
	for (int i = 0; i < m_scWindLvInfoEx.size(); i++)	{
		strWindLvInfoTemp = tool::CodeHelper::WStr2Str(m_scWindLvInfoEx[i].strCode);
		m_sz_tga_windlv[index_wind_lv] = strWindLvInfoTemp[0];
		index_wind_lv++;
	}
}
