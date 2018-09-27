#include "CThreadSafeEx.h"

CThreadSafeEx::CThreadSafeEx()
{
	InitializeCriticalSection(&m_CriticalSection);
}

CThreadSafeEx::~CThreadSafeEx()
{
	DeleteCriticalSection(&m_CriticalSection);
}

inline void CThreadSafeEx::Enter()
{
	EnterCriticalSection(&m_CriticalSection);
}

inline void CThreadSafeEx::Leave()
{
	LeaveCriticalSection(&m_CriticalSection);
}
