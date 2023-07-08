#include "SocketClient.h"
#include "DualIp.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <tuple>
#include <unistd.h>

using namespace std;

SocketClient::SocketClient(const int &fd, const int &timeout)
	: fd(fd), address(""), port(0), timeout(timeout > 0 ? timeout * 1000 : -1),
	  peerAddress(""), peerPort(0) {
	this->Initialize(this->fd);
}

SocketClient::SocketClient(const string &address, const in_port_t &port,
						   const int &timeout)
	: fd(-1), address(address), port(port),
	  timeout(timeout > 0 ? timeout * 1000 : -1), peerAddress(""), peerPort(0) {
	this->Initialize(this->address, this->port);
}

SocketClient::~SocketClient() { this->Finalize(); }

bool SocketClient::Initialize(const int &fd) {
	if (fd <= 0) {
		return false;
	}

	if (this->SetFlags(O_NONBLOCK) == false) {
		return false;
	}

	sockaddr_in sockAddrIn;
	socklen_t sockLen = sizeof(sockAddrIn);
	getsockname(fd, (struct sockaddr *)&sockAddrIn, &sockLen);

	this->address = inet_ntoa(sockAddrIn.sin_addr);
	this->port = ntohs(sockAddrIn.sin_port);

	return this->SetPeerInfo();
}

bool SocketClient::Initialize(const string &address, const in_port_t &port) {
	if (this->Connect(address, port) == false) {
		return false;
	}

	return this->SetPeerInfo();
}

bool SocketClient::Finalize() { return this->DisConnect(); }

bool SocketClient::Poll(const short int &flags) const {
	pollfd pollFD = pollfd{.fd = this->fd, .events = flags, .revents = 0};

	switch (poll(&pollFD, 1, this->timeout)) {
	case 0:
		errno = ETIMEDOUT;
		return false;
	case -1:
		return false;
	default:
		break;
	}

	if (!(pollFD.revents & flags) || pollFD.revents & POLLERR ||
		pollFD.revents & POLLNVAL || pollFD.revents & POLLHUP) {
		return false;
	}

	int error = 0;
	socklen_t sockLen = sizeof(error);
	if (getsockopt(this->fd, SOL_SOCKET, SO_ERROR, (void *)&error,
				   (socklen_t *)&sockLen) == -1) {
		return false;
	}

	if (error) {
		errno = error;
		return false;
	}

	return true;
}

bool SocketClient::Connect(addrinfo *addrInfo) {
	this->DisConnect();

	this->fd = socket(addrInfo->ai_family, addrInfo->ai_socktype,
					  addrInfo->ai_protocol);
	if (this->fd == -1) {
		return false;
	}

	if (this->SetFlags(O_NONBLOCK) == false) {
		return false;
	}

	if (connect(this->fd, addrInfo->ai_addr, addrInfo->ai_addrlen) == 0) {
		return true;
	}

	if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINPROGRESS) {
		return false;
	}

	const short int flags = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI |
							POLLOUT | POLLWRNORM | POLLWRBAND | POLLERR |
							POLLHUP | POLLNVAL;
	if (this->Poll(flags) == false) {
		return false;
	}

	return true;
}

bool SocketClient::Connect(const string &address, const in_port_t &port) {
	this->DisConnect();

	if (address.empty()) {
		return false;
	}

	DualIp dualIp(address, port);
	if (dualIp.Valid() == false) {
		return false;
	}

	addrinfo *addrInfo = dualIp.GetAddrInfo();
	while (addrInfo) {
		if (this->Connect(addrInfo)) {
			return true;
		}

		addrInfo = addrInfo->ai_next;
	}

	this->DisConnect();

	return false;
}

bool SocketClient::DisConnect() {
	if (this->fd <= 0) {
		return true;
	}

	const auto result = close(this->fd);
	this->fd = -1;

	return result == 0 ? true : false;
}

tuple<bool, string> SocketClient::Read(const int &readSize, bool &end) const {
	const short int flags = POLLIN | POLLRDNORM;

	if (this->Poll(flags) == false) {
		return make_tuple(false, "");
	}

	unique_ptr<char[]> data = make_unique<char[]>(readSize);

	int resultLen = 0;
	while ((resultLen = read(this->fd, data.get(), readSize)) == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS) {
			if (this->Poll(flags) == false) {
				return make_tuple(false, nullptr);
			}

			continue;
		} else if (errno == EINTR) {
			continue;
		}

		break;
	}

	if (resultLen == 0 || resultLen == -1) {
		return make_tuple(false, "");
	}

	if (data.get()[resultLen - 2] == '\r' &&
		data.get()[resultLen - 1] == '\n') {
		end = true;
	}

	data.get()[resultLen] = '\0';

	return make_tuple(true, data.get());
}

bool SocketClient::Write(const string &data) const {
	const short int flags = POLLOUT | POLLWRNORM;

	if (this->Poll(flags) == false) {
		return false;
	}

	decltype(data.size()) resultLen = 0;
	while (resultLen < data.size()) {
		ssize_t size =
			write(this->fd, data.c_str() + resultLen, data.size() - resultLen);
		if (size == 0) {
			return true;
		} else if (size == -1) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				return false;
			}

			if (this->Poll(flags) == false) {
				return false;
			}

			continue;
		}

		resultLen += size;
	}

	if (resultLen != data.size()) {
		return false;
	}

	return true;
}

bool SocketClient::ReadGarbage() const {
	bool end = false;

	while (end == false) {
		auto result = this->Read(1024, end);
		if (get<0>(result) == false) {
			return false;
		}
	}

	return true;
}

short int SocketClient::GetFlags() const { return fcntl(this->fd, F_GETFL, 0); }

bool SocketClient::SetFlags(const short int &flags) const {
	if (fcntl(this->fd, F_SETFL, this->GetFlags() | flags) == -1) {
		return false;
	}

	return true;
}

bool SocketClient::SetPeerInfo() {
	if (this->fd == -1) {
		return false;
	}

	sockaddr_in sockAddrIn;
	socklen_t sockLen = sizeof(sockAddrIn);

	getpeername(this->fd, (sockaddr *)&sockAddrIn, &sockLen);

	this->peerAddress = inet_ntoa(sockAddrIn.sin_addr);
	this->peerPort = ntohs(sockAddrIn.sin_port);

	return true;
}

string SocketClient::GetPeerAddress() const { return this->peerAddress; }

in_port_t SocketClient::GetPeerPort() const { return this->peerPort; }
