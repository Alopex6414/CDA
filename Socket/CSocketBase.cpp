#include "CSocketBase.h"

#include <Windows.h>
#include <Ws2tcpip.h>
#include <process.h>

#pragma warning(disable:4996)

char CCSocketBase::m_pcLocalIP[SOB_IP_LENGTH] = { 0 };
USHORT CCSocketBase::m_sLocalPort = 0;

// CCSocketBase ���캯��
CCSocketBase::CCSocketBase()
{
	m_socket = NULL;
	m_SocketWriteEvent = NULL;
	m_SocketReadEvent = NULL;

	m_bIsConnected = false;
	m_sMaxCount = SOB_DEFAULT_MAX_CLIENT;

	memset(m_pcRemoteIP, 0, SOB_IP_LENGTH);
	m_sRemotePort = 0;

	memset(m_pcHostIP, 0, SOB_IP_LENGTH);
	m_sHostPort = 0;
}

// CCSocketBase ��������
CCSocketBase::~CCSocketBase()
{
	if (m_socket)
	{
		closesocket(m_socket);
		m_socket = NULL;
	}

	if (m_SocketWriteEvent)
	{
		WSACloseEvent(m_SocketWriteEvent);
		m_SocketWriteEvent = NULL;
	}

	if (m_SocketReadEvent)
	{
		WSACloseEvent(m_SocketReadEvent);
		m_SocketReadEvent = NULL;
	}

}

// CCSocketBase ��ʼ��Socket
void CCSocketBase::CCSocketBaseLibInit()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int nErr;

	wVersionRequested = MAKEWORD(2, 2);

	nErr = WSAStartup(wVersionRequested, &wsaData);	// ��ʼ��SOCKET����
	if (nErr != 0)
	{
		return;
	}
}

// CCSocketBase �ͷ�Socket
void CCSocketBase::CCSocketBaseLibRelease()
{
	WSACleanup();	// ����SOCKET����
}

// CCSocketBase ����TCP�׽���
SOCKET CCSocketBase::CreateTCPSocket()
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	// TCP Socket

	// �������������������Ϊ��
	if (s == INVALID_SOCKET)
	{
		s = NULL;
		m_nLastWSAError = WSAGetLastError();
	}

	// �����첽�¼�
	if (m_SocketWriteEvent == NULL)
	{
		m_SocketWriteEvent = WSACreateEvent();
	}

	if (m_SocketReadEvent == NULL)
	{
		m_SocketReadEvent = WSACreateEvent();
	}

	// ����SOCKET����
	const char chOpt = 1;
	int nRet = setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));

	return s;
}

// CCSocketBase ����UDP�׽���
SOCKET CCSocketBase::CreateUDPSocket()
{
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	// UDP Socket

	// �������������������Ϊ��
	if (s == INVALID_SOCKET)
	{
		s = NULL;
		m_nLastWSAError = WSAGetLastError();
	}

	// �����첽�¼�
	if (m_SocketWriteEvent == NULL)
	{
		m_SocketWriteEvent = WSACreateEvent();
	}

	if (m_SocketReadEvent == NULL)
	{
		m_SocketReadEvent = WSACreateEvent();
	}

	return s;
}

// CCSocketBase �󶨷���˶˿�
bool CCSocketBase::CCSocketBaseBindOnPort(USHORT uPort)
{
	// ���socket��Ч���½���Ϊ�˿����ظ�����
	if (m_socket == NULL)
	{
		m_socket = CreateTCPSocket();
	}

	// �������ؼ���
	SOCKADDR_IN addrLocal;
	memset(&addrLocal, 0, sizeof(addrLocal));

	addrLocal.sin_family = AF_INET;
	addrLocal.sin_addr.s_addr = htonl(INADDR_ANY);
	addrLocal.sin_port = htons(uPort);

	m_sHostPort = uPort;

	// ��
	int nRet = bind(m_socket, (PSOCKADDR)&addrLocal, sizeof(addrLocal));

	// Ψһ��ԭ���Ƕ˿ڱ�ռ��
	if (nRet == SOCKET_ERROR)
	{
		m_nLastWSAError = WSAGetLastError();
		return false;
	}

	return true;
}

// CCSocketBase ��������˶˿�
bool CCSocketBase::CCSocketBaseListen()
{
	// ����
	int nRet = listen(m_socket, 5);

	// Ψһ��ԭ���Ƕ˿ڱ�ռ��
	if (nRet == SOCKET_ERROR)
	{
		m_nLastWSAError = WSAGetLastError();
		return false;
	}

	return true;
}

// CCSocketBase ���տͻ�����������
bool CCSocketBase::CCSocketBaseAccept(HANDLE_ACCEPT_THREAD pThreadFunc, HANDLE_ACCEPT_CALLBACK pCallback, DWORD dwUser, BOOL * pExitFlag, USHORT nLoopTimeOutSec)
{
	// ע�������¼�
	WSAResetEvent(m_SocketReadEvent);           // ���֮ǰ��δ������¼�
	WSAEventSelect(m_socket, m_SocketReadEvent, FD_ACCEPT | FD_CLOSE);

	// �ȴ�����
	while ((pExitFlag == NULL ? TRUE : !(*pExitFlag)))
	{
		DWORD dwRet = WSAWaitForMultipleEvents(1, &m_SocketReadEvent, FALSE, nLoopTimeOutSec * 1000, FALSE);

		if (dwRet == WSA_WAIT_EVENT_0)
		{
			// ��������¼�����
			WSANETWORKEVENTS wsaEvents;
			memset(&wsaEvents, 0, sizeof(wsaEvents));

			WSAResetEvent(m_SocketReadEvent);
			WSAEnumNetworkEvents(m_socket, m_SocketReadEvent, &wsaEvents);

			if ((wsaEvents.lNetworkEvents & FD_ACCEPT) &&
				(wsaEvents.iErrorCode[FD_ACCEPT_BIT] == 0))
			{
				// ��¼Զ�̵�ַ
				SOCKADDR_IN addrRemote;
				memset(&addrRemote, 0, sizeof(addrRemote));
				int nAddrSize = sizeof(addrRemote);

				SOCKET sockRemote = accept(m_socket, (PSOCKADDR)&addrRemote, &nAddrSize);

				// ��Ч����
				if (sockRemote == INVALID_SOCKET)
				{
					m_nLastWSAError = WSAGetLastError();

					// ���̳߳�������
					continue;
				}

				// ��������̺߳����������߳�
				if (pThreadFunc)
				{
					// �����´����߳�
					HANDLE hThread;
					unsigned unThreadID;
					S_CLIENTINFO sClientInfo = { 0 };

					memset(&sClientInfo.SocketAddr, 0, sizeof(sClientInfo.SocketAddr));
					sClientInfo.Socket = sockRemote;
					sClientInfo.SocketAddr = addrRemote;

					hThread = (HANDLE)_beginthreadex(NULL, 0, pThreadFunc, (void*)(&sClientInfo), 0, &unThreadID);

					// �Ѿ�����Ҫ��HANDLE
					CloseHandle(hThread);
				}
				else if (pCallback)		// �������ص�����лص�
				{
					pCallback(&addrRemote, sockRemote, dwUser);
				}
			}
		}
		else
		{
			//�ȴ���ʱ�����¿�ʼ
			continue;
		}
	}

	return true;
}

// CCSocketBase ���ͻ�������(����Ӧ�ñȴ�������Ҫ��һ��Ű�ȫ)<����ȫ������>
int CCSocketBase::CCSocketBaseSendOnce(SOCKET Socket, char * pSendBuffer, USHORT nTimeOutSec)
{
	bool bIsTimeOut = false;

	// ����ǰע���¼�
	WSAResetEvent(m_SocketWriteEvent);
	WSAEventSelect(Socket, m_SocketWriteEvent, FD_WRITE | FD_CLOSE);

	// ���Է���
	int nRet = send(Socket, pSendBuffer, (int)strlen(pSendBuffer), NULL);

	if (nRet == SOCKET_ERROR)
	{
		m_nLastWSAError = WSAGetLastError();

		// ��������
		if (m_nLastWSAError == WSAEWOULDBLOCK)
		{
			DWORD dwRet = WSAWaitForMultipleEvents(1, &m_SocketWriteEvent, FALSE, nTimeOutSec * 1000, FALSE);

			// ��������¼�����
			WSANETWORKEVENTS wsaEvents;
			memset(&wsaEvents, 0, sizeof(wsaEvents));

			if (dwRet == WSA_WAIT_EVENT_0)
			{
				WSAResetEvent(m_SocketWriteEvent);
				WSAEnumNetworkEvents(Socket, m_SocketWriteEvent, &wsaEvents);

				// ������Ϳ��Խ��в���û�д�����
				if ((wsaEvents.lNetworkEvents & FD_WRITE) &&
					(wsaEvents.iErrorCode[FD_WRITE_BIT] == 0))
				{
					// �ٴη����ı�
					nRet = (int)send(Socket, pSendBuffer, (int)strlen(pSendBuffer), NULL);

					if (nRet > 0)
					{
						// ��������ֽڴ���0���������ͳɹ�
						return SOB_RET_OK;
					}
				}
			}
			else
			{
				// ��ʱ
				bIsTimeOut = true;
			}
		}
		else
		{

		}
	}
	else
	{
		// ��һ�α㷢�ͳɹ�
		return SOB_RET_OK;
	}

	// �����ʱ
	if (bIsTimeOut)
	{
		return SOB_RET_TIMEOUT;
	}

	// �����������ʧ��
	m_nLastWSAError = WSAGetLastError();

	return SOB_RET_FAIL;
}

// CCSocketBase ���ͻ�������(����Ӧ�ñȴ�������Ҫ��һ��Ű�ȫ)<����һ������>
int CCSocketBase::CCSocketBaseSendBuffer(SOCKET Socket, char * pSendBuffer, UINT uiBufferSize, USHORT nTimeOutSec)
{
	bool bIsTimeOut = false;

	// ����ǰע���¼�
	WSAResetEvent(m_SocketWriteEvent);
	WSAEventSelect(Socket, m_SocketWriteEvent, FD_WRITE | FD_CLOSE);

	// ����������
	int nSent = 0;

	// �ܷ��ʹ�����Ϊ���ʹ�����һ���޶�
	int nSendTimes = 0;
	int nSendLimitTimes = (int)((float)uiBufferSize / 500 + 1.5);		// �ٶ���ǰÿ�η��Ϳ϶�������500�ֽ�
	UINT uiLeftBuffer = uiBufferSize;									// δ������Ļ����С

	// �����α�
	char* pcSentPos = pSendBuffer;

	// ֱ�����еĻ��嶼�������
	while (nSent < (int)uiBufferSize)
	{
		// ��鷢�ʹ����Ƿ���
		if (nSendTimes > nSendLimitTimes)
		{
			break;
		}

		int nRet = send(Socket, pcSentPos, uiLeftBuffer, NULL);

		if (nRet == SOCKET_ERROR)
		{
			m_nLastWSAError = WSAGetLastError();

			// ��������
			if (m_nLastWSAError == WSAEWOULDBLOCK)
			{
				DWORD dwRet = WSAWaitForMultipleEvents(1, &m_SocketWriteEvent, FALSE, nTimeOutSec * 1000, FALSE);

				// ��������¼�����
				WSANETWORKEVENTS wsaEvents;
				memset(&wsaEvents, 0, sizeof(wsaEvents));

				if (dwRet == WSA_WAIT_EVENT_0)
				{
					WSAResetEvent(m_SocketWriteEvent);
					WSAEnumNetworkEvents(Socket, m_SocketWriteEvent, &wsaEvents);

					// ������Ϳ��Խ��в���û�д�����
					if ((wsaEvents.lNetworkEvents & FD_WRITE) &&
						(wsaEvents.iErrorCode[FD_WRITE_BIT] == 0))
					{
						// �ٴη����ı�
						nRet = send(Socket, pcSentPos, uiLeftBuffer, NULL);

						if (nRet > 0)
						{
							// ��������ֽڴ���0���������ͳɹ�
							nSendTimes++;

							nSent += nRet;
							uiLeftBuffer -= nRet;
							pcSentPos += nRet;
						}
						else
						{
							m_nLastWSAError = WSAGetLastError();

							// ������յ��¼�����ʱҲ�����ˣ���ȴ�һ�����ں�����
							if (m_nLastWSAError == WSAEWOULDBLOCK)
							{
								Sleep(1);
							}
							else
							{
								// ������������ֱ���˳�
								break;
							}
						}
					}
				}
				else
				{
					// ��ʱ
					bIsTimeOut = true;
					break;
				}
			}
			else
			{
				// ��������֮��Ĵ���ֱ���˳�
				break;
			}
		}
		else
		{
			// ���ͳɹ����ۼӷ������������α�
			nSendTimes++;

			nSent += nRet;
			uiLeftBuffer -= nRet;
			pcSentPos += nRet;
		}
	}

	// ����������
	if (nSent == uiBufferSize)
	{
		return SOB_RET_OK;
	}

	// �����ʱ
	if (bIsTimeOut)
	{
		return SOB_RET_TIMEOUT;
	}

	// û�ܳɹ�����
	m_nLastWSAError = WSAGetLastError();

	return SOB_RET_FAIL;
}

// CCSocketBase ���ջ�������(����Ӧ�ñȴ�������Ҫ��һ��Ű�ȫ)<����ȫ������>
int CCSocketBase::CCSocketBaseRecvOnce(SOCKET Socket, char * pRecvBuffer, UINT uiBufferSize, UINT & uiRecv, USHORT nTimeOutSec)
{
	bool bIsTimeOut = false;

	// ����ǰע���¼�
	WSAResetEvent(m_SocketReadEvent);
	WSAEventSelect(Socket, m_SocketReadEvent, FD_READ | FD_CLOSE);

	// ���Խ���
	int nRet = recv(Socket, pRecvBuffer, uiBufferSize, NULL);

	if (nRet == SOCKET_ERROR)
	{
		m_nLastWSAError = WSAGetLastError();

		// ��������
		if (m_nLastWSAError == WSAEWOULDBLOCK)
		{
			DWORD dwRet = WSAWaitForMultipleEvents(1, &m_SocketReadEvent, FALSE, nTimeOutSec * 1000, FALSE);

			// ��������¼�����
			WSANETWORKEVENTS wsaEvents;
			memset(&wsaEvents, 0, sizeof(wsaEvents));

			if (dwRet == WSA_WAIT_EVENT_0)
			{
				WSAResetEvent(m_SocketReadEvent);
				int nEnum = WSAEnumNetworkEvents(Socket, m_SocketReadEvent, &wsaEvents);

				// ������ܿ��Խ��в���û�д�����
				if ((wsaEvents.lNetworkEvents & FD_READ) &&
					(wsaEvents.iErrorCode[FD_READ_BIT] == 0))
				{
					// �ٴν����ı�
					nRet = recv(Socket, pRecvBuffer, uiBufferSize, NULL);

					if (nRet > 0)
					{
						// ��������ֽڴ���0���������ͳɹ�
						uiRecv = nRet;
						return SOB_RET_OK;
					}
				}
			}
			else
			{
				bIsTimeOut = true;
			}
		}
		else
		{
		}
	}
	else
	{
		// ��һ�α���ճɹ�
		uiRecv = nRet;
		return SOB_RET_OK;
	}

	// �����ʱ
	if (bIsTimeOut)
	{
		return SOB_RET_TIMEOUT;
	}

	// �����������ʧ��
	m_nLastWSAError = WSAGetLastError();

	return SOB_RET_FAIL;
}

