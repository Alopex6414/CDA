#pragma once

#ifndef __CSERIALPORTBASE_H__
#define __CSERIALPORTBASE_H__

//Include Window Header File
#include <Windows.h>

//Include C/C++ Header File
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <mmreg.h>
#include <wchar.h>
#include <tchar.h>
#include <time.h>
#include <mmsystem.h>

#include <iostream>
#include <map>
#include <vector>

using namespace std;

//Template Release
template<class T>
void SafeDelete(T*& t)
{
	if (nullptr != t)
	{
		delete t;
		t = nullptr;
	}
}

template<class T>
void SafeDeleteArray(T*& t)
{
	if (nullptr != t)
	{
		delete[] t;
		t = nullptr;
	}
}

template<class T>
void SafeRelease(T*& t)
{
	if (nullptr != t)
	{
		t->Release();
		t = nullptr;
	}
}

//Class Definition
class CCSerialPortBase
{
private:
	HANDLE m_hCOM;			// CCSerialPortBase SerialPort Handle(���ھ��)
	HANDLE m_hListenThread;	// CCSerialPortBase SerialPort Listen Thread Handle(���ڼ����߳̾��)

private:
	static bool m_sbExit;	// CCSerialPortBase Exit Flag(�����˳���־)
	CRITICAL_SECTION m_csCOMSync;	// CCSerialPortBase Critical Section Sync(�����첽�����ٽ���)

protected:
	map<int, string> m_mapEnumCOM;	// CCSerialPortBase Enum SerialPort Map(����ö���б�)

public:
	void EnumSerialPort();	// CCSerialPortBase ö�ٴ���

public:
	CCSerialPortBase();		// CCSerialPortBase ���캯��
	~CCSerialPortBase();	// CCSerialPortBase ��������

};

#endif // !__CSERIALPORTBASE_H__
