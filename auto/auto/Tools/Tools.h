#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
namespace tool {

	class FileHelp {
	public:
		static std::wstring GetIniKeyValue(std::wstring strFilePath, std::wstring strNode, std::wstring strKey, std::wstring strValue = L"");
		static bool WriteIniKeyValue(std::wstring strFilePath, std::wstring strNode, std::wstring strKey, std::wstring strValue);

		// 操作的ini文件
		static bool ReadConfigIniFile(std::wstring strFilePath, std::vector<ContentListItem>& vec);
		static bool WriteConfigIniFile(std::wstring strFilePath, std::vector<ContentListItem>& vecIndex);

		// list Item拓展文件
		static bool ReadConfigIniFile(std::wstring strFilePath, std::vector<ItemExtern>& vec);

		// modify temp
		static bool ReadTempConfigFle(std::wstring strFilePath, std::vector<TempItem>& vec);
		static bool WriteTempConfigFle(std::wstring strFilePath, std::vector<TempItem>& vecIndex);

		// date modify
		static bool ReadDateRecConfig(std::wstring strFilePath, std::vector<DateModifyItem>& vec);
	};

	class CodeHelper {
	public:
		static std::string UnicodeToUtf8(const std::wstring& str);
		static std::wstring Utf8ToUnicode(const std::string& str);
		static std::string WStr2Str(const std::wstring& wstr);
		static std::wstring Str2WStr(const std::string& str);
		static std::wstring trim(const std::wstring& str);
		static std::vector<std::wstring> vStringSplit(const std::wstring& s, const std::wstring& delim /*= ","*/);
	};

	class ChinaStringHandle {
	public:
		// y_s：英文字符串   ll：中文字符串 i:英文长度
		static	void EnglishToChinese(char* y_s, unsigned short* ll, int i);
		// ll：英文转中文的字符串   chinastr：中文字符串 i:英文长度
		static int Filter(char* chinastr, unsigned short* ll, int j);
	};

	class TimeTool {
	public:
		static std::wstring AddDay(int nDayAddNum);
	};
};