#pragma once

#include "Json.h"
#include "rabbit.hpp"
#include <any>
#include <map>
#include <string>
#include <vector>

using namespace std;

class RabbitJson : public Json {
	private:
		mutable rabbit::document document;

		virtual any GetObject(const vector<string> &key) const override;
		virtual any GetValueDerived(const any &object) const override;

	public:
		RabbitJson() = default;
		virtual ~RabbitJson() = default;

		virtual bool ParsingFromString(const string &contents) const override;

		virtual vector<map<string, any>> GetArray(const vector<string> &key) const override;
};
