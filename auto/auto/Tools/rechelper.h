#pragma once
#include "stdafx.h"
class RECHelper {
public:
	static RECHelper* Instance(){
		static RECHelper p;
		return &p;
	}
	RECHelper();
	~RECHelper() = default;
	bool ModifyRecFileAll(std::vector<ContentListItem>& vec);
	bool ModifyRecFile(ContentListItem& index, std::wstring strRecFile);
	bool ModifyRecFileEx(ContentListItem& index);

	void ModifyRecFileWithDate(std::vector<DateModifyItem>& vec);
private:
	std::vector<std::string> Spliter4To2(std::string str); //0101->[01,01]
	std::vector<std::string> GetWeatherModifyTgaFromItem(ContentListItem& index);
	std::vector<std::string> GetWindAndLevelModifyTgaFromItem(ContentListItem& index);

	void HandleWeatherIndex(unsigned char* buf, std::vector<std::string>& mdfvec, int nIndexgta, int ntotalgta);
	void HandleWindAndLevelIndex(unsigned char* buf, std::vector<std::string>& mdfvec, int nIndexgta, int ntotalgta);
	
	void HandleTempIndex(unsigned char* buf, ContentListItem& index, int flag, char cfalg);

	void HandleTempIndexSignal(unsigned char* buf, ContentListItem& index, int flag, char cfalg); // 处理单温度调整为多温度
	void ModifyRecBuffer(ContentListItem& index, unsigned char* recbuf, int nSize);
	void HandleJSGVIndex(ContentListItem& index, unsigned char* buff);

	void ModifySingleRecDateFile(DateModifyItem& item);
	void RealModifyDateFunction(DateModifyItem& item, unsigned char* buf, int filesize);
private:
	std::wstring m_strRecFile;
};