#pragma once

#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <tuple>

using namespace std;

class SocketClient {
	private:
		int fd;
		pollfd pollFD;

		string address;
		in_port_t port;
		int timeout;

		string peerAddress;
		in_port_t peerPort;

		bool Initialize(const int &fd);
		bool Initialize(const string &address, const in_port_t &port);
		bool Finalize();

		bool Poll(const short int &flags) const;

		bool Connect(addrinfo *addrInfo);
		bool Connect(const string &address, const in_port_t &port);
		bool DisConnect();

		short int GetFlags() const;
		bool SetFlags(const short int &flags) const;

		bool SetPeerInfo();

	public:
		SocketClient(const int &fd, const int &timeout);
		SocketClient(const string &address, const in_port_t &port, const int &timeout);
		virtual ~SocketClient();

		tuple<bool, string> Read(const int &readSize, bool &end) const;
		bool Write(const string &data) const;

		bool ReadGarbage() const;

		string GetPeerAddress() const;
		in_port_t GetPeerPort() const;
};
