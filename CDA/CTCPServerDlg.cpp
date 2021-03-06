// CTCPServerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CDA.h"
#include "CTCPServerDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

//Macro Definition
#define WM_USER_MSG_ONRECEIVEBUFFER		(WM_USER + 1)
#define WM_USER_MSG_ONCLIENTCONNECT		(WM_USER + 2)
#define WM_USER_MSG_ONCLIENTDISCONNECT	(WM_USER + 3)
#define WM_USER_MSG_ONUPDATESENDOBJECT	(WM_USER + 4)
#define WM_USER_MSG_ONUPDATECLIENT		(WM_USER + 5)

// Variable Definition
CTCPServerDlg* g_pTCPServerDlg = nullptr;

// CTCPServerDlg 对话框

IMPLEMENT_DYNAMIC(CTCPServerDlg, CDialogEx)

CTCPServerDlg::CTCPServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TCPSERVER, pParent)
{
	ConstructionExtra();
}

CTCPServerDlg::~CTCPServerDlg()
{
	m_Brush.DeleteObject();
}

// CTCPServerDlg 构造函数
void CTCPServerDlg::ConstructionExtra()
{
	m_pSocket = NULL;

	m_hAccept = NULL;
	m_bListen = false;
	m_bListenExit = FALSE;
	m_bExit = false;
	m_bShareInfo = false;
	g_pTCPServerDlg = this;

	m_nClientNo = 0;
	m_mapClientInfo.clear();
	m_uSendCount = 0;
	m_uRecvCount = 0;

	memset(&m_sClientInfo, 0, sizeof(m_sClientInfo));
	memset(m_chSendBuf, 0, sizeof(m_chSendBuf));
	memset(m_chRecvBuf, 0, sizeof(m_chRecvBuf));

	m_Brush.CreateSolidBrush(RGB(255, 255, 255));
}

// CTCPServerDlg 初始化窗口形状
void CTCPServerDlg::InitWindowSharp()
{
	GetClientRect(&m_cWindowRect);
}

// CTCPServerDlg 初始化控件
void CTCPServerDlg::InitControls()
{
	BYTE byHostIP[4] = { 0 };

	// 设置本机IP地址
	CCSocketBase::SetLocalIP(NULL, NULL);	// 清空主机IP地址
	CCSocketBase::SetLocalPort(0);	// 清空主机端口
	CCSocketBase::GetLocalIPAddr();	// 获取本机IP地址
	BreakIpAddress(CCSocketBase::GetLocalIP(), byHostIP);	// 拆分本机IP地址
	m_cHostIPAdc.SetAddress(byHostIP[0], byHostIP[1], byHostIP[2], byHostIP[3]);	// 设置本机IP地址

	// 设置本机端口号
	m_cHostPortEdt.SetWindowTextW(_T("6000"));	// 默认本机端口号

	// 设置最大连接数
	m_cServerLinkMaxEdt.SetWindowTextW(_T("10"));	// 默认最大连接数10

	// 设置连接方式
	m_cObjectCbx.ResetContent();
	m_cObjectCbx.InsertString(0, _T("广播"));
	m_cObjectCbx.SetCurSel(0);

}

// CTCPServerDlg 窗口重绘
void CTCPServerDlg::RePaintWindow(CDC & dc)
{
	CDC MemDC;
	CBitmap MemBitmap;
	CRect MemRect;

	GetClientRect(&MemRect);

	MemDC.CreateCompatibleDC(&dc);
	MemBitmap.CreateCompatibleBitmap(&dc, MemRect.Width(), MemRect.Height());

	MemDC.SelectObject(&MemBitmap);
	MemDC.FillSolidRect(&MemRect, RGB(255, 255, 255));

	dc.BitBlt(m_cWindowRect.left, m_cWindowRect.top, m_cWindowRect.Width(), m_cWindowRect.Height(), &MemDC, MemRect.left, MemRect.top, SRCCOPY);

	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}

// CTCPServerDlg 初始化Socket
void CTCPServerDlg::InitSocket()
{
	CCSocketBase::CCSocketBaseLibInit();	// 初始化Socket库
}

// CTCPServerDlg 释放Socket
void CTCPServerDlg::ReleaseSocket()
{
	CCSocketBase::CCSocketBaseLibRelease();	// 释放Socket库
}

// CTCPServerDlg 建立监听接收线程
void CTCPServerDlg::StartAcceptThread()
{
	unsigned int uThreadID = 0;
	m_hAccept = (HANDLE)::_beginthreadex(NULL, 0, (_beginthreadex_proc_type)OnWaitAcceptThread, this, 0, &uThreadID);
}

// CTCPServerDlg 监听接收线程(监听客户端)<等待客户端接入>
unsigned int CALLBACK CTCPServerDlg::OnWaitAcceptThread(LPVOID lpParameters)
{
	g_pTCPServerDlg->m_pSocket->CCSocketBaseAccept(OnHandleAcceptThread, NULL, (DWORD)g_pTCPServerDlg, &(g_pTCPServerDlg->m_bListenExit));
	return 0;
}

// CTCPServerDlg 客户端连接线程(并发)
unsigned int CALLBACK CTCPServerDlg::OnHandleAcceptThread(LPVOID lpParameters)
{
	S_CLIENTINFO* pSendInfo = reinterpret_cast<S_CLIENTINFO*>(lpParameters);
	S_CLIENTINFO* pClientInfo = new S_CLIENTINFO();

	memset(pClientInfo, 0, sizeof(S_CLIENTINFO));
	memcpy_s(pClientInfo, sizeof(S_CLIENTINFO), pSendInfo, sizeof(S_CLIENTINFO));
	g_pTCPServerDlg->m_mapClientInfo.insert(pair<int, S_CLIENTINFO>(g_pTCPServerDlg->m_nClientNo++, *pClientInfo));

	// 发送上线通知
	::PostMessageA(g_pTCPServerDlg->GetSafeHwnd(), WM_USER_MSG_ONCLIENTCONNECT, (WPARAM)0, (LPARAM)((LPVOID)(pClientInfo)));

	// 发送更新发送对象信息
	::PostMessageA(g_pTCPServerDlg->GetSafeHwnd(), WM_USER_MSG_ONUPDATESENDOBJECT, (WPARAM)0, (LPARAM)0);

	while (true)
	{
		char chRecvBuf[SOB_TCP_RECV_BUFFER] = { 0 };
		UINT uRecvCount = 0;
		int nRet = 0;

		g_pTCPServerDlg->m_csThreadSafe.Enter();
		if (g_pTCPServerDlg->m_bExit)
		{
			g_pTCPServerDlg->m_csThreadSafe.Leave();
			break;
		}
		g_pTCPServerDlg->m_csThreadSafe.Leave();

		while (true)
		{
			g_pTCPServerDlg->m_csThreadSafe.Enter();
			if (!g_pTCPServerDlg->m_bShareInfo)
			{
				g_pTCPServerDlg->m_bShareInfo = true;
				g_pTCPServerDlg->m_csThreadSafe.Leave();
				break;
			}
			g_pTCPServerDlg->m_csThreadSafe.Leave();
			Sleep(10);
		}

		nRet = g_pTCPServerDlg->m_pSocket->CCSocketBaseRecvOnce(pClientInfo->Socket, chRecvBuf, SOB_TCP_RECV_BUFFER, uRecvCount);

		if (nRet == SOB_RET_OK)
		{
			if (strcmp("", chRecvBuf))
			{
				g_pTCPServerDlg->m_uRecvCount = uRecvCount;
				memset(g_pTCPServerDlg->m_chRecvBuf, 0, sizeof(g_pTCPServerDlg->m_chRecvBuf));
				memcpy_s(g_pTCPServerDlg->m_chRecvBuf, sizeof(g_pTCPServerDlg->m_chRecvBuf), chRecvBuf, uRecvCount);
				::PostMessageA(g_pTCPServerDlg->GetSafeHwnd(), WM_USER_MSG_ONRECEIVEBUFFER, (WPARAM)((LPVOID)(g_pTCPServerDlg->m_chRecvBuf)), (LPARAM)((LPVOID)(pClientInfo)));
			}
			else
			{
				g_pTCPServerDlg->m_bShareInfo = false;
			}

		}
		else if (nRet == SOB_RET_CLOSE)
		{
			g_pTCPServerDlg->m_bShareInfo = false;

			// 关闭Socket通信
			closesocket(pClientInfo->Socket);

			// 发送客户端下线消息通知
			memset(&g_pTCPServerDlg->m_sClientInfo, 0, sizeof(g_pTCPServerDlg->m_sClientInfo));
			memcpy_s(&g_pTCPServerDlg->m_sClientInfo, sizeof(g_pTCPServerDlg->m_sClientInfo), pClientInfo, sizeof(S_CLIENTINFO));
			::PostMessageA(g_pTCPServerDlg->GetSafeHwnd(), WM_USER_MSG_ONCLIENTDISCONNECT, (WPARAM)0, (LPARAM)((LPVOID)(&g_pTCPServerDlg->m_sClientInfo)));

			// 发送更新客户端连接消息
			::PostMessageA(g_pTCPServerDlg->GetSafeHwnd(), WM_USER_MSG_ONUPDATECLIENT, (WPARAM)0, (LPARAM)((LPVOID)(&g_pTCPServerDlg->m_sClientInfo)));

			break;
		}
		else
		{
			g_pTCPServerDlg->m_bShareInfo = false;
		}

		Sleep(10);
	}

	delete pClientInfo;
	pClientInfo = NULL;

	return 0;
}

