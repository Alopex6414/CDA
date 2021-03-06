#pragma once

#include <atlimage.h>

// CAboutExDlg 对话框

class CAboutExDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAboutExDlg)

public:
	CAboutExDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAboutExDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUT_DIALOG };
#endif

// 成员
private:
	CRect m_cWindowRect;

private:
	CBrush m_Brush;

// 成员函数
public:
	void ConstructionExtra();
	void InitWindowSharp();
	void RePaintWindow(CDC& dc);

private:
	void LoadMemImage(CImage& Image, void* pMemData, long nSize);
	void ReDrawImage(CDC& MemDC, void* pData, long nSize, int X, int Y);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
