#include "../stdafx.h"
#include "../Controller/AtController.h"
#include "AtDbgMonitor.h"

AtDbgMonitor::AtDbgMonitor()
{
}

AtDbgMonitor::~AtDbgMonitor()
{

}

int AtDbgMonitor::CreateDbgBuffer()
{
	int		nErrCode		= 0;

	m_hDBWinBuffer		= CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, "DBWIN_BUFFER");
	if (m_hDBWinBuffer == NULL)
	{
		nErrCode = ERROR_CODE(nErrCode, 1);
		goto _exit;
	}

	m_pDBWinBuffer	= MapViewOfFile(m_hDBWinBuffer, FILE_MAP_READ, 0, 0, 0);
	if (m_pDBWinBuffer == NULL)
	{
		nErrCode = ERROR_CODE(nErrCode, 2);
		goto _exit;
	}

	m_hDBWinDataReady	= CreateEvent(NULL, TRUE, FALSE, _T("DBWIN_DATA_READY"));
	m_hDBWinBufferReady	= CreateEvent(NULL, TRUE, TRUE, _T("DBWIN_BUFFER_READY"));
	if (m_hDBWinBufferReady == NULL || m_hDBWinDataReady == NULL)
	{

		nErrCode = ERROR_CODE(nErrCode, 3);
		goto _exit;
	}

	return 0;

_exit:

	CloseHandle(m_hDBWinBuffer);
	m_hDBWinBuffer	= NULL;

	CloseHandle(m_hDBWinBufferReady);
	m_hDBWinBufferReady		= NULL;

	CloseHandle(m_hDBWinDataReady);
	m_hDBWinDataReady		= NULL;

	return nErrCode;
}

int AtDbgMonitor::DestoryDbgBuffer()
{
	if (m_hDBWinBufferReady)
	{
		CloseHandle(m_hDBWinBufferReady);
		m_hDBWinBufferReady	= NULL;
	}

	if (m_hDBWinDataReady)
	{
		CloseHandle(m_hDBWinDataReady);
		m_hDBWinDataReady	= NULL;
	}

	if (m_pDBWinBuffer)
	{
		UnmapViewOfFile(m_pDBWinBuffer);
		m_pDBWinBuffer = NULL;
	}

	if (m_hDBWinBuffer)
	{
		CloseHandle(m_hDBWinBuffer);
		m_hDBWinBuffer = NULL;
	}

	return 0;
}

unsigned int AtDbgMonitor::CheckCondition()
{
	unsigned int		nErrCode		= MakeErrorMonitor(def_AT_MONITOR_Failed);
	const AtCondition*	pCondition		= GetCondition();
	DWORD				dwWaitCode		= 0;

	clock_t	beginClock		= clock();
	clock_t	timeout			= 5000;			// 默认等待5s

	if (pCondition->timeout > 0)
	{
		timeout = pCondition->timeout * 1000;
	}

	if (CreateDbgBuffer() != 0)
	{
		goto End;
	}

	while(1) {
		// 检查是否超时
		if (clock() - beginClock > timeout)
		{
			nErrCode = MakeErrorMonitor(def_AT_MONITOR_Timeout);
			goto End;
		}

		dwWaitCode = WaitForSingleObject(m_hDBWinDataReady, 100);
		if (dwWaitCode == WAIT_TIMEOUT)
		{
			continue;
		}

		// Buffer可读
		LPCSTR pDbgString = (LPCSTR)m_pDBWinBuffer;
		pDbgString += sizeof(DWORD);

		//printf("[DbgView]: %s\n", pDbgString);

		if (strstr(pDbgString, pCondition->target.c_str()) != NULL)
		{
			// 发现目标
			nErrCode = MakeErrorMonitor(def_AT_MONITOR_Success);
			goto End;
		}

		::SetEvent(m_hDBWinBufferReady);
	}

End:
	DestoryDbgBuffer();

	return nErrCode;
}