// CTCPServerDlg 拆分IP地址
void CTCPServerDlg::BreakIpAddress(const char * pArr, BYTE byArr[4])
{
	char* p = NULL;
	char* pNext = NULL;
	char* pStr = NULL;
	int nSize = 0;
	vector<string> vecStr;

	nSize = strlen(pArr) + 1;
	pStr = new char[nSize];
	memset(pStr, 0, nSize);
	strcpy_s(pStr, nSize, pArr);

	vecStr.clear();

	p = strtok_s(pStr, ".", &pNext);
	while (p != NULL)
	{
		vecStr.push_back(p);
		p = strtok_s(NULL, ".", &pNext);
	}

	for (auto i = 0; i < 4; ++i)
	{
		byArr[i] = atoi(vecStr[i].c_str());
	}

	delete[] pStr;
	pStr = NULL;
}

// CTCPServerDlg 拆分空格
void CTCPServerDlg::BreakSpace(const unsigned char * pStr, vector<string>& vecStr)
{
	unsigned char* pString = const_cast<unsigned char*>(pStr);
	char* pNewString = nullptr;
	int nSize = 0;
	char* pTemp = nullptr;
	char* pArr = nullptr;

	nSize = strlen((char*)pString) + 1;
	pNewString = new char[nSize];
	memset(pNewString, 0, nSize);
	memcpy_s(pNewString, nSize, pString, strlen((char*)pString));

	vecStr.clear();

	pTemp = strtok_s(pNewString, " ", &pArr);
	while (pTemp)
	{
		vecStr.push_back(pTemp);
		pTemp = strtok_s(NULL, " ", &pArr);
	}

	delete[] pNewString;
	pNewString = nullptr;
}

// CTCPServerDlg 16进制转换
void CTCPServerDlg::Convert2Hex(vector<string>& vecStr, unsigned char * pStr, int nSize)
{
	char* pArr = nullptr;
	int nDelta = 0;

	pArr = new char[nSize];
	memset(pArr, 0, nSize);

	for (auto iter = vecStr.begin(); iter != vecStr.end(); ++iter)
	{
		char* pTemp = const_cast<char*>((*iter).c_str());
		for (size_t i = 0; (i <= (*iter).size()) && ((i + 1) <= (*iter).size()); i += 2, pTemp += 2)
		{
			if (((*pTemp >= '0' && *pTemp <= '9') || (*pTemp >= 'A' && *pTemp <= 'F') || (*pTemp >= 'a' && *pTemp <= 'f'))
				&& ((*(pTemp + 1) >= '0' && *(pTemp + 1) <= '9') || (*(pTemp + 1) >= 'A' && *(pTemp + 1) <= 'F') || (*(pTemp + 1) >= 'a' && *(pTemp + 1) <= 'f')))
			{
				char ch = 0;
				char cl = 0;

				if (*pTemp >= '0' && *pTemp <= '9')
				{
					ch = *pTemp - 48;
				}
				else if (*pTemp >= 'A' && *pTemp <= 'F')
				{
					ch = *pTemp - 55;
				}
				else
				{
					ch = *pTemp - 87;
				}

				if (*(pTemp + 1) >= '0' && *(pTemp + 1) <= '9')
				{
					cl = *(pTemp + 1) - 48;
				}
				else if (*(pTemp + 1) >= 'A' && *(pTemp + 1) <= 'F')
				{
					cl = *(pTemp + 1) - 55;
				}
				else
				{
					cl = *(pTemp + 1) - 87;
				}

				*(pArr + nDelta) = ch * 16 + cl;
				nDelta++;
				if (nDelta >= nSize)
				{
					break;
				}

			}

		}

	}

	m_uSendCount = nDelta;
	memcpy_s(pStr, nSize, pArr, nSize);

	delete[] pArr;
	pArr = nullptr;
}

// CTCPServerDlg 接收到Socket消息
LRESULT CTCPServerDlg::OnRecvSocketBufferMsg(WPARAM wParam, LPARAM lParam)
{
	CThreadSafe ThreadSafe(m_csThreadSafe.GetCriticalSection());
	S_CLIENTINFO* pClientInfo = reinterpret_cast<S_CLIENTINFO*>((LPVOID)lParam);

	char chClientInfo[MAX_PATH] = { 0 };
	int nLen = 0;

	SYSTEMTIME syTime = { 0 };

	USES_CONVERSION;

	GetLocalTime(&syTime);
	sprintf_s(chClientInfo, "[%d.%d.%d.%d:%d] %02d:%02d:%02d", pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b1, pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b2, pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b3, pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b4, pClientInfo->SocketAddr.sin_port,
		syTime.wHour, syTime.wMinute, syTime.wSecond);

	// 16进制显示数据
	if (TRUE == m_cRecvHexCbx.GetCheck())
	{
		CString csRecvBuff = _T("");
		for (int i = 0; i < m_uRecvCount; ++i)
		{
			CString csTemp = _T("");
			csTemp.Format(_T("%02X "), m_chRecvBuf[i]);
			csRecvBuff += csTemp;
		}

		memset(m_chRecvBuf, 0, sizeof(m_chRecvBuf));
		strcpy_s((char*)m_chRecvBuf, sizeof(m_chRecvBuf), T2A(csRecvBuff));
	}

	// 接收区文本显示
	nLen = m_cRecvEdt.GetWindowTextLengthW();
	if (nLen >= 30000)
	{
		m_cRecvEdt.SetWindowTextW(_T(""));
		nLen = -1;
	}

	m_cRecvEdt.SetSel(nLen, nLen);
	m_cRecvEdt.ReplaceSel(A2T(chClientInfo));
	m_cRecvEdt.ReplaceSel(A2T("\r\n"));
	m_cRecvEdt.ReplaceSel(A2T((char*)m_chRecvBuf));
	m_cRecvEdt.ReplaceSel(A2T("\r\n"));

	m_bShareInfo = false;

	return 0;
}

// CTCPServerDlg 客户端上线消息
LRESULT CTCPServerDlg::OnClientConnectServerMsg(WPARAM wParam, LPARAM lParam)
{
	S_CLIENTINFO* pClientInfo = reinterpret_cast<S_CLIENTINFO*>((LPVOID)lParam);

	char chClientInfo[MAX_PATH] = { 0 };
	int nLen = 0;
	SYSTEMTIME syTime = { 0 };

	USES_CONVERSION;

	GetLocalTime(&syTime);
	sprintf_s(chClientInfo, "上线通知 [%d.%d.%d.%d:%d] %02d:%02d:%02d", pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b1, pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b2, pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b3, pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b4, pClientInfo->SocketAddr.sin_port,
		syTime.wHour, syTime.wMinute, syTime.wSecond);

	// 接收区文本显示
	nLen = m_cRecvEdt.GetWindowTextLengthW();
	if (nLen >= 30000)
	{
		m_cRecvEdt.SetWindowTextW(_T(""));
		nLen = -1;
	}

	m_cRecvEdt.SetSel(nLen, nLen);
	m_cRecvEdt.ReplaceSel(A2T(chClientInfo));
	m_cRecvEdt.ReplaceSel(A2T("\r\n"));

	return 0;
}

// CTCPServerDlg 客户端下线消息
LRESULT CTCPServerDlg::OnClientDisConnectServerMsg(WPARAM wParam, LPARAM lParam)
{
	S_CLIENTINFO* pClientInfo = reinterpret_cast<S_CLIENTINFO*>((LPVOID)lParam);

	char chClientInfo[MAX_PATH] = { 0 };
	int nLen = 0;
	SYSTEMTIME syTime = { 0 };

	USES_CONVERSION;

	GetLocalTime(&syTime);
	sprintf_s(chClientInfo, "下线通知 [%d.%d.%d.%d:%d] %02d:%02d:%02d", pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b1, pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b2, pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b3, pClientInfo->SocketAddr.sin_addr.S_un.S_un_b.s_b4, pClientInfo->SocketAddr.sin_port,
		syTime.wHour, syTime.wMinute, syTime.wSecond);

	// 接收区文本显示
	nLen = m_cRecvEdt.GetWindowTextLengthW();
	if (nLen >= 30000)
	{
		m_cRecvEdt.SetWindowTextW(_T(""));
		nLen = -1;
	}

	m_cRecvEdt.SetSel(nLen, nLen);
	m_cRecvEdt.ReplaceSel(A2T(chClientInfo));
	m_cRecvEdt.ReplaceSel(A2T("\r\n"));

	return 0;
}

// CTCPServerDlg 更新发送对象消息
LRESULT CTCPServerDlg::OnUpdateSendObject(WPARAM wParam, LPARAM lParam)
{
	m_cObjectCbx.ResetContent();
	m_cObjectCbx.InsertString(0, _T("广播"));

	for (auto iter = m_mapClientInfo.begin(); iter != m_mapClientInfo.end(); ++iter)
	{
		char chClientInfo[MAX_PATH] = { 0 };

		USES_CONVERSION;
		sprintf_s(chClientInfo, "%d.%d.%d.%d:%d", iter->second.SocketAddr.sin_addr.S_un.S_un_b.s_b1, iter->second.SocketAddr.sin_addr.S_un.S_un_b.s_b2, iter->second.SocketAddr.sin_addr.S_un.S_un_b.s_b3, iter->second.SocketAddr.sin_addr.S_un.S_un_b.s_b4, iter->second.SocketAddr.sin_port);
		m_cObjectCbx.InsertString(iter->first + 1, A2T(chClientInfo));
	}

	m_cObjectCbx.SetCurSel(0);

	return 0;
}

// CTCPServerDlg 更新客户端连接
LRESULT CTCPServerDlg::OnUpdateClient(WPARAM wParam, LPARAM lParam)
{
	S_CLIENTINFO* pClientInfo = reinterpret_cast<S_CLIENTINFO*>((LPVOID)lParam);
	int nClientNo = 0;

	// 根据Socket查询Client序号
	for (auto iter = m_mapClientInfo.begin(); iter != m_mapClientInfo.end(); ++iter)
	{
		if (iter->second.Socket == pClientInfo->Socket)
		{
			nClientNo = iter->first;
			m_nClientNo--;
			m_mapClientInfo.erase(iter);
			break;
		}
	}

	// 等待线程关闭释放
	//::WaitForSingleObject(m_pSocket->CCSocketBaseGetConnectMap()[nClientNo], INFINITE);
	::CloseHandle(m_pSocket->CCSocketBaseGetConnectMap()[nClientNo]);

	// 根据Client序号查询线程
	for (auto iter = m_pSocket->CCSocketBaseGetConnectMap().begin(); iter != m_pSocket->CCSocketBaseGetConnectMap().end(); ++iter)
	{
		if (iter->first == nClientNo)
		{
			m_pSocket->CCSocketBaseGetConnectCount()--;
			m_pSocket->CCSocketBaseGetConnectMap().erase(iter);
			break;
		}
	}

	// 发送更新发送对象更新通知
	::PostMessageA(this->GetSafeHwnd(), WM_USER_MSG_ONUPDATESENDOBJECT, (WPARAM)0, (LPARAM)0);

	return 0;
}

void CTCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_NET_SERVER_IP, m_cHostIPAdc);
	DDX_Control(pDX, IDC_EDIT_NET_SERVER_PORT, m_cHostPortEdt);
	DDX_Control(pDX, IDC_EDIT_NET_SERVER_CONNECT_MAX, m_cServerLinkMaxEdt);
	DDX_Control(pDX, IDC_BUTTON_NET_SERVER_LISTEN, m_cListenBtn);
	DDX_Control(pDX, IDC_CHECK_NET_SERVER_RECV_HEX, m_cRecvHexCbx);
	DDX_Control(pDX, IDC_CHECK_NET_SERVER_SEND_HEX, m_cSendHexCbx);
	DDX_Control(pDX, IDC_BUTTON_NET_SERVER_RECV_CLEAR, m_cRecvClearBtn);
	DDX_Control(pDX, IDC_BUTTON_NET_SERVER_SEND_CLEAR, m_cSendClearBtn);
	DDX_Control(pDX, IDC_EDIT_NET_SERVER_RECV, m_cRecvEdt);
	DDX_Control(pDX, IDC_EDIT_NET_SERVER_SEND, m_cSendEdt);
	DDX_Control(pDX, IDC_COMBO_NET_SERVER_CONNECT_STYLE, m_cObjectCbx);
	DDX_Control(pDX, IDC_BUTTON_NET_SERVER_SEND, m_cSendBtn);
}


