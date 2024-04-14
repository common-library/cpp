#include "../SocketServer.h"
#include "gtest/gtest.h"
#include <future>
#include <string>
#include <vector>

using namespace std;

TEST(SocketServerTest, Start) {
	const string greeting = "greeting\r\n";

	SocketServer socketServer;

	EXPECT_FALSE(socketServer.Start(0, 0, 0, nullptr));

	for (int i = 0; i < 3; ++i) {
		EXPECT_TRUE(socketServer.Start(12345, 5, i, [greeting](const SocketClient &socketClient) {
			EXPECT_TRUE(socketClient.Write(greeting));
		}));

		vector<future<void>> futures{};
		for (int j = 0; j < 100; ++j) {
			socketServer.SetPoolSize(j % 10);

			futures.push_back(async(launch::async, [greeting]() {
				SocketClient socketClient("127.0.0.1", 12345, 5);

				bool end = false;
				const auto result = socketClient.Read(1024, end);

				EXPECT_TRUE(end);
				EXPECT_TRUE(get<0>(result));
				EXPECT_STREQ(get<1>(result).c_str(), greeting.c_str());
			}));
		}
		for (auto &iter : futures) {
			iter.get();
		}
		futures.clear();

		EXPECT_TRUE(socketServer.Stop());
	}
}

TEST(SocketServerTest, Stop) {
	SocketServer socketServer;

	EXPECT_TRUE(socketServer.Stop());
}

TEST(SocketServerTest, SetPoolSize) {
	SocketServer socketServer;

	socketServer.SetPoolSize(1);
}
