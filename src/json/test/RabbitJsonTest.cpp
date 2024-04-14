#include "../RabbitJson.h"
#include "FileManager.h"
#include "test.h"
#include "gtest/gtest.h"
#include <string>

using namespace std;

TEST(RabbitJsonTest, ParsingFromFile) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	if (const auto [ok, errorCode] = FileManager::Instance().CreateDirectory(dirPath); errorCode) {
		EXPECT_EQ(errorCode.value(), -1);
		EXPECT_STREQ(errorCode.message().c_str(), "");
	} else if (ok == false) {
		EXPECT_STREQ("invalid", "");
	}

	auto job = [&dirPath](const auto &contents, const auto &check) {
		const auto path = dirPath + "/" + "json.json";
		EXPECT_FALSE(FileManager::Instance().Write(path, contents, ios::trunc));

		RabbitJson json;

		EXPECT_TRUE(json.ParsingFromFile(path));
		check(json, false);
	};

	job(contents1, check_contents1);
	job(contents2, check_contents2);

	if (const auto [ok, errorCode] = FileManager::Instance().RemoveAll(dirPath); errorCode) {
		EXPECT_EQ(errorCode.value(), -1);
		EXPECT_STREQ(errorCode.message().c_str(), "");
	} else if (ok == false) {
		EXPECT_STREQ("invalid", "");
	}
}

TEST(RabbitJsonTest, ParsingFromString) {
	RabbitJson json;

	EXPECT_FALSE(json.ParsingFromString("invalid"));

	EXPECT_TRUE(json.ParsingFromString(contents1));
	check_contents1(json, false);

	EXPECT_TRUE(json.ParsingFromString(contents2));
	check_contents2(json, false);
}

TEST(RabbitJsonTest, GetValue) {
	RabbitJson json;

	check_contents1(json, true);
	check_contents2(json, true);
}

TEST(RabbitJsonTest, GetArray) {
	RabbitJson json;

	check_contents1(json, true);
	check_contents2(json, true);
}
