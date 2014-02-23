#include "../stdafx.h"

#include "AtController.h"
#include "../Starter/AtStarter.h"
#include "../Monitor/AtMonitor.h"
#include "../Monitor/AtDbgMonitor.h"

AtController::AtController()
{

}

AtController::~AtController()
{

}

int AtController::init(const std::string& strXmlPath)
{
	int		nErrCode		= 0;

	m_pAutoTestFile	= new TiXmlDocument(strXmlPath.c_str());
	if (m_pAutoTestFile == NULL)
	{
		nErrCode = ERROR_CODE(nErrCode, 1);
		goto _Failed;
	}

	if(false == m_pAutoTestFile->LoadFile())
	{
		nErrCode = ERROR_CODE(nErrCode, 2);
		goto _Failed;
	}

	nErrCode = initTestGroup();
	if (nErrCode)
	{
		nErrCode = ERROR_CODE(nErrCode, 3);
		goto _Failed;
	}

_Failed:

	return nErrCode;
}

int AtController::release()
{
	int		nErrCode		= 0;

	if (m_pAutoTestFile)
	{
		delete m_pAutoTestFile;
	}

	return nErrCode;
}

int AtController::initScriptCmd(TiXmlElement* pxeScriptCmd, AtScriptCmd* pCmd)
{
	int				nErrCode			= 0;
	const char*		pAttrValue			= NULL;

	if (pxeScriptCmd == NULL || pCmd == NULL)
	{
		return ERROR_CODE(nErrCode, 1);
	}

	// 1. type
	pCmd->type	= def_AT_SCRIPT_TYPE_Unknown;
	pAttrValue = pxeScriptCmd->Attribute("type");
	if (strcmp(pAttrValue, "BAT") == 0)
	{
		// windows cmd script
		pCmd->type	= def_AT_SCRIPT_TYPE_bat;
	}

	// 2. script path
	pCmd->ScriptPath += pxeScriptCmd->GetText();

	return nErrCode;
}

int AtController::initContidion(TiXmlElement* pxeCondition, AtCondition* pCondition)
{
	int				nErrCode			= 0;
	const char*		pAttrValue			= NULL;

	if (pxeCondition == NULL || pCondition == NULL)
	{
		return ERROR_CODE(nErrCode, 1);
	}

	// 1. type
	pCondition->type	= def_AT_CONDITION_TYPE_Unknown;
	pAttrValue = pxeCondition->Attribute("type");
	if (strcmp(pAttrValue, "file") == 0)
	{
		// windows cmd script
		pCondition->type	= def_AT_CONDITION_TYPE_File;
	}
	else if (strcmp(pAttrValue, "DbgView") == 0)
	{
		// windows DbgView
		pCondition->type	= def_AT_CONDITION_TYPE_DbgView;
	}

	// 2. log path
	pCondition->path += pxeCondition->Attribute("path");

	// 3. timeout
	pxeCondition->Attribute("timeout", &pCondition->timeout);

	// 3. target
	pCondition->target += pxeCondition->GetText();

	return nErrCode;
}

int AtController::initTestCase(TiXmlElement* pxeTestCase, AtTestCase* pTestCase)
{
	int					nErrCode		= 0;
	TiXmlElement		*pxeChild		= NULL;

	if (pxeTestCase == NULL || pTestCase == NULL)
	{
		return	ERROR_CODE(nErrCode, 1);
	}

	// 0. name
	pTestCase->name += pxeTestCase->Attribute("name");

	// 1. case::setup
	pxeChild	= (TiXmlElement*)pxeTestCase->FirstChild("setup");
	nErrCode = initScriptCmd(pxeChild, &pTestCase->setup);
	if (nErrCode)
	{
		return ERROR_CODE(nErrCode, 2);
	}

	// 2. case::cleanup
	pxeChild	= (TiXmlElement*)pxeTestCase->FirstChild("cleanup");
	nErrCode = initScriptCmd(pxeChild, &pTestCase->cleanup);
	if (nErrCode)
	{
		return ERROR_CODE(nErrCode, 3);
	}

	// 3. case::condition
	pxeChild	= (TiXmlElement*)pxeTestCase->FirstChild("condition");
	nErrCode = initContidion(pxeChild, &pTestCase->condition);
	if (nErrCode)
	{
		return ERROR_CODE(nErrCode, 4);
	}

	return	nErrCode;
}

