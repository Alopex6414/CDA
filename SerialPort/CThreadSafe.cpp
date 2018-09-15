#include "CThreadSafe.h"

CThreadSafe::CThreadSafe(const CRITICAL_SECTION* pCriticalSection, const bool bThreadSafe)
{
	m_pCriticalSection = (CRITICAL_SECTION*)pCriticalSection;
	m_bThreadSafe = bThreadSafe;

	if (m_bThreadSafe) EnterCriticalSection(m_pCriticalSection);
}

CThreadSafe::~CThreadSafe()
{
	if (m_bThreadSafe) LeaveCriticalSection(m_pCriticalSection);
}