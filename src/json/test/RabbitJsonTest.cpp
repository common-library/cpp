#include "../RabbitJson.h"
#include "test.h"
#include "gtest/gtest.h"

TEST(RabbitJsonTest, ParsingFromFile) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	auto job = [&dirPath](const auto &contents, const auto &check) {
		const auto path = dirPath + "/" + "json.json";
		EXPECT_TRUE(FileManager::Instance().Write(path, contents, ios::trunc));

		RabbitJson json;

		EXPECT_TRUE(json.ParsingFromFile(path));
		check(json, false);
	};

	job(contents1, check_contents1);
	job(contents2, check_contents2);

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
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
