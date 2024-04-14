#include "../FileManager.h"
#include "gtest/gtest.h"
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <future>
#include <map>
#include <ranges>
#include <string>
#include <vector>

using namespace std;

static const int repetitionCount = 100;

void createDirectory(filesystem::path path, bool result, int value, string message);
void createDirectories(filesystem::path path, bool result, int value, string message);
void remove(filesystem::path path);
void removeAll(filesystem::path path);

TEST(FileManagerTest, IsExist) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			const auto [tempPath, errorCode] = FileManager::Instance().GetTempPath();
			EXPECT_FALSE(errorCode);

			const auto path = tempPath.string() + "/tmpXXXXXX_" + to_string(i);

			EXPECT_FALSE(get<0>(FileManager::Instance().IsExist(path)));
			EXPECT_FALSE(get<1>(FileManager::Instance().IsExist(path)));

			createDirectory(path, true, 0, "Success");

			EXPECT_TRUE(get<0>(FileManager::Instance().IsExist(path)));
			EXPECT_FALSE(get<1>(FileManager::Instance().IsExist(path)));

			remove(path);
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, IsRegularFile) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, dirPath]() {
			const auto filePath = dirPath + "/test_" + to_string(i) + ".txt";

			EXPECT_FALSE(FileManager::Instance().Write(filePath, "data", ios::trunc));

			EXPECT_FALSE(get<0>(FileManager::Instance().IsRegularFile(dirPath)));
			EXPECT_FALSE(get<1>(FileManager::Instance().IsRegularFile(dirPath)));

			EXPECT_TRUE(get<0>(FileManager::Instance().IsRegularFile(filePath)));
			EXPECT_FALSE(get<1>(FileManager::Instance().IsRegularFile(filePath)));
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(dirPath);
}

TEST(FileManagerTest, IsDirectory) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
			EXPECT_FALSE(errorCode);

			const auto dirPath = tempDir.string() + "/tmpXXXXXX" + to_string(i);
			createDirectory(dirPath, true, 0, "Success");

			const auto filePath = dirPath + "/test.txt";
			EXPECT_FALSE(FileManager::Instance().Write(filePath, "data", ios::trunc));

			EXPECT_TRUE(get<0>(FileManager::Instance().IsDirectory(dirPath)));
			EXPECT_FALSE(get<1>(FileManager::Instance().IsDirectory(dirPath)));

			EXPECT_FALSE(get<0>(FileManager::Instance().IsDirectory(filePath)));
			EXPECT_FALSE(get<1>(FileManager::Instance().IsDirectory(filePath)));

			removeAll(dirPath);
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, LockBetweenProcess_input_fd) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	const auto filePath = dirPath + "/test.txt";
	EXPECT_FALSE(FileManager::Instance().Write(filePath, "data", ios::trunc));

	const mode_t mode = 0775;
	const int fd = open(filePath.c_str(), O_CREAT | O_RDWR, mode);
	if (fd < 0) {
		EXPECT_STREQ(strerror(errno), "");
	}

	EXPECT_FALSE(FileManager::Instance().LockBetweenProcess(fd));

	EXPECT_FALSE(FileManager::Instance().UnLockBetweenProcess(fd));

	EXPECT_EQ(close(fd), 0);

	if (const error_code errorCode = FileManager::Instance().LockBetweenProcess(fd); errorCode) {
		EXPECT_TRUE(errorCode);
		EXPECT_EQ(errorCode.value(), 9);
		EXPECT_STREQ(errorCode.message().c_str(), "Bad file descriptor");
	} else {
		EXPECT_STREQ("invalid", "");
	}

	removeAll(dirPath);
}

