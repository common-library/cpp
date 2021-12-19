#include <arpa/inet.h>

#include <cstring>
#include <algorithm>
using namespace std;

#include "DualIp.h"

DualIp::DualIp(const sockaddr_storage &sSockAddrStorage)
	: eIpType(IPTYPE::NOTMATCHED), sInAddr({}), sIn6Addr({}), iPort(0), psAddrInfo(nullptr)
{
	this->Initialize(sSockAddrStorage); 
}

DualIp::DualIp(const string &strAddress, const in_port_t &iPort)
	: eIpType(IPTYPE::NOTMATCHED), sInAddr({}), sIn6Addr({}), iPort(iPort), psAddrInfo(nullptr)
{
	this->Initialize(strAddress);
}

DualIp::~DualIp()
{
	this->Finalize();
}

bool DualIp::Initialize(const string &strAddress)
{
	if(this->Valid(strAddress)) {
		this->AddressToStruct(strAddress);
	} else {
		addrinfo *psAddrInfo = nullptr;
		addrinfo *psAddrInfoTemp = nullptr;

		if(this->GetAddrInfo(strAddress, 0, AF_UNSPEC, SOCK_STREAM, AI_CANONNAME, &psAddrInfo) == false) {
			if(psAddrInfo) {
				this->FreeAddrInfo(*psAddrInfo);
				psAddrInfo = nullptr;
			}
			return false;
		}

		psAddrInfoTemp = psAddrInfo;
		while(psAddrInfoTemp) {
			void *vAddr = nullptr;
			switch(psAddrInfoTemp->ai_family) {
				case AF_INET:
					vAddr = &((sockaddr_in *) psAddrInfoTemp->ai_addr)->sin_addr;
					break;
				case AF_INET6:
					vAddr = &((sockaddr_in6 *) psAddrInfoTemp->ai_addr)->sin6_addr;
					break;
				default:
					vAddr = nullptr;
					break;
			}

			if(vAddr) {
				char caAddress[INET6_ADDRSTRLEN];
				memset(caAddress, 0x00, sizeof(caAddress));
				if(inet_ntop(psAddrInfoTemp->ai_family, vAddr, caAddress, sizeof(caAddress))) {
					this->AddressToStruct(caAddress);
					break;
				}
			}

			psAddrInfoTemp = psAddrInfoTemp->ai_next;
		}

		if(psAddrInfo) {
			this->FreeAddrInfo(*psAddrInfo);
			psAddrInfo = nullptr;
		}
	}

	return this->Valid();
}

bool DualIp::Initialize(const sockaddr_storage &sSockAddrStorage)
{
	switch(sSockAddrStorage.ss_family) {
		case AF_INET:
			this->eIpType = IPTYPE::V4MAPPED;
			this->sInAddr = (in_addr)((sockaddr_in*)&sSockAddrStorage)->sin_addr;
			this->sIn6Addr.s6_addr32[0] = 0;
			this->sIn6Addr.s6_addr32[1] = 0;
			this->sIn6Addr.s6_addr32[2] = htonl(0xffff);
			this->sIn6Addr.s6_addr32[3] = this->sInAddr.s_addr;
			this->iPort = ((sockaddr_in*)&sSockAddrStorage)->sin_port;

			break;
		case AF_INET6:
			this->sIn6Addr = ((sockaddr_in6 *)&sSockAddrStorage)->sin6_addr;
			this->eIpType = this->GetIpType();

			switch(this->eIpType) {
				case IPTYPE::V4MAPPED:
					this->sInAddr.s_addr = this->sIn6Addr.s6_addr32[3];
					break;
				case IPTYPE::NOTMATCHED:
				case IPTYPE::UNSPECIFIED:
					return false;
				default:
					break;
			}

			break;
		default:
			return false;
	}

	return true;
}

bool DualIp::Finalize()
{
	if(this->psAddrInfo) {
		this->FreeAddrInfo(*this->psAddrInfo);
		this->psAddrInfo = nullptr;
	}

	return true;
}

bool DualIp::Valid()
{
	int iRet = -1;

	if(this->eIpType == IPTYPE::V4MAPPED) {
		iRet = !(this->sInAddr.s_addr);
	} else {
		iRet = !(this->sIn6Addr.s6_addr32[0] ||
					this->sIn6Addr.s6_addr32[1] ||
					this->sIn6Addr.s6_addr32[2] ||
					this->sIn6Addr.s6_addr32[3]);
	}

	return iRet == 0 ? true : false;
}

bool DualIp::Valid(const string &strAddress)
{
	in6_addr sIn6Addr;

	if(inet_pton(AF_INET, strAddress.c_str(), &sIn6Addr) == 1 ||
			inet_pton(AF_INET6, strAddress.c_str(), &sIn6Addr) == 1) {
		return true;
	}

	return false;
}

