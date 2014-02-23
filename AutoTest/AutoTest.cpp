// AutoTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Controller/AtController.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

int _tmain(int argc, _TCHAR* argv[])
{
	int				i					= 0;
	int				nErrCode			= 0;
	AtController	Controller;

	// 1. 扫描TestCase目录，列出所有的xml文件
	std::vector<fs::path>		vSubPath;
	std::vector<fs::path>		vXml;

	fs::path  TestCaseDir("TestCase");
	try {
		if (!fs::exists(TestCaseDir) || fs::is_regular_file(TestCaseDir))
		{
			printf("TestCase Directory not exist!!!\n");
			return 0;
		}

		std::copy(fs::directory_iterator(TestCaseDir), fs::directory_iterator(), back_inserter(vSubPath));

		i = 0;
		for (std::vector<fs::path>::const_iterator it = vSubPath.begin(); it != vSubPath.end(); ++it, i++)
		{
			if (fs::is_regular_file(*it) && it->extension() == ".xml")
			{
				printf("[%2d]: %s\n", i, it->string().c_str());
				vXml.push_back(*it);
			}
		}
	}
	catch(const fs::filesystem_error& ex)
	{
		printf("[error]: %s\n", ex.what());
	}

	// 2. 等待用户选择xml
	printf("选择测试文件: ");
	std::cin >> i;

	// 3. 执行选择的测试用例
	
	nErrCode = Controller.init(vXml[i].string());
	if (nErrCode)
	{
		printf("[error]: Controller.init return %d\n", nErrCode);
		return	0;
	}

	Controller.PrintTestCases();

	Controller.ProcessTestCases();

	return 0;
}

