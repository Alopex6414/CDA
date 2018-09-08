// CRealCurveDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CDA.h"
#include "CRealCurveDlg.h"
#include "afxdialogex.h"


// CRealCurveDlg 对话框

IMPLEMENT_DYNAMIC(CRealCurveDlg, CDialogEx)

CRealCurveDlg::CRealCurveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REALCURVE_DIALOG, pParent)
{
	ConstructionExtra();
}

CRealCurveDlg::~CRealCurveDlg()
{
	m_Brush.DeleteObject();
}

// CRealCurveDlg 构造函数
void CRealCurveDlg::ConstructionExtra()
{
	m_Brush.CreateSolidBrush(RGB(255, 255, 255));
}

// CRealCurveDlg 初始化窗口形状
void CRealCurveDlg::InitWindowSharp()
{
	GetClientRect(&m_cWindowRect);
}

// CRealCurveDlg 重绘窗口
void CRealCurveDlg::RePaintWindow(CDC & dc)
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

void CRealCurveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRealCurveDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CRealCurveDlg 消息处理程序


BOOL CRealCurveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitWindowSharp();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CRealCurveDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	RePaintWindow(dc);
}


HBRUSH CRealCurveDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
