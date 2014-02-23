#include "../stdafx.h"
#include "../Controller/AtController.h"
#include "AtMonitor.h"

AtMonitor::AtMonitor()
{
	DWORD		dwThreadId		= 0;

	m_nExitThread		= 0;
	m_nMonitor			= 0;

	m_pCondition		= NULL;
	m_pfnCallback		= NULL;

	m_hThread = ::CreateThread(NULL, 0, AtMonitor::MonitorThreadProc, this, 0, &dwThreadId);

	if (m_hThread == INVALID_HANDLE_VALUE)
	{
		throw std::exception("Monitor::CreateThread Failed");
	}

	m_hStatusSemaphore = ::CreateSemaphoreA(NULL, 0, 1, NULL);
	if (m_hStatusSemaphore == INVALID_HANDLE_VALUE)
	{
		throw std::exception("Monitor::CreateMutex Failed");
	}
}

AtMonitor::~AtMonitor()
{
	m_nMonitor		= 0;
	m_nExitThread	= 1;

	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 1000))
		{
			TerminateThread(m_hThread, -1);
		}
	}

	if (m_hStatusSemaphore != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hStatusSemaphore);
	}
}

DWORD __stdcall AtMonitor::MonitorThreadProc(void* This)
{
	unsigned int	nErrCode	= def_AT_MONITOR_Success;
	AtMonitor*		monitor		= (AtMonitor*)This;

	while(!monitor->IsExit())
	{
		if (!monitor->IsMonitor())
		{
			::Sleep(100);
			continue;
		}

		// 检查条件是否符合
		nErrCode = monitor->CheckCondition();

		// 调用回调通知controller
		monitor->DoCallback(nErrCode);

	}
	return 0;
}

int AtMonitor::start(const AtCondition* pCondition, FN_MonitorCallback pfnCallback)
{
	m_pCondition	= pCondition;
	m_pfnCallback	= pfnCallback;
	m_nStatus		= MakeErrorMonitor(def_AT_MONITOR_Success);

	m_nMonitor		= 1;

	return MakeErrorMonitor(def_AT_MONITOR_Success);
}

unsigned int AtMonitor::Wait()
{
	::WaitForSingleObject(m_hStatusSemaphore, INFINITE);

	return	m_nStatus;
}

void AtMonitor::DoCallback(unsigned int nErrCode)
{
	if (m_pfnCallback)
	{
		m_pfnCallback(nErrCode);
	}

	m_nStatus		= nErrCode;
	m_nMonitor		= 0;
	m_pfnCallback	= NULL;
	m_pCondition	= NULL;

	::ReleaseSemaphore(m_hStatusSemaphore, 1, NULL);
}


unsigned int AtMonitor::CheckCondition()
{
	char	szLine[2048]	= {0};
	FILE*	fd				= NULL;
	clock_t	beginClock		= clock();
	clock_t	timeout			= 5000;			// 默认等待5s
	
	if (m_pCondition->timeout > 0)
	{
		timeout = m_pCondition->timeout * 1000;
	}

	do {
	
		errno_t		error = fopen_s(&fd, m_pCondition->path.c_str(), "r");
		if (error != 0 || fd == NULL)
		{
			return MakeErrorMonitor(def_AT_MONITOR_PathNotExist);
		}

		while(!feof(fd))
		{
			// 读取文件全部内存，检查目标
			fgets(szLine, 2048, fd);

			if (strstr(szLine, m_pCondition->target.c_str()) != NULL)
			{
				// 找到目标
				fclose(fd);
				return MakeErrorMonitor(def_AT_MONITOR_Success);
			}


			if (clock() - beginClock > timeout)
			{
				fclose(fd);
				return MakeErrorMonitor(def_AT_MONITOR_Timeout);
			}
		}

		fclose(fd);

	}while(1);

	return MakeErrorMonitor(def_AT_MONITOR_Success);
}


