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
	HANDLE m_hCOM;			// CCSerialPortBase SerialPort Handle(串口句柄)
	HANDLE m_hListenThread;	// CCSerialPortBase SerialPort Listen Thread Handle(串口监听线程句柄)

private:
	static bool m_sbExit;	// CCSerialPortBase Exit Flag(串口退出标志)
	CRITICAL_SECTION m_csCOMSync;	// CCSerialPortBase Critical Section Sync(串口异步接收临界区)

protected:
	map<int, string> m_mapEnumCOM;	// CCSerialPortBase Enum SerialPort Map(串口枚举列表)

public:
	void EnumSerialPort();	// CCSerialPortBase 枚举串口

public:
	CCSerialPortBase();		// CCSerialPortBase 构造函数
	~CCSerialPortBase();	// CCSerialPortBase 析构函数

};

#endif // !__CSERIALPORTBASE_H__
