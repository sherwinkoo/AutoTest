#pragma once

typedef	void (*FN_MonitorCallback)(unsigned int ErrorCode);

class AtMonitor
{
public:
	AtMonitor();
	~AtMonitor();

	int start(const AtCondition* pCondition, FN_MonitorCallback pfnCallback = NULL);

	int	IsExit(){return m_nExitThread;};
	int	IsMonitor(){return m_nMonitor;}
	
	const AtCondition* GetCondition() {return m_pCondition;}
	
	void		DoCallback(unsigned int nErrCode);
	
	virtual unsigned int CheckCondition();

	unsigned int Wait();

	bool	CheckTimeout();

	static DWORD __stdcall MonitorThreadProc(void* This);

private:
	int						m_nExitThread;
	int						m_nMonitor;
	HANDLE					m_hThread;
	
	unsigned int			m_nStatus;
	HANDLE					m_hStatusSemaphore;
	
	FN_MonitorCallback		m_pfnCallback;
	const AtCondition		*m_pCondition;
};
