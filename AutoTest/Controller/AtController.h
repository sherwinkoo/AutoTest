#pragma once

#include <string>
#include <vector>

#define		def_AT_SCRIPT_TYPE_bat			1
#define		def_AT_SCRIPT_TYPE_Unknown		100

#define		def_AT_CONDITION_TYPE_File		1
#define		def_AT_CONDITION_TYPE_DbgView	2
#define		def_AT_CONDITION_TYPE_Unknown	100

struct AtScriptCmd {
	unsigned int		type;
	std::string			ScriptPath;
};

struct	AtCondition {
	unsigned int		type;
	std::string			path;
	std::string			target;
	int					timeout;
};

struct AtTestCase {
	std::string		name;
	AtScriptCmd		setup;
	AtScriptCmd		cleanup;
	AtCondition		condition;
};

struct AtTestResult {
	std::string		case_name;		// 测试用例名
	unsigned int	status;			// Monitor的检测结果
	unsigned int	error_code;		// ScriptRunner的运行结果
};

class AtController
{
public:
	AtController();
	~AtController();

	int init(const std::string& strXmlPath);
	int release();

	// 根据xml解析的结果，处理每一个TestCase
	int ProcessTestCases();

	void PrintTestCases();

private:
	int	initTestGroup();
	int initScriptCmd(TiXmlElement* pxeScriptCmd, AtScriptCmd* pCmd);
	int initTestCase(TiXmlElement* pxeTestCase, AtTestCase* pTestCase);
	int	initContidion(TiXmlElement* pxeCondition, AtCondition* pCondition);

	void PrintTestCaseResult(int index, std::string& name, unsigned int status, unsigned int nErrCode);

private:
	TiXmlDocument*		m_pAutoTestFile;

	std::string			m_TestGroupName;
	AtScriptCmd			m_TestGroupSetup;
	AtScriptCmd			m_TestGroupCleanup;

	std::vector<AtTestCase>	m_vCases;

	std::vector<AtTestResult>		m_vResult;
};