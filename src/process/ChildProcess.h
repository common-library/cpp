#pragma once

#include "Process.h"

class ChildProcess : public Process {
	protected:
		virtual bool Job() = 0;

	public:
		ChildProcess() = default;
		virtual ~ChildProcess() = default;

		virtual bool Start() override final;
		virtual bool Stop() override final;
};
