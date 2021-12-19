#include <fcntl.h>
#include <unistd.h>

#include <cstring>
using namespace std;

#include "ThreadPool.h"

#include "SocketServer.h"

SocketServer::SocketServer()
	: bStart(false),
		iFD(-1)
{
}

SocketServer::~SocketServer()
{
	this->Stop();
}

bool SocketServer::Create()
{
	this->iFD = socket(AF_INET, SOCK_STREAM, 0);
	if(this->iFD == -1) {
		return false;
	}

	int iOn = 1;
	if(setsockopt(this->iFD, SOL_SOCKET, SO_REUSEADDR, &iOn, sizeof(int))) {
		return false;
	}

	if(fcntl(this->iFD, F_SETFL, fcntl(this->iFD, F_GETFL, 0) | O_NONBLOCK) == -1) {
		return false;
	}

	return true;
}

bool SocketServer::Bind(const int &iPort)
{
	if(iPort < 0) {
		return false;
	}

	struct sockaddr_in sSockAddrIn;
	memset(&sSockAddrIn, 0x00, sizeof(sSockAddrIn));
	sSockAddrIn.sin_family = AF_INET;
	sSockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);
	sSockAddrIn.sin_port = htons(iPort);

	if(bind(this->iFD, (struct sockaddr *)&sSockAddrIn, sizeof(sSockAddrIn)) == -1) {
		return false;
	}

	return true;
}

bool SocketServer::Listen(const int &iListenQueueLen)
{
	if(listen(this->iFD, iListenQueueLen) == -1) {
		return false;
	}

	return true;
}

bool SocketServer::Accept(int &iClientFD)
{
	struct sockaddr_in sSockaddr;
	const int iSizeofSockaddr = sizeof(sSockaddr);

	iClientFD = accept(this->iFD, (struct sockaddr *)&sSockaddr, (socklen_t *)&iSizeofSockaddr);
	if(iClientFD != -1) {
		return true;
	}

	switch(errno) {
		case EAGAIN:
			this_thread::sleep_for(std::chrono::milliseconds(1000));
			break;
		default:
			break;
	}

	return false;
}

bool SocketServer::Open(const int &iPort, const int &iListenQueueLen)
{
	if(this->Create() == false) {
		return false;
	}

	if(this->Bind(iPort) == false) {
		this->Close();
		return false;
	}

	if(this->Listen(iListenQueueLen) == false) {
		this->Close();
		return false;
	}

	return true;
}

bool SocketServer::Close()
{
	int iResult = 0;
	if(this->iFD != -1) {
		iResult = close(this->iFD);
		this->iFD = -1;
	}

	return iResult == 0 ? true : false;
}

bool SocketServer::Start(const int &iPort, const int iTimeOut, const int iJobPoolSize, const function<void(const SocketClient &socketClient)> &jobFunc)
{
	if(this->bStart) {
		this->Stop();
	}

	this->bStart.store(true);

	if(this->Open(iPort) == false) {
		return false;
	}

	ThreadPool threadPool(iJobPoolSize);

	vector<future<void>> vecFuture;
	vecFuture.clear();

	while(this->bStart) {
		int iClientFD = -1;
		if(this->Accept(iClientFD) == false) {
			continue;
		}

		auto jobFinal = [=]() {
			jobFunc(SocketClient(iClientFD, iTimeOut));
		};

		vecFuture.push_back(threadPool.AddJob(jobFinal));

		if((int)vecFuture.size() == iJobPoolSize) {
			for(auto &iter : vecFuture) {
				iter.get();
			}
			vecFuture.clear();
		}
	}

	for(auto &iter : vecFuture) {
		iter.get();
	}
	vecFuture.clear();

	this->Close();

	return true;
}

bool SocketServer::Stop()
{
	this->bStart.store(false);

	return true;
}
