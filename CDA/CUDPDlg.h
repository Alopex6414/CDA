#pragma once


// CUDPDlg 对话框

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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
