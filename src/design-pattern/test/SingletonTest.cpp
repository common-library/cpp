#include "../Singleton.h"

#include "gtest/gtest.h"

class TestClass {
private:
	int i;
public:
	TestClass(const int &i = 0) : i(i) {};
	~TestClass() = default;

	int GetI() {return this->i;}
};

TEST(SingletonTest, Instance)
{
	if(&Singleton<TestClass>::Instance() != &Singleton<TestClass>::Instance()) {
		EXPECT_STREQ("invalid singleton", "");
	}

	for(int i = 0 ; i < 1000 ; i++) {
		EXPECT_EQ(&Singleton<TestClass>::Instance(i), &Singleton<TestClass>::Instance(i));

		EXPECT_EQ(Singleton<TestClass>::Instance(i).GetI(), Singleton<TestClass>::Instance(i).GetI());
	}
}
