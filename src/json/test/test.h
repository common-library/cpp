#pragma once

#include "../Json.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <map>
#include <string>
#include <vector>

using namespace std;

static const string contents1 = "{\
		\"bool_1\" : true,\
		\"bool_2\" : false,\
		\"int_1\" : 123,\
		\"double_1\" : 3.14,\
		\"string_1\" : \"string_1\",\
		\"null_1\" : null,\
		\"array_1\" : [1, 2, 3],\
		\"array_2\" : [\
						{\
							\"value_1\" : 1,\
							\"value_2\" : 2\
						},\
						{\
							\"value_1\" : 3,\
							\"value_2\" : 4\
						},\
						{\
							\"value_1\" : 5,\
							\"value_2\" : 6\
						}\
					]\
	}";

static const string contents2 = "{\"BODY\" :\
		{\
			\"bool_1\" : true,\
			\"bool_2\" : false,\
			\"int_1\" : 123,\
			\"double_1\" : 3.14,\
			\"string_1\" : \"string_1\",\
			\"null_1\" : null\
		}\
	}";

static void check_contents1(const Json &json, const bool &doParsing);
static void check_contents2(const Json &json, const bool &doParsing);

void check_contents1(const Json &json, const bool &doParsing) {
	if (doParsing) {
		EXPECT_TRUE(json.ParsingFromString(contents1));
	}

	for (int i = 0; i < 100; ++i) {
		EXPECT_FALSE(json.WhetherTheKeyExists({"aaa"}));
		EXPECT_FALSE(json.IsType<bool>({"aaa"}));
		EXPECT_FALSE(json.GetValue<bool>({"aaa"}));

		EXPECT_TRUE(json.WhetherTheKeyExists({"bool_1"}));
		EXPECT_FALSE(json.IsType<int64_t>({"bool_1"}));
		EXPECT_TRUE(json.IsType<bool>({"bool_1"}));
		EXPECT_TRUE(json.GetValue<bool>({"bool_1"}));

		EXPECT_TRUE(json.WhetherTheKeyExists({"bool_2"}));
		EXPECT_FALSE(json.IsType<int64_t>({"bool_2"}));
		EXPECT_TRUE(json.IsType<bool>({"bool_2"}));
		EXPECT_FALSE(json.GetValue<bool>({"bool_2"}));

		EXPECT_TRUE(json.WhetherTheKeyExists({"int_1"}));
		EXPECT_FALSE(json.IsType<string>({"int_1"}));
		EXPECT_TRUE(json.IsType<int64_t>({"int_1"}));
		EXPECT_EQ(json.GetValue<int64_t>({"int_1"}), 123);

		EXPECT_TRUE(json.WhetherTheKeyExists({"double_1"}));
		EXPECT_FALSE(json.IsType<int64_t>({"double_1"}));
		EXPECT_TRUE(json.IsType<double>({"double_1"}));
		EXPECT_EQ(json.GetValue<double>({"double_1"}), 3.14);

		EXPECT_TRUE(json.WhetherTheKeyExists({"string_1"}));
		EXPECT_FALSE(json.IsType<nullptr_t>({"string_1"}));
		EXPECT_TRUE(json.IsType<string>({"string_1"}));
		EXPECT_STREQ(json.GetValue<string>({"string_1"}).c_str(), "string_1");

		EXPECT_TRUE(json.WhetherTheKeyExists({"null_1"}));
		EXPECT_FALSE(json.IsType<int64_t>({"null_1"}));
		EXPECT_TRUE(json.IsType<nullptr_t>({"null_1"}));
		EXPECT_EQ(json.GetValue<nullptr_t>({"null_1"}), nullptr);

		EXPECT_EQ(json.GetArray({""}).size(), 0);
		EXPECT_EQ(json.GetArray({"array_0"}).size(), 0);
		EXPECT_EQ(json.GetArray({"bool_1"}).size(), 0);

		vector<int> resultForArray1{};
		const vector<int> answerForArray1{1, 2, 3};
		for (const auto &iter1 : json.GetArray({"array_1"})) {
			for (const auto &iter2 : iter1) {
				resultForArray1.push_back(any_cast<int64_t>(iter2.second));
			}
		}
		EXPECT_TRUE(resultForArray1 == answerForArray1);

		map<string, vector<int>> resultForArray2{};
		const map<string, vector<int>> answerForArray2{{"value_1", {1, 3, 5}},
													   {"value_2", {2, 4, 6}}};
		for (const auto &iter1 : json.GetArray({"array_2"})) {
			for (const auto &iter2 : iter1) {
				resultForArray2[iter2.first].push_back(any_cast<int64_t>(iter2.second));
			}
		}
		EXPECT_TRUE(resultForArray2 == answerForArray2);
	}
}

void check_contents2(const Json &json, const bool &doParsing) {
	if (doParsing) {
		EXPECT_TRUE(json.ParsingFromString(contents2));
	}

	for (int i = 0; i < 100; ++i) {
		EXPECT_FALSE(json.WhetherTheKeyExists({"BODY", "aaa"}));
		EXPECT_FALSE(json.IsType<bool>({"BODY", "aaa"}));
		EXPECT_FALSE(json.GetValue<bool>({"BODY", "aaa"}));

		EXPECT_TRUE(json.WhetherTheKeyExists({"BODY", "bool_1"}));
		EXPECT_FALSE(json.IsType<int64_t>({"BODY", "bool_1"}));
		EXPECT_TRUE(json.IsType<bool>({"BODY", "bool_1"}));
		EXPECT_TRUE(json.GetValue<bool>({"BODY", "bool_1"}));

		EXPECT_TRUE(json.WhetherTheKeyExists({"BODY", "bool_2"}));
		EXPECT_FALSE(json.IsType<int64_t>({"BODY", "bool_2"}));
		EXPECT_TRUE(json.IsType<bool>({"BODY", "bool_2"}));
		EXPECT_FALSE(json.GetValue<bool>({"BODY", "bool_2"}));

		EXPECT_TRUE(json.WhetherTheKeyExists({"BODY", "int_1"}));
		EXPECT_FALSE(json.IsType<string>({"BODY", "int_1"}));
		EXPECT_TRUE(json.IsType<int64_t>({"BODY", "int_1"}));
		EXPECT_EQ(json.GetValue<int64_t>({"BODY", "int_1"}), 123);

		EXPECT_TRUE(json.WhetherTheKeyExists({"BODY", "double_1"}));
		EXPECT_FALSE(json.IsType<int64_t>({"BODY", "double_1"}));
		EXPECT_TRUE(json.IsType<double>({"BODY", "double_1"}));
		EXPECT_EQ(json.GetValue<double>({"BODY", "double_1"}), 3.14);

		EXPECT_TRUE(json.WhetherTheKeyExists({"BODY", "string_1"}));
		EXPECT_FALSE(json.IsType<nullptr_t>({"BODY", "string_1"}));
		EXPECT_TRUE(json.IsType<string>({"BODY", "string_1"}));
		EXPECT_STREQ(json.GetValue<string>({"BODY", "string_1"}).c_str(), "string_1");

		EXPECT_TRUE(json.WhetherTheKeyExists({"BODY", "null_1"}));
		EXPECT_FALSE(json.IsType<int64_t>({"BODY", "null_1"}));
		EXPECT_TRUE(json.IsType<nullptr_t>({"BODY", "null_1"}));
		EXPECT_EQ(json.GetValue<nullptr_t>({"BODY", "null_1"}), nullptr);
	}
}
