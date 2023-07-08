#pragma once

#include "Json.h"
#include "rapidjson/document.h"
#include <any>
#include <map>
#include <string>
#include <vector>

using namespace std;

class RapidjsonJson : public Json {
	private:
		mutable rapidjson::Document document;
		mutable rapidjson::Document documentOrg;

		virtual any GetObject(const vector<string> &key) const override;
		virtual any GetValueDerived(const any &object) const override;
		any GetValueDerived(const rapidjson::Value &value) const;

	public:
		RapidjsonJson() = default;
		virtual ~RapidjsonJson() = default;

		virtual bool ParsingFromString(const string &contents) const override;

		virtual vector<map<string, any>>
		GetArray(const vector<string> &key) const override;
};
