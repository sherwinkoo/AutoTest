#pragma once

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

class AtHTMLGenerator
{
public:
	int		SetOption(fs::path html_dir, fs::path& template_file);

	int		Start();										// 加载模板文件

	int		AddItem(const AtTestResult& TestResult);		// 添加一个测试用例的测试结果

	int		Stop();											// 保存文件

private:
	fs::path			m_TemplatePath;			// html模板文件路径	
	fs::path			m_ResultPath;			// 生成的目标文件的路径
	TiXmlDocument*		m_pxdResult;			// 
};