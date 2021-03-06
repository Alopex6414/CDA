#pragma once

#include "CSocketBase.h"
#include "CThreadSafe.h"
#include "CThreadSafeEx.h"

#include <iostream>
#include <vector>
#include <map>

// CUDPDlg 对话框
#define SAFE_RELEASE(Pointer)	{if(Pointer){(Pointer)->Release();(Pointer) = NULL;}}
#define SAFE_DELETE(Pointer)	{if(Pointer){delete(Pointer);(Pointer) = NULL;}}
#define SAFE_DELETE_ARRAY(Pointer) {if(Pointer){delete[](Pointer);(Pointer) = NULL;}}

class CUDPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUDPDlg)

public:
	CUDPDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CUDPDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_UDP };
#endif

// 成员
private:
	CRect m_cWindowRect;

private:
	CBrush m_Brush;

// 网络通信类
private:
	CCSocketBase* m_pSocket;
	CThreadSafeEx m_csThreadSafe;

// 网络通信线程变量
public:
	HANDLE m_hListen;
	volatile bool m_bConnect;

	bool m_bExit;
	bool m_bShareInfo;

// 数据收发
public:
	unsigned char m_chSendBuf[SOB_TCP_SEND_BUFFER];
	unsigned char m_chRecvBuf[SOB_TCP_RECV_BUFFER];
	UINT m_uSendCount;
	UINT m_uRecvCount;

// 成员函数
public:
	void ConstructionExtra();
	void InitWindowSharp();
	void InitControls();
	void RePaintWindow(CDC& dc);

// 网络通信相关
public:
	void InitSocket();
	void ReleaseSocket();

// 网络通信接收线程
public:
	void StartListenThread();
	static unsigned int CALLBACK OnHandleListenThread(LPVOID lpParameters);

private:
	void BreakIpAddress(const char* pArr, BYTE byArr[4]);
	bool CheckIpAddress(const char* pArr);
	void BreakSpace(const unsigned char* pStr, vector<string>& vecStr);
	void Convert2Hex(vector<string>& vecStr, unsigned char* pStr, int nSize);

// 自定义函数
public:
	LRESULT OnRecvSocketBufferMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnUDPDisConnectMsg(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	CIPAddressCtrl m_cHostIPAdc;
	CEdit m_cHostPortEdt;
	CButton m_cOpenBtn;
	CButton m_cRecvHexCbx;
	CButton m_cSendHexCbx;
	CButton m_cRecvClearBtn;
	CButton m_cSendClearBtn;
	CEdit m_cRecvEdt;
	CEdit m_cSendEdt;
	CIPAddressCtrl m_cRemoteIPAdc;
	CEdit m_cRemotePortEdt;
	CButton m_cSendBtn;
	afx_msg void OnBnClickedButtonNetUdpOpen();
	afx_msg void OnBnClickedButtonNetUdpRecvClear();
	afx_msg void OnBnClickedButtonNetUdpSendClear();
	afx_msg void OnBnClickedButtonNetUdpSend();
};

// Variable Definition
extern CUDPDlg* g_pUDPDlg;
