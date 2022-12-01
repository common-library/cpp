#pragma once

#include <netdb.h>
#include <sys/poll.h>

#include <string>
using namespace std;

class SocketClient {
	private:
		int iFD;
		string strAddress;
		in_port_t iPort;
		int iTimeOut;

		string strPeerAddress;
		in_port_t iPeerPort;

		bool InitializeFromFD();
		bool InitializeFromAddress();
		bool Finalize();

		bool InitPoll(pollfd& sPollFD, const short int& siFlags) const;
		bool Poll(const short int& siFlags) const;
		bool Poll(char* pcBuffer, const int& iBufferLen, int& iResultLen,
				  const short int& siFlags) const;
		bool PollIn(char* pcBuffer, const int& iBufferLen, int& iResultLen,
					const short int& siFlags) const;
		bool PollOut(char* pcBuffer, const int& iBufferLen, int& iResultLen,
					 const short int& siFlags) const;

		bool Connect();
		bool Connect(addrinfo* psAddrInfo);
		bool DisConnect();

		short int GetFlags();
		bool SetFlags(const short int& siFlags);

		bool SetPeerInfo();

	public:
		SocketClient(const int& iFD, const int& iTimeOut);
		SocketClient(const string& strAddress, const in_port_t& iPort,
					 const int& iTimeout);
		virtual ~SocketClient();

		string ReadGarbage() const;

		bool Read(string& strRead, const int& iReadLen, bool& bEnd) const;
		bool Read(char* pcRead, const int& iReadLen, int& iResultLen, bool& bEnd) const;

		bool Write(const string& strWrite) const;
		bool Write(const char* const pcWrite, const int& iWriteLen,
				   int& iResultLen) const;

		string GetPeerAddress() const;
		in_port_t GetPeerPort() const;
};
