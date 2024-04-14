#include "../SocketClient.h"
#include "../SocketServer.h"
#include "gtest/gtest.h"
#include <string>

using namespace std;

static const string greeting = "greeting\r\n";
static const string message = "message\r\n";

TEST(SocketClientTest, Read) {
	SocketServer socketServer;

	EXPECT_TRUE(socketServer.Start(12345, 5, 10, [](const SocketClient &socketClient) {
		EXPECT_TRUE(socketClient.Write(greeting));

		bool end = false;
		const auto result = socketClient.Read(1024, end);
		EXPECT_TRUE(end);
		EXPECT_TRUE(get<0>(result));
		EXPECT_STREQ(get<1>(result).c_str(), message.c_str());
	}));

	SocketClient socketClient("127.0.0.1", 12345, 5);

	bool end = false;
	const auto result = socketClient.Read(1024, end);
	EXPECT_TRUE(end);
	EXPECT_TRUE(get<0>(result));
	EXPECT_STREQ(get<1>(result).c_str(), greeting.c_str());

	EXPECT_TRUE(socketClient.Write(message));
}

TEST(SocketClientTest, Write) {
	SocketServer socketServer;

	EXPECT_TRUE(socketServer.Start(12345, 5, 10, [](const SocketClient &socketClient) {
		EXPECT_TRUE(socketClient.Write(greeting));

		bool end = false;
		const auto result = socketClient.Read(1024, end);
		EXPECT_TRUE(end);
		EXPECT_TRUE(get<0>(result));
		EXPECT_STREQ(get<1>(result).c_str(), message.c_str());
	}));

	SocketClient socketClient("127.0.0.1", 12345, 5);

	bool end = false;
	const auto result = socketClient.Read(1024, end);
	EXPECT_TRUE(end);
	EXPECT_TRUE(get<0>(result));
	EXPECT_STREQ(get<1>(result).c_str(), greeting.c_str());

	EXPECT_TRUE(socketClient.Write(message));
}

TEST(SocketClientTest, ReadGarbage) {
	SocketServer socketServer;

	EXPECT_TRUE(socketServer.Start(12345, 5, 10, [](const SocketClient &socketClient) {
		EXPECT_TRUE(socketClient.Write(greeting));
	}));

	SocketClient socketClient("127.0.0.1", 12345, 5);

	bool end = false;

	auto result = socketClient.Read(1, end);
	EXPECT_FALSE(end);
	EXPECT_TRUE(get<0>(result));
	EXPECT_STREQ(get<1>(result).c_str(), greeting.substr(0, 1).c_str());

	result = socketClient.Read(2, end);
	EXPECT_FALSE(end);
	EXPECT_TRUE(get<0>(result));
	EXPECT_STREQ(get<1>(result).c_str(), greeting.substr(1, 2).c_str());

	EXPECT_TRUE(socketClient.ReadGarbage());

	result = socketClient.Read(2, end);
	EXPECT_FALSE(get<0>(result));
}

TEST(SocketClientTest, GetPeerAddress) {
	SocketServer socketServer;

	EXPECT_TRUE(socketServer.Start(12345, 5, 10, [](const SocketClient &socketClient) {
		EXPECT_STREQ(socketClient.GetPeerAddress().c_str(), "127.0.0.1");

		EXPECT_TRUE(socketClient.Write(greeting));
	}));

	SocketClient socketClient("127.0.0.1", 12345, 5);

	EXPECT_STREQ(socketClient.GetPeerAddress().c_str(), "0.0.0.0");

	EXPECT_TRUE(socketClient.ReadGarbage());
}

TEST(SocketClientTest, GetPeerPort) {
	SocketServer socketServer;

	EXPECT_TRUE(socketServer.Start(12345, 5, 10, [](const SocketClient &socketClient) {
		EXPECT_NE(socketClient.GetPeerPort(), 0);

		EXPECT_TRUE(socketClient.Write(greeting));
	}));

	SocketClient socketClient("127.0.0.1", 12345, 5);

	EXPECT_EQ(socketClient.GetPeerPort(), 12345);

	EXPECT_TRUE(socketClient.ReadGarbage());
}
