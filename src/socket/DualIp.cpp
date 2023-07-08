#include "DualIp.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <string>

using namespace std;

DualIp::DualIp(const sockaddr_storage &sockAddrStorage)
	: address(""), port(0), ipType(IP_TYPE::NOTMATCHED), inAddr({}),
	  in6Addr({}), addrInfo(nullptr) {
	this->Initialize(sockAddrStorage);
}

DualIp::DualIp(const string &address, const in_port_t &port)
	: address(address), port(port), ipType(IP_TYPE::NOTMATCHED), inAddr({}),
	  in6Addr({}), addrInfo(nullptr) {
	this->Initialize(this->address);
}

DualIp::~DualIp() { this->Finalize(); }

bool DualIp::Initialize(const string &address) {
	this->Finalize();

	if (this->Valid(address)) {
		return this->AddressToStruct(address) && this->Valid();
	}

	addrinfo *addrInfo =
		this->GetAddrInfo(address, 0, AF_UNSPEC, SOCK_STREAM, AI_CANONNAME);

	addrinfo *addrInfoTemp = addrInfo;
	while (addrInfoTemp) {
		void *addr = nullptr;
		switch (addrInfoTemp->ai_family) {
		case AF_INET:
			addr = &((sockaddr_in *)addrInfoTemp->ai_addr)->sin_addr;
			break;
		case AF_INET6:
			addr = &((sockaddr_in6 *)addrInfoTemp->ai_addr)->sin6_addr;
			break;
		default:
			addr = nullptr;
			break;
		}

		if (addr) {
			char addressTemp[INET6_ADDRSTRLEN];
			memset(addressTemp, 0x00, sizeof(addressTemp));
			if (inet_ntop(addrInfoTemp->ai_family, addr, addressTemp,
						  sizeof(addressTemp))) {
				this->AddressToStruct(addressTemp);
				break;
			}
		}

		addrInfoTemp = addrInfoTemp->ai_next;
	}

	this->Finalize(&addrInfo);

	return this->Valid();
}