TEST(FileManagerTest, LockBetweenProcess_input_path) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	const auto filePath = dirPath + "/test.txt";
	EXPECT_FALSE(FileManager::Instance().Write(filePath, "data", ios::trunc));

	const auto [fd, errorCode2] = FileManager::Instance().LockBetweenProcess(filePath);
	EXPECT_FALSE(errorCode2);

	EXPECT_FALSE(FileManager::Instance().UnLockBetweenProcess(fd));

	EXPECT_EQ(close(fd), 0);

	if (const error_code errorCode = FileManager::Instance().LockBetweenProcess(fd); errorCode) {
		EXPECT_TRUE(errorCode);
		EXPECT_EQ(errorCode.value(), 9);
		EXPECT_STREQ(errorCode.message().c_str(), "Bad file descriptor");
	} else {
		EXPECT_STREQ("invalid", "");
	}

	removeAll(dirPath);
}

TEST(FileManagerTest, UnLockBetweenProcess) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	const auto filePath = dirPath + "/test.txt";
	EXPECT_FALSE(FileManager::Instance().Write(filePath, "data", ios::trunc));

	const auto [fd, errorCode2] = FileManager::Instance().LockBetweenProcess(filePath);
	EXPECT_FALSE(errorCode2);

	EXPECT_FALSE(FileManager::Instance().UnLockBetweenProcess(fd));

	EXPECT_EQ(close(fd), 0);

	if (const error_code errorCode = FileManager::Instance().LockBetweenProcess(fd); errorCode) {
		EXPECT_TRUE(errorCode);
		EXPECT_EQ(errorCode.value(), 9);
		EXPECT_STREQ(errorCode.message().c_str(), "Bad file descriptor");
	} else {
		EXPECT_STREQ("invalid", "");
	}

	removeAll(dirPath);
}

