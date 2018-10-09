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

// CRealCurveDlg 初始化窗口控件
void CRealCurveDlg::InitControls()
{
	// 曲线初始化

	// 禁止控件自动刷新
	m_cChartCtrl.EnableRefresh(false);

	// 创建曲线坐标轴(底部、左侧)
	CChartStandardAxis* pAxisBot = m_cChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	CChartStandardAxis* pAxisLef = m_cChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);

	// 设置坐标轴范围
	pAxisBot->SetAutomatic(true);
	pAxisLef->SetAutomatic(true);

	// 设置坐标轴名称
	m_cChartCtrl.GetLeftAxis()->GetLabel()->SetText(_T("数值"));
	m_cChartCtrl.GetBottomAxis()->GetLabel()->SetText(_T("时间"));

	// 设置曲线标题名称
	m_cChartCtrl.GetTitle()->AddString(_T("串口曲线"));
	m_cChartCtrl.SetEdgeType(EDGE_SUNKEN);

	// 创建曲线
	/*CChartLineSerie* pLineSeries1 = m_cChartCtrl.CreateLineSerie();
	CChartLineSerie* pLineSeries2 = m_cChartCtrl.CreateLineSerie();

	pLineSeries1->SetSeriesOrdering(poNoOrdering);
	pLineSeries2->SetSeriesOrdering(poNoOrdering);

	for (int i = 0; i < 720; ++i)
	{
		double dX1 = 0.0;
		double dX2 = 0.0;
		double dY1 = 0.0;
		double dY2 = 0.0;

		dX1 = dX2 = i / 360.0 * 2 * 3.1415926;
		dY1 = 10 * sin(i / 360.0 * 2 * 3.1415926);
		dY2 = 10 * cos(i / 360.0 * 2 * 3.1415926);

		pLineSeries1->AddPoint(dX1, dY1);
		pLineSeries2->AddPoint(dX2, dY2);
	}*/

	// 开启控件自动刷新
	m_cChartCtrl.EnableRefresh(true);

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
	DDX_Control(pDX, IDC_CUSTOM_CURVE, m_cChartCtrl);
}


BEGIN_MESSAGE_MAP(CRealCurveDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CRealCurveDlg 消息处理程序


BOOL CRealCurveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitWindowSharp();
	InitControls();

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


void CRealCurveDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
}
