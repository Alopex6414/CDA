#pragma once

#ifndef __CSOCKETBASE_H__
#define __CSOCKETBASE_H__

//Include WinSock2 Header File
#include <WinSock2.h>

//Include C/C++ Header File
#include <iostream>
#include <map>
#include <vector>

//Include WinSock2 Library
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

//Macro Definition
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#define _WINSOCKAPI_

#define SOB_IP_LENGTH				128             //IP地址长度

#define SOB_TCP_SEND_BUFFER			32*1024			//TCP发送缓冲32K
#define SOB_TCP_RECV_BUFFER			32*1024			//TCP接收缓冲32K
#define SOB_UDP_RECV_BUFFER			32*1024			//UDP接收缓冲32K

#define SOB_DEFAULT_TIMEOUT_SEC		5				//默认的超时时间
#define SOB_DEFAULT_MAX_CLIENT		20				//默认服务端最大连接数

#define SOB_RET_OK					1				//正常
#define SOB_RET_FAIL				0				//错误
#define SOB_RET_TIMEOUT				-1				//超时
#define SOB_RET_CLOSE				-2				//断开

//Struct Definition
typedef struct
{
	SOCKET Socket;
	SOCKADDR_IN SocketAddr;
}S_CLIENTINFO, *LPS_CLIENTINFO;

//Callback Definition
typedef unsigned(__stdcall *HANDLE_ACCEPT_THREAD)(void*);		//定义接受连接线程函数
typedef void(__stdcall *HANDLE_ACCEPT_CALLBACK)(SOCKADDR_IN* pRemoteAddr, SOCKET s, DWORD dwUser);		//定义接受连接线程函数

//Class Definition
class CCSocketBase
{
public:
	CCSocketBase();			// CCSocketBase 构造函数
	~CCSocketBase();		// CCSocketBase 析构函数

public:
	static void CCSocketBaseLibInit();			// CCSocketBase 初始Socket环境
	static void CCSocketBaseLibRelease();		// CCSocketBase 清理Socket环境

private:
	SOCKET CreateTCPSocket();					// CCSocketBase 创建TCP套接字
	SOCKET CreateUDPSocket();					// CCSocketBase 创建UDP套接字

// 公用成员函数
public:
	void CCSocketBaseSetRecvTimeOut(UINT uiMSec);			// CCSocketBase 设置接收超时时长
	void CCSocketBaseSetSendTimeOut(UINT uiMSec);			// CCSocketBase 设置发送超时时长
	void CCSocketBaseSetRecvBufferSize(UINT uiByte);		// CCSocketBase 设置接收数组长度
	void CCSocketBaseSetSendBufferSize(UINT uiByte);		// CCSocketBase 设置发送数组长度

	SOCKET CCSocketBaseGetRawSocket() const;				// CCSocketBase 获取Socket句柄
	bool CCSocketBaseAttachRawSocket(SOCKET s, bool bIsConnected);	// CCSocketBase 绑定Socket套接字
	void CCSocketBaseDettachRawSocket();					// CCSocketBase 分离Socket套接字

	const char* CCSocketBaseGetRemoteIP() const;			// CCSocketBase 获取远端IP地址(ASCII)
	const wchar_t* CCSocketBaseGetRemoteIPW() const;		// CCSocketBase 获取远端IP地址(Unicode)
	ULONG CCSocketBaseGetRemoteIPUL() const;				// CCSocketBase 获取远端IP地址(ULONG)
	USHORT CCSocketBaseGetRemotePort() const;				// CCSocketBase 获取远程端口号
	bool CCSocketBaseIsConnected() const;					// CCSocketBase 获取连接状态(客户端)
	void CCSocketBaseDestory();								// CCSocketBase 删除SocketBase类

// TCP服务端成员函数
public:
	bool CCSocketBaseBindOnPort(USHORT uPort);	// CCSocketBase 绑定服务端端口
	bool CCSocketBaseListen();					// CCSocketBase 监听服务端端口
	bool CCSocketBaseAccept(HANDLE_ACCEPT_THREAD pThreadFunc, HANDLE_ACCEPT_CALLBACK pCallback, DWORD dwUser, BOOL* pExitFlag = NULL, USHORT nLoopTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);	// CCSocketBase 接收客户端连接请求

	int CCSocketBaseSendOnce(SOCKET Socket, char* pSendBuffer, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);										// CCSocketBase 发送缓冲数据(发送全部数据)
	int CCSocketBaseSendBuffer(SOCKET Socket, char* pSendBuffer, UINT uiBufferSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);					// CCSocketBase 发送缓冲数据(发送一定数据)
	int CCSocketBaseRecvOnce(SOCKET Socket, char* pRecvBuffer, UINT uiBufferSize, UINT& uiRecv, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);		// CCSocketBase 接收缓冲数据(接收全部数据)
	int CCSocketBaseRecvBuffer(SOCKET Socket, char* pRecvBuffer, UINT uiBufferSize, UINT uiRecvSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);	// CCSocketBase 接收缓冲数据(接收一定数据)

