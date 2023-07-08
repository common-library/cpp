#include "../FileManager.h"
#include "gtest/gtest.h"
#include <cstring>
#include <fcntl.h>
#include <future>
#include <map>
#include <ranges>
#include <string>
#include <vector>

using namespace std;

static const int repetitionCount = 500;

TEST(FileManagerTest, IsExist) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			const auto path = FileManager::Instance().GetTempPath() +
							  "/tmpXXXXXX_" + to_string(i);

			EXPECT_FALSE(FileManager::Instance().IsExist(path));

			EXPECT_TRUE(FileManager::Instance().MakeDir(path));

			EXPECT_TRUE(FileManager::Instance().IsExist(path));

			EXPECT_TRUE(FileManager::Instance().Remove(path));
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
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, dirPath]() {
			const auto filePath = dirPath + "/test_" + to_string(i) + ".txt";

			EXPECT_TRUE(
				FileManager::Instance().Write(filePath, "data", ios::trunc));

			EXPECT_FALSE(FileManager::Instance().IsRegularFile(dirPath));
			EXPECT_TRUE(FileManager::Instance().IsRegularFile(filePath));
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, IsDirectory) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			const auto dirPath = FileManager::Instance().GetTempPath() +
								 "/tmpXXXXXX_" + to_string(i);
			EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

			const auto filePath = dirPath + "/test.txt";
			EXPECT_TRUE(
				FileManager::Instance().Write(filePath, "data", ios::trunc));

			EXPECT_TRUE(FileManager::Instance().IsDirectory(dirPath));
			EXPECT_FALSE(FileManager::Instance().IsDirectory(filePath));

			EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
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
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	const auto filePath = dirPath + "/test.txt";
	EXPECT_TRUE(FileManager::Instance().Write(filePath, "data", ios::trunc));

	const mode_t mode = 0775;
	const int fd = open(filePath.c_str(), O_CREAT | O_RDWR, mode);
	if (fd < 0) {
		EXPECT_STREQ(strerror(errno), "");
	}

	EXPECT_TRUE(FileManager::Instance().LockBetweenProcess(fd));

	EXPECT_TRUE(FileManager::Instance().UnLockBetweenProcess(fd));

	EXPECT_EQ(close(fd), 0);

	EXPECT_FALSE(FileManager::Instance().LockBetweenProcess(fd));

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, LockBetweenProcess_input_path) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	const auto filePath = dirPath + "/test.txt";
	EXPECT_TRUE(FileManager::Instance().Write(filePath, "data", ios::trunc));

	const int fd = FileManager::Instance().LockBetweenProcess(filePath);
	if (fd < -1) {
		EXPECT_STREQ(strerror(errno), "");
	}

	EXPECT_TRUE(FileManager::Instance().UnLockBetweenProcess(fd));

	EXPECT_EQ(close(fd), 0);

	EXPECT_FALSE(FileManager::Instance().LockBetweenProcess(fd));

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, UnLockBetweenProcess) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	const auto filePath = dirPath + "/test.txt";
	EXPECT_TRUE(FileManager::Instance().Write(filePath, "data", ios::trunc));

	const int fd = FileManager::Instance().LockBetweenProcess(filePath);
	if (fd < -1) {
		EXPECT_STREQ(strerror(errno), "");
	}

	EXPECT_TRUE(FileManager::Instance().UnLockBetweenProcess(fd));

	EXPECT_EQ(close(fd), 0);

	EXPECT_FALSE(FileManager::Instance().UnLockBetweenProcess(fd));

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, Read) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [dirPath, i]() {
			const auto filePath = dirPath + "/test_" + to_string(i) + ".txt";

			EXPECT_TRUE(FileManager::Instance().Write(filePath, to_string(i),
													  ios::trunc));

			const auto result = FileManager::Instance().Read(filePath);
			EXPECT_TRUE(get<0>(result));
			EXPECT_STREQ(get<1>(result).c_str(), to_string(i).c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, Write1) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [dirPath, i]() {
			const auto filePath = dirPath + "/test_" + to_string(i) + ".txt";

			EXPECT_TRUE(FileManager::Instance().Write(filePath, to_string(i),
													  ios::trunc));

			const auto result = FileManager::Instance().Read(filePath);
			EXPECT_TRUE(get<0>(result));
			EXPECT_STREQ(get<1>(result).c_str(), to_string(i).c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, Write2) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	const auto filePath = dirPath + "/test.txt";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [filePath, i]() {
			EXPECT_TRUE(FileManager::Instance().Write(
				filePath, to_string(i) + "-", ios::app));
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	map<string, int> result{};
	EXPECT_TRUE(get<0>(FileManager::Instance().Read(filePath)));
	for (const auto &iter : ranges::views::split(
			 get<1>(FileManager::Instance().Read(filePath)), '-')) {
		++result[string(iter.begin(), iter.end())];
	}

	EXPECT_EQ(result.size(), repetitionCount + 1);
	for (int i = 0; i < repetitionCount; ++i) {
		if (result.find(to_string(i)) == result.end()) {
			EXPECT_STREQ(to_string(i).c_str(), "there are no");
		}
	}

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, MakeDir) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		const auto path = dirPath + "/" + to_string(i);

		futures.push_back(async(launch::async, [path]() {
			EXPECT_FALSE(FileManager::Instance().IsExist(path));
			EXPECT_TRUE(FileManager::Instance().MakeDir(path));
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_TRUE(FileManager::Instance().IsDirectory(path));
			EXPECT_TRUE(FileManager::Instance().MakeDir(path));

			EXPECT_FALSE(FileManager::Instance().MakeDir(path + "/a/b"));
			EXPECT_EQ(errno, 2);
			EXPECT_STREQ(strerror(errno), "No such file or directory");

			const auto fileName = path + "/test.txt";
			EXPECT_TRUE(
				FileManager::Instance().Write(fileName, "data", ios::trunc));
			EXPECT_FALSE(FileManager::Instance().MakeDir(fileName));
			EXPECT_EQ(errno, 17);
			EXPECT_STREQ(strerror(errno), "File exists");
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, MakeDirs) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		const auto path = dirPath + "/" + to_string(i) + "/" + to_string(i);

		futures.push_back(async(launch::async, [path]() {
			EXPECT_FALSE(FileManager::Instance().IsExist(path));
			EXPECT_TRUE(FileManager::Instance().MakeDirs(path));
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_TRUE(FileManager::Instance().IsDirectory(path));
			EXPECT_TRUE(FileManager::Instance().MakeDirs(path));

			const auto fileName = path + "/test.txt";
			EXPECT_TRUE(
				FileManager::Instance().Write(fileName, "data", ios::trunc));
			EXPECT_FALSE(FileManager::Instance().MakeDirs(fileName));
			EXPECT_EQ(errno, 20);
			EXPECT_STREQ(strerror(errno), "Not a directory");
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, Copy) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, dirPath]() {
			const auto fromFilePath =
				dirPath + "/from_" + to_string(i) + ".txt";
			const auto toFilePath = dirPath + "/to_" + to_string(i) + ".txt";

			EXPECT_TRUE(FileManager::Instance().Write(
				fromFilePath, to_string(i), ios::trunc));

			EXPECT_TRUE(FileManager::Instance().Copy(fromFilePath, toFilePath));
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			const auto result = FileManager::Instance().Read(toFilePath);
			EXPECT_TRUE(get<0>(result));
			EXPECT_STREQ(get<1>(result).c_str(), to_string(i).c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, Copy_with_option) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, dirPath]() {
			const auto workDir = dirPath + "/" + to_string(i);
			EXPECT_TRUE(FileManager::Instance().MakeDir(workDir));

			const auto fromDirPath = workDir + "/" + "from";
			EXPECT_TRUE(FileManager::Instance().MakeDir(fromDirPath));

			const auto toDirPath = workDir + "/" + "to";
			EXPECT_TRUE(FileManager::Instance().MakeDir(toDirPath));

			const auto dirName = "dir";
			const auto fileName = "file";

			EXPECT_TRUE(
				FileManager::Instance().MakeDir(fromDirPath + "/" + dirName));
			EXPECT_TRUE(FileManager::Instance().Write(
				fromDirPath + "/" + fileName, to_string(i), ios::trunc));

			EXPECT_TRUE(FileManager::Instance().Copy(
				fromDirPath, toDirPath,
				filesystem::copy_options::directories_only |
					filesystem::copy_options::recursive));
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_TRUE(
				FileManager::Instance().IsExist(toDirPath + "/" + dirName));
			EXPECT_FALSE(
				FileManager::Instance().IsExist(toDirPath + "/" + fileName));
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, CopyAll) {
	const auto dirPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(dirPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, dirPath]() {
			const auto workDir = dirPath + "/" + to_string(i);
			EXPECT_TRUE(FileManager::Instance().MakeDir(workDir));

			const auto fromDirPath = workDir + "/" + "from";
			EXPECT_TRUE(FileManager::Instance().MakeDir(fromDirPath));

			const auto toDirPath = workDir + "/" + "to";
			EXPECT_TRUE(FileManager::Instance().MakeDir(toDirPath));

			const auto dirName = "dir";
			const auto fileName = "file";

			EXPECT_TRUE(
				FileManager::Instance().MakeDir(fromDirPath + "/" + dirName));
			EXPECT_TRUE(FileManager::Instance().Write(
				fromDirPath + "/" + fileName, to_string(i), ios::trunc));

			EXPECT_TRUE(
				FileManager::Instance().CopyAll(fromDirPath, toDirPath));
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_TRUE(
				FileManager::Instance().IsExist(toDirPath + "/" + dirName));
			EXPECT_TRUE(
				FileManager::Instance().IsExist(toDirPath + "/" + fileName));
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
}

TEST(FileManagerTest, Remove) {
	const auto tempPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(tempPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, tempPath]() {
			const auto filePath =
				tempPath + "/" + "test_" + to_string(i) + "txt";
			EXPECT_TRUE(FileManager::Instance().Write(filePath, to_string(i),
													  ios::trunc));
			EXPECT_TRUE(FileManager::Instance().IsRegularFile(filePath));
			EXPECT_TRUE(FileManager::Instance().Remove(filePath));
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");
			EXPECT_FALSE(FileManager::Instance().IsExist(filePath));

			const auto dirPath = tempPath + "/" + to_string(i);
			EXPECT_TRUE(FileManager::Instance().MakeDirs(dirPath));
			EXPECT_TRUE(FileManager::Instance().IsDirectory(dirPath));
			EXPECT_TRUE(FileManager::Instance().Remove(dirPath));
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");
			EXPECT_FALSE(FileManager::Instance().IsExist(dirPath));

			const auto depthPath =
				dirPath + "/" + to_string(i) + "/" + to_string(i);
			EXPECT_TRUE(FileManager::Instance().MakeDirs(depthPath));
			EXPECT_TRUE(FileManager::Instance().IsDirectory(depthPath));
			EXPECT_FALSE(FileManager::Instance().Remove(dirPath));
			EXPECT_EQ(errno, 39);
			EXPECT_STREQ(strerror(errno), "Directory not empty");
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(tempPath));
}

TEST(FileManagerTest, RemoveAll) {
	const auto tempPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";
	EXPECT_TRUE(FileManager::Instance().MakeDir(tempPath));

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, tempPath]() {
			const auto dirPath = tempPath + "/" + to_string(i);
			const auto depthPath =
				dirPath + "/" + to_string(i) + "/" + to_string(i);
			EXPECT_TRUE(FileManager::Instance().MakeDirs(depthPath));
			EXPECT_TRUE(FileManager::Instance().IsDirectory(depthPath));
			EXPECT_TRUE(FileManager::Instance().RemoveAll(dirPath));
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

	EXPECT_TRUE(FileManager::Instance().RemoveAll(tempPath));
}

TEST(FileManagerTest, ToAbsolutePath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			EXPECT_STREQ(
				FileManager::Instance().ToAbsolutePath("./").c_str(),
				(FileManager::Instance().GetCurrentPath() + "/./").c_str());
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
			EXPECT_STREQ(FileManager::Instance().ToCanonicalPath("./").c_str(),
						 (FileManager::Instance().GetCurrentPath()).c_str());
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();
}

TEST(FileManagerTest, ToRelativePathToRootPath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i]() {
			const auto path =
				FileManager::Instance().GetCurrentPath() + "/" + to_string(i);

			EXPECT_STREQ(
				FileManager::Instance().ToRelativePathToRootPath(path).c_str(),
				path.data() + FileManager::Instance().GetRootPath(path).size());
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
			FileManager::Instance().GetTempPath();
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

TEST(FileManagerTest, GetRootPath) {

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, []() {
			EXPECT_STREQ(FileManager::Instance().GetRootPath("./a").c_str(),
						 "");
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_STREQ(FileManager::Instance().GetRootPath("./a/b").c_str(),
						 "");
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_STREQ(FileManager::Instance().GetRootPath("/a").c_str(),
						 "/");
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_STREQ(FileManager::Instance().GetRootPath("/a/b").c_str(),
						 "/");
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
			const string dirName = to_string(i);
			EXPECT_STREQ(
				FileManager::Instance().GetRelativePath("./" + dirName).c_str(),
				dirName.c_str());
			EXPECT_STREQ(
				FileManager::Instance()
					.GetRelativePath(FileManager::Instance().GetCurrentPath() +
									 "/" + dirName)
					.c_str(),
				dirName.c_str());
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
	const auto tempPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, tempPath]() {
			const auto basePath = tempPath + "/" + to_string(i);
			const vector<string> paths = {
				basePath + "/a",
				basePath + "/a/b",
				basePath + "/a/b/c",
			};
			for (const auto &iter : paths) {
				EXPECT_TRUE(FileManager::Instance().MakeDirs(iter));
			}

			EXPECT_EQ(
				FileManager::Instance().GetSubDirectories(basePath).size(), 1);
			EXPECT_TRUE(FileManager::Instance().GetSubDirectories(basePath) ==
						vector<string>{paths.at(0)});
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(tempPath));
}

TEST(FileManagerTest, GetRecursivePathList) {
	const auto tempPath = FileManager::Instance().GetTempPath() + "/tmpXXXXXX";

	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, [i, tempPath]() {
			const auto basePath = tempPath + "/" + to_string(i);
			const vector<string> paths = {
				basePath + "/a",
				basePath + "/a/b",
				basePath + "/a/b/c",
			};
			for (const auto &iter : paths) {
				EXPECT_TRUE(FileManager::Instance().MakeDirs(iter));
			}

			EXPECT_TRUE(FileManager::Instance().GetRecursiveSubDirectories(
							basePath) == paths);
		}));
	}

	for (auto &iter : futures) {
		if (iter.valid()) {
			iter.get();
		}
	}
	futures.clear();

	EXPECT_TRUE(FileManager::Instance().RemoveAll(tempPath));
}

TEST(FileManagerTest, GetCurrentPath) {
	vector<future<void>> futures = {};
	for (int i = 0; i < repetitionCount; ++i) {
		futures.push_back(async(launch::async, []() {
			const string currentPath = FileManager::Instance().GetCurrentPath();
			EXPECT_EQ(errno, 0);
			EXPECT_STREQ(strerror(errno), "Success");

			EXPECT_STREQ(currentPath.c_str(),
						 FileManager::Instance().ToCanonicalPath("./").c_str());
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
	const string tempPath = FileManager::Instance().GetTempPath();

	EXPECT_TRUE(FileManager::Instance().SetCurrentPath(tempPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_STREQ(FileManager::Instance().GetCurrentPath().c_str(),
				 tempPath.c_str());
}
