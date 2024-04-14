#include "trim.h"
#include <algorithm>
#include <string>

using namespace std;

string &trim(string &str) { return ltrim(rtrim(str)); }

string trim_copy(string str) { return ltrim(rtrim(str)); }

string &ltrim(string &str) {
	str.erase(str.begin(),
			  find_if(str.begin(), str.end(), [](unsigned char c) { return !isspace(c); }));

	return str;
}

string ltrim_copy(string str) { return ltrim(str); }

string &rtrim(string &str) {
	str.erase(find_if(str.rbegin(), str.rend(), [](unsigned char c) { return !isspace(c); }).base(),
			  str.end());

	return str;
}

string rtrim_copy(string str) { return rtrim(str); }
