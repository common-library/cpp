#include "../RabbitJson.h"

#include "gtest/gtest.h"

#include "json_info.h"

TEST(RabbitJsonTest, strJson1) {
	RabbitJson rabbitJson;

	check_json1_basic(rabbitJson);

	check_json1_array1(rabbitJson);

	check_json1_array2(rabbitJson);
}

TEST(RabbitJsonTest, strJson2) {
	RabbitJson rabbitJson;

	check_json2_basic(rabbitJson);

	check_json2_array1(rabbitJson);
}
