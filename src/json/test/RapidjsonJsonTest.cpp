#include "../RapidjsonJson.h"

#include "gtest/gtest.h"

#include "json_info.h"

TEST(RapidjsonJsonTest, strJson1)
{
	RapidjsonJson rapidjsonJson;

	check_json1_basic(rapidjsonJson);

	check_json1_array1(rapidjsonJson);

	check_json1_array2(rapidjsonJson);
}

TEST(RapidjsonJsonTest, strJson2)
{
	RapidjsonJson rapidjsonJson;

	check_json2_basic(rapidjsonJson);

	check_json2_array1(rapidjsonJson);
}