string DualIp::StructToAddress()
{
	char caAddress[INET6_ADDRSTRLEN];
	memset(caAddress, 0x00, sizeof(caAddress));

	const char *pcResult = nullptr;
	if(this->eIpType == IPTYPE::V4MAPPED) {
		pcResult = inet_ntop(AF_INET, &this->sInAddr, caAddress, sizeof(caAddress));
	} else {
		pcResult = inet_ntop(AF_INET6, &this->sIn6Addr, caAddress, sizeof(caAddress));
	}

	return pcResult ? caAddress : "";
}

bool DualIp::AddressToStruct(const string &strAddress)
{
	int iFamily = AF_INET;
	any_of(strAddress.begin(), strAddress.end(),
				[&](const char &cValue) {
					if(cValue == ':') {
						iFamily = AF_INET6;
					}

					return '\0';
				}
			);

	char caIP[INET6_ADDRSTRLEN];
	sprintf(caIP, "%s%s", iFamily == AF_INET ? "::ffff:" : "", strAddress.c_str());

	const string strIP((iFamily == AF_INET ? "::ffff:" : "") + strAddress);

	if(inet_pton(AF_INET6, (char *)caIP, &this->sIn6Addr) != 1) {
		return false;
	}

	this->eIpType = this->GetIpType();

	if(this->eIpType == IPTYPE::V4MAPPED) {
		if(inet_pton(AF_INET, strAddress.c_str(), &this->sInAddr) != 1) {
			return false;
		}
	}

	return true;
}

DualIp::IPTYPE DualIp::GetIpType()
{
	IPTYPE eIpType = IPTYPE::OTHERIPV6;

	if(IN6_IS_ADDR_UNSPECIFIED(&this->sIn6Addr)) {
		eIpType = IPTYPE::UNSPECIFIED;
	} else if(IN6_IS_ADDR_LOOPBACK(&this->sIn6Addr)) {
		eIpType = IPTYPE::LOOPBACK;
	} else if(IN6_IS_ADDR_MULTICAST(&this->sIn6Addr)) {
		eIpType = IPTYPE::MULTICAST;
	} else if(IN6_IS_ADDR_LINKLOCAL(&this->sIn6Addr)) {
		eIpType = IPTYPE::LINKLOCAL;
	} else if(IN6_IS_ADDR_SITELOCAL(&this->sIn6Addr)) {
		eIpType = IPTYPE::SITELOCAL;
	} else if(IN6_IS_ADDR_V4COMPAT(&this->sIn6Addr)) {
		eIpType = IPTYPE::V4COMPAT;
	} else if(IN6_IS_ADDR_MC_NODELOCAL(&this->sIn6Addr)) {
		eIpType = IPTYPE::NODELOCAL;
	} else if(IN6_IS_ADDR_MC_LINKLOCAL(&this->sIn6Addr)) {
		eIpType = IPTYPE::LINKLOCAL;
	} else if(IN6_IS_ADDR_MC_SITELOCAL(&this->sIn6Addr)) {
		eIpType = IPTYPE::SITELOCAL;
	} else if(IN6_IS_ADDR_MC_ORGLOCAL(&this->sIn6Addr)) {
		eIpType = IPTYPE::ORGLOCAL;
	} else if(IN6_IS_ADDR_MC_GLOBAL(&this->sIn6Addr)) {
		eIpType = IPTYPE::GLOBAL;
	} else if(IN6_IS_ADDR_V4MAPPED(&this->sIn6Addr)) {
		eIpType = IPTYPE::V4MAPPED;
	}

	return eIpType;
}

bool DualIp::GetAddrInfo(const string &strAddress, const in_port_t &iPort, const int &iFamily, const int &iSockType, const int &iFlags, addrinfo **ppAddrInfo)
{
	addrinfo sAddrInfo;

	memset(&sAddrInfo, 0, sizeof(sAddrInfo));

	if(strAddress.empty()) {
		sAddrInfo.ai_flags = AI_PASSIVE;
	}

	sAddrInfo.ai_family = iFamily;
	sAddrInfo.ai_socktype = iSockType;
	sAddrInfo.ai_flags |= iFlags;

	char szTemp[256];
	sprintf(szTemp, "%d", iPort);

	const int iRet = getaddrinfo(strAddress.c_str(), iPort > 0 ? szTemp : nullptr, &sAddrInfo, ppAddrInfo);
	if(iRet < 0) {
		return false;
	}

	return true;
}

bool DualIp::FreeAddrInfo(addrinfo &sAddrInfo)
{
	freeaddrinfo(&sAddrInfo);

	return true;
}

addrinfo* DualIp::GetAddrInfo()
{
	if(this->psAddrInfo == nullptr) {
		if(this->GetAddrInfo(this->StructToAddress(), this->iPort, AF_UNSPEC, SOCK_STREAM, 0, &this->psAddrInfo) == false) {
			if(this->psAddrInfo) {
				this->FreeAddrInfo(*this->psAddrInfo);
				this->psAddrInfo = nullptr;
			}
		}
	}

	return this->psAddrInfo;
}
