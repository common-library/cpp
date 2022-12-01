#include "../BoostJson.h"

#include "gtest/gtest.h"

#include "json_info.h"

TEST(BoostJsonTest, strJson1) {
	BoostJson boostJson;

	check_json1_basic(boostJson);

	check_json1_array1(boostJson);

	check_json1_array2(boostJson);
}

TEST(BoostJsonTest, strJson2) {
	BoostJson boostJson;

	check_json2_basic(boostJson);

	check_json2_array1(boostJson);
}
