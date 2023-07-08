#include "RapidjsonJson.h"
#include <any>
#include <map>
#include <string>
#include <vector>

using namespace std;

bool RapidjsonJson::ParsingFromString(const string &contents) const {
	this->document.Parse(contents.c_str());

	if (this->document.HasParseError()) {
		return false;
	}

	this->documentOrg.CopyFrom(this->document,
							   this->documentOrg.GetAllocator());

	return true;
}

any RapidjsonJson::GetObject(const vector<string> &key) const {
	if (this->document.HasMember(key[0].c_str()) == false) {
		return any{};
	}

	rapidjson::Value &value = this->document[key[0].c_str()];

	for (unsigned int index = 1; index < key.size(); ++index) {
		if (value.IsObject() == false ||
			value.HasMember(key[index].c_str()) == false) {
			return any{};
		}

		value = value[key[index].c_str()];
	}

	this->document.CopyFrom(this->documentOrg, this->document.GetAllocator());

	return &value;
}

any RapidjsonJson::GetValueDerived(const any &object) const {
	return this->GetValueDerived(*any_cast<rapidjson::Value *>(object));
}

any RapidjsonJson::GetValueDerived(const rapidjson::Value &value) const {
	if (value.IsBool()) {
		return value.GetBool();
	} else if (value.IsInt()) {
		return int64_t(value.GetInt());
	} else if (value.IsInt64()) {
		return int64_t(value.GetInt64());
	} else if (value.IsUint()) {
		return uint32_t(value.GetUint());
	} else if (value.IsUint64()) {
		return uint64_t(value.GetUint64());
	} else if (value.IsDouble()) {
		return value.GetDouble();
	} else if (value.IsString()) {
		return string(value.GetString());
	} else if (value.IsNull()) {
		return nullptr;
	}

	return any{};
}

vector<map<string, any>>
RapidjsonJson::GetArray(const vector<string> &key) const {
	const auto object = this->GetObject(key);
	if (object.has_value() == false) {
		return {};
	}

	const rapidjson::Value &value = *any_cast<rapidjson::Value *>(object);
	if (value.IsArray() == false) {
		return {};
	}

	vector<map<string, any>> result{};
	for (const auto &iter1 : value.GetArray()) {
		map<string, any> temp{};

		if (iter1.IsObject()) {
			for (auto &iter2 : iter1.GetObject()) {
				temp[iter2.name.GetString()] =
					this->GetValueDerived(iter2.value);
			}
		} else {
			temp[""] = this->GetValueDerived(iter1);
		}

		result.push_back(temp);
	}

	return result;
}
