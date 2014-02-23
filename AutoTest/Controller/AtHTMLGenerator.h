#pragma once

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

class AtHTMLGenerator
{
public:
	int		SetOption(fs::path html_dir, fs::path& template_file);

	int		Start();										// ����ģ���ļ�

	int		AddItem(const AtTestResult& TestResult);		// ���һ�����������Ĳ��Խ��

	int		Stop();											// �����ļ�

private:
	fs::path			m_TemplatePath;			// htmlģ���ļ�·��	
	fs::path			m_ResultPath;			// ���ɵ�Ŀ���ļ���·��
	TiXmlDocument*		m_pxdResult;			// 
};