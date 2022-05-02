#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <winerror.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <wchar.h>
#include <list>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

using namespace std;

typedef struct AdapterInfo {
	WCHAR wsFriendlyName[128];
	WCHAR wsDesc[256];
	char sIP[64];
	char sMac[64];
	char sMask[64];
}AI, *PAI;

class NetAdapterInfo {
private:
	list<PAI> m_adapterInfoList;
	PIP_ADAPTER_ADDRESSES m_pAddrs;
public:
	NetAdapterInfo();

	void GetAdaptersAddrs();
	void GetAdaptersInfo();
	int GetAdaptersCount();
	void PrintAdaptersInfo();
	
	~NetAdapterInfo();
};