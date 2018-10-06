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

#define SOB_IP_LENGTH				128             //IP��ַ����

#define SOB_TCP_SEND_BUFFER			32*1024			//TCP���ͻ���32K
#define SOB_TCP_RECV_BUFFER			32*1024			//TCP���ջ���32K
#define SOB_UDP_RECV_BUFFER			32*1024			//UDP���ջ���32K

#define SOB_DEFAULT_TIMEOUT_SEC		5				//Ĭ�ϵĳ�ʱʱ��
#define SOB_DEFAULT_MAX_CLIENT		20				//Ĭ�Ϸ�������������

#define SOB_RET_OK					1				//����
#define SOB_RET_FAIL				0				//����
#define SOB_RET_TIMEOUT				-1				//��ʱ
#define SOB_RET_CLOSE				-2				//�Ͽ�

//Struct Definition
typedef struct
{
	SOCKET Socket;
	SOCKADDR_IN SocketAddr;
}S_CLIENTINFO, *LPS_CLIENTINFO;

//Callback Definition
typedef unsigned(__stdcall *HANDLE_ACCEPT_THREAD)(void*);		//������������̺߳���
typedef void(__stdcall *HANDLE_ACCEPT_CALLBACK)(SOCKADDR_IN* pRemoteAddr, SOCKET s, DWORD dwUser);		//������������̺߳���

//Class Definition
class CCSocketBase
{
public:
	CCSocketBase();			// CCSocketBase ���캯��
	~CCSocketBase();		// CCSocketBase ��������

public:
	static void CCSocketBaseLibInit();			// CCSocketBase ��ʼSocket����
	static void CCSocketBaseLibRelease();		// CCSocketBase ����Socket����

private:
	SOCKET CreateTCPSocket();					// CCSocketBase ����TCP�׽���
	SOCKET CreateUDPSocket();					// CCSocketBase ����UDP�׽���

// ���ó�Ա����
public:
	void CCSocketBaseSetRecvTimeOut(UINT uiMSec);			// CCSocketBase ���ý��ճ�ʱʱ��
	void CCSocketBaseSetSendTimeOut(UINT uiMSec);			// CCSocketBase ���÷��ͳ�ʱʱ��
	void CCSocketBaseSetRecvBufferSize(UINT uiByte);		// CCSocketBase ���ý������鳤��
	void CCSocketBaseSetSendBufferSize(UINT uiByte);		// CCSocketBase ���÷������鳤��

	SOCKET CCSocketBaseGetRawSocket() const;				// CCSocketBase ��ȡSocket���
	bool CCSocketBaseAttachRawSocket(SOCKET s, bool bIsConnected);	// CCSocketBase ��Socket�׽���
	void CCSocketBaseDettachRawSocket();					// CCSocketBase ����Socket�׽���

	const char* CCSocketBaseGetRemoteIP() const;			// CCSocketBase ��ȡԶ��IP��ַ(ASCII)
	const wchar_t* CCSocketBaseGetRemoteIPW() const;		// CCSocketBase ��ȡԶ��IP��ַ(Unicode)
	ULONG CCSocketBaseGetRemoteIPUL() const;				// CCSocketBase ��ȡԶ��IP��ַ(ULONG)
	USHORT CCSocketBaseGetRemotePort() const;				// CCSocketBase ��ȡԶ�̶˿ں�
	bool CCSocketBaseIsConnected() const;					// CCSocketBase ��ȡ����״̬(�ͻ���)
	void CCSocketBaseDestory();								// CCSocketBase ɾ��SocketBase��

// TCP����˳�Ա����
public:
	bool CCSocketBaseBindOnPort(USHORT uPort);	// CCSocketBase �󶨷���˶˿�
	bool CCSocketBaseListen();					// CCSocketBase ��������˶˿�
	bool CCSocketBaseAccept(HANDLE_ACCEPT_THREAD pThreadFunc, HANDLE_ACCEPT_CALLBACK pCallback, DWORD dwUser, BOOL* pExitFlag = NULL, USHORT nLoopTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);	// CCSocketBase ���տͻ�����������

	int CCSocketBaseSendOnce(SOCKET Socket, char* pSendBuffer, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);										// CCSocketBase ���ͻ�������(����ȫ������)
	int CCSocketBaseSendBuffer(SOCKET Socket, char* pSendBuffer, UINT uiBufferSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);					// CCSocketBase ���ͻ�������(����һ������)
	int CCSocketBaseRecvOnce(SOCKET Socket, char* pRecvBuffer, UINT uiBufferSize, UINT& uiRecv, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);		// CCSocketBase ���ջ�������(����ȫ������)
	int CCSocketBaseRecvBuffer(SOCKET Socket, char* pRecvBuffer, UINT uiBufferSize, UINT uiRecvSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);	// CCSocketBase ���ջ�������(����һ������)

	USHORT CCSocketBaseGetConnectMaxCount() const;																									// CCSocketBase ��ȡ�����������
	int& CCSocketBaseGetConnectCount();																										// CCSocketBase ��ȡ��ǰ���ӵ�����
	map<int, HANDLE>& CCSocketBaseGetConnectMap();																									// CCSocketBase ��ȡ��ǰ���ӵ�Map																									// CCSocketBase ��ȡ��ǰ����״̬Map

	void CCSocketBaseSetConnectMaxCount(USHORT sMaxCount);																							// CCSocketBase ���������������
	void CCSocketBaseSetConnectCount(int nAcceptCount);																								// CCSocketBase ���õ�ǰ��������

// TCP�ͻ��˳�Ա����
public:
	bool CCSocketBaseConnect(const char* pcRemoteIP = NULL, USHORT sPort = 0, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);						// CCSocketBase ���ͷ�������������
	bool CCSocketBaseReConnect();																													// CCSocketBase �����������ӷ�����
	void CCSocketBaseDisConnect();																													// CCSocketBase �Ͽ��������������

	int CCSocketBaseSendOnce(char* pSendBuffer, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);														// CCSocketBase ���ͻ�������(����ȫ������)
	int CCSocketBaseSendBuffer(char* pSendBuffer, UINT uiBufferSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);									// CCSocketBase ���ͻ�������(����һ������)
	int CCSocketBaseRecvOnce(char* pRecvBuffer, UINT uiBufferSize, UINT& uiRecv, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);						// CCSocketBase ���ջ�������(����ȫ������)
	int CCSocketBaseRecvBuffer(char* pRecvBuffer, UINT uiBufferSize, UINT uiRecvSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);				// CCSocketBase ���ջ�������(����һ������)

// UDP��Ա����
public:
	bool CCSocketBaseUDPBindOnPort(const char* pcRemoteIP, UINT uiPort);																							// CCSocketBase �󶨶˿�(UDP)

	int CCSocketBaseUDPSendBuffer(const char* pcIP, SHORT sPort, char* pBuffer, UINT uiBufferSize, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);					// CCSocketBase �������ݻ���(UDP)
	int CCSocketBaseUDPRecvBuffer(char* pBuffer, UINT uiBufferSize, UINT& uiRecv, char* pcIP, USHORT& uPort, USHORT nTimeOutSec = SOB_DEFAULT_TIMEOUT_SEC);			// CCSocketBase �������ݻ���(UDP)

// ��������
public:
	static bool ResolveAddressToIp(const char* pcAddress, char* pcIp);			// CCSocketBase ��ַת��ΪIP��ַ
	static void GetLocalIPAddr();												// CCSocketBase ��ȡ����IP��ַ
	
	static const char* GetLocalIP();
	static USHORT GetLocalPort();
	static void SetLocalIP(const char* pLocalIP, int nSize);
	static void SetLocalPort(USHORT sLocalPort);

// ���ó�Ա
private:
	SOCKET m_socket;				// CCSocketBase Socket�׽���
	WSAEVENT m_SocketWriteEvent;	// CCSocketBase Socket�����¼�
	WSAEVENT m_SocketReadEvent;		// CCSocketBase Socket��ȡ�¼�

	char m_pcRemoteIP[SOB_IP_LENGTH];			// CCSocketBase Զ�̶�IP��ַ
	USHORT m_sRemotePort;						// CCSocketBase Զ�̶˶˿ں�

	char m_pcHostIP[SOB_IP_LENGTH];				// CCSocketBase ����IP��ַ
	USHORT m_sHostPort;							// CCSocketBase ���ض˿ں�

	wchar_t m_pwcRemoteIP[SOB_IP_LENGTH];

public:
	int m_nLastWSAError;			// CCSocketBase WSA�������

// TCP����˳�Ա
private:
	map<int, HANDLE> m_mapAccept;	// CCSocketBase ����������߳�
	int m_nAcceptCount;				// CCSocketBase �������������
	USHORT m_sMaxCount;				// CCSocketBase ��������������

// TCP�ͻ��˳�Ա
private:
	bool m_bIsConnected;			// CCSocketBase Socket����״̬

// UDP��Ա
private:


// ������Ա
private:
	static char m_pcLocalIP[SOB_IP_LENGTH];			// CCSocketBase ����IP��ַ
	static USHORT m_sLocalPort;						// CCSocketBase �����˿ں�

};


#endif // !__CSOCKETBASE_H__
