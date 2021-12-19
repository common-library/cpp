static const string strJson1 =
	"{\
		\"string_1\" : \"string\",\
		\"bool_1\" : true ,\
		\"bool_2\" : false,\
		\"null_1\" : null,\
		\"int_1\" : 123,\
		\"double_1\" : 3.14,\
		\"array_1\" : [1, 2, 3],\
		\"array_2\" : [\
						{\
							\"value_1\" : 1,\
							\"value_2\" : 2\
						},\
						{\
							\"value_1\" : 3,\
							\"value_2\" : 4\
						},\
						{\
							\"value_1\" : 5,\
							\"value_2\" : 6\
						}\
					]\
	}";

static const string strJson2 =
	"{\"BODY\" :\
		{\
			\"string_1\" : \"string\",\
			\"bool_1\" : true,\
			\"bool_2\" : false,\
			\"null_1\" : null,\
			\"int_1\" : 123,\
			\"double_1\" : 3.14,\
			\"array_1\" : [\
							{\
								\"value_b\" : true,\
								\"value_i\" : 1,\
								\"value_d\" : 3.14,\
								\"value_s\" : \"str\"\
							}\
						]\
		}\
	}";

template <class T>
void check_json1_basic(T &t);

template <class T>
void check_json1_array1(T &t);

template <class T>
void check_json1_array2(T &t);

template <class T>
void check_json2_basic(T &t);

template <class T>
void check_json2_array1(T &t);

template <class T>
void check_json1_basic(T &t)
{
	EXPECT_TRUE(t.Parsing(strJson1));

	EXPECT_STREQ(boost::get<string>(t.GetValue({"string_1"}, string())).c_str(), "string");
	EXPECT_TRUE(boost::get<bool>(t.GetValue({"bool_1"}, bool())));
	EXPECT_FALSE(boost::get<bool>(t.GetValue({"bool_2"}, bool())));
	EXPECT_EQ(boost::get<int>(t.GetValue({"int_1"}, int())), 123);
	EXPECT_DOUBLE_EQ(boost::get<double>(t.GetValue({"double_1"}, double())), 3.14);
}

template <class T>
void check_json1_array1(T &t)
{
	const map<string, JSON_VALUE_TYPE> mapValueInfo = {{"", int()}};
	const vector<map<string, JSON_VALUE_TYPE>> vecResult =
														{
															{{"", 1}}, {{"", 2}}, {{"", 3}},
														};

	const vector<map<string, JSON_VALUE_TYPE>> vecArray = t.GetArray({}, "array_1", mapValueInfo);

	for(unsigned int ui = 0 ; ui < vecResult.size() ; ui++) {
		for(const auto &iter : vecResult[ui]) {
			EXPECT_EQ(vecArray.at(ui).at(iter.first), iter.second);
		}
	}
}

template <class T>
void check_json1_array2(T &t)
{
	const map<string, JSON_VALUE_TYPE> mapValueInfo = {{"value_1", int()}, {"value_2", int()}};
	const vector<map<string, JSON_VALUE_TYPE>> vecResult =
														{
															{{"value_1", 1}, {"value_2", 2}},
															{{"value_1", 3}, {"value_2", 4}},
															{{"value_1", 5}, {"value_2", 6}},
														};
	const vector<map<string, JSON_VALUE_TYPE>> vecArray = t.GetArray({}, "array_2", mapValueInfo);
	for(unsigned int ui = 0 ; ui < vecResult.size() ; ui++) {
		for(const auto &iter : vecResult[ui]) {
			EXPECT_EQ(vecArray.at(ui).at(iter.first), iter.second);
		}
	}
}

template <class T>
void check_json2_basic(T &t)
{
	EXPECT_TRUE(t.Parsing(strJson2));

	EXPECT_STREQ(boost::get<string>(t.GetValue({"BODY", "string_1"}, string())).c_str(), "string");

	EXPECT_TRUE(boost::get<bool>(t.GetValue({"BODY", "bool_1"}, bool())));
	EXPECT_FALSE(boost::get<bool>(t.GetValue({"BODY", "bool_2"}, bool())));
	EXPECT_EQ(boost::get<int>(t.GetValue({"BODY", "int_1"}, int())), 123);
	EXPECT_DOUBLE_EQ(boost::get<double>(t.GetValue({"BODY", "double_1"}, double())), 3.14);
}

template <class T>
void check_json2_array1(T &t)
{
	const map<string, JSON_VALUE_TYPE> mapValueInfo = {{"value_b", bool()}, {"value_i", int()}, {"value_d", double()}, {"value_s", string()}};
	const vector<map<string, JSON_VALUE_TYPE>> vecResult = {{{"value_b", true}, {"value_i", 1}, {"value_d", 3.14}, {"value_s", "str"}}};
	const vector<map<string, JSON_VALUE_TYPE>> vecArray = t.GetArray({"BODY"}, "array_1", mapValueInfo);
	for(unsigned int ui = 0 ; ui < vecResult.size() ; ui++) {
		for(const auto &iter : vecResult[ui]) {
			if(typeid(decltype(iter.second)) == typeid(bool)) {
				EXPECT_TRUE(boost::get<bool>(vecArray.at(ui).at(iter.first)));
			} else if(typeid(decltype(iter.second)) == typeid(int)) {
				EXPECT_EQ(boost::get<int>(vecArray.at(ui).at(iter.first)), boost::get<int>(iter.second));
			} else if(typeid(decltype(iter.second)) == typeid(double)) {
				EXPECT_DOUBLE_EQ(boost::get<double>(vecArray.at(ui).at(iter.first)), boost::get<double>(iter.second));
			} else if(typeid(decltype(iter.second)) == typeid(string)) {
				EXPECT_STREQ(boost::get<string>(vecArray.at(ui).at(iter.first)).c_str(), boost::get<string>(iter.second).c_str());
			}
		}
	}
}

