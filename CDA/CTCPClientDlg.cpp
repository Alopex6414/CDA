// CTCPClientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CDA.h"
#include "CTCPClientDlg.h"
#include "afxdialogex.h"


// CTCPClientDlg 对话框

IMPLEMENT_DYNAMIC(CTCPClientDlg, CDialogEx)

CTCPClientDlg::CTCPClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TCPCLIENT, pParent)
{

}

CTCPClientDlg::~CTCPClientDlg()
{
}

void CTCPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTCPClientDlg, CDialogEx)
END_MESSAGE_MAP()


// CTCPClientDlg 消息处理程序
