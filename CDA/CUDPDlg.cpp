// CUDPDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CDA.h"
#include "CUDPDlg.h"
#include "afxdialogex.h"


// CUDPDlg 对话框

IMPLEMENT_DYNAMIC(CUDPDlg, CDialogEx)

CUDPDlg::CUDPDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_UDP, pParent)
{

}

CUDPDlg::~CUDPDlg()
{
}

void CUDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUDPDlg, CDialogEx)
END_MESSAGE_MAP()


// CUDPDlg 消息处理程序
