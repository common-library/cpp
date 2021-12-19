#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>
#include <arpa/inet.h>

#include <memory>
#include <cstring>
using namespace std;

#include "DualIp.h"

#include "SocketClient.h"

SocketClient::SocketClient(const int &iFD, const int &iTimeOut)
	: iFD(iFD), strAddress(""), iPort(-1), iTimeOut(iTimeOut)
{
	this->InitializeFromFD();
}

SocketClient::SocketClient(const string &strAddress, const in_port_t &iPort, const int &iTimeOut)
	: iFD(-1), strAddress(strAddress), iPort(iPort), iTimeOut(iTimeOut)
{
	this->InitializeFromAddress();
}

SocketClient::~SocketClient()
{
	this->Finalize();
}

bool SocketClient::InitializeFromFD()
{
	if(this->iFD < 0) {
		return false;
	}

	if(this->SetFlags(O_NONBLOCK) == false) {
		return false;
	}

	sockaddr_in sSockAddrIn;
	socklen_t sockLen = sizeof(sSockAddrIn);
	getsockname(this->iFD, (struct sockaddr *)&sSockAddrIn, &sockLen);

	this->strAddress = inet_ntoa(sSockAddrIn.sin_addr);
	this->iPort = ntohs(sSockAddrIn.sin_port);

	return this->SetPeerInfo();
}

bool SocketClient::InitializeFromAddress()
{
	if(this->Connect() == false) {
		return false;
	}

	return this->SetPeerInfo();
}

bool SocketClient::Finalize()
{
	return this->DisConnect();
}

bool SocketClient::InitPoll(pollfd &sPollFD, const short int &siFlags) const
{
	sPollFD.fd = this->iFD;
	sPollFD.events = siFlags;
	sPollFD.revents = 0;

	const int iResult = poll(&sPollFD, 1, (this->iTimeOut) ? (this->iTimeOut * (1000)) : -1);
	if(iResult == 0) {
		errno = ETIMEDOUT;
		return false;
	} else if(iResult == -1) {
		return false;
	}

	return true;
}

bool SocketClient::Poll(const short int &siFlags) const
{
	if(this->iFD < 0) {
		return false;
	}

	pollfd sPollFD;
	if(this->InitPoll(sPollFD, siFlags) == false) {
		return false;
	}

	if((sPollFD.revents & POLLIN) || (sPollFD.revents & POLLRDNORM) ||
			(sPollFD.revents & POLLRDBAND) || (sPollFD.revents & POLLPRI) ||
			(sPollFD.revents & POLLOUT) || (sPollFD.revents & POLLWRNORM) ||
			(sPollFD.revents & POLLWRBAND) || (sPollFD.revents & POLLERR)) {
		int iError = 0;
		socklen_t sockLen = sizeof(iError);
		if(getsockopt(this->iFD, SOL_SOCKET, SO_ERROR, (void *)&iError, (socklen_t*)&sockLen) == -1) {
			return false;
		}

		if(iError) {
			errno = iError;
			return false;
		}
	} else if(sPollFD.revents & POLLNVAL) {
		return false;
	} else if(sPollFD.revents & POLLHUP) {
		return false;
	} else {
		return false;
	}

	return true;
}

bool SocketClient::Poll(char *pcBuffer, const int &iBufferLen, int &iResultLen, const short int &siFlags) const
{
	if(this->iFD < 0) {
		return false;
	}

	if(pcBuffer == nullptr || iBufferLen <= 0) {
		return false;
	}

	iResultLen = 0;

	if(siFlags & POLLIN) {
		memset(pcBuffer, 0x00, iBufferLen);
	}

	pollfd sPollFD;
	if(this->InitPoll(sPollFD, siFlags) == false) {
		return false;
	}

	if(sPollFD.revents & siFlags) {
		if(siFlags & POLLIN) {
			return this->PollIn(pcBuffer, iBufferLen, iResultLen, siFlags);
		} else if(siFlags & POLLOUT) {
			return this->PollOut(pcBuffer, iBufferLen, iResultLen, siFlags);
		}
	} else if(sPollFD.revents & POLLERR) {
		return false;
	} else if(sPollFD.revents & POLLNVAL) {
		return false;
	} else if(sPollFD.revents & POLLHUP) {
		return false;
	} else {
		return false;
	}

	return true;
}

bool SocketClient::PollIn(char *pcBuffer, const int &iBufferLen, int &iResultLen, const short int &siFlags) const
{
	ssize_t iSize = -1;
	while((iSize = read(this->iFD, pcBuffer, iBufferLen)) == -1) {
		if(errno == EINTR) {
			continue;
		} else if(errno == EAGAIN || errno == EINPROGRESS || errno == EWOULDBLOCK) {
			pollfd sPollFD;
			if(this->InitPoll(sPollFD, siFlags) == false) {
				return false;
			}

			continue;
		}

		break;
	}

	if(iSize == 0 || iSize == -1) {
		return false;
	}

	iResultLen = iSize;
	pcBuffer[iResultLen] = '\0';

	return true;
}

