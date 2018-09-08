// CDataAnalysisDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CDA.h"
#include "CDataAnalysisDlg.h"
#include "afxdialogex.h"


// CDataAnalysisDlg 对话框

IMPLEMENT_DYNAMIC(CDataAnalysisDlg, CDialogEx)

CDataAnalysisDlg::CDataAnalysisDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DATAANALYSIS_DIALOG, pParent)
{
	ConstructionExtra();
}

CDataAnalysisDlg::~CDataAnalysisDlg()
{
	m_Brush.DeleteObject();
}

// CDataAnalysisDlg 构造函数
void CDataAnalysisDlg::ConstructionExtra()
{
	m_Brush.CreateSolidBrush(RGB(255, 255, 255));
}

// CDataAnalysisDlg 初始化窗口形状
void CDataAnalysisDlg::InitWindowSharp()
{
	GetClientRect(&m_cWindowRect);
}

// CDataAnalysisDlg 重绘窗口
void CDataAnalysisDlg::RePaintWindow(CDC & dc)
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

void CDataAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDataAnalysisDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDataAnalysisDlg 消息处理程序

BOOL CDataAnalysisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitWindowSharp();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDataAnalysisDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	RePaintWindow(dc);
}


HBRUSH CDataAnalysisDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
