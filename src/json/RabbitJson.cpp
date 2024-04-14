#include "RabbitJson.h"
#include <any>
#include <array>
#include <map>
#include <string>
#include <vector>

using namespace std;

bool RabbitJson::ParsingFromString(const string &contents) const {
	try {
		this->document.parse(contents.c_str());
	} catch (exception &ex) {
		return false;
	}

	return true;
}

any RabbitJson::GetObject(const vector<string> &key) const {
	if (this->document.has(key[0].c_str()) == false) {
		return any{};
	}

	rabbit::object object = this->document[key[0]];

	for (unsigned int index = 1; index < key.size(); index++) {
		if (object.has(key[index]) == false) {
			return any{};
		}

		object = object[key[index]];
	}

	return object;
}

any RabbitJson::GetValueDerived(const any &object) const {
	const rabbit::object &o = any_cast<rabbit::object>(object);

	if (o.is_bool()) {
		return o.as_bool();
	} else if (o.is_int()) {
		return int64_t(o.as_int());
	} else if (o.is_int64()) {
		return int64_t(o.as_int64());
	} else if (o.is_uint()) {
		return uint32_t(o.as_uint());
	} else if (o.is_uint64()) {
		return uint64_t(o.as_uint64());
	} else if (o.is_double()) {
		return o.as_double();
	} else if (o.is_string()) {
		return o.str();
	} else if (o.is_null()) {
		return any{};
	}

	return nullptr;
}

vector<map<string, any>> RabbitJson::GetArray(const vector<string> &key) const {
	if (key.empty()) {
		return {};
	}

	const auto object = this->GetObject(key);
	if (object.has_value() == false) {
		return {};
	}

	const rabbit::object &objectTemp = any_cast<rabbit::object>(object);
	if (objectTemp.is_array() == false) {
		return {};
	}

	vector<map<string, any>> result{};
	for (auto iter1 : rabbit::array{objectTemp}) {
		map<string, any> temp{};

		if (iter1.is_object()) {
			for (auto iter2 = iter1.member_begin(); iter2 != iter1.member_end(); ++iter2) {
				temp[iter2->name()] =
					this->GetValueDerived(make_any<rabbit::object>(iter2->value()));
			}
		} else {
			temp[""] = this->GetValueDerived(make_any<rabbit::object>(iter1));
		}

		result.push_back(temp);
	}

	return result;
}
