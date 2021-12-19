#include "gtest/gtest.h"

int main(int iArgc, char *pcArgv[])
{
	testing::InitGoogleTest(&iArgc, pcArgv);

	return RUN_ALL_TESTS();
}
