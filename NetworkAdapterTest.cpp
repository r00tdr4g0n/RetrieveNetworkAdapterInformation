#include "NetworkAdapterTest.h"

NetAdapterInfo::NetAdapterInfo()
{
	m_adapterInfoList.clear();
	m_pAddrs = NULL;
}

NetAdapterInfo::~NetAdapterInfo()
{
	list<PAI>::iterator iter;

	while (m_adapterInfoList.empty() == false) { 
		iter = m_adapterInfoList.begin();
		
		if (*iter) {
			free(*iter);
			m_adapterInfoList.erase(iter);
		}
	}
}

void NetAdapterInfo::GetAdaptersAddrs()
{
	unsigned long ulRet;
	unsigned long ulBufSize = sizeof(IP_ADAPTER_ADDRESSES);

	m_pAddrs = (PIP_ADAPTER_ADDRESSES)malloc(ulBufSize);

	if (!m_pAddrs) {
		return;
	}

	ulRet = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, m_pAddrs, &ulBufSize);
	
	if (ulRet == ERROR_BUFFER_OVERFLOW) {
		free(m_pAddrs);

		m_pAddrs = (PIP_ADAPTER_ADDRESSES)malloc(ulBufSize);
		
		if (!m_pAddrs) {
			return;
		}
	}

	ulRet = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, m_pAddrs, &ulBufSize);

	if (ulRet != NO_ERROR) {
		free(m_pAddrs);

		return;
	}
}

void NetAdapterInfo::GetAdaptersInfo()
{
	PIP_ADAPTER_ADDRESSES pTmp = m_pAddrs;
	PIP_ADAPTER_UNICAST_ADDRESS pUniAddr = NULL;
	PIP_ADAPTER_PREFIX pPrefix = NULL;
	PAI pAI = NULL;
	ULONG ulMask = NULL;
	struct sockaddr_in* pAddr;

	while (pTmp) {
		if (pTmp->OperStatus != IfOperStatusUp) {
			pTmp = pTmp->Next;
			continue;
		}

		pAI = (PAI)malloc(sizeof(AI));

		if (!pAI) {
			return;
		}

		pUniAddr = pTmp->FirstUnicastAddress;
		pPrefix = pTmp->FirstPrefix;

		ZeroMemory(pAI->sIP, sizeof(pAI->sIP));
		ZeroMemory(pAI->sMac, sizeof(pAI->sMac));
		ZeroMemory(pAI->sMask, sizeof(pAI->sMask));
		ZeroMemory(pAI->wsFriendlyName, sizeof(pAI->wsFriendlyName));
		ZeroMemory(pAI->wsDesc, sizeof(pAI->wsDesc));

		while (pUniAddr) {
			pAddr = (struct sockaddr_in *)pUniAddr->Address.lpSockaddr;
			inet_ntop(AF_INET, &pAddr->sin_addr, pAI->sIP, sizeof(pAI->sIP));
			pUniAddr = pUniAddr->Next;
		}

		while (pPrefix) {
			ConvertLengthToIpv4Mask(pTmp->FirstPrefix->PrefixLength, &ulMask);
			inet_ntop(AF_INET, &ulMask, pAI->sMask, sizeof(pAI->sMask));
			pPrefix = pPrefix->Next;
		}
		
		sprintf_s(pAI->sMac, "%02x:%02x:%02x:%02x:%02x:%02x",
			pTmp->PhysicalAddress[0],
			pTmp->PhysicalAddress[1], 
			pTmp->PhysicalAddress[2], 
			pTmp->PhysicalAddress[3], 
			pTmp->PhysicalAddress[4], 
			pTmp->PhysicalAddress[5]);

		memcpy(pAI->wsDesc, pTmp->Description, wcslen(pTmp->Description) * 2);
		memcpy(pAI->wsFriendlyName, pTmp->FriendlyName, wcslen(pTmp->FriendlyName) * 2);

		m_adapterInfoList.push_back(pAI);
		pAI = NULL;

		pTmp = pTmp->Next;
	}
}

int NetAdapterInfo::GetAdaptersCount()
{
	return (int)(m_adapterInfoList.size());
}

void NetAdapterInfo::PrintAdaptersInfo()
{
	int idx = 1;
	for (list<PAI>::iterator iter = m_adapterInfoList.begin(); iter != m_adapterInfoList.end(); iter++, idx++) {
		cout << "==================== " << idx << " ====================" << endl;
		wcout << "Name: " << (*iter)->wsFriendlyName << endl;
		wcout << "Desc: " << (*iter)->wsDesc << endl;
		cout << "IPv4: " << (*iter)->sIP << endl;
		cout << "Mask: " << (*iter)->sMask << endl;
		cout << "MAC : " << (*iter)->sMac << endl;
		cout << endl;
	}
}