#pragma once

#include "FileManager.h"
#include <any>
#include <concepts>
#include <map>
#include <string>
#include <vector>

using namespace std;

template <typename T>
concept json_data_type =
	is_same<T, nullptr_t>::value || is_same<T, bool>::value || is_same<T, int64_t>::value ||
	is_same<T, uint64_t>::value || is_same<T, double>::value || is_same<T, string>::value;

class Json {
	private:
		virtual any GetObject(const vector<string> &key) const = 0;

		virtual any GetValueDerived(const any &object) const = 0;

	public:
		Json() = default;
		virtual ~Json() = default;

		virtual bool ParsingFromFile(const string &path) const final;
		virtual bool ParsingFromString(const string &contents) const = 0;

		virtual bool WhetherTheKeyExists(const vector<string> &key) const final;

		template <typename T>
			requires json_data_type<T>
		bool IsType(const vector<string> &key) const {
			auto object = this->GetObject(key);
			if (object.has_value() == false) {
				return false;
			}

			try {
				any_cast<T>(this->GetValueDerived(object));
			} catch (const bad_any_cast &e) {
				return false;
			}

			return true;
		}

		template <typename T>
			requires json_data_type<T>
		T GetValue(const vector<string> &key) const {
			if (this->WhetherTheKeyExists(key) == false) {
				return T();
			}

			return any_cast<T>(this->GetValueDerived(this->GetObject(key)));
		}

		virtual vector<map<string, any>> GetArray(const vector<string> &key) const = 0;
};
