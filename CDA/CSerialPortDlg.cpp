// CSerialPortDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CDA.h"
#include "CSerialPortDlg.h"
#include "afxdialogex.h"


// CSerialPortDlg 对话框

IMPLEMENT_DYNAMIC(CSerialPortDlg, CDialogEx)

CSerialPortDlg::CSerialPortDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERIALPORT_DIALOG, pParent)
{
	ConstructionExtra();
}

CSerialPortDlg::~CSerialPortDlg()
{
	m_Brush.DeleteObject();
}

// CSerialPortDlg 构造函数
void CSerialPortDlg::ConstructionExtra()
{
	m_Brush.CreateSolidBrush(RGB(255, 255, 255));
}

// CSerialPortDlg 初始化窗口形状
void CSerialPortDlg::InitWindowSharp()
{
	GetClientRect(&m_cWindowRect);
}

// CSerialPortDlg 重绘窗口
void CSerialPortDlg::RePaintWindow(CDC & dc)
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

void CSerialPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSerialPortDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSerialPortDlg 消息处理程序


BOOL CSerialPortDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitWindowSharp();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CSerialPortDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	RePaintWindow(dc);
}


HBRUSH CSerialPortDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