bool DualIp::Initialize(const sockaddr_storage &sockAddrStorage) {
	this->Finalize();

	switch (sockAddrStorage.ss_family) {
	case AF_INET:
		this->ipType = IP_TYPE::V4MAPPED;
		this->inAddr = (in_addr)((sockaddr_in *)&sockAddrStorage)->sin_addr;
		this->in6Addr.s6_addr32[0] = 0;
		this->in6Addr.s6_addr32[1] = 0;
		this->in6Addr.s6_addr32[2] = htonl(0xffff);
		this->in6Addr.s6_addr32[3] = this->inAddr.s_addr;
		this->port = ((sockaddr_in *)&sockAddrStorage)->sin_port;

		break;
	case AF_INET6:
		this->in6Addr = ((sockaddr_in6 *)&sockAddrStorage)->sin6_addr;
		this->ipType = this->GetIpType();

		switch (this->ipType) {
		case IP_TYPE::V4MAPPED:
			this->inAddr.s_addr = this->in6Addr.s6_addr32[3];
			break;
		case IP_TYPE::NOTMATCHED:
		case IP_TYPE::UNSPECIFIED:
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

bool DualIp::Finalize() {
	this->Finalize(&this->addrInfo);

	return true;
}

bool DualIp::Finalize(addrinfo **addrInfo) {
	if (*addrInfo) {
		freeaddrinfo(*addrInfo);
		*addrInfo = nullptr;
	}

	return true;
}

bool DualIp::Valid() const {
	int result = -1;

	if (this->ipType == IP_TYPE::V4MAPPED) {
		result = !(this->inAddr.s_addr);
	} else {
		result = !(this->in6Addr.s6_addr32[0] || this->in6Addr.s6_addr32[1] ||
				   this->in6Addr.s6_addr32[2] || this->in6Addr.s6_addr32[3]);
	}

	return result == 0 ? true : false;
}

bool DualIp::Valid(const string &address) const {
	in_addr inAddr;
	in6_addr in6Addr;

	if (inet_pton(AF_INET, address.c_str(), &inAddr) != 1 &&
		inet_pton(AF_INET6, address.c_str(), &in6Addr) != 1) {
		return false;
	}

	return true;
}

string DualIp::StructToAddress() const {
	char address[INET6_ADDRSTRLEN];
	memset(address, 0x00, sizeof(address));

	const char *result = nullptr;
	if (this->ipType == IP_TYPE::V4MAPPED) {
		result = inet_ntop(AF_INET, &this->inAddr, address, sizeof(address));
	} else {
		result = inet_ntop(AF_INET6, &this->in6Addr, address, sizeof(address));
	}

	return result ? address : "";
}

bool DualIp::AddressToStruct(const string &address) {
	this->ipType = this->GetIpType();

	if (this->ipType == IP_TYPE::V4MAPPED) {
		if (inet_pton(AF_INET, address.c_str(), &this->inAddr) != 1) {
			return false;
		}
	}

	int family = AF_INET;
	any_of(address.begin(), address.end(), [&family](const char &cValue) {
		if (cValue == ':') {
			family = AF_INET6;
		}

		return '\0';
	});

	const string ip((family == AF_INET ? "::ffff:" : "") + address);
	if (inet_pton(AF_INET6, ip.c_str(), &this->in6Addr) != 1) {
		return false;
	}

	return true;
}

DualIp::IP_TYPE DualIp::GetIpType() const {
	IP_TYPE ipType = IP_TYPE::OTHERIPV6;

	if (IN6_IS_ADDR_UNSPECIFIED(&this->in6Addr)) {
		ipType = IP_TYPE::UNSPECIFIED;
	} else if (IN6_IS_ADDR_LOOPBACK(&this->in6Addr)) {
		ipType = IP_TYPE::LOOPBACK;
	} else if (IN6_IS_ADDR_V4MAPPED(&this->in6Addr)) {
		ipType = IP_TYPE::V4MAPPED;
	} else if (IN6_IS_ADDR_V4COMPAT(&this->in6Addr)) {
		ipType = IP_TYPE::V4COMPAT;
	} else if (IN6_IS_ADDR_MULTICAST(&this->in6Addr)) {
		ipType = IP_TYPE::MULTICAST;
	} else if (IN6_IS_ADDR_LINKLOCAL(&this->in6Addr)) {
		ipType = IP_TYPE::LINKLOCAL;
	} else if (IN6_IS_ADDR_MC_LINKLOCAL(&this->in6Addr)) {
		ipType = IP_TYPE::LINKLOCAL;
	} else if (IN6_IS_ADDR_SITELOCAL(&this->in6Addr)) {
		ipType = IP_TYPE::SITELOCAL;
	} else if (IN6_IS_ADDR_MC_NODELOCAL(&this->in6Addr)) {
		ipType = IP_TYPE::NODELOCAL;
	} else if (IN6_IS_ADDR_MC_SITELOCAL(&this->in6Addr)) {
		ipType = IP_TYPE::SITELOCAL;
	} else if (IN6_IS_ADDR_MC_ORGLOCAL(&this->in6Addr)) {
		ipType = IP_TYPE::ORGLOCAL;
	} else if (IN6_IS_ADDR_MC_GLOBAL(&this->in6Addr)) {
		ipType = IP_TYPE::GLOBAL;
	}

	return ipType;
}

addrinfo *DualIp::GetAddrInfo() {
	if (this->addrInfo) {
		return this->addrInfo;
	}

	this->addrInfo = this->GetAddrInfo(this->StructToAddress(), this->port,
									   AF_UNSPEC, SOCK_STREAM, 0);

	return this->addrInfo;
}

addrinfo *DualIp::GetAddrInfo(const string &address, const in_port_t &port,
							  const int &family, const int &sockType,
							  const int &flags) const {
	addrinfo addrInfoTemp;
	memset(&addrInfoTemp, 0, sizeof(addrInfoTemp));

	if (address.empty()) {
		addrInfoTemp.ai_flags = AI_PASSIVE;
	}

	addrInfoTemp.ai_family = family;
	addrInfoTemp.ai_socktype = sockType;
	addrInfoTemp.ai_flags |= flags;

	addrinfo *addrInfo = nullptr;

	const int result = getaddrinfo(address.c_str(), to_string(port).c_str(),
								   &addrInfoTemp, &addrInfo);

	return result == 0 ? addrInfo : nullptr;
}
