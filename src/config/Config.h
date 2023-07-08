#pragma once

#include "Json.h"
#include <memory>
#include <string>

using namespace std;

class Config {
	private:
		const string fileName;

		virtual bool InitializeDerived() = 0;

	protected:
		const unique_ptr<Json> json;

	public:
		Config(const string &fileName);
		virtual ~Config() = default;

		virtual bool Initialize(const string &path) final;

		virtual string GetFileName() const final;
};
