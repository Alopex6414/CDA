#include "CThreadSafeEx.h"

CThreadSafeEx::CThreadSafeEx()
{
	InitializeCriticalSection(&m_CriticalSection);
}

CThreadSafeEx::~CThreadSafeEx()
{
	DeleteCriticalSection(&m_CriticalSection);
}

void CThreadSafeEx::Enter()
{
	EnterCriticalSection(&m_CriticalSection);
}

void CThreadSafeEx::Leave()
{
	LeaveCriticalSection(&m_CriticalSection);
}
