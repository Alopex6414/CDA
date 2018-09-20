// CNetWorkDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CDA.h"
#include "CNetWorkDlg.h"
#include "afxdialogex.h"


// CNetWorkDlg 对话框

IMPLEMENT_DYNAMIC(CNetWorkDlg, CDialogEx)

CNetWorkDlg::CNetWorkDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NETWORK_DIALOG, pParent)
{
	ConstructionExtra();
}

CNetWorkDlg::~CNetWorkDlg()
{
	m_Brush.DeleteObject();
}

// CNetWorkDlg 构造函数
void CNetWorkDlg::ConstructionExtra()
{
	m_Brush.CreateSolidBrush(RGB(255, 255, 255));
}

// CNetWorkDlg 初始化窗口形状
void CNetWorkDlg::InitWindowSharp()
{
	GetClientRect(&m_cWindowRect);
}

// CNetWorkDlg 初始化TabControl控件
void CNetWorkDlg::InitTabControl()
{
	m_cTabNetTbc.InsertItem(0, _T("TCP服务端"));
	m_cTabNetTbc.InsertItem(1, _T("TCP客户端"));
	m_cTabNetTbc.InsertItem(2, _T("UDP"));
}

// CNetWorkDlg 初始化子窗口
void CNetWorkDlg::InitChildWindow()
{
}

// CNetWorkDlg 重绘窗口
void CNetWorkDlg::RePaintWindow(CDC & dc)
{
	CDC MemDC;
	CBitmap MemBitmap;
	CRect MemRect;

	GetClientRect(&MemRect);

	MemDC.CreateCompatibleDC(&dc);
	MemBitmap.CreateCompatibleBitmap(&dc, MemRect.Width(), MemRect.Height());

	MemDC.SelectObject(&MemBitmap);
	MemDC.FillSolidRect(&MemRect, RGB(255, 255, 255));

	dc.BitBlt(m_cWindowRect.left, m_cWindowRect.top, m_cWindowRect.Width(), m_cWindowRect.Height(), &MemDC, MemRect.left, MemRect.top, SRCCOPY);

	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}

void CNetWorkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_NET, m_cTabNetTbc);
}


BEGIN_MESSAGE_MAP(CNetWorkDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_NET, &CNetWorkDlg::OnTcnSelchangeTabNet)
END_MESSAGE_MAP()


// CNetWorkDlg 消息处理程序


BOOL CNetWorkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitWindowSharp();
	InitTabControl();
	InitChildWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CNetWorkDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	RePaintWindow(dc);
}


HBRUSH CNetWorkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(255, 255, 255));
		return m_Brush;
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CNetWorkDlg::OnTcnSelchangeTabNet(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