int AtController::initTestGroup()
{
	int				nErrCode			= 0;
	TiXmlElement	*pxeTestGroup		= NULL;
	TiXmlElement	*pxeSetup			= NULL;
	TiXmlElement	*pxeCleanup			= NULL;
	TiXmlElement	*pxeTestCase		= NULL;

	if (m_pAutoTestFile == NULL)
	{
		return	ERROR_CODE(nErrCode, 1);
	}

	// 开始解析文件
	pxeTestGroup	= (TiXmlElement*)m_pAutoTestFile->FirstChild("TestGroup");
	if (pxeTestGroup == NULL)
	{
		return ERROR_CODE(nErrCode, 2);
	}

	m_TestGroupName += pxeTestGroup->Attribute("name");

	// TestGroup::setup
	pxeSetup	= (TiXmlElement*)pxeTestGroup->FirstChild("setup");
	nErrCode = initScriptCmd(pxeSetup, &m_TestGroupSetup);
	if (nErrCode)
	{
		return ERROR_CODE(nErrCode, 3);
	}

	// TestGroup::cleanup
	pxeCleanup	= (TiXmlElement*)pxeTestGroup->FirstChild("cleanup");
	nErrCode = initScriptCmd(pxeCleanup, &m_TestGroupCleanup);
	if (nErrCode)
	{
		return ERROR_CODE(nErrCode, 4);
	}

	// TestGroup::case
	for (pxeTestCase = (TiXmlElement*)pxeTestGroup->FirstChild("Case"); pxeTestCase; pxeTestCase = pxeTestCase->NextSiblingElement("Case"))
	{
		AtTestCase		TestCase;
		nErrCode	= initTestCase(pxeTestCase, &TestCase);
		if (nErrCode)
		{
			return ERROR_CODE(nErrCode, 5);
		}

		m_vCases.push_back(TestCase);
	}

	return	nErrCode;
}

int AtController::ProcessTestCases()
{
	AtStarter		Starter;
	AtMonitor*		Monitor			= new AtDbgMonitor;
	unsigned int	nRunCode		= 0;
	unsigned int	nStatus			= MakeErrorMonitor(def_AT_MONITOR_Failed);

	// TODO: 处理返回值
	Starter.start(&m_TestGroupSetup);

	for (unsigned int i = 0; i < m_vCases.size(); i++)
	{
		AtTestCase	TestCase	= m_vCases[i];

		// 1. 通知Monitor开始监视condition和timeout
		nRunCode = Monitor->start(&TestCase.condition);
		if (!AtSuccess(nRunCode))
		{
			goto PrintResult;
		}

		// 2. 通知Starter启动setup命令
		nRunCode = Starter.start(&TestCase.setup);
		if (!AtSuccess(nRunCode))
		{
			goto PrintResult;
		}

		// 3. 等待Monitor返回测试结果
		nStatus = Monitor->Wait();

		// 4. 通知Starter执行cleanup命令
		nRunCode = Starter.start(&TestCase.cleanup);

PrintResult:
		// 5. 打印测试信息
		PrintTestCaseResult(i, TestCase.name, nStatus, nRunCode);
	}

	// TODO: 处理返回值
	Starter.start(&m_TestGroupCleanup);

	delete Monitor;
	Monitor		= NULL;

	return	0;
}

void AtController::PrintTestCases()
{
	printf("**********************************************************\n");
	printf("*	TestGroup: %s		count: %d\n", m_TestGroupName.c_str(), m_vCases.size());
	printf("----------------------------------------------------------\n");

	for (unsigned int i = 0; i < m_vCases.size(); i++)
	{
		printf("* [%d] name: %s, condition: %s, timeout: %ds\n", i, m_vCases[i].name.c_str(), m_vCases[i].condition.target.c_str(), m_vCases[i].condition.timeout);
	}
	
	printf("**********************************************************\n");

}

void AtController::PrintTestCaseResult(int index, std::string& name, unsigned int status, unsigned int nErrCode)
{
	std::string			strStatus;
	std::string			strReason;
	AtTestResult		Result		= {name, status, nErrCode};

	strStatus	= AtGetErrorString(status);
	strReason	= AtGetErrorString(nErrCode);

	m_vResult.push_back(Result);

	HANDLE		hStdout		= GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == NULL)
	{
		printf("[%s]: %-20s %s\n", index, name.c_str(), strStatus.c_str(), strReason.c_str());
		return ;
	}

		if (AtSuccess(status) && AtSuccess(nErrCode))
	{
		::SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN);
		printf("[PASSED]: ");
		strReason = "none";
	}
	else
	{
		::SetConsoleTextAttribute(hStdout, FOREGROUND_RED);
		printf("[FAILED]: ");
		strReason += ", " + strStatus;
	}

	::SetConsoleTextAttribute(hStdout, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
	printf("%-20s %s\n", name.c_str(), strReason.c_str());

}

