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
