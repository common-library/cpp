#pragma once

#include <netdb.h>
#include <netinet/in.h>

#include <string>
using namespace std;

class DualIp {
private:
	enum class IPTYPE {
		NOTMATCHED = 0,
		UNSPECIFIED,
		LOOPBACK,
		MULTICAST,
		LINKLOCAL,
		V4MAPPED,
		V4COMPAT,
		NODELOCAL,
		SITELOCAL,
		ORGLOCAL,
		GLOBAL,
		OTHERIPV6
	};

	IPTYPE eIpType;
	in_addr sInAddr;
	in6_addr sIn6Addr;
	in_port_t iPort;
	addrinfo *psAddrInfo;

	bool Initialize(const string &strAddress);
	bool Initialize(const sockaddr_storage &sSockAddrStorage);
	bool Finalize();

	bool Valid(const string &strAddress);

	string StructToAddress();
	bool AddressToStruct(const string &strAddress);

	IPTYPE GetIpType();

	bool GetAddrInfo(const string &strAddress, const in_port_t &iPort, const int &iFamily, const int &iSockType, const int &iFlags, addrinfo **ppAddrInfo);
	bool FreeAddrInfo(addrinfo &sAddrInfo);
public:
	DualIp(const sockaddr_storage &sSockAddrStorage);
	DualIp(const string &strAddress, const in_port_t &iPort);
	virtual ~DualIp();

	bool Valid();

	addrinfo* GetAddrInfo();
};
