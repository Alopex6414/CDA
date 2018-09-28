#pragma once

#ifndef __THREADSAFEEX_H_
#define __THREADSAFEEX_H_

#include <Windows.h>

class CThreadSafeEx
{
private:
	CRITICAL_SECTION m_CriticalSection;

public:
	CRITICAL_SECTION* GetCriticalSection();

public:
	CThreadSafeEx();
	~CThreadSafeEx();

	void Enter();
	void Leave();
};

#endif // !__THREADSAFEEX_H_

