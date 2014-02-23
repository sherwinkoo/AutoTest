// AutoTest.cpp : �������̨Ӧ�ó������ڵ㡣
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

	// 1. ɨ��TestCaseĿ¼���г����е�xml�ļ�
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

	// 2. �ȴ��û�ѡ��xml
	printf("ѡ������ļ�: ");
	std::cin >> i;

	// 3. ִ��ѡ��Ĳ�������
	
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

