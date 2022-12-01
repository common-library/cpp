#include "../trim.h"

#include "gtest/gtest.h"

TEST(utilTest, trim) {
	const map<string, string> mapResult = {
		{"abc", "abc"},			{"abc\t", "abc"},			{"abc\r", "abc"},
		{"abc\n", "abc"},		{"abc\t\r", "abc"},			{"abc\t\r\n", "abc"},
		{"abc ", "abc"},		{"abc  ", "abc"},			{"abc   ", "abc"},

		{"\tabc", "abc"},		{"\rabc", "abc"},			{"\nabc", "abc"},
		{"\t\rabc", "abc"},		{"\t\r\nabc", "abc"},		{" abc", "abc"},
		{"  abc", "abc"},		{"   abc", "abc"},

		{"\tabc\t", "abc"},		{"\rabc\r", "abc"},			{"\nabc\n", "abc"},
		{"\t\rabc\t\r", "abc"}, {"\t\r\nabc\t\r\n", "abc"}, {" abc ", "abc"},
		{"  abc  ", "abc"},		{"   abc   ", "abc"},
	};

	for (const auto& iter : mapResult) {
		string strInput = iter.first;

		string strOuput = trim(strInput);

		EXPECT_STREQ(strInput.c_str(), strOuput.c_str());

		EXPECT_STREQ(strOuput.c_str(), iter.second.c_str());
	}
}

TEST(utilTest, trim_copy) {
	const map<string, string> mapResult = {
		{"abc", "abc"},			{"abc\t", "abc"},			{"abc\r", "abc"},
		{"abc\n", "abc"},		{"abc\t\r", "abc"},			{"abc\t\r\n", "abc"},
		{"abc ", "abc"},		{"abc  ", "abc"},			{"abc   ", "abc"},

		{"\tabc", "abc"},		{"\rabc", "abc"},			{"\nabc", "abc"},
		{"\t\rabc", "abc"},		{"\t\r\nabc", "abc"},		{" abc", "abc"},
		{"  abc", "abc"},		{"   abc", "abc"},

		{"\tabc\t", "abc"},		{"\rabc\r", "abc"},			{"\nabc\n", "abc"},
		{"\t\rabc\t\r", "abc"}, {"\t\r\nabc\t\r\n", "abc"}, {" abc ", "abc"},
		{"  abc  ", "abc"},		{"   abc   ", "abc"},
	};

	for (const auto& iter : mapResult) {
		string strInput = iter.first;

		string strOuput = trim_copy(strInput);

		EXPECT_STREQ(strInput.c_str(), iter.first.c_str());

		EXPECT_STREQ(strOuput.c_str(), iter.second.c_str());
	}
}

TEST(utilTest, ltrim) {
	const map<string, string> mapResult = {
		{"abc", "abc"},
		{"abc\t", "abc\t"},
		{"abc\r", "abc\r"},
		{"abc\n", "abc\n"},
		{"abc\t\r", "abc\t\r"},
		{"abc\t\r\n", "abc\t\r\n"},
		{"abc ", "abc "},
		{"abc  ", "abc  "},
		{"abc   ", "abc   "},

		{"\tabc", "abc"},
		{"\rabc", "abc"},
		{"\nabc", "abc"},
		{"\t\rabc", "abc"},
		{"\t\r\nabc", "abc"},
		{" abc", "abc"},
		{"  abc", "abc"},
		{"   abc", "abc"},

		{"\tabc\t", "abc\t"},
		{"\rabc\r", "abc\r"},
		{"\nabc\n", "abc\n"},
		{"\t\rabc\t\r", "abc\t\r"},
		{"\t\r\nabc\t\r\n", "abc\t\r\n"},
		{" abc ", "abc "},
		{"  abc  ", "abc  "},
		{"   abc   ", "abc   "},
	};

	for (const auto& iter : mapResult) {
		string strInput = iter.first;

		string strOuput = ltrim(strInput);

		EXPECT_STREQ(strInput.c_str(), strOuput.c_str());

		EXPECT_STREQ(strOuput.c_str(), iter.second.c_str());
	}
}

