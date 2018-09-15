#pragma once

#ifndef __CTHREADSAFE_H_
#define __CTHREADSAFE_H_

#include <Windows.h>

class CThreadSafe
{
private:
	CRITICAL_SECTION* m_pCriticalSection;
	bool m_bThreadSafe;

public:
	CThreadSafe(const CRITICAL_SECTION* pCriticalSection, const bool bThreadSafe = true);
	~CThreadSafe();
};

#endif