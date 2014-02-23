#pragma once

#include "AtMonitor.h"

class AtDbgMonitor: public AtMonitor
{
public:
	AtDbgMonitor();
	~AtDbgMonitor();

	virtual unsigned int CheckCondition();

private:
	int		CreateDbgBuffer();
	int		DestoryDbgBuffer();

private:
	HANDLE			m_hDBWinBuffer;
	HANDLE			m_hDBWinBufferReady;
	HANDLE			m_hDBWinDataReady;

	LPVOID			m_pDBWinBuffer;
};
