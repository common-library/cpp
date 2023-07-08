#pragma once

#include <atomic>

using namespace std;

class Process {
	protected:
		atomic_bool condition;

		virtual bool Initialize() = 0;
		virtual bool Finalize() = 0;

	public:
		Process();
		virtual ~Process() = default;

		virtual bool Start() = 0;
		virtual bool Stop() = 0;

		virtual bool GetCondition() const final;
};
