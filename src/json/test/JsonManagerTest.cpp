#include "../JsonManager.h"

#include "gtest/gtest.h"

#include "FileManager.h"

#include "json_info.h"

enum class E_PARSING_TYPE {
	FILE,
	STRING
};

static void test_json1(const E_PARSING_TYPE &eParsingType, const JsonManager &jsonManager)
{
	switch(eParsingType) {
		case E_PARSING_TYPE::FILE: {
			const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
			const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
			const string strPath = strOutputPath + "/" + "test.config";

			EXPECT_TRUE(FileManager().Write(strPath, strJson1, ios::trunc));

			EXPECT_TRUE(jsonManager.ParsingFile(strPath));

			EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));

			break;
		}
		case E_PARSING_TYPE::STRING: {
			EXPECT_TRUE(jsonManager.ParsingString(strJson1));

			break;
		}
	}

	EXPECT_STREQ(jsonManager.GetValue<string>({"string_1"}).c_str(), "string");
	EXPECT_TRUE(jsonManager.GetValue<bool>({"bool_1"}));
	EXPECT_FALSE(jsonManager.GetValue<bool>({"bool_2"}));
	EXPECT_EQ(jsonManager.GetValue<int>({"int_1"}), 123);
	EXPECT_DOUBLE_EQ(jsonManager.GetValue<double>({"double_1"}), 3.14);

	check_json1_array1(jsonManager);

	check_json1_array2(jsonManager);
}

static void test_json2(const E_PARSING_TYPE &eParsingType, const JsonManager &jsonManager)
{
	switch(eParsingType) {
		case E_PARSING_TYPE::FILE: {
			const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
			const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
			const string strPath = strOutputPath + "/" + "test.config";

			EXPECT_TRUE(FileManager().Write(strPath, strJson2, ios::trunc));
			EXPECT_TRUE(jsonManager.ParsingFile(strPath));
			EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));

			break;
		}
		case E_PARSING_TYPE::STRING: {
			EXPECT_TRUE(jsonManager.ParsingString(strJson2));
			break;
		}
	}

	EXPECT_STREQ(jsonManager.GetValue<string>({"BODY", "string_1"}).c_str(), "string");
	EXPECT_TRUE(jsonManager.GetValue<bool>({"BODY", "bool_1"}));
	EXPECT_FALSE(jsonManager.GetValue<bool>({"BODY", "bool_2"}));
	EXPECT_EQ(jsonManager.GetValue<int>({"BODY", "int_1"}), 123);
	EXPECT_DOUBLE_EQ(jsonManager.GetValue<double>({"BODY", "double_1"}), 3.14);

	check_json2_array1(jsonManager);
}

TEST(JsonManagerTest, boost)
{
	JsonManager jsonManager(E_JSON_PARSER::BOOST);

	test_json1(E_PARSING_TYPE::FILE, jsonManager);
	test_json1(E_PARSING_TYPE::STRING, jsonManager);

	test_json2(E_PARSING_TYPE::FILE, jsonManager);
	test_json2(E_PARSING_TYPE::STRING, jsonManager);
}

TEST(JsonManagerTest, rabbit)
{
	JsonManager jsonManager(E_JSON_PARSER::RABBIT);

	test_json1(E_PARSING_TYPE::FILE, jsonManager);
	test_json1(E_PARSING_TYPE::STRING, jsonManager);

	test_json2(E_PARSING_TYPE::FILE, jsonManager);
	test_json2(E_PARSING_TYPE::STRING, jsonManager);
}

TEST(JsonManagerTest, rapidjson)
{
	JsonManager jsonManager(E_JSON_PARSER::RAPIDJSON);

	test_json1(E_PARSING_TYPE::FILE, jsonManager);
	test_json1(E_PARSING_TYPE::STRING, jsonManager);

	test_json2(E_PARSING_TYPE::FILE, jsonManager);
	test_json2(E_PARSING_TYPE::STRING, jsonManager);
}
