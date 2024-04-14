#pragma once

#include <netdb.h>
#include <netinet/in.h>
#include <string>

using namespace std;

class DualIp {
	private:
		enum class IP_TYPE {
			NOTMATCHED = 0,
			UNSPECIFIED,
			LOOPBACK,
			V4MAPPED,
			V4COMPAT,
			MULTICAST,
			LINKLOCAL,
			NODELOCAL,
			SITELOCAL,
			ORGLOCAL,
			GLOBAL,
			OTHERIPV6
		};

		string address;
		in_port_t port;

		IP_TYPE ipType;
		in_addr inAddr;
		in6_addr in6Addr;
		addrinfo *addrInfo;

		bool Initialize(const string &address);
		bool Initialize(const sockaddr_storage &sockAddrStorage);
		bool Finalize();
		bool Finalize(addrinfo **sAddrInfo);

		bool Valid(const string &address) const;

		string StructToAddress() const;
		bool AddressToStruct(const string &address);

		IP_TYPE GetIpType() const;

		addrinfo *GetAddrInfo(const string &address, const in_port_t &port, const int &family,
							  const int &sockType, const int &flags) const;

	public:
		DualIp(const sockaddr_storage &sockAddrStorage);
		DualIp(const string &address, const in_port_t &port);
		~DualIp();

		bool Valid() const;

		addrinfo *GetAddrInfo();
};
