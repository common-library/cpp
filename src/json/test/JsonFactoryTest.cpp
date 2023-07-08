#include "../JsonFactory.h"
#include "test.h"
#include "type_name.h"
#include "gtest/gtest.h"

TEST(JsonFactoryTest, Make) {
	auto rabbitJson = JsonFactory::Instance().Make(JSON_TYPE::RABBIT);
	EXPECT_TRUE(rabbitJson->ParsingFromString(contents1));
	check_contents1(*rabbitJson.get(), false);
	EXPECT_TRUE(rabbitJson->ParsingFromString(contents2));
	check_contents2(*rabbitJson.get(), false);

	auto rapidjsonJson = JsonFactory::Instance().Make(JSON_TYPE::RAPIDJSON);
	EXPECT_TRUE(rapidjsonJson->ParsingFromString(contents1));
	check_contents1(*rapidjsonJson.get(), false);
	EXPECT_TRUE(rapidjsonJson->ParsingFromString(contents2));
	check_contents2(*rapidjsonJson.get(), false);
}

TEST(JsonFactoryTest, Instance) {
	for (int i = 0; i < 100; ++i) {
		EXPECT_EQ(&JsonFactory::Instance(), &JsonFactory::Instance());
	}
}
