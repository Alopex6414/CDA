#pragma once

#ifndef __CSOCKETBASE_H__
#define __CSOCKETBASE_H__

//Include WinSock2 Header File
#include <WinSock2.h>

//Include WinSock2 Library
#pragma comment(lib, "Ws2_32.lib")

//Macro Definition
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#define _WINSOCKAPI_								//我们不需要旧的winsock.h
#define SOB_IP_LENGTH				128             //IP字符串长度
#define SOB_UDP_RECV_BUFFER			32*1024			//接收缓冲32K
#define SOB_DEFAULT_TIMEOUT_SEC		5				//默认的超时时间

#define SOB_RET_OK					1				//正常
#define SOB_RET_FAIL				0				//错误
#define SOB_RET_TIMEOUT				-1				//超时

//Callback Definition
typedef unsigned(__stdcall *HANDLE_ACCEPT_THREAD)(void*);		//定义接受连接线程函数
typedef void(__stdcall *HANDLE_ACCEPT_CALLBACK)(SOCKADDR_IN* pRemoteAddr, SOCKET s, DWORD dwUser);		//定义接受连接线程函数

//Class Definition
class CSocketBase
{
public:
	CSocketBase();
	~CSocketBase();

public:
	static void InitSocketLib();
	static void ReleaseSocketLib();

	void SetRecvTimeOut(UINT uiMSec);
	void SetSendTimeOut(UINT uiMsec);

	void SetRecvBufferSize(UINT uiByte);
	void SetSendBufferSize(UINT uiByte);

	bool ConnectRemote(const char* pcRemoteIP = NULL, USHORT sPort = 0, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);
	bool BindOnPort(UINT uiPort);
	bool BindOnUDPPort(const char* pcRemoteIP, UINT uiPort);
	bool StartListenAndAccept(HANDLE_ACCEPT_THREAD pThreadFunc, HANDLE_ACCEPT_CALLBACK pCallback, DWORD dwUser, BOOL* pExitFlag = NULL, USHORT nLoopTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);
	bool Reconnect();
	void Disconnect();

	int SendMsg(const char* pcMsg, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);
	int SendBuffer(char* pBuffer, UINT uiBufferSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);
	int SendUDPBuffer(const char* pcIP, SHORT sPort, char* pBuffer, UINT uiBufferSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);
	int RecvUDPMsg(char* pBuffer, UINT uiBufferSize, UINT& uiRecv, char* pcIP, USHORT& uPort, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);
	int RecvOnce(char* pRecvBuffer, UINT uiBufferSize, UINT& uiRecv, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);
	int RecvBuffer(char* pRecvBuffer, UINT uiBufferSize, UINT uiRecvSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);

	static bool ResolveAddressToIp(const char* pcAddress, char* pcIp);
public:
	SOCKET getRawSocket();
	bool attachRawSocket(SOCKET s, bool bIsConnected);
	void detachSocket();
	const char* getHostIP();
	USHORT getHostPort();
	const char* getRemoteIP();
	const wchar_t* getRemotewIP();
	ULONG getRemoteULIP();
	USHORT getRemotePort();
	bool IsConnected();
	void Destroy();

	int m_nLastWSAError;

public:
	void GetHostIP();

private:
	SOCKET CreateTCPSocket();
	SOCKET CreateUDPSocket();

private:
	SOCKET m_socket;
	WSAEVENT m_SocketWriteEvent;
	WSAEVENT m_SocketReadEvent;

	bool m_bIsConnected;
	char m_pcHostIP[SOB_IP_LENGTH];
	USHORT m_sHostPort;
	char m_pcRemoteIP[SOB_IP_LENGTH];
	wchar_t m_pwcRemoteIP[SOB_IP_LENGTH];
	USHORT m_sRemotePort;

};

#endif // !__CSOCKETBASE_H__
