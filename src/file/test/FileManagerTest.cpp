#include <fcntl.h>

#include <thread>
using namespace std;

#include "../FileManager.h"

#include "gtest/gtest.h"

TEST(FileManagerTest, IsExist)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));

	EXPECT_TRUE(FileManager().IsExist(strOutputPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));

	EXPECT_FALSE(FileManager().IsExist(strOutputPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");
}

TEST(FileManagerTest, IsRegularFile)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strPath = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strPath, strData, ios::trunc));

	EXPECT_TRUE(FileManager().IsRegularFile(strPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_FALSE(FileManager().IsRegularFile(strOutputPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, IsDirectory)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strPath = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strPath, strData, ios::trunc));

	EXPECT_TRUE(FileManager().IsDirectory(strOutputPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_FALSE(FileManager().IsDirectory(strPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, LockBetweenProcess_input_fd)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strPath = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strPath, strData, ios::trunc));

	const mode_t mode = 0775;
	const int iFD = open(strPath.c_str(), O_CREAT | O_RDWR, mode);
	if(iFD < 0) {
		EXPECT_STREQ(strerror(errno), "");
	}

	EXPECT_TRUE(FileManager().LockBetweenProcess(iFD));
	EXPECT_TRUE(FileManager().LockBetweenProcess(iFD));

	EXPECT_TRUE(FileManager().UnLockBetweenProcess(iFD));

	EXPECT_EQ(close(iFD), 0);

	EXPECT_FALSE(FileManager().LockBetweenProcess(iFD));

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, LockBetweenProcess_input_path)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strPath = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strPath, strData, ios::trunc));

	const int iFD = FileManager().LockBetweenProcess(strPath);
	if(iFD < -1) {
		EXPECT_STREQ(strerror(errno), "");
	}

	EXPECT_TRUE(FileManager().UnLockBetweenProcess(iFD));

	EXPECT_EQ(close(iFD), 0);

	EXPECT_FALSE(FileManager().LockBetweenProcess(iFD));

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, UnLockBetweenProcess)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strPath = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strPath, strData, ios::trunc));

	const int iFD = FileManager().LockBetweenProcess(strPath);
	if(iFD < -1) {
		EXPECT_STREQ(strerror(errno), "");
	}

	EXPECT_TRUE(FileManager().UnLockBetweenProcess(iFD));

	EXPECT_EQ(close(iFD), 0);

	EXPECT_FALSE(FileManager().UnLockBetweenProcess(iFD));

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, Read)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strPath = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strPath, strData, ios::trunc));

	string strResult = "";
	EXPECT_TRUE(FileManager().Read(strPath, strResult));
	EXPECT_STREQ(strResult.c_str(), strData.c_str());

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, Write)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strPath = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strPath, strData, ios::trunc));

	string strResult = "";
	EXPECT_TRUE(FileManager().Read(strPath, strResult));
	EXPECT_STREQ(strResult.c_str(), strData.c_str());

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, MakeDir)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strPath1 = strOutputPath + "/depth";
	const string strPath2 = strOutputPath + "/depth1" + "/depth2";
	const string strPath3 = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strPath3, strData, ios::trunc));

	EXPECT_TRUE(FileManager().MakeDir(strPath1));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_FALSE(FileManager().MakeDir(strPath2));
	EXPECT_EQ(errno, 2);
	EXPECT_STREQ(strerror(errno), "No such file or directory");

	EXPECT_FALSE(FileManager().MakeDir(strPath3));
	EXPECT_EQ(errno, 17);
	EXPECT_STREQ(strerror(errno), "File exists");

	EXPECT_TRUE(FileManager().IsExist(strPath1));
	EXPECT_FALSE(FileManager().IsExist(strPath2));
	EXPECT_TRUE(FileManager().IsDirectory(strPath1));

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, MakeDirs)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strPath1 = strOutputPath + "/depth1" + "/depth2";
	const string strPath2 = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strPath2, strData, ios::trunc));

	EXPECT_TRUE(FileManager().MakeDirs(strPath1));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_FALSE(FileManager().MakeDirs(strPath2));
	EXPECT_EQ(errno, 20);
	EXPECT_STREQ(strerror(errno), "Not a directory");

	EXPECT_TRUE(FileManager().IsExist(strPath1));

	EXPECT_TRUE(FileManager().IsDirectory(strPath1));

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, Copy)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strFromPath = strOutputPath + "/" + "from.txt";
	const string strToPath = strOutputPath + "/" + "to.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().Write(strFromPath, strData, ios::trunc));

	EXPECT_TRUE(FileManager().Copy(strFromPath, strToPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	string strResult = "";
	EXPECT_TRUE(FileManager().Read(strToPath, strResult));
	EXPECT_STREQ(strResult.c_str(), strData.c_str());

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, Copy_input_option)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strFromPath = strOutputPath + "/" + "from";
	const string strToPath = strOutputPath + "/" + "to";
	const string strDirName = "test";
	const string strFileName = "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().MakeDirs(strFromPath + "/" + strDirName));
	EXPECT_TRUE(FileManager().Write(strFromPath + "/" + strFileName, strData, ios::trunc));

	EXPECT_TRUE(FileManager().Copy(strFromPath, strToPath, filesystem::copy_options::directories_only | filesystem::copy_options::recursive));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_TRUE(FileManager().IsExist(strToPath + "/" + strDirName));
	EXPECT_FALSE(FileManager().IsExist(strToPath + "/" + strFileName));

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, CopyAll)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strFromPath = strOutputPath + "/" + "from";
	const string strToPath = strOutputPath + "/" + "to";
	const string strDirName = "test";
	const string strFileName = "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().MakeDirs(strFromPath + "/" + strDirName));
	EXPECT_TRUE(FileManager().Write(strFromPath + "/" + strFileName, strData, ios::trunc));

	EXPECT_TRUE(FileManager().CopyAll(strFromPath, strToPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_TRUE(FileManager().IsExist(strToPath + "/" + strDirName));
	EXPECT_TRUE(FileManager().IsExist(strToPath + "/" + strFileName));

	string strResult = "";
	EXPECT_TRUE(FileManager().Read(strToPath + "/" + strFileName, strResult));
	EXPECT_STREQ(strResult.c_str(), strData.c_str());


	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, Remove)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strDirPath = strOutputPath + "/" + "depth";
	const string strFilePath = strOutputPath + "/" + "test.txt";
	const string strDepthPath = strOutputPath + "/depth1" + "/depth2";
	const string strData = "data";

	EXPECT_TRUE(FileManager().MakeDir(strDirPath));
	EXPECT_TRUE(FileManager().Remove(strDirPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");
	EXPECT_FALSE(FileManager().IsExist(strDirPath));

	EXPECT_TRUE(FileManager().Write(strFilePath, strData, ios::trunc));
	EXPECT_TRUE(FileManager().Remove(strFilePath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");
	EXPECT_FALSE(FileManager().IsExist(strFilePath));

	EXPECT_TRUE(FileManager().MakeDirs(strDepthPath));
	EXPECT_FALSE(FileManager().Remove(strOutputPath));
	EXPECT_EQ(errno, 39);
	EXPECT_STREQ(strerror(errno), "Directory not empty");
	EXPECT_TRUE(FileManager().IsExist(strOutputPath));

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, RemoveAll)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strDirPath = strOutputPath + "/" + "depth";
	const string strFilePath = strOutputPath + "/" + "test.txt";
	const string strData = "data";

	EXPECT_TRUE(FileManager().MakeDir(strDirPath));

	EXPECT_TRUE(FileManager().Write(strFilePath, strData, ios::trunc));

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_FALSE(FileManager().IsExist(strOutputPath));
}

TEST(FileManagerTest, ToAbsolutePath)
{
	const string strPath = "./test";
	const string strComparePath = FileManager().GetCurrentPath() + "/" + strPath;

	const string strAbsolutePath = FileManager().ToAbsolutePath(strPath);
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_STREQ(strAbsolutePath.c_str(), strComparePath.c_str());
}

TEST(FileManagerTest, ToCanonicalPath)
{
	const size_t currentPathSize = FileManager().GetCurrentPath().size();
	const string strTemplate = FileManager().GetCurrentPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strDirName =  strOutputPath.substr(currentPathSize + 1, strOutputPath.size() - currentPathSize + 1);
	const string strComparePath = FileManager().GetCurrentPath() + "/" + strDirName;

	const string strCanonicalPath = FileManager().ToCanonicalPath("./" + strDirName);
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_STREQ(strCanonicalPath.c_str(), strComparePath.c_str());

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, ToRelativePathToRootPath)
{
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strRootPath = FileManager().GetRootPath(strOutputPath);
	const string strComparePath = strOutputPath.substr(strRootPath.size(), strOutputPath.size() - strRootPath.size());

	const string strRelativePath = FileManager().ToRelativePathToRootPath(strOutputPath);
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_STREQ(strRelativePath.c_str(), strComparePath.c_str());

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, GetTempPath)
{
	EXPECT_STREQ(FileManager().GetTempPath().c_str(), "/tmp");
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");
}

TEST(FileManagerTest, GetRootPath)
{
	EXPECT_STREQ(FileManager().GetRootPath("./").c_str(), "");
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_STREQ(FileManager().GetRootPath("/test").c_str(), "/");
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");
}

TEST(FileManagerTest, GetRelativePath)
{
	const size_t currentPathSize = FileManager().GetCurrentPath().size();
	const string strTemplate = FileManager().GetCurrentPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));
	const string strDirName =  strOutputPath.substr(currentPathSize + 1, strOutputPath.size() - currentPathSize + 1);

	const string strRelativePath = FileManager().GetRelativePath(strOutputPath);
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_STREQ(strRelativePath.c_str(), strDirName.c_str());

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}


TEST(FileManagerTest, GetPathList)
{
	const string strTemplate = FileManager().GetCurrentPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));

	const vector<string> vecPath = {
										strOutputPath + "/depth",
										strOutputPath + "/depth1",
										strOutputPath + "/depth1-1",
									};
	for(const auto &iter : vecPath) {
		EXPECT_TRUE(FileManager().MakeDirs(iter + "/depth2"));
	}

	const vector<string> vecResultPath = FileManager().GetPathList(strOutputPath);

	EXPECT_EQ(vecPath.size(), vecResultPath.size());

	for(size_t i = 0 ; i < vecPath.size() ; i++) {
		EXPECT_STREQ(vecPath.at(i).c_str(), vecResultPath.at(i).c_str());
	}

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, GetRecursivePathList)
{
	const string strTemplate = FileManager().GetCurrentPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char *)(strTemplate.c_str()));

	const vector<string> vecPath = {
										strOutputPath + "/depth",
										strOutputPath + "/depth1",
										strOutputPath + "/depth1-1",
										strOutputPath + "/depth1-1/depth2",
									};
	for(const auto &iter : vecPath) {
		EXPECT_TRUE(FileManager().MakeDirs(iter));
	}

	const vector<string> vecResultPath = FileManager().GetRecursivePathList(strOutputPath);

	EXPECT_EQ(vecPath.size(), vecResultPath.size());

	for(size_t i = 0 ; i < vecPath.size() ; i++) {
		EXPECT_STREQ(vecPath.at(i).c_str(), vecResultPath.at(i).c_str());
	}

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileManagerTest, GetCurrentPath)
{
	const string strCurrentPath = FileManager().GetCurrentPath();
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_STREQ(strCurrentPath.c_str(), FileManager().ToCanonicalPath("./").c_str());
}

TEST(FileManagerTest, SetCurrentPath)
{
	const string strTempPath = FileManager().GetTempPath();

	EXPECT_TRUE(FileManager().SetCurrentPath(strTempPath));
	EXPECT_EQ(errno, 0);
	EXPECT_STREQ(strerror(errno), "Success");

	EXPECT_STREQ(FileManager().GetCurrentPath().c_str(), strTempPath.c_str());
}