BEGIN_MESSAGE_MAP(CTCPServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_NET_SERVER_LISTEN, &CTCPServerDlg::OnBnClickedButtonNetServerListen)
	ON_BN_CLICKED(IDC_BUTTON_NET_SERVER_RECV_CLEAR, &CTCPServerDlg::OnBnClickedButtonNetServerRecvClear)
	ON_BN_CLICKED(IDC_BUTTON_NET_SERVER_SEND_CLEAR, &CTCPServerDlg::OnBnClickedButtonNetServerSendClear)
	ON_BN_CLICKED(IDC_BUTTON_NET_SERVER_SEND, &CTCPServerDlg::OnBnClickedButtonNetServerSend)
	ON_MESSAGE(WM_USER_MSG_ONRECEIVEBUFFER, &CTCPServerDlg::OnRecvSocketBufferMsg)
	ON_MESSAGE(WM_USER_MSG_ONCLIENTCONNECT, &CTCPServerDlg::OnClientConnectServerMsg)
	ON_MESSAGE(WM_USER_MSG_ONCLIENTDISCONNECT, &CTCPServerDlg::OnClientDisConnectServerMsg)
	ON_MESSAGE(WM_USER_MSG_ONUPDATESENDOBJECT, &CTCPServerDlg::OnUpdateSendObject)
	ON_MESSAGE(WM_USER_MSG_ONUPDATECLIENT, &CTCPServerDlg::OnUpdateClient)
END_MESSAGE_MAP()


// CTCPServerDlg 消息处理程序


BOOL CTCPServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitWindowSharp();
	InitSocket();
	InitControls();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CTCPServerDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	RePaintWindow(dc);
}


HBRUSH CTCPServerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(255, 255, 255));
		return m_Brush;
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CTCPServerDlg::OnClose()
{
	SAFE_DELETE(m_pSocket);
	ReleaseSocket();
	CDialogEx::OnClose();
}

