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
#include <process.h>

using namespace std;

//Macro Definition
#define SERIALPORT_COMM_INPUT_BUFFER_SIZE	4096	// ����ͨ�����뻺������С
#define SERIALPORT_COMM_OUTPUT_BUFFER_SIZE	4096	// ����ͨ�������������С

//Struct Definition
typedef struct
{
	CHAR chPort[MAX_PATH];	// ���ں�
	DWORD dwBaudRate;		// ���ڲ�����
	BYTE byDataBits;		// ��������λ
	BYTE byStopBits;		// ����ֹͣλ
	BYTE byCheckBits;		// ����У��λ
}S_SERIALPORT_PROPERTY, *LPS_SERIALPORT_PROPERTY;

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
	OVERLAPPED m_ovWrite;	// CCSerialPortBase OverLapped Write
	OVERLAPPED m_ovRead;	// CCSerialPortBase OverLapped Read
	OVERLAPPED m_ovWait;	// CCSerialPortBase OverLapped Wait

public:
	volatile bool m_bOpen;	// CCSerialPortBase Open Flag(���ڴ򿪱�־)

private:
	CRITICAL_SECTION m_csCOMSync;	// CCSerialPortBase Critical Section Sync(�����첽�����ٽ���)

public:
	map<int, string> m_mapEnumCOM;	// CCSerialPortBase Enum SerialPort Map(����ö���б�)

// ��������
public:
	unsigned char m_chSendBuf[SERIALPORT_COMM_INPUT_BUFFER_SIZE];
	unsigned char m_chRecvBuf[SERIALPORT_COMM_OUTPUT_BUFFER_SIZE];

public:
	void EnumSerialPort();	// CCSerialPortBase ö�ٴ���

protected:
	bool CCSerialPortBaseCreate(const char* szPort);					// CCSerialPortBase �򿪴���(��������)
	bool CCSerialPortBaseConfig(S_SERIALPORT_PROPERTY sCommProperty);	// CCSerialPortBase ���ô���

protected:
	bool CCSerialPortBaseInit(S_SERIALPORT_PROPERTY sCommProperty);		// CCSerialPortBase ��ʼ������
	bool CCSerialPortBaseInitListen();									// CCSerialPortBase ��ʼ�����ڼ���
	void CCSerialPortBaseClose();										// CCSerialPortBase �رմ���
	void CCSerialPortBaseCloseListen();									// CCSerialPortBase �رմ��ڼ���

public:
	CCSerialPortBase();		// CCSerialPortBase ���캯��
	~CCSerialPortBase();	// CCSerialPortBase ��������

	bool CCSerialPortBaseGetStatus() const;			// CCSerialPortBase ��ȡ����״̬

	void CCSerialPortBaseSetSendBuf(unsigned char* pBuff, int nSize);	// CCSerialPortBase ���÷��ͻ���
	void CCSerialPortBaseGetRecvBuf(unsigned char* pBuff, int nSize);	// CCSerialPortBase ��ȡ���ջ���

	bool CCSerialPortBaseOpenPort(S_SERIALPORT_PROPERTY sCommProperty);	// CCSerialPortBase �򿪴���
	void CCSerialPortBaseClosePort();									// CCSerialPortBase �رմ���

	bool OnTranslateBuffer();											// CCSerialPortBase ���ڷ�������
	static unsigned int CALLBACK OnReceiveBuffer(LPVOID lpParameters);	// CCSerialPortBase ���ڽ����߳�

};

#endif // !__CSERIALPORTBASE_H__
