#include "test.h"

#include "../CommonConfig.h"

#include "gtest/gtest.h"

static CommonConfig get_config()
{
	CommonConfig commonConfig;

	EXPECT_TRUE(commonConfig.Initialize(GstrConfigPath));

	return commonConfig;
}

TEST(CommonConfigTest, Initialize)
{
	CommonConfig commonConfig;

	EXPECT_TRUE(commonConfig.Initialize(GstrConfigPath));
	EXPECT_FALSE(commonConfig.Initialize(""));
}

TEST(CommonConfigTest, GetWorkingPath)
{
	EXPECT_STREQ(get_config().GetWorkingPath().c_str(), "/tmp/test/socket_server");
}

TEST(CommonConfigTest, GetLogLevel)
{
	if(get_config().GetLogLevel() != E_LOG_LEVEL::DEBUG) {
		EXPECT_STREQ("invalid GetLogLevel()", "");
	}
}

TEST(CommonConfigTest, GetThreadMode)
{
	EXPECT_TRUE(get_config().GetThreadMode());
}

TEST(CommonConfigTest, GetLogOutputPath)
{
	EXPECT_STREQ(get_config().GetLogOutputPath().c_str(), "/tmp/test/log/");
}

TEST(CommonConfigTest, GetLogFileNamePrefix)
{
	EXPECT_STREQ(get_config().GetLogFileNamePrefix().c_str(), "socket_server");
}