// CTCPServerDlg 监听
void CTCPServerDlg::OnBnClickedButtonNetServerListen()
{
	bool bRet = false;
	CString csHostPort;
	CString csMaxCount;
	int nHostPort;
	int nMaxCount;

	USES_CONVERSION;

	if (!m_bListen)
	{
		// CSocketBase实例是否存在
		if (m_pSocket == NULL)
		{
			m_pSocket = new CCSocketBase();
		}

		m_cHostPortEdt.GetWindowTextW(csHostPort);
		if (csHostPort.IsEmpty())
		{
			MessageBox(_T("端口号不能为空!"), _T("警告"), MB_OK | MB_ICONWARNING);
			return;
		}

		nHostPort = atoi(T2A(csHostPort));
		if (nHostPort < 0 || nHostPort > 65535)
		{
			MessageBox(_T("端口号不合法!"), _T("警告"), MB_OK | MB_ICONWARNING);
			return;
		}

		m_cServerLinkMaxEdt.GetWindowTextW(csMaxCount);
		if (csMaxCount.IsEmpty())
		{
			MessageBox(_T("最大连接数不能为空!"), _T("警告"), MB_OK | MB_ICONWARNING);
			return;
		}

		nMaxCount = atoi(T2A(csMaxCount));
		if (nMaxCount < 0 || nMaxCount > 65535)
		{
			MessageBox(_T("最大连接数不合法!"), _T("警告"), MB_OK | MB_ICONWARNING);
			return;
		}

		m_pSocket->CCSocketBaseSetConnectMaxCount(nMaxCount);

		bRet = m_pSocket->CCSocketBaseBindOnPort(nHostPort);
		if (!bRet)
		{
			MessageBox(_T("端口号已被占用!"), _T("警告"), MB_OK | MB_ICONWARNING);
			return;
		}

		bRet = m_pSocket->CCSocketBaseListen();
		if (!bRet)
		{
			MessageBox(_T("端口监听失败!"), _T("警告"), MB_OK | MB_ICONWARNING);
			return;
		}

		StartAcceptThread();

		// 接收区文本提示
		int nLen = 0;

		nLen = m_cRecvEdt.GetWindowTextLengthW();
		if (nLen >= 30000)
		{
			m_cRecvEdt.SetWindowTextW(_T(""));
			nLen = -1;
		}

		m_cRecvEdt.SetSel(nLen, nLen);
		m_cRecvEdt.ReplaceSel(_T("服务端已经启动,正在监听\r\n"));

		m_cHostPortEdt.SetReadOnly(TRUE);
		m_cServerLinkMaxEdt.SetReadOnly(TRUE);

		m_bListen = true;
		m_cListenBtn.SetWindowTextW(_T("停止"));
	}
	else
	{
		if (m_pSocket->CCSocketBaseGetConnectCount() != 0)
		{
			HANDLE* pAcceptArr = NULL;
			int nSize = 0;

			nSize = m_pSocket->CCSocketBaseGetConnectCount();
			pAcceptArr = new HANDLE[nSize];
			memset(pAcceptArr, 0, nSize * sizeof(HANDLE));

			// 线程句柄数组赋值
			for (auto iter = m_pSocket->CCSocketBaseGetConnectMap().begin(); iter != m_pSocket->CCSocketBaseGetConnectMap().end(); ++iter)
			{
				pAcceptArr[iter->first] = iter->second;
			}

			// 关闭客户端通信Socket
			for (auto iter = m_mapClientInfo.begin(); iter != m_mapClientInfo.end(); ++iter)
			{
				// 关闭Socket通信
				::closesocket(iter->second.Socket);
				
				// 接收区文本提示
				char chClientInfo[MAX_PATH] = { 0 };
				int nLen = 0;
				SYSTEMTIME syTime = { 0 };

				USES_CONVERSION;

				GetLocalTime(&syTime);
				sprintf_s(chClientInfo, "下线通知 [%d.%d.%d.%d:%d] %02d:%02d:%02d", iter->second.SocketAddr.sin_addr.S_un.S_un_b.s_b1, iter->second.SocketAddr.sin_addr.S_un.S_un_b.s_b2, iter->second.SocketAddr.sin_addr.S_un.S_un_b.s_b3, iter->second.SocketAddr.sin_addr.S_un.S_un_b.s_b4, iter->second.SocketAddr.sin_port,
					syTime.wHour, syTime.wMinute, syTime.wSecond);

				nLen = m_cRecvEdt.GetWindowTextLengthW();
				if (nLen >= 30000)
				{
					m_cRecvEdt.SetWindowTextW(_T(""));
					nLen = -1;
				}

				m_cRecvEdt.SetSel(nLen, nLen);
				m_cRecvEdt.ReplaceSel(A2T(chClientInfo));
				m_cRecvEdt.ReplaceSel(A2T("\r\n"));

			}

			// 发送更新发送对象消息
			::PostMessageA(this->GetSafeHwnd(), WM_USER_MSG_ONUPDATESENDOBJECT, (WPARAM)0, (LPARAM)0);

			m_bExit = true;	// 退出线程
			::WaitForMultipleObjects(nSize, pAcceptArr, TRUE, INFINITE);	// 等待所有客户端接收线程均退出

			// 关闭线程句柄
			for (auto iter = m_pSocket->CCSocketBaseGetConnectMap().begin(); iter != m_pSocket->CCSocketBaseGetConnectMap().end(); ++iter)
			{
				::CloseHandle(iter->second);
				iter->second = NULL;
			}

			// 清空线程句柄
			m_pSocket->CCSocketBaseSetConnectCount(0);
			m_pSocket->CCSocketBaseGetConnectMap().clear();

			// 清空已连接客户端信息
			m_nClientNo = 0;
			m_mapClientInfo.clear();

			delete[] pAcceptArr;
			pAcceptArr = NULL;
		}

		m_bListenExit = TRUE;
		if (NULL != m_hAccept)
		{
			::WaitForSingleObject(m_hAccept, INFINITE);
			::CloseHandle(m_hAccept);
			m_hAccept = NULL;
		}

		m_bExit = false;
		m_bListenExit = FALSE;
		SAFE_DELETE(m_pSocket);

		// 接收区文本提示
		int nLen = 0;

		nLen = m_cRecvEdt.GetWindowTextLengthW();
		if (nLen >= 30000)
		{
			m_cRecvEdt.SetWindowTextW(_T(""));
			nLen = -1;
		}

		m_cRecvEdt.SetSel(nLen, nLen);
		m_cRecvEdt.ReplaceSel(_T("服务端已经关闭,停止监听\r\n"));

		m_cHostPortEdt.SetReadOnly(FALSE);
		m_cServerLinkMaxEdt.SetReadOnly(FALSE);

		m_bListen = false;
		m_cListenBtn.SetWindowTextW(_T("监听"));
	}

}