TEST(FileManagerTest, Read) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [dirPath, i]() {
			const auto filePath = dirPath + "/test_" + to_string(i) + ".txt";

			EXPECT_FALSE(FileManager::Instance().Write(filePath, to_string(i), ios::trunc));

			const auto [data, errorCode] = FileManager::Instance().Read(filePath);
			EXPECT_FALSE(errorCode);
			EXPECT_STREQ(data.c_str(), to_string(i).c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(dirPath);
}

TEST(FileManagerTest, Write1) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [dirPath, i]() {
			const auto filePath = dirPath + "/test_" + to_string(i) + ".txt";

			EXPECT_FALSE(FileManager::Instance().Write(filePath, to_string(i), ios::trunc));

			const auto [data, errorCode] = FileManager::Instance().Read(filePath);
			EXPECT_FALSE(errorCode);
			EXPECT_STREQ(data.c_str(), to_string(i).c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(dirPath);
}

TEST(FileManagerTest, Write2) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	const auto filePath = dirPath + "/test.txt";
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [filePath, i]() {
			EXPECT_FALSE(FileManager::Instance().Write(filePath, to_string(i) + "-", ios::app));
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	map<string, int> result{};
	EXPECT_FALSE(get<1>(FileManager::Instance().Read(filePath)));
	for (const auto &iter :
		 ranges::views::split(get<0>(FileManager::Instance().Read(filePath)), '-')) {
		++result[string(iter.begin(), iter.end())];
	}

	EXPECT_EQ(result.size(), repetitionCount + 1);
	for (int i = 0; i < repetitionCount; ++i) {
		if (result.find(to_string(i)) == result.end()) {
			EXPECT_STREQ(to_string(i).c_str(), "there are no");
		}
	}

	removeAll(dirPath);
}

TEST(FileManagerTest, MakeDir) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		const auto path = dirPath + "/" + to_string(i);

		futures.push_back(async(launch::async, [path]() {
			EXPECT_FALSE(get<0>(FileManager::Instance().IsExist(path)));
			createDirectory(path, true, 0, "Success");

			EXPECT_TRUE(get<0>(FileManager::Instance().IsDirectory(path)));
			createDirectory(path, false, 0, "Success");

			createDirectory(path + "/a/b", false, 2, "No such file or directory");

			const auto fileName = path + "/test.txt";
			EXPECT_FALSE(FileManager::Instance().Write(fileName, "data", ios::trunc));

			createDirectory(fileName, false, 17, "File exists");
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(dirPath);
}

TEST(FileManagerTest, CreateDirectories) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		const auto path = dirPath + "/" + to_string(i) + "/" + to_string(i);

		futures.push_back(async(launch::async, [path]() {
			EXPECT_FALSE(get<0>(FileManager::Instance().IsExist(path)));
			createDirectories(path, true, 0, "Success");

			EXPECT_TRUE(get<0>(FileManager::Instance().IsDirectory(path)));
			createDirectories(path, false, 0, "Success");

			const auto fileName = path + "/test.txt";
			EXPECT_FALSE(FileManager::Instance().Write(fileName, "data", ios::trunc));
			createDirectories(fileName, false, 20, "Not a directory");
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(dirPath);
}

TEST(FileManagerTest, Copy) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, dirPath]() {
			const auto fromFilePath = dirPath + "/from_" + to_string(i) + ".txt";
			const auto toFilePath = dirPath + "/to_" + to_string(i) + ".txt";

			EXPECT_FALSE(FileManager::Instance().Write(fromFilePath, to_string(i), ios::trunc));

			EXPECT_FALSE(FileManager::Instance().Copy(fromFilePath, toFilePath));

			const auto [data, errorCode] = FileManager::Instance().Read(toFilePath);
			EXPECT_FALSE(errorCode);
			EXPECT_STREQ(data.c_str(), to_string(i).c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(dirPath);
}

TEST(FileManagerTest, Copy_with_option) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, dirPath]() {
			const auto workDir = dirPath + "/" + to_string(i);
			createDirectory(workDir, true, 0, "Success");

			const auto fromDirPath = workDir + "/" + "from";
			createDirectory(fromDirPath, true, 0, "Success");

			const auto toDirPath = workDir + "/" + "to";
			createDirectory(toDirPath, true, 0, "Success");

			const auto dirName = "dir";
			const auto fileName = "file";

			createDirectory(fromDirPath + "/" + dirName, true, 0, "Success");

			EXPECT_FALSE(FileManager::Instance().Write(fromDirPath + "/" + fileName, to_string(i),
													   ios::trunc));

			EXPECT_FALSE(FileManager::Instance().Copy(fromDirPath, toDirPath,
													  filesystem::copy_options::directories_only |
														  filesystem::copy_options::recursive));

			EXPECT_TRUE(get<0>(FileManager::Instance().IsExist(toDirPath + "/" + dirName)));
			EXPECT_FALSE(get<0>(FileManager::Instance().IsExist(toDirPath + "/" + fileName)));
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(dirPath);
}

TEST(FileManagerTest, CopyAll) {
	const auto [tempDir, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto dirPath = tempDir.string() + "/tmpXXXXXX";
	createDirectory(dirPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, dirPath]() {
			const auto workDir = dirPath + "/" + to_string(i);
			createDirectory(workDir, true, 0, "Success");

			const auto fromDirPath = workDir + "/" + "from";
			createDirectory(fromDirPath, true, 0, "Success");

			const auto toDirPath = workDir + "/" + "to";
			createDirectory(toDirPath, true, 0, "Success");

			const auto dirName = "dir";
			const auto fileName = "file";

			createDirectory(fromDirPath + "/" + dirName, true, 0, "Success");

			EXPECT_FALSE(FileManager::Instance().Write(fromDirPath + "/" + fileName, to_string(i),
													   ios::trunc));

			EXPECT_FALSE(FileManager::Instance().CopyAll(fromDirPath, toDirPath));

			EXPECT_TRUE(get<0>(FileManager::Instance().IsExist(toDirPath + "/" + dirName)));
			EXPECT_TRUE(get<0>(FileManager::Instance().IsExist(toDirPath + "/" + fileName)));
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(dirPath);
}

TEST(FileManagerTest, Remove) {
	const auto [temp, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto tempPath = temp.string() + "/tmpXXXXXX";
	createDirectory(tempPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, tempPath]() {
			const auto filePath = tempPath + "/" + "test_" + to_string(i) + "txt";
			EXPECT_FALSE(FileManager::Instance().Write(filePath, to_string(i), ios::trunc));
			EXPECT_TRUE(get<0>(FileManager::Instance().IsRegularFile(filePath)));
			remove(filePath);
			EXPECT_FALSE(get<0>(FileManager::Instance().IsExist(filePath)));

			const auto dirPath = tempPath + "/" + to_string(i);
			createDirectories(dirPath, true, 0, "Success");
			EXPECT_TRUE(get<0>(FileManager::Instance().IsDirectory(dirPath)));
			remove(dirPath);
			EXPECT_FALSE(get<0>(FileManager::Instance().IsExist(dirPath)));

			const auto depthPath = dirPath + "/" + to_string(i) + "/" + to_string(i);
			createDirectories(depthPath, true, 0, "Success");
			EXPECT_TRUE(get<0>(FileManager::Instance().IsDirectory(depthPath)));
			auto [ok, errorCode] = FileManager::Instance().Remove(dirPath);
			EXPECT_FALSE(ok);
			EXPECT_EQ(errorCode.value(), 39);
			EXPECT_STREQ(errorCode.message().c_str(), "Directory not empty");
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(tempPath);
}

TEST(FileManagerTest, RemoveAll) {
	const auto [temp, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto tempPath = temp.string() + "/tmpXXXXXX";
	createDirectory(tempPath, true, 0, "Success");

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, tempPath]() {
			const auto dirPath = tempPath + "/" + to_string(i);
			const auto depthPath = dirPath + "/" + to_string(i) + "/" + to_string(i);
			createDirectories(depthPath, true, 0, "Success");
			EXPECT_TRUE(get<0>(FileManager::Instance().IsDirectory(depthPath)));

			removeAll(dirPath);
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(tempPath);
}

TEST(FileManagerTest, ToAbsolutePath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			auto [currentPath, errorCode1] = FileManager::Instance().GetCurrentPath();
			EXPECT_FALSE(errorCode1);

			auto [absolutePath, errorCode2] = FileManager::Instance().ToAbsolutePath("./");
			EXPECT_FALSE(errorCode2);

			EXPECT_STREQ(absolutePath.c_str(), (currentPath.string() + "/./").c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, ToCanonicalPath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			auto [currentPath, errorCode1] = FileManager::Instance().GetCurrentPath();
			EXPECT_FALSE(errorCode1);

			auto [canonicalPath, errorCode2] = FileManager::Instance().ToCanonicalPath("./");
			EXPECT_FALSE(errorCode2);

			EXPECT_STREQ(canonicalPath.c_str(), currentPath.c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, ToRelativePath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			auto [currentPath, errorCode] = FileManager::Instance().GetCurrentPath();
			EXPECT_FALSE(errorCode);

			currentPath = currentPath.string() + "/" + to_string(i);

			EXPECT_STREQ(FileManager::Instance().ToRelativePath(currentPath).c_str(),
						 currentPath.c_str() +
							 FileManager::Instance().GetRootPath(currentPath).string().size());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, GetTempPath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, []() {
			const auto [tempPath, errorCode] = FileManager::Instance().GetTempPath();
			EXPECT_FALSE(errorCode);
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, GetRootPath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, []() {
			EXPECT_STREQ(FileManager::Instance().GetRootPath("./a").c_str(), "");
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_STREQ(FileManager::Instance().GetRootPath("./a/b").c_str(), "");
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_STREQ(FileManager::Instance().GetRootPath("/a").c_str(), "/");
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_STREQ(FileManager::Instance().GetRootPath("/a/b").c_str(), "/");
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, GetRelativePath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			const auto dirName = to_string(i);

			{
				auto [result, errorCode] = FileManager::Instance().GetRelativePath("./" + dirName);
				EXPECT_FALSE(errorCode);
				EXPECT_STREQ(result.c_str(), dirName.c_str());
			}

			{
				auto [currentPath, errorCode1] = FileManager::Instance().GetCurrentPath();
				EXPECT_FALSE(errorCode1);

				auto [result, errorCode2] =
					FileManager::Instance().GetRelativePath(currentPath.string() + "/" + dirName);
				EXPECT_FALSE(errorCode2);

				EXPECT_STREQ(result.c_str(), dirName.c_str());
			}
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, GetSubDirectories) {
	const auto [temp, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto tempPath = temp.string() + "/tmpXXXXXX";

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, tempPath]() {
			const auto basePath = tempPath + "/" + to_string(i);
			const vector<filesystem::path> paths = {basePath + "/a", basePath + "/a/b",
													basePath + "/a/b/c"};
			for (const auto &iter : paths) {
				createDirectories(iter, true, 0, "Success");
			}

			EXPECT_EQ(FileManager::Instance().GetSubDirectories(basePath).size(), 1);
			EXPECT_TRUE(FileManager::Instance().GetSubDirectories(basePath) ==
						vector<filesystem::path>{paths.at(0)});
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(tempPath);
}

TEST(FileManagerTest, GetRecursiveSubDirectories) {
	const auto [temp, errorCode] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode);

	const auto tempPath = temp.string() + "/tmpXXXXXX";

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, tempPath]() {
			const auto basePath = tempPath + "/" + to_string(i);
			const vector<filesystem::path> paths = {
				basePath + "/a",
				basePath + "/a/b",
				basePath + "/a/b/c",
			};
			for (const auto &iter : paths) {
				createDirectories(iter, true, 0, "Success");
			}

			EXPECT_TRUE(FileManager::Instance().GetRecursiveSubDirectories(basePath) == paths);
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	removeAll(tempPath);
}

TEST(FileManagerTest, GetCurrentPath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, []() {
			auto [currentPath, errorCode1] = FileManager::Instance().GetCurrentPath();
			EXPECT_FALSE(errorCode1);

			auto [canonicalPath, errorCode2] = FileManager::Instance().ToCanonicalPath("./");
			EXPECT_FALSE(errorCode2);

			EXPECT_STREQ(currentPath.c_str(), canonicalPath.c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, SetCurrentPath) {
	const auto [tempPath, errorCode1] = FileManager::Instance().GetTempPath();
	EXPECT_FALSE(errorCode1);

	EXPECT_FALSE(FileManager::Instance().SetCurrentPath(tempPath));

	const auto [currentPath, errorCode2] = FileManager::Instance().GetCurrentPath();
	EXPECT_FALSE(errorCode2);

	EXPECT_STREQ(currentPath.c_str(), tempPath.c_str());
}

void createDirectory(filesystem::path path, bool result, int value, string message) {
	const auto [ok, errorCode] = FileManager::Instance().CreateDirectory(path);
	if (result) {
		EXPECT_TRUE(ok);
	} else {
		EXPECT_FALSE(ok);
	}

	EXPECT_EQ(errorCode.value(), value);
	EXPECT_STREQ(errorCode.message().c_str(), message.c_str());
}

void createDirectories(filesystem::path path, bool result, int value, string message) {
	const auto [ok, errorCode] = FileManager::Instance().CreateDirectories(path);
	if (result) {
		EXPECT_TRUE(ok);
	} else {
		EXPECT_FALSE(ok);
	}

	EXPECT_EQ(errorCode.value(), value);
	EXPECT_STREQ(errorCode.message().c_str(), message.c_str());
}

void remove(filesystem::path path) {
	auto [ok, errorCode] = FileManager::Instance().Remove(path);
	EXPECT_FALSE(errorCode);
	EXPECT_TRUE(ok);
}

void removeAll(filesystem::path path) {
	const auto [ok, errorCode] = FileManager::Instance().RemoveAll(path);
	EXPECT_FALSE(errorCode);
	EXPECT_TRUE(ok);
}
