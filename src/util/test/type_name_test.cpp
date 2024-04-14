#include "../type_name.h"
#include "gtest/gtest.h"
#include <string>

using namespace std;

TEST(type_name_test, type_name) {
	EXPECT_STREQ(string(type_name<int>()).c_str(), "int");
	EXPECT_STREQ(string(type_name<int *>()).c_str(), "int*");

	EXPECT_STREQ(string(type_name<char>()).c_str(), "char");
	EXPECT_STREQ(string(type_name<char *>()).c_str(), "char*");

	EXPECT_STREQ(string(type_name<string>()).c_str(), "std::__cxx11::basic_string<char>");
	EXPECT_STREQ(string(type_name<string *>()).c_str(), "std::__cxx11::basic_string<char>*");
}
