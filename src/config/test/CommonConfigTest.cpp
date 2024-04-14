#include "../CommonConfig.h"
#include "test.h"
#include "gtest/gtest.h"
#include <memory>

static unique_ptr<CommonConfig> get_config() {
	auto commonConfig = make_unique<CommonConfig>();

	EXPECT_TRUE(commonConfig->Initialize(CONFIG_PATH));

	return commonConfig;
}

TEST(CommonConfigTest, Initialize) {
	CommonConfig commonConfig;

	EXPECT_TRUE(commonConfig.Initialize(CONFIG_PATH));
	EXPECT_FALSE(commonConfig.Initialize(""));
}

TEST(CommonConfigTest, GetFileName) {
	EXPECT_STREQ(get_config()->GetFileName().c_str(), "common.config");
}

TEST(CommonConfigTest, GetLogLevel) { EXPECT_EQ(get_config()->GetLogLevel(), LOG_LEVEL::DEBUG); }

TEST(CommonConfigTest, GetWorkingPath) {
	EXPECT_STREQ(get_config()->GetWorkingPath().c_str(), "/tmp/test/");
}

TEST(CommonConfigTest, GetLogOutputPath) {
	EXPECT_STREQ(get_config()->GetLogOutputPath().c_str(), "/tmp/test/log/");
}

TEST(CommonConfigTest, GetLogFileName) {
	EXPECT_STREQ(get_config()->GetLogFileName().c_str(), "test");
}

TEST(CommonConfigTest, GetLogLinePrint) { EXPECT_TRUE(get_config()->GetLogLinePrint()); }

TEST(CommonConfigTest, GetLogThreadMode) { EXPECT_TRUE(get_config()->GetLogThreadMode()); }
