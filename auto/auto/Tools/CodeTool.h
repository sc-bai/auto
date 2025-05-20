#pragma once

#include <string>
#include <vector>
#include <iosfwd>
#include <tchar.h>

using namespace std;

class MD5
{
public:
	typedef unsigned int size_type; // must be 32bit  

	MD5();
	MD5(const std::string& text);
	void update(const unsigned char *buf, size_type length);
	void update(const char *buf, size_type length);
	MD5& finalize();
	std::string hexdigest() const;
	friend std::ostream& operator<<(std::ostream&, MD5 md5);

private:
	void init();
	typedef unsigned char uint1; //  8bit  
	typedef unsigned int uint4;  // 32bit  
	enum { blocksize = 64 }; // VC6 won't eat a const static int here  

	void transform(const uint1 block[blocksize]);
	static void decode(uint4 output[], const uint1 input[], size_type len);
	static void encode(uint1 output[], const uint4 input[], size_type len);

	bool finalized;
	uint1 buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk  
	uint4 count[2];   // 64bit counter for number of bits (lo, hi)  
	uint4 state[4];   // digest so far  
	uint1 digest[16]; // the result  

	// low level logic operations  
	static inline uint4 F(uint4 x, uint4 y, uint4 z);
	static inline uint4 G(uint4 x, uint4 y, uint4 z);
	static inline uint4 H(uint4 x, uint4 y, uint4 z);
	static inline uint4 I(uint4 x, uint4 y, uint4 z);
	static inline uint4 rotate_left(uint4 x, int n);
	static inline void FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
};

class CodeTool
{
public:
	static string GbkToUtf8(const char* src_str);
	static string Utf8ToGbk(const char* src_str);

	static string EncodeBase64(const unsigned char * str, int bytes);
	static string DecodeBase64(const char *str, int bytes);
	static void DecodeBase64(const char *str, int bytes, char*& dest, int& len);

	static std::wstring Str2WStr(const string& str);
	static std::string WStr2Str(const wstring& wstr);

	static const string UnicodeToUtf8(const std::wstring & wstr);
	static const wstring Utf8ToUnicode(const std::string & str);

	static string md5(const std::string str);

	static void ReadRegEditToPluGinVersion(const std::wstring& strPluGinName, std::wstring& strVersion);
	static void WriteRegEditToPluGinVersion(const std::wstring& strPluGinName, const std::wstring& strVersion);
	static void CreateRegEditPluGinMan(const std::wstring& strPluGinName, const std::wstring& strVersion);
	static string& Replace_all(string& src, const string& old_value, const string& new_value);
};