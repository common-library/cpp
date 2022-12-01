#include <thread>
using namespace std;

#include "../SocketClient.h"

#include "gtest/gtest.h"

#include "../SocketServer.h"

static const string strGreeting = "greeting\r\n";
static const string strWrite = "test line\r\n";
static const string strPrefixResponse = "[response] ";

TEST(SocketClientTest, ReadGarbage) {
	auto serverJobFunc = [](const SocketClient& socketClient) {
		EXPECT_TRUE(socketClient.Write(strGreeting));

		bool bEnd = false;
		string strRead = "";

		EXPECT_TRUE(socketClient.Read(strRead, 1024, bEnd));

		EXPECT_STREQ(strRead.c_str(), strWrite.c_str());
	};

	SocketServer socketServer;

	bool bServerResult = false;
	auto serverThreadFunc = [&]() {
		bServerResult = socketServer.Start(12345, 3, 1, serverJobFunc);
	};

	thread serverThread(serverThreadFunc);

	this_thread::sleep_for(std::chrono::seconds(1));

	SocketClient socketClient("127.0.0.1", 12345, 3);

	bool bEnd = false;
	string strRead = "";
	const int iIndex = 3;

	EXPECT_TRUE(socketClient.Read(strRead, iIndex, bEnd));
	EXPECT_FALSE(bEnd);
	EXPECT_STREQ(strRead.c_str(), strGreeting.substr(0, iIndex).c_str());

	EXPECT_STREQ(socketClient.ReadGarbage().c_str(), strGreeting.c_str() + iIndex);

	EXPECT_TRUE(socketClient.Write(strWrite));

	EXPECT_TRUE(socketServer.Stop());

	serverThread.join();

	EXPECT_TRUE(bServerResult);
}

TEST(SocketClientTest, GetPeerAddress) {
	auto serverJobFunc = [](const SocketClient& socketClient) {
		EXPECT_STREQ(socketClient.GetPeerAddress().c_str(), "127.0.0.1");

		EXPECT_TRUE(socketClient.Write(strGreeting));
	};

	SocketServer socketServer;

	auto serverThreadFunc = [&]() { socketServer.Start(12345, 3, 1, serverJobFunc); };

	thread serverThread(serverThreadFunc);

	this_thread::sleep_for(std::chrono::seconds(1));

	SocketClient socketClient("127.0.0.1", 12345, 3);

	bool bEnd = false;
	string strRead = "";
	EXPECT_TRUE(socketClient.Read(strRead, 1024, bEnd));
	EXPECT_STREQ(strRead.c_str(), strGreeting.c_str());

	EXPECT_TRUE(socketServer.Stop());

	serverThread.join();
}

TEST(SocketClientTest, GetPeerPort) {
	auto serverJobFunc = [](const SocketClient& socketClient) {
		EXPECT_NE(socketClient.GetPeerPort(), -1);

		EXPECT_TRUE(socketClient.Write(strGreeting));
	};

	SocketServer socketServer;

	auto serverThreadFunc = [&]() { socketServer.Start(12345, 3, 1, serverJobFunc); };

	thread serverThread(serverThreadFunc);

	this_thread::sleep_for(std::chrono::seconds(1));

	SocketClient socketClient("127.0.0.1", 12345, 3);

	bool bEnd = false;
	string strRead = "";
	EXPECT_TRUE(socketClient.Read(strRead, 1024, bEnd));
	EXPECT_STREQ(strRead.c_str(), strGreeting.c_str());

	EXPECT_TRUE(socketServer.Stop());

	serverThread.join();
}

TEST(SocketClientTest, Total_char) {
	auto serverJobFunc = [](const SocketClient& socketClient) {
		int iResultLen = 0;

		bool bResult = false;
		bResult = socketClient.Write(strGreeting.c_str(), strGreeting.size(), iResultLen);
		EXPECT_TRUE(bResult);
		EXPECT_EQ(iResultLen, strGreeting.size());

		bool bEnd = false;
		char caBuffer[1024];
		bResult = socketClient.Read(caBuffer, sizeof(caBuffer), iResultLen, bEnd);
		EXPECT_TRUE(bResult);
		EXPECT_EQ(iResultLen, strlen(caBuffer));
		EXPECT_STREQ(caBuffer, strWrite.c_str());

		const string strResponse = strPrefixResponse + caBuffer;
		bResult = socketClient.Write(strResponse.c_str(), strResponse.size(), iResultLen);
		EXPECT_TRUE(bResult);
		EXPECT_EQ(iResultLen, strResponse.size());
	};

	SocketServer socketServer;

	bool bServerResult = false;
	auto serverThreadFunc = [&]() {
		bServerResult = socketServer.Start(12345, 3, 1, serverJobFunc);
	};

	thread serverThread(serverThreadFunc);

	this_thread::sleep_for(std::chrono::seconds(1));

	SocketClient socketClient("127.0.0.1", 12345, 3);

	bool bEnd = false;
	int iResultLen = 0;
	char caBuffer[1024];
	bool bResult = false;
	bResult = socketClient.Read(caBuffer, sizeof(caBuffer), iResultLen, bEnd);
	EXPECT_TRUE(bResult);
	EXPECT_EQ(iResultLen, strlen(caBuffer));
	EXPECT_STREQ(caBuffer, strGreeting.c_str());

	const string strWrite = "test line\r\n";
	bResult = socketClient.Write(strWrite.c_str(), strWrite.size(), iResultLen);
	EXPECT_TRUE(bResult);
	EXPECT_EQ(iResultLen, strWrite.size());

	bResult = socketClient.Read(caBuffer, sizeof(caBuffer), iResultLen, bEnd);
	EXPECT_TRUE(bResult);
	EXPECT_EQ(iResultLen, strlen(caBuffer));
	EXPECT_STREQ(caBuffer, (strPrefixResponse + strWrite).c_str());

	EXPECT_TRUE(socketServer.Stop());

	serverThread.join();

	EXPECT_TRUE(bServerResult);
}

TEST(SocketClientTest, total_string) {
	auto serverJobFunc = [](const SocketClient& socketClient) {
		EXPECT_TRUE(socketClient.Write(strGreeting));

		bool bEnd = false;
		string strRead = "";
		EXPECT_TRUE(socketClient.Read(strRead, 1024, bEnd));
		EXPECT_STREQ(strRead.c_str(), strWrite.c_str());

		EXPECT_TRUE(socketClient.Write(strPrefixResponse + strRead));
	};

	SocketServer socketServer;

	bool bServerResult = false;
	auto serverThreadFunc = [&]() {
		bServerResult = socketServer.Start(12345, 3, 1, serverJobFunc);
	};

	thread serverThread(serverThreadFunc);

	this_thread::sleep_for(std::chrono::seconds(1));

	SocketClient socketClient("127.0.0.1", 12345, 3);

	bool bEnd = false;
	string strRead = "";
	EXPECT_TRUE(socketClient.Read(strRead, 1024, bEnd));
	EXPECT_STREQ(strRead.c_str(), strGreeting.c_str());

	EXPECT_TRUE(socketClient.Write(strWrite));

	EXPECT_TRUE(socketClient.Read(strRead, 1024, bEnd));
	EXPECT_STREQ(strRead.c_str(), (strPrefixResponse + strWrite).c_str());

	EXPECT_TRUE(socketServer.Stop());

	serverThread.join();

	EXPECT_TRUE(bServerResult);
}
