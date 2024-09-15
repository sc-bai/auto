
#pragma once
using namespace std;
class WavHelper {
public:
	static WavHelper* Instance() {
		static WavHelper p;
		return &p;
	}

	WavHelper();
	~WavHelper() {}
	bool BuildWavWithOnceCall(std::vector<ContentListItem>& itemlist);
	bool BuildAndCatWavAll(std::vector<ContentListItem>& itemlist);
	bool BuildAndCatWav(ContentListItem& item);
private:
	bool GetWavFileFromWeatherIndex(std::wstring strIndex1, std::wstring strIndex2, vector<std::wstring>& vec);
	bool GetWavFileFromTempIndex(ContentListItem& item, vector<std::wstring>& vec);
	bool GetWavFileFromWindIndex(std::wstring strIndex1, std::wstring strIndex2, vector<std::wstring>& vec);
	bool GetWavFileFromWindlvIndex(std::wstring strIndex1, std::wstring strIndex2, vector<std::wstring>& vec);


	//std::wstring GetWavFileFromWeatherIndex(std::wstring strIndex1, std::wstring strIndex2);

	//std::wstring GetWavFileFromTempIndex(std::wstring strIndex);
	//std::wstring GetWavFileFromTempIndexEx(std::wstring strIndex);

	//std::wstring GetWavFileFromWindLvIndex(std::wstring strIndex);
	//std::wstring GetWavFileFromWindLvIndexEx(std::wstring strIndex);

	//std::wstring GetWavFileFromWindIndex(std::wstring strIndex);
	//std::wstring GetWavFileFromWindIndexEx(std::wstring strIndex);

	std::wstring GetWavFileFromPrecipitationIndex(std::wstring strIndex);

	std::vector<std::wstring> GetWavFilesFromIndex(ContentListItem& item);
	bool GetWavFilesFromIndexWithCheck(ContentListItem& item, std::vector<std::wstring> &files);
	
	void StartWavCatExe(std::wstring strExecPathFile, std::wstring strCmd);

private:
	bool WriteCmdFile(std::wstring strFile, std::wstring strContent);
private:
	std::wstring m_strWavDir;
	std::vector<std::wstring> m_vecParam;
};
