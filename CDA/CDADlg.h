
// CDADlg.h: 头文件
//

#pragma once

#include "CSerialPortDlg.h"
#include "CRealCurveDlg.h"
#include "CNetWorkDlg.h"
#include "CDataAnalysisDlg.h"
#include "CHelpDlg.h"
#include "CAboutExDlg.h"

// CCDADlg 对话框
class CCDADlg : public CDialogEx
{
// 构造
public:
	CCDADlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CDA_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 成员
private:
	CSerialPortDlg m_cSerialPortDlg;
	CRealCurveDlg m_cRealCurveDlg;
	CNetWorkDlg m_cNetWorkDlg;
	CDataAnalysisDlg m_cDataAnalysisDlg;
	CHelpDlg m_cHelpDlg;
	CAboutExDlg m_cAboutExDlg;

// 成员函数
public:
	void ConstructionExtra();
	void InitTabControl();
	void InitChildWindow();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_cTabMainTbc;
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
};
