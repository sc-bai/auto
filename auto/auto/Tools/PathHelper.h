#pragma once
#include "stdafx.h"

class PathHelper
{
public:
	static PathHelper* Instance() {
		static PathHelper p;
		return &p;
	}
	PathHelper() {
		wchar_t path[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, path, MAX_PATH);
		m_strExeDir = path;
		int offset = m_strExeDir.rfind(L"\\");
		if (0 >= offset)
			return;
		m_strExeDir = m_strExeDir.substr(0, offset + 1);
	}
	std::wstring GetCurrentDir() {
		return m_strExeDir;
	}
	std::wstring GetDataDir() {
		return m_strExeDir + L"dat\\";
	}
	std::wstring GetLibDir() {
		return m_strExeDir + L"lib\\";
	}
	std::wstring GetWavDir() {
		return m_strExeDir + L"wav\\";
	}
	std::wstring GetTTSWavDir() {
		return m_strExeDir + L"tts\\wav\\";
	}
	std::wstring GetRecDir() {
		return m_strExeDir + L"rec\\";
	}

	std::wstring GetTTSConfig() {
		return m_strExeDir + L"tts_config.ini";
	}

	void SetCurrentModifyFile(std::wstring str) {
		m_strTempModifyFile = str;
	}
	std::wstring GetModifyTempFile() {
		return m_strTempModifyFile;
	}
	std::wstring GetDateModifyIniFile() {
		return m_strExeDir + L"DateModify.ini";
	}
	std::wstring get_accented_word_file() {
		return m_strExeDir + L"accented_word.txt";
	}
	std::wstring get_tts_auto_open_file() {
		return m_strExeDir + L"auto2.0.txt";
	}

private:
	std::wstring m_strExeDir;
	std::wstring m_strTempModifyFile;
};