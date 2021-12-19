#pragma once

#include <mutex>
#include <atomic>
#include <functional>
using namespace std;

#include "SocketClient.h"

class SocketServer {
private:
	mutex mutexLock;
	atomic<bool> bStart;

	int iFD;

	bool Create();
	bool Bind(const int &iPort);
	bool Listen(const int &iListenQueueLen);
	bool Accept(int &iClientFD);

	bool Open(const int &iPort, const int &iListenQueueLen = 1024);
	bool Close();
public:
	SocketServer();
	virtual ~SocketServer();

	bool Start(const int &iPort, const int iTimeOut, const int iJobPoolSize, const function<void(const SocketClient &socketClient)> &jobFunc);

	bool Stop();
};
