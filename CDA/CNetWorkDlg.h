#pragma once

#include "CTCPServerDlg.h"
#include "CTCPClientDlg.h"
#include "CUDPDlg.h"

// CNetWorkDlg 对话框

class CNetWorkDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNetWorkDlg)

public:
	CNetWorkDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CNetWorkDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETWORK_DIALOG };
#endif

// 成员
private:
	CRect m_cWindowRect;

private:
	CBrush m_Brush;

private:
	CTCPServerDlg m_cTCPServerDlg;
	CTCPClientDlg m_cTCPClientDlg;
	CUDPDlg m_cUDPDlg;

// 成员函数
public:
	void ConstructionExtra();
	void InitWindowSharp();
	void InitTabControl();
	void InitChildWindow();
	void RePaintWindow(CDC& dc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CTabCtrl m_cTabNetTbc;
	afx_msg void OnTcnSelchangeTabNet(NMHDR *pNMHDR, LRESULT *pResult);
};