TEST(utilTest, ltrim_copy) {
	const map<string, string> mapResult = {
		{"abc", "abc"},
		{"abc\t", "abc\t"},
		{"abc\r", "abc\r"},
		{"abc\n", "abc\n"},
		{"abc\t\r", "abc\t\r"},
		{"abc\t\r\n", "abc\t\r\n"},
		{"abc ", "abc "},
		{"abc  ", "abc  "},
		{"abc   ", "abc   "},

		{"\tabc", "abc"},
		{"\rabc", "abc"},
		{"\nabc", "abc"},
		{"\t\rabc", "abc"},
		{"\t\r\nabc", "abc"},
		{" abc", "abc"},
		{"  abc", "abc"},
		{"   abc", "abc"},

		{"\tabc\t", "abc\t"},
		{"\rabc\r", "abc\r"},
		{"\nabc\n", "abc\n"},
		{"\t\rabc\t\r", "abc\t\r"},
		{"\t\r\nabc\t\r\n", "abc\t\r\n"},
		{" abc ", "abc "},
		{"  abc  ", "abc  "},
		{"   abc   ", "abc   "},
	};

	for (const auto& iter : mapResult) {
		string strInput = iter.first;

		string strOuput = ltrim_copy(strInput);

		EXPECT_STREQ(strInput.c_str(), iter.first.c_str());

		EXPECT_STREQ(strOuput.c_str(), iter.second.c_str());
	}
}

TEST(utilTest, rtrim) {
	const map<string, string> mapResult = {
		{"abc", "abc"},
		{"abc\t", "abc"},
		{"abc\r", "abc"},
		{"abc\n", "abc"},
		{"abc\t\r", "abc"},
		{"abc\t\r\n", "abc"},
		{"abc ", "abc"},
		{"abc  ", "abc"},
		{"abc   ", "abc"},

		{"\tabc", "\tabc"},
		{"\rabc", "\rabc"},
		{"\nabc", "\nabc"},
		{"\t\rabc", "\t\rabc"},
		{"\t\r\nabc", "\t\r\nabc"},
		{" abc", " abc"},
		{"  abc", "  abc"},
		{"   abc", "   abc"},

		{"\tabc\t", "\tabc"},
		{"\rabc\r", "\rabc"},
		{"\nabc\n", "\nabc"},
		{"\t\rabc\t\r", "\t\rabc"},
		{"\t\r\nabc\t\r\n", "\t\r\nabc"},
		{" abc ", " abc"},
		{"  abc  ", "  abc"},
		{"   abc   ", "   abc"},
	};

	for (const auto& iter : mapResult) {
		string strInput = iter.first;

		string strOuput = rtrim(strInput);

		EXPECT_STREQ(strInput.c_str(), strOuput.c_str());

		EXPECT_STREQ(strOuput.c_str(), iter.second.c_str());
	}
}

TEST(utilTest, rtrim_copy) {
	const map<string, string> mapResult = {
		{"abc", "abc"},
		{"abc\t", "abc"},
		{"abc\r", "abc"},
		{"abc\n", "abc"},
		{"abc\t\r", "abc"},
		{"abc\t\r\n", "abc"},
		{"abc ", "abc"},
		{"abc  ", "abc"},
		{"abc   ", "abc"},

		{"\tabc", "\tabc"},
		{"\rabc", "\rabc"},
		{"\nabc", "\nabc"},
		{"\t\rabc", "\t\rabc"},
		{"\t\r\nabc", "\t\r\nabc"},
		{" abc", " abc"},
		{"  abc", "  abc"},
		{"   abc", "   abc"},

		{"\tabc\t", "\tabc"},
		{"\rabc\r", "\rabc"},
		{"\nabc\n", "\nabc"},
		{"\t\rabc\t\r", "\t\rabc"},
		{"\t\r\nabc\t\r\n", "\t\r\nabc"},
		{" abc ", " abc"},
		{"  abc  ", "  abc"},
		{"   abc   ", "   abc"},
	};

	for (const auto& iter : mapResult) {
		string strInput = iter.first;

		string strOuput = rtrim_copy(strInput);

		EXPECT_STREQ(strInput.c_str(), iter.first.c_str());

		EXPECT_STREQ(strOuput.c_str(), iter.second.c_str());
	}
}
