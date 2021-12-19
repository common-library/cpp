#include <thread>
using namespace std;

#include "../SocketServer.h"

#include "gtest/gtest.h"

TEST(SocketServerTest, Start)
{
	SocketServer socketServer;

	EXPECT_FALSE(socketServer.Start(-1, 0, 0, nullptr));

	auto serverJobFunc = [] (const SocketClient &socketClient) {
	};

	bool bServerResult = false;
	auto serverThreadFunc = [&] () {
		bServerResult = socketServer.Start(12345, 3, 1, serverJobFunc);
	};

	thread serverThread(serverThreadFunc);

	this_thread::sleep_for(std::chrono::seconds(1));

	EXPECT_TRUE(socketServer.Stop());

	serverThread.join();

	EXPECT_TRUE(bServerResult);
}

TEST(SocketServerTest, Stop)
{
	SocketServer socketServer;

	EXPECT_TRUE(socketServer.Stop());
}