// CTCPServerDlg 清空接收区
void CTCPServerDlg::OnBnClickedButtonNetServerRecvClear()
{
	m_cRecvEdt.SetWindowTextW(_T(""));
}

// CTCPServerDlg 清空发送区
void CTCPServerDlg::OnBnClickedButtonNetServerSendClear()
{
	m_cSendEdt.SetWindowTextW(_T(""));
}

// CTCPServerDlg 发送
void CTCPServerDlg::OnBnClickedButtonNetServerSend()
{
	unsigned char chSendBuf[SOB_TCP_SEND_BUFFER] = { 0 };
	int nRet = 0;

	USES_CONVERSION;

	// 获取准备发送的内容
	CString csSendBuf;

	m_cSendEdt.GetWindowTextW(csSendBuf);
	memset(chSendBuf, 0, sizeof(chSendBuf));
	strcpy_s((char*)chSendBuf, sizeof(chSendBuf), T2A(csSendBuf));

	// 16进制发送数据
	if (TRUE == m_cSendHexCbx.GetCheck())
	{
		vector<string> vecSendBuf;

		BreakSpace(chSendBuf, vecSendBuf);
		memset(chSendBuf, 0, sizeof(chSendBuf));
		Convert2Hex(vecSendBuf, chSendBuf, sizeof(chSendBuf));
	}

	// 消息区显示发送的内容
	char chServerInfo[MAX_PATH] = { 0 };
	int nLen = 0;
	SYSTEMTIME syTime = { 0 };
	CString csPort = _T("");

	m_cHostPortEdt.GetWindowTextW(csPort);

	GetLocalTime(&syTime);
	sprintf_s(chServerInfo, "[%s:%s] %02d:%02d:%02d", CCSocketBase::GetLocalIP(), T2A(csPort), syTime.wHour, syTime.wMinute, syTime.wSecond);

	// 接收区文本显示
	nLen = m_cRecvEdt.GetWindowTextLengthW();
	if (nLen >= 30000)
	{
		m_cRecvEdt.SetWindowTextW(_T(""));
		nLen = -1;
	}

	m_cRecvEdt.SetSel(nLen, nLen);
	m_cRecvEdt.ReplaceSel(A2T(chServerInfo));
	m_cRecvEdt.ReplaceSel(A2T("\r\n"));
	m_cRecvEdt.ReplaceSel(A2T((char*)chSendBuf));
	m_cRecvEdt.ReplaceSel(A2T("\r\n"));

	// 发送模式
	int nMode = 0;

	nMode = m_cObjectCbx.GetCurSel();

	if (nMode == 0)
	{
		// 广播模式
		for (auto iter = m_mapClientInfo.begin(); iter != m_mapClientInfo.end(); ++iter)
		{
			// 16进制发送数据
			if (TRUE == m_cSendHexCbx.GetCheck())
			{
				nRet = m_pSocket->CCSocketBaseSendBuffer(iter->second.Socket, (char*)chSendBuf, m_uSendCount);
				if (nRet == SOB_RET_OK)
				{
					// 发送消息成功响应...
				}
			}
			else
			{
				nRet = m_pSocket->CCSocketBaseSendOnce(iter->second.Socket, (char*)chSendBuf);
				if (nRet == SOB_RET_OK)
				{
					// 发送消息成功响应...
				}
			}

		}
	}
	else
	{
		// 选择模式
		for (auto iter = m_mapClientInfo.begin(); iter != m_mapClientInfo.end(); ++iter)
		{
			if (iter->first == nMode - 1)
			{
				// 16进制发送数据
				if (TRUE == m_cSendHexCbx.GetCheck())
				{
					nRet = m_pSocket->CCSocketBaseSendBuffer(iter->second.Socket, (char*)chSendBuf, m_uSendCount);
					if (nRet == SOB_RET_OK)
					{
						// 发送消息成功响应...
					}
				}
				else
				{
					nRet = m_pSocket->CCSocketBaseSendOnce(iter->second.Socket, (char*)chSendBuf);
					if (nRet == SOB_RET_OK)
					{
						// 发送消息成功响应...
					}
				}

				break;
			}

		}
	}

}
