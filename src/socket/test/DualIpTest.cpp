#include "../DualIp.h"
#include "gtest/gtest.h"

TEST(DualIpTest, Valid) {
	EXPECT_TRUE(DualIp("127.0.0.1", 10000).Valid());
	EXPECT_FALSE(DualIp("999.0.0.1", 10000).Valid());
}

TEST(DualIpTest, GetAddrInfo) { EXPECT_NE(DualIp("127.0.0.1", 10000).GetAddrInfo(), nullptr); }