bool SocketClient::PollOut(char *pcBuffer, const int &iBufferLen, int &iResultLen, const short int &siFlags) const
{
	while(iResultLen < iBufferLen) {
		ssize_t iSize = write(this->iFD, pcBuffer + iResultLen, iBufferLen - iResultLen);
		if(iSize == 0) {
			return true;
		} else if(iSize == -1) {
			if(errno == EAGAIN || errno == EWOULDBLOCK) {
				pollfd sPollFD;
				if(this->InitPoll(sPollFD, siFlags) == false) {
					return false;
				}

				continue;
			}

			return false;
		}

		iResultLen += iSize;
	}

	return true;
}

bool SocketClient::Connect()
{
	if(this->strAddress.empty() || this->iPort < 0) {
		return false;
	}

	DualIp dualIp(this->strAddress, this->iPort);
	if(dualIp.Valid() == false) {
		return false;
	}

	bool bConnect = false;
	addrinfo *psAddrInfo = dualIp.GetAddrInfo();
	while(psAddrInfo) {
		if(this->Connect(psAddrInfo)) {
			bConnect = true;
			break;
		}

		psAddrInfo = psAddrInfo->ai_next;
	}

	if(bConnect == false) {
		this->DisConnect();

		return false;
	}

	return true;
}

bool SocketClient::Connect(addrinfo *psAddrInfo)
{
	this->DisConnect();

	this->iFD = socket(psAddrInfo->ai_family, psAddrInfo->ai_socktype, psAddrInfo->ai_protocol);
	if(this->iFD == -1) {
		return false;
	}

	if(this->SetFlags(O_NONBLOCK) == false) {
		return false;
	}

	if(connect(this->iFD, psAddrInfo->ai_addr, psAddrInfo->ai_addrlen) == 0) {
		return true;
	}

	if(errno != EINPROGRESS && errno != EWOULDBLOCK) {
		return false;
	}

	const short int iFlags = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI | POLLOUT | POLLWRNORM | POLLWRBAND | POLLERR | POLLHUP | POLLNVAL;
	if(this->Poll(iFlags) == false) {
		return false;
	}

	return true;
}

bool SocketClient::DisConnect()
{
	int iResult = 0;
	if(this->iFD >= 0) {
		iResult = close(this->iFD);
		this->iFD = -1;
	}

	return iResult == 0 ? true : false;
}

string SocketClient::ReadGarbage() const
{
	bool bEnd = false;
	string strRead = "";

	while(bEnd == false) {
		string strTemp = "";
		if(this->Read(strTemp, 1024, bEnd) == false) {
			break;
		}

		strRead += strTemp;
	}

	return strRead;
}

bool SocketClient::Read(string &strRead, const int &iReadLen, bool &bEnd) const
{
	int iResultLen = 0;

	unique_ptr<char[]> uniqRead = make_unique<char[]>(iReadLen);

	const bool bResult = this->Read(uniqRead.get(), iReadLen, iResultLen, bEnd);

	strRead = uniqRead.get();

	return bResult;
}

bool SocketClient::Read(char *pcRead, const int &iReadLen, int &iResultLen, bool &bEnd) const
{
	bEnd = false;
	const bool bResult = this->Poll(pcRead, iReadLen, iResultLen, POLLIN | POLLRDNORM);
	if(bResult) {
		if(pcRead[iResultLen -2] == '\r' && pcRead[iResultLen -1] == '\n') {
			bEnd = true;
		}
	}

	return bResult;
}

bool SocketClient::Write(const string &strWrite) const
{
	int iResultLen = 0;

	return this->Write(strWrite.c_str(), strWrite.size(), iResultLen);
}

bool SocketClient::Write(const char * const pcWrite, const int &iWriteLen, int &iResultLen) const
{
	return this->Poll((char *)pcWrite, iWriteLen, iResultLen, POLLOUT | POLLWRNORM);
}

short int SocketClient::GetFlags()
{
	return fcntl(this->iFD, F_GETFL, 0);
}

bool SocketClient::SetFlags(const short int &siFlags)
{
	if(fcntl(this->iFD, F_SETFL, this->GetFlags() | siFlags) == -1) {
		return false;
	}

	return true;
}

bool SocketClient::SetPeerInfo()
{
	if(this->iFD < 0) {
		return false;
	}

	sockaddr_in sSockAddrIn;
	socklen_t sockLen = sizeof(sSockAddrIn);

	getpeername(this->iFD, (sockaddr *)&sSockAddrIn, &sockLen);

	this->strPeerAddress = inet_ntoa(sSockAddrIn.sin_addr);
	this->iPeerPort = sSockAddrIn.sin_port;

	return true;
}

string SocketClient::GetPeerAddress() const
{
	return this->strPeerAddress;
}

in_port_t SocketClient::GetPeerPort() const
{
	return this->iPeerPort;
}
