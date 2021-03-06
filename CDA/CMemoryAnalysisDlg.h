#pragma once

#include <iostream>
#include <vector>
#include <Psapi.h>

#include "ChartCtrl.h"
#include "ChartTitle.h"
#include "ChartAxisLabel.h"
#include "ChartBarSerie.h"
#include "ChartLineSerie.h"

#pragma comment(lib, "Psapi.lib")

using namespace std;

// CMemoryAnalysisDlg 对话框

class CMemoryAnalysisDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMemoryAnalysisDlg)

public:
	CMemoryAnalysisDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMemoryAnalysisDlg();

// 成员
private:
	CRect m_cWindowRect;

	bool m_bStart;
	vector<double> m_vecMemory;

private:
	CBrush m_Brush;

// 成员函数
public:
	void ConstructionExtra();
	void InitWindowSharp();
	void InitControls();
	void RePaintWindow(CDC& dc);

public:
	void CurveAddPoint(double dValue);
	void CurveDraw();
	void CurveExportData();
	void CurveExportPicture();

protected:
	void OnGetProcessMemory();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MEMERY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CChartCtrl m_cChartCtrl;
	CEdit m_cProcessIDEdt;
	CEdit m_cProcessHandleEdt;
	CButton m_cFindBtn;
	CButton m_cStartBtn;
	CButton m_cClearBtn;
	CButton m_cExportDataBtn;
	CButton m_cExportPictureBtn;
	afx_msg void OnBnClickedButtonMemoryFind();
	afx_msg void OnBnClickedButtonMemoryStart();
	afx_msg void OnBnClickedButtonMemoryExportdata();
	afx_msg void OnBnClickedButtonMemoryExportpicture();
	afx_msg void OnBnClickedButtonMemoryClear();
};
