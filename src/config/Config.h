#pragma once

#include "JsonManager.h"

class Config {
	private:
		const string strFileName;

		virtual bool InitializeDerived() = 0;

	protected:
		JsonManager jsonManager;

	public:
		Config(const string& strFileName,
			   const E_JSON_PARSER& eJsonParser = E_JSON_PARSER::RAPIDJSON);
		virtual ~Config() = default;

		bool Initialize(const string& strConfigPath);
};