// CCSocketBase ���ջ�������(����Ӧ�ñȴ�������Ҫ��һ��Ű�ȫ)<����һ������>
int CCSocketBase::CCSocketBaseRecvBuffer(SOCKET Socket, char * pRecvBuffer, UINT uiBufferSize, UINT uiRecvSize, USHORT nTimeOutSec)
{
	bool bIsTimeOut = false;

	// ����ǰע���¼�
	WSAResetEvent(m_SocketReadEvent);
	WSAEventSelect(Socket, m_SocketReadEvent, FD_READ | FD_CLOSE);

	// ����������
	int nReceived = 0;

	// �ܽ��մ�����Ϊ���մ�����һ���޶�
	int nRecvTimes = 0;
	int nRecvLimitTimes = (int)((float)uiRecvSize / 500 + 1.5);      // �ٶ���ǰÿ�ν��տ϶�������500�ֽ�

	// �����α�
	char* pcRecvPos = pRecvBuffer;

	// ֱ�����յ��㹻ָ�����Ļ���
	while (nReceived < (int)uiRecvSize)
	{
		// �����մ����Ƿ���
		if (nRecvTimes > nRecvLimitTimes)
		{
			break;
		}

		int nRet = recv(Socket, pcRecvPos, uiBufferSize, NULL);

		if (nRet == SOCKET_ERROR)
		{
			m_nLastWSAError = WSAGetLastError();

			// ��������
			if (m_nLastWSAError == WSAEWOULDBLOCK)
			{
				DWORD dwRet = WSAWaitForMultipleEvents(1, &m_SocketReadEvent, FALSE, nTimeOutSec * 1000, FALSE);

				// ��������¼�����
				WSANETWORKEVENTS wsaEvents;
				memset(&wsaEvents, 0, sizeof(wsaEvents));

				if (dwRet == WSA_WAIT_EVENT_0)
				{
					WSAResetEvent(m_SocketReadEvent);
					WSAEnumNetworkEvents(Socket, m_SocketReadEvent, &wsaEvents);

					// ������տ��Խ��в���û�д�����
					if ((wsaEvents.lNetworkEvents & FD_READ) &&
						(wsaEvents.iErrorCode[FD_READ_BIT] == 0))
					{
						// �ٴν���
						nRet = recv(Socket, pcRecvPos, uiBufferSize, NULL);

						if (nRet > 0)
						{
							// ��������ֽڴ���0���������ͳɹ�
							nRecvTimes++;

							nReceived += nRet;
							uiBufferSize -= nRet;
							pcRecvPos += nRet;
						}
						else
						{
							m_nLastWSAError = WSAGetLastError();

							// ������յ��¼�����ʱҲ�����ˣ���ȴ�һ�����ں�����
							if (m_nLastWSAError == WSAEWOULDBLOCK)
							{
								Sleep(1);
							}
							else
							{
								// ������������ֱ���˳�
								break;
							}
						}
					}
				}
				else
				{
					// ��ʱ
					bIsTimeOut = true;
					break;
				}
			}
			else
			{
				// ��������֮��Ĵ���ֱ���˳�
				break;
			}
		}
		else if (nRet == 0)		// �Է������ر�����
		{
			break;
		}
		else
		{
			// ���ճɹ����ۼӽ������������α�
			nRecvTimes++;

			nReceived += nRet;
			uiBufferSize -= nRet;
			pcRecvPos += nRet;
		}
	}

	// ����������
	if (nReceived == uiRecvSize)
	{
		return SOB_RET_OK;
	}

	// �����ʱ
	if (bIsTimeOut)
	{
		return SOB_RET_TIMEOUT;
	}

	// û�ܳɹ�����
	m_nLastWSAError = WSAGetLastError();

	return SOB_RET_FAIL;
}

// CCSocketBase ��ȡ����IP��ַ
void CCSocketBase::GetLocalIPAddr()
{
	char chHostName[MAX_PATH] = { 0 };

	if (gethostname(chHostName, sizeof(chHostName)) == 0)
	{
		hostent* pHostent = gethostbyname(chHostName);
		hostent& he = *pHostent;
		sockaddr_in sa;
		for (int nAdapter = 0; he.h_addr_list[nAdapter]; nAdapter++)
		{
			memset(m_pcLocalIP, 0, SOB_IP_LENGTH);
			memcpy(&sa.sin_addr.s_addr, he.h_addr_list[nAdapter], he.h_length);
			strcpy(m_pcLocalIP, inet_ntoa(*(struct in_addr *)he.h_addr_list[nAdapter]));
		}
	}

}

// CCSocketBase ���ʱ���IP��ַ(��̬)
const char * CCSocketBase::GetLocalIP()
{
	return m_pcLocalIP;
}

// CCSocketBase ���ʱ����˿ں�(��̬)
USHORT CCSocketBase::GetLocalPort()
{
	return m_sLocalPort;
}

// CCSocketBase ���ñ���IP��ַ(��̬)
void CCSocketBase::SetLocalIP(const char * pLocalIP, int nSize)
{
	memset(m_pcLocalIP, 0, SOB_IP_LENGTH);
	memcpy_s(m_pcLocalIP, SOB_IP_LENGTH, pLocalIP, nSize);
}

// CCSocketBase ���ñ����˿ں�(��̬)
void CCSocketBase::SetLocalPort(USHORT sLocalPort)
{
	m_sLocalPort = sLocalPort;
}
