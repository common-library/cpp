#include "SocketServer.h"
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <netinet/in.h>
#include <thread>

using namespace std;

SocketServer::SocketServer() : fd(-1), start(false), threadPool(0) {}

SocketServer::~SocketServer() { this->Stop(); }

bool SocketServer::Open(const in_port_t &port, const int &listenQueueLen) {
	if (this->Create() == false) {
		return false;
	}

	if (this->Bind(port) == false) {
		this->Close();
		return false;
	}

	if (this->Listen(listenQueueLen) == false) {
		this->Close();
		return false;
	}

	return true;
}

bool SocketServer::Close() {
	if (this->fd == -1) {
		return true;
	}

	const int result = close(this->fd);
	this->fd = -1;

	return result == 0 ? true : false;
}

bool SocketServer::Create() {
	this->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fd == -1) {
		return false;
	}

	int optval = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))) {
		return false;
	}

	if (fcntl(this->fd, F_SETFL, fcntl(this->fd, F_GETFL, 0) | O_NONBLOCK) ==
		-1) {
		return false;
	}

	return true;
}

bool SocketServer::Bind(const in_port_t &port) {
	if (port <= 0) {
		return false;
	}

	struct sockaddr_in sockAddrIn;
	memset(&sockAddrIn, 0x00, sizeof(sockAddrIn));
	sockAddrIn.sin_family = AF_INET;
	sockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddrIn.sin_port = htons(port);

	const auto result =
		bind(this->fd, (struct sockaddr *)&sockAddrIn, sizeof(sockAddrIn));

	return result != -1 ? true : false;
}

bool SocketServer::Listen(const int &listenQueueLen) {
	return listen(this->fd, listenQueueLen) != -1 ? true : false;
}

int SocketServer::Accept() {
	struct sockaddr_in sSockaddr;
	socklen_t sockLen = sizeof(sSockaddr);

	const auto clientFD =
		accept(this->fd, (struct sockaddr *)&sSockaddr, &sockLen);
	if (clientFD != -1) {
		return clientFD;
	}

	switch (errno) {
	case EAGAIN:
		this_thread::sleep_for(100ms);
		break;
	default:
		break;
	}

	return -1;
}

bool SocketServer::Start(
	const in_port_t &port, const int &timeout, const poolSizeType &poolSize,
	const function<void(const SocketClient &socketClient)> &job) {
	if (this->start) {
		this->Stop();
	}

	this->start.store(true);

	if (this->Open(port) == false) {
		return false;
	}

	this->threadPool.SetPoolSize(poolSize + 1);

	this->threadPool.AddJob([this, job, timeout]() {
		while (this->start) {
			const auto clientFD = this->Accept();
			if (clientFD == -1) {
				continue;
			}

			this->threadPool.AddJob(
				[=]() { job(SocketClient(clientFD, timeout)); });
		}
	});

	return true;
}

bool SocketServer::Stop() {
	this->start.store(false);

	this->threadPool.SetPoolSize(0, false);

	return this->Close();
}

void SocketServer::SetPoolSize(const poolSizeType &poolSize) {
	this->threadPool.SetPoolSize(poolSize + 1);
}
