// CTCPServerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CDA.h"
#include "CTCPServerDlg.h"
#include "afxdialogex.h"


// CTCPServerDlg 对话框

IMPLEMENT_DYNAMIC(CTCPServerDlg, CDialogEx)

CTCPServerDlg::CTCPServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TCPSERVER, pParent)
{

}

CTCPServerDlg::~CTCPServerDlg()
{
}

void CTCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTCPServerDlg, CDialogEx)
END_MESSAGE_MAP()


// CTCPServerDlg 消息处理程序
