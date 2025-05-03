#pragma once

#include "hv/requests.h"
#include "hv/axios.h"
using namespace hv;

#include "hv/hthread.h" // import hv_gettid

enum class HTTP_METHOD {
	http_delete = 0,
	http_get = 1,
	http_post = 3,
};

class HvNetManager
{
public:
	static HvNetManager* instance() {
		static HvNetManager h;
		return &h;
	}

	/*
		Õ¨≤Ω«Î«Û
	*/
	bool HttpRequestionSync(std::string strUrl, std::string strBody, std::string& strRespon, HTTP_METHOD nMethod = HTTP_METHOD::http_get, int nWaitTime = 5);
private:
	HttpClient sync_client;
};