	USHORT CCSocketBaseGetConnectMaxCount() const;																									// CCSocketBase 获取最大连接数量
	int& CCSocketBaseGetConnectCount();																										// CCSocketBase 获取当前连接的数量
	map<int, HANDLE>& CCSocketBaseGetConnectMap();																									// CCSocketBase 获取当前连接的Map																									// CCSocketBase 获取当前连接状态Map

	void CCSocketBaseSetConnectMaxCount(USHORT sMaxCount);																							// CCSocketBase 设置最大连接数量
	void CCSocketBaseSetConnectCount(int nAcceptCount);																								// CCSocketBase 设置当前连接数量

// TCP客户端成员函数
public:
	bool CCSocketBaseConnect(const char* pcRemoteIP = NULL, USHORT sPort = 0, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);						// CCSocketBase 发送服务器连接请求
	bool CCSocketBaseReConnect();																													// CCSocketBase 尝试重新连接服务器
	void CCSocketBaseDisConnect();																													// CCSocketBase 断开与服务器的连接

	int CCSocketBaseSendOnce(char* pSendBuffer, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);														// CCSocketBase 发送缓冲数据(发送全部数据)
	int CCSocketBaseSendBuffer(char* pSendBuffer, UINT uiBufferSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);									// CCSocketBase 发送缓冲数据(发送一定数据)
	int CCSocketBaseRecvOnce(char* pRecvBuffer, UINT uiBufferSize, UINT& uiRecv, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);						// CCSocketBase 接收缓冲数据(接收全部数据)
	int CCSocketBaseRecvBuffer(char* pRecvBuffer, UINT uiBufferSize, UINT uiRecvSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);				// CCSocketBase 接收缓冲数据(接收一定数据)

// UDP成员函数
public:
	bool CCSocketBaseUDPBindOnPort(const char* pcRemoteIP, UINT uiPort);																							// CCSocketBase 绑定端口(UDP)

	int CCSocketBaseUDPSendBuffer(const char* pcIP, SHORT sPort, char* pBuffer, UINT uiBufferSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);					// CCSocketBase 发送数据缓冲(UDP)
	int CCSocketBaseUDPRecvBuffer(char* pBuffer, UINT uiBufferSize, UINT& uiRecv, char* pcIP, USHORT& uPort, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);			// CCSocketBase 接收数据缓冲(UDP)

// 其他函数
public:
	static bool ResolveAddressToIp(const char* pcAddress, char* pcIp);			// CCSocketBase 网址转换为IP地址
	static void GetLocalIPAddr();												// CCSocketBase 获取本地IP地址
	
	static const char* GetLocalIP();
	static USHORT GetLocalPort();
	static void SetLocalIP(const char* pLocalIP, int nSize);
	static void SetLocalPort(USHORT sLocalPort);

// 公用成员
private:
	SOCKET m_socket;				// CCSocketBase Socket套接字
	WSAEVENT m_SocketWriteEvent;	// CCSocketBase Socket发送事件
	WSAEVENT m_SocketReadEvent;		// CCSocketBase Socket获取事件

	char m_pcRemoteIP[SOB_IP_LENGTH];			// CCSocketBase 远程端IP地址
	USHORT m_sRemotePort;						// CCSocketBase 远程端端口号

	char m_pcHostIP[SOB_IP_LENGTH];				// CCSocketBase 本地IP地址
	USHORT m_sHostPort;							// CCSocketBase 本地端口号

	wchar_t m_pwcRemoteIP[SOB_IP_LENGTH];

public:
	int m_nLastWSAError;			// CCSocketBase WSA错误代码

// TCP服务端成员
private:
	map<int, HANDLE> m_mapAccept;	// CCSocketBase 服务端连接线程
	int m_nAcceptCount;				// CCSocketBase 服务端连接数量
	USHORT m_sMaxCount;				// CCSocketBase 服务端最大连接数

// TCP客户端成员
private:
	bool m_bIsConnected;			// CCSocketBase Socket连接状态

// UDP成员
private:


// 其他成员
private:
	static char m_pcLocalIP[SOB_IP_LENGTH];			// CCSocketBase 本机IP地址
	static USHORT m_sLocalPort;						// CCSocketBase 本机端口号

};


#endif // !__CSOCKETBASE_H__
