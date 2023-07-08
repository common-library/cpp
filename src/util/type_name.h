#pragma once

#include <cstring>
#include <string>
#include <string_view>

using namespace std;

// template <typename T> string type_name();

#if __cplusplus == 199711L
template <typename T> string type_name() {
	const string s = __PRETTY_FUNCTION__;
	const int prefixSize = s.find("[with T = ") + strlen("[with T = ");

	return string(s.data() + prefixSize, s.find(';') - prefixSize);
}
#elif __cplusplus == 201103L
template <typename T> constexpr string type_name() {
	const string s = __PRETTY_FUNCTION__;
	const int prefixSize = s.find("[with T = ") + strlen("[with T = ");

	return string(s.data() + prefixSize, s.find(';') - prefixSize);
}
#elif __cplusplus >= 201703L
template <typename T> constexpr string_view type_name() {
	const string_view s = __PRETTY_FUNCTION__;
	const int prefixSize = s.find("[with T = ") + strlen("[with T = ");

	return string_view(s.data() + prefixSize, s.find(';') - prefixSize);
}
#endif
