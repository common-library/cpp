#pragma once

#include <atomic>
#include <csignal>
#include <functional>
#include <map>
#include <mutex>

using namespace std;

class Signal {
	private:
		atomic<pid_t> pid;

		mutex mutexForInfos;
		mutex mutexForHandler;
		map<int, function<void(int)>> infos;

		Signal();
		~Signal() = default;

		bool CheckPid();

		void SetSignal(const int &signalValue, __sighandler_t handler);

		static mutex mutexForInitialize;
		static atomic_bool initialize;

		static void Handler(int signalValue);

	public:
		void Add(const int &signalValue, function<void(int)> handler);

		void Delete();
		void Delete(const int &signalValue);

		static Signal &Instance();
};
