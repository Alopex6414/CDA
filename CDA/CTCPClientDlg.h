#pragma once


// CTCPClientDlg 对话框

class CTCPClientDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTCPClientDlg)

public:
	CTCPClientDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTCPClientDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TCPCLIENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
