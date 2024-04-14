#pragma once

#include "SocketClient.h"
#include "ThreadPool.h"
#include <atomic>
#include <functional>
#include <netinet/in.h>

using namespace std;

class SocketServer {
	private:
		int fd;
		atomic_bool start;
		ThreadPool threadPool;

		bool Open(const in_port_t &port, const int &listenQueueLen = 1024);
		bool Close();

		bool Create();
		bool Bind(const in_port_t &port);
		bool Listen(const int &listenQueueLen);
		int Accept();

	public:
		SocketServer();
		virtual ~SocketServer();

		bool Start(const in_port_t &port, const int &timeout, const poolSizeType &poolSize,
				   const function<void(const SocketClient &socketClient)> &job);
		bool Stop();

		void SetPoolSize(const poolSizeType &poolSize);
};
