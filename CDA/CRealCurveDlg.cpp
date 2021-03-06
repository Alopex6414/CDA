// CRealCurveDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CDA.h"
#include "CRealCurveDlg.h"
#include "afxdialogex.h"
#include "CThreadSafe.h"

//Macro Definition
#define WM_USER_TIMER_ONREFRESHRECVINFO		0
#define WM_USER_TIMER_ONREFRESHCURVECHART	1

#define WM_USER_MSG_ONRECEIVEBUFFER		(WM_USER + 1)

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
	m_hRecv = NULL;

	m_dwRecvCount = 0;
	m_dwRecvSize = 0;
	memset(m_chRecvBuf, 0, sizeof(m_chRecvBuf));

	m_vecCurve1.clear();
	m_vecCurve2.clear();
	m_vecCurve3.clear();
	m_vecCurve4.clear();
	m_vecCurve5.clear();
	m_vecCurve6.clear();
	m_vecCurve7.clear();
	m_vecCurve8.clear();

	m_bShareInfo = false;
	InitializeCriticalSection(&m_csThreadSafe);

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
	// 串口初始化
	m_cSerialPort.EnumSerialPort();		// 枚举串口

	// 添加串口
	m_cSerialPortNameCbx.ResetContent();	// 清空Cbx
	for (auto iter = m_cSerialPort.m_mapEnumCOM.begin(); iter != m_cSerialPort.m_mapEnumCOM.end(); ++iter)
	{
		USES_CONVERSION;
		m_cSerialPortNameCbx.InsertString(iter->first, A2T((iter->second).c_str()));
	}

	m_cSerialPortNameCbx.SetCurSel(0);

	// 添加波特率
	m_cSerialPortBaudCbx.ResetContent();	// 清空Cbx
	m_cSerialPortBaudCbx.InsertString(0, _T("9600"));
	m_cSerialPortBaudCbx.InsertString(1, _T("14400"));
	m_cSerialPortBaudCbx.InsertString(2, _T("19200"));
	m_cSerialPortBaudCbx.InsertString(3, _T("38400"));
	m_cSerialPortBaudCbx.InsertString(4, _T("57600"));
	m_cSerialPortBaudCbx.InsertString(5, _T("115200"));
	m_cSerialPortBaudCbx.InsertString(6, _T("128000"));
	m_cSerialPortBaudCbx.InsertString(7, _T("256000"));
	m_cSerialPortBaudCbx.SetCurSel(5);

	// 添加数据位
	m_cSerialPortDataBitCbx.ResetContent();	// 清空Cbx
	m_cSerialPortDataBitCbx.InsertString(0, _T("5"));
	m_cSerialPortDataBitCbx.InsertString(1, _T("6"));
	m_cSerialPortDataBitCbx.InsertString(2, _T("7"));
	m_cSerialPortDataBitCbx.InsertString(3, _T("8"));
	m_cSerialPortDataBitCbx.SetCurSel(3);

	// 添加停止位
	m_cSerialPortStopBitCbx.ResetContent();	// 清空Cbx
	m_cSerialPortStopBitCbx.InsertString(0, _T("1"));
	m_cSerialPortStopBitCbx.InsertString(1, _T("2"));
	m_cSerialPortStopBitCbx.SetCurSel(0);

	// 添加校验位
	m_cSerialPortCheckBitCbx.ResetContent();	// 清空Cbx
	m_cSerialPortCheckBitCbx.InsertString(0, _T("无校验"));
	m_cSerialPortCheckBitCbx.InsertString(1, _T("奇校验"));
	m_cSerialPortCheckBitCbx.InsertString(2, _T("偶校验"));
	m_cSerialPortCheckBitCbx.SetCurSel(0);

	// 辅助功能
	m_cCurve1Cbx.SetCheck(TRUE);
	m_cCurve2Cbx.SetCheck(TRUE);
	m_cCurve3Cbx.SetCheck(TRUE);
	m_cCurve4Cbx.SetCheck(TRUE);
	m_cCurve5Cbx.SetCheck(FALSE);
	m_cCurve6Cbx.SetCheck(FALSE);
	m_cCurve7Cbx.SetCheck(FALSE);
	m_cCurve8Cbx.SetCheck(FALSE);

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

// CRealCurveDlg 检查串口设置
bool CRealCurveDlg::SerialPortConfigCheck()
{
	CString csSerialPortName;
	CString csSerialPortBaud;
	CString csSerialPortDataBits;
	CString csSerialPortStopBits;
	CString csSerialPortCheckBits;

	m_cSerialPortNameCbx.GetWindowTextW(csSerialPortName);
	m_cSerialPortBaudCbx.GetWindowTextW(csSerialPortBaud);
	m_cSerialPortDataBitCbx.GetWindowTextW(csSerialPortDataBits);
	m_cSerialPortStopBitCbx.GetWindowTextW(csSerialPortStopBits);
	m_cSerialPortCheckBitCbx.GetWindowTextW(csSerialPortCheckBits);

	if (_T("") == csSerialPortName)
	{
		MessageBoxW(_T("串口名未设置!"), _T("警告"), MB_OK | MB_ICONWARNING);
		return false;
	}

	if (_T("") == csSerialPortBaud)
	{
		MessageBoxW(_T("串口波特率未设置!"), _T("警告"), MB_OK | MB_ICONWARNING);
		return false;
	}

	if (_T("") == csSerialPortDataBits)
	{
		MessageBoxW(_T("串口数据位未设置!"), _T("警告"), MB_OK | MB_ICONWARNING);
		return false;
	}

	if (_T("") == csSerialPortStopBits)
	{
		MessageBoxW(_T("串口停止位未设置!"), _T("警告"), MB_OK | MB_ICONWARNING);
		return false;
	}

	if (_T("") == csSerialPortCheckBits)
	{
		MessageBoxW(_T("串口校验位未设置!"), _T("警告"), MB_OK | MB_ICONWARNING);
		return false;
	}

	return true;
}

// CRealCurveDlg 填充初始化结构体
void CRealCurveDlg::SerialPortSetValue(S_SERIALPORT_PROPERTY * pSerialPortInfo)
{
	CString csSerialPortName;
	CString csSerialPortBaud;
	CString csSerialPortDataBits;
	CString csSerialPortStopBits;
	CString csSerialPortCheckBits;

	m_cSerialPortNameCbx.GetWindowTextW(csSerialPortName);
	m_cSerialPortBaudCbx.GetWindowTextW(csSerialPortBaud);
	m_cSerialPortDataBitCbx.GetWindowTextW(csSerialPortDataBits);
	m_cSerialPortStopBitCbx.GetWindowTextW(csSerialPortStopBits);
	m_cSerialPortCheckBitCbx.GetWindowTextW(csSerialPortCheckBits);

	USES_CONVERSION;
	strcpy_s(pSerialPortInfo->chPort, T2A(csSerialPortName));
	pSerialPortInfo->dwBaudRate = _ttol(csSerialPortBaud);
	pSerialPortInfo->byDataBits = _ttoi(csSerialPortDataBits);

	if (1 == _ttoi(csSerialPortStopBits))
	{
		pSerialPortInfo->byStopBits = 0;
	}
	else if (2 == _ttoi(csSerialPortStopBits))
	{
		pSerialPortInfo->byStopBits = 2;
	}

	if (!strcmp("无校验", T2A(csSerialPortCheckBits)))
	{
		pSerialPortInfo->byCheckBits = 0;
	}
	else if (!strcmp("奇校验", T2A(csSerialPortCheckBits)))
	{
		pSerialPortInfo->byCheckBits = 1;
	}
	else if (!strcmp("偶校验", T2A(csSerialPortCheckBits)))
	{
		pSerialPortInfo->byCheckBits = 2;
	}

}

// CRealCurveDlg 设置打开串口窗口显示
void CRealCurveDlg::SerialPortSetOpenStatus()
{
	CString csPort;
	CString csBaud;

	// 打开串口按钮显示
	m_cSerialPortOpenBtn.SetWindowTextW(_T("关闭串口"));

	// 禁止按钮选择
	m_cSerialPortRefreshBtn.EnableWindow(FALSE);

	m_cSerialPortNameCbx.EnableWindow(FALSE);
	m_cSerialPortBaudCbx.EnableWindow(FALSE);
	m_cSerialPortDataBitCbx.EnableWindow(FALSE);
	m_cSerialPortStopBitCbx.EnableWindow(FALSE);
	m_cSerialPortCheckBitCbx.EnableWindow(FALSE);

	// 串口信息提示
	m_cSerialPortNameCbx.GetLBText(m_cSerialPortNameCbx.GetCurSel(), csPort);
	m_cSerialPortBaudCbx.GetLBText(m_cSerialPortBaudCbx.GetCurSel(), csBaud);
	m_cSerialInfoEdt.SetWindowTextW(_T("串口已打开!"));
	m_cSerialInfoPortEdt.SetWindowTextW(csPort);
	m_cSerialInfoBaudEdt.SetWindowTextW(csBaud);
	m_cCurveExportBtn.EnableWindow(FALSE);
	m_cCurvePictureBtn.EnableWindow(FALSE);

}

// CRealCurveDlg 设置关闭串口窗口显示
void CRealCurveDlg::SerialPortSetCloseStatus()
{
	CString csPort;
	CString csBaud;

	// 打开串口按钮显示
	m_cSerialPortOpenBtn.SetWindowTextW(_T("打开串口"));

	// 禁止按钮选择
	m_cSerialPortRefreshBtn.EnableWindow(TRUE);

	m_cSerialPortNameCbx.EnableWindow(TRUE);
	m_cSerialPortBaudCbx.EnableWindow(TRUE);
	m_cSerialPortDataBitCbx.EnableWindow(TRUE);
	m_cSerialPortStopBitCbx.EnableWindow(TRUE);
	m_cSerialPortCheckBitCbx.EnableWindow(TRUE);

	// 串口信息提示
	m_cSerialPortNameCbx.GetLBText(m_cSerialPortNameCbx.GetCurSel(), csPort);
	m_cSerialPortBaudCbx.GetLBText(m_cSerialPortBaudCbx.GetCurSel(), csBaud);
	m_cSerialInfoEdt.SetWindowTextW(_T("串口已关闭!"));
	m_cSerialInfoPortEdt.SetWindowTextW(csPort);
	m_cSerialInfoBaudEdt.SetWindowTextW(csBaud);
	m_cCurveExportBtn.EnableWindow(TRUE);
	m_cCurvePictureBtn.EnableWindow(TRUE);

}

// CRealCurveDlg 开始串口接收线程
bool CRealCurveDlg::SerialPortStartRecvThread()
{
	unsigned int uThreadID;

	m_hRecv = (HANDLE)::_beginthreadex(NULL, 0, (_beginthreadex_proc_type)OnReceiveBuffer, this, 0, &uThreadID);
	if (!m_hRecv)
	{
		return false;
	}

	return true;
}

// CRealCurveDlg 关闭串口接收线程
void CRealCurveDlg::SerialPortCloseRecvThread()
{
	if (NULL != m_hRecv)
	{
		m_bShareInfo = false;
		::WaitForSingleObject(m_hRecv, INFINITE);
		::CloseHandle(m_hRecv);
		m_hRecv = NULL;
	}

}

// CRealCurveDlg 设置串口曲线显示
void CRealCurveDlg::SerialPortSetShowCurve(int nCount)
{
	switch (nCount)
	{
	case 0:
		m_cCurve1Cbx.SetCheck(FALSE);
		m_cCurve2Cbx.SetCheck(FALSE);
		m_cCurve3Cbx.SetCheck(FALSE);
		m_cCurve4Cbx.SetCheck(FALSE);
		m_cCurve5Cbx.SetCheck(FALSE);
		m_cCurve6Cbx.SetCheck(FALSE);
		m_cCurve7Cbx.SetCheck(FALSE);
		m_cCurve8Cbx.SetCheck(FALSE);
		break;
	case 1:
		m_cCurve1Cbx.SetCheck(TRUE);
		m_cCurve2Cbx.SetCheck(FALSE);
		m_cCurve3Cbx.SetCheck(FALSE);
		m_cCurve4Cbx.SetCheck(FALSE);
		m_cCurve5Cbx.SetCheck(FALSE);
		m_cCurve6Cbx.SetCheck(FALSE);
		m_cCurve7Cbx.SetCheck(FALSE);
		m_cCurve8Cbx.SetCheck(FALSE);
		break;
	case 2:
		m_cCurve1Cbx.SetCheck(TRUE);
		m_cCurve2Cbx.SetCheck(TRUE);
		m_cCurve3Cbx.SetCheck(FALSE);
		m_cCurve4Cbx.SetCheck(FALSE);
		m_cCurve5Cbx.SetCheck(FALSE);
		m_cCurve6Cbx.SetCheck(FALSE);
		m_cCurve7Cbx.SetCheck(FALSE);
		m_cCurve8Cbx.SetCheck(FALSE);
		break;
	case 3:
		m_cCurve1Cbx.SetCheck(TRUE);
		m_cCurve2Cbx.SetCheck(TRUE);
		m_cCurve3Cbx.SetCheck(TRUE);
		m_cCurve4Cbx.SetCheck(FALSE);
		m_cCurve5Cbx.SetCheck(FALSE);
		m_cCurve6Cbx.SetCheck(FALSE);
		m_cCurve7Cbx.SetCheck(FALSE);
		m_cCurve8Cbx.SetCheck(FALSE);
		break;
	case 4:
		m_cCurve1Cbx.SetCheck(TRUE);
		m_cCurve2Cbx.SetCheck(TRUE);
		m_cCurve3Cbx.SetCheck(TRUE);
		m_cCurve4Cbx.SetCheck(TRUE);
		m_cCurve5Cbx.SetCheck(FALSE);
		m_cCurve6Cbx.SetCheck(FALSE);
		m_cCurve7Cbx.SetCheck(FALSE);
		m_cCurve8Cbx.SetCheck(FALSE);
		break;
	case 5:
		m_cCurve1Cbx.SetCheck(TRUE);
		m_cCurve2Cbx.SetCheck(TRUE);
		m_cCurve3Cbx.SetCheck(TRUE);
		m_cCurve4Cbx.SetCheck(TRUE);
		m_cCurve5Cbx.SetCheck(TRUE);
		m_cCurve6Cbx.SetCheck(FALSE);
		m_cCurve7Cbx.SetCheck(FALSE);
		m_cCurve8Cbx.SetCheck(FALSE);
		break;
	case 6:
		m_cCurve1Cbx.SetCheck(TRUE);
		m_cCurve2Cbx.SetCheck(TRUE);
		m_cCurve3Cbx.SetCheck(TRUE);
		m_cCurve4Cbx.SetCheck(TRUE);
		m_cCurve5Cbx.SetCheck(TRUE);
		m_cCurve6Cbx.SetCheck(TRUE);
		m_cCurve7Cbx.SetCheck(FALSE);
		m_cCurve8Cbx.SetCheck(FALSE);
		break;
	case 7:
		m_cCurve1Cbx.SetCheck(TRUE);
		m_cCurve2Cbx.SetCheck(TRUE);
		m_cCurve3Cbx.SetCheck(TRUE);
		m_cCurve4Cbx.SetCheck(TRUE);
		m_cCurve5Cbx.SetCheck(TRUE);
		m_cCurve6Cbx.SetCheck(TRUE);
		m_cCurve7Cbx.SetCheck(TRUE);
		m_cCurve8Cbx.SetCheck(FALSE);
		break;
	case 8:
		m_cCurve1Cbx.SetCheck(TRUE);
		m_cCurve2Cbx.SetCheck(TRUE);
		m_cCurve3Cbx.SetCheck(TRUE);
		m_cCurve4Cbx.SetCheck(TRUE);
		m_cCurve5Cbx.SetCheck(TRUE);
		m_cCurve6Cbx.SetCheck(TRUE);
		m_cCurve7Cbx.SetCheck(TRUE);
		m_cCurve8Cbx.SetCheck(TRUE);
		break;
	default:
		break;
	}

}

// CRealCurveDlg 曲线添加点
void CRealCurveDlg::SerialPortCurveAddPoint(INT16 uVarArr[8])
{
	static int nVectorSize = 500;

	// 曲线1
	if (m_vecCurve1.size() < nVectorSize)
	{
		m_vecCurve1.push_back(uVarArr[0]);
	}
	else
	{
		m_vecCurve1.erase(m_vecCurve1.begin());
		m_vecCurve1.push_back(uVarArr[0]);
	}

	// 曲线2
	if (m_vecCurve2.size() < nVectorSize)
	{
		m_vecCurve2.push_back(uVarArr[1]);
	}
	else
	{
		m_vecCurve2.erase(m_vecCurve2.begin());
		m_vecCurve2.push_back(uVarArr[1]);
	}

	// 曲线3
	if (m_vecCurve3.size() < nVectorSize)
	{
		m_vecCurve3.push_back(uVarArr[2]);
	}
	else
	{
		m_vecCurve3.erase(m_vecCurve3.begin());
		m_vecCurve3.push_back(uVarArr[2]);
	}

	// 曲线4
	if (m_vecCurve4.size() < nVectorSize)
	{
		m_vecCurve4.push_back(uVarArr[3]);
	}
	else
	{
		m_vecCurve4.erase(m_vecCurve4.begin());
		m_vecCurve4.push_back(uVarArr[3]);
	}

	// 曲线5
	if (m_vecCurve5.size() < nVectorSize)
	{
		m_vecCurve5.push_back(uVarArr[4]);
	}
	else
	{
		m_vecCurve5.erase(m_vecCurve5.begin());
		m_vecCurve5.push_back(uVarArr[4]);
	}

	// 曲线6
	if (m_vecCurve6.size() < nVectorSize)
	{
		m_vecCurve6.push_back(uVarArr[5]);
	}
	else
	{
		m_vecCurve6.erase(m_vecCurve6.begin());
		m_vecCurve6.push_back(uVarArr[5]);
	}

	// 曲线7
	if (m_vecCurve7.size() < nVectorSize)
	{
		m_vecCurve7.push_back(uVarArr[6]);
	}
	else
	{
		m_vecCurve7.erase(m_vecCurve7.begin());
		m_vecCurve7.push_back(uVarArr[6]);
	}

	// 曲线8
	if (m_vecCurve8.size() < nVectorSize)
	{
		m_vecCurve8.push_back(uVarArr[7]);
	}
	else
	{
		m_vecCurve8.erase(m_vecCurve8.begin());
		m_vecCurve8.push_back(uVarArr[7]);
	}

}

// CRealCurveDlg 导出数据
void CRealCurveDlg::SerialPortCurveExportData()
{
	CString	csFilePathName;
	CString csFileName;

	setlocale(LC_ALL, "chs");
	CFileDialog cFileDialog(false, _T("*.csv"), _T("SerialCurve"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("*.csv|*.csv||"));
	if (IDOK == cFileDialog.DoModal())
	{
		csFilePathName = cFileDialog.GetPathName();
		csFileName = cFileDialog.GetFileName();

		// 删除已存在文件
		CFileFind findFile;
		BOOL bWorking = findFile.FindFile(csFilePathName);
		if (bWorking)
		{
			CFile::Remove((LPCTSTR)csFilePathName);
		}
		Sleep(100);
		CStdioFile  file(csFilePathName, CFile::modeCreate | CFile::modeReadWrite); // 重新创建
		Sleep(100);

		CString csTip = _T("CDA SerialPort Curve Export\n");
		CString csTitle = _T("曲线1,曲线2,曲线3,曲线4,曲线5,曲线6,曲线7,曲线8\n");
		file.WriteString(csTip);
		file.WriteString(csTitle);

		for (size_t i = 0; i < m_vecCurve1.size(); ++i)
		{
			CString csText = _T("");
			CString csCurve1Text = _T("");
			CString csCurve2Text = _T("");
			CString csCurve3Text = _T("");
			CString csCurve4Text = _T("");
			CString csCurve5Text = _T("");
			CString csCurve6Text = _T("");
			CString csCurve7Text = _T("");
			CString csCurve8Text = _T("");

			csCurve1Text.Format(_T("%d,"), m_vecCurve1[i]);
			csCurve2Text.Format(_T("%d,"), m_vecCurve2[i]);
			csCurve3Text.Format(_T("%d,"), m_vecCurve3[i]);
			csCurve4Text.Format(_T("%d,"), m_vecCurve4[i]);
			csCurve5Text.Format(_T("%d,"), m_vecCurve5[i]);
			csCurve6Text.Format(_T("%d,"), m_vecCurve6[i]);
			csCurve7Text.Format(_T("%d,"), m_vecCurve7[i]);
			csCurve8Text.Format(_T("%d\n"), m_vecCurve8[i]);

			csText = csCurve1Text + csCurve2Text + csCurve3Text + csCurve4Text + csCurve5Text + csCurve6Text + csCurve7Text + csCurve8Text;
			file.WriteString(csText);
		}

		file.Close();
	}

}

// CRealCurveDlg 导出图片
void CRealCurveDlg::SerialPortCurveExportPicture()
{
	CString	csFilePathName;
	CString csFileName;

	setlocale(LC_ALL, "chs");
	CFileDialog cFileDialog(false, _T("*.png"), _T("CurvePhoto"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("*.png|*.png||"));
	if (IDOK == cFileDialog.DoModal())
	{
		csFilePathName = cFileDialog.GetPathName();
		csFileName = cFileDialog.GetFileName();

		CRect Rect;
		TChartString csFile(csFilePathName);

		m_cChartCtrl.GetClientRect(&Rect);
		m_cChartCtrl.SaveAsImage(csFile, Rect, 32, GUID_NULL);

		MessageBoxW(_T("曲线图片保存成功!"), _T("提示"), MB_OK | MB_ICONASTERISK);

	}

}

// CRealCurveDlg 绘制曲线
void CRealCurveDlg::SerialPortCurveDraw()
{
	m_cChartCtrl.EnableRefresh(false);

	// 清空图表
	m_cChartCtrl.RemoveAllSeries();

	// 绘制图表
	CChartLineSerie* pCurveSeries1 = m_cChartCtrl.CreateLineSerie();
	CChartLineSerie* pCurveSeries2 = m_cChartCtrl.CreateLineSerie();
	CChartLineSerie* pCurveSeries3 = m_cChartCtrl.CreateLineSerie();
	CChartLineSerie* pCurveSeries4 = m_cChartCtrl.CreateLineSerie();
	CChartLineSerie* pCurveSeries5 = m_cChartCtrl.CreateLineSerie();
	CChartLineSerie* pCurveSeries6 = m_cChartCtrl.CreateLineSerie();
	CChartLineSerie* pCurveSeries7 = m_cChartCtrl.CreateLineSerie();
	CChartLineSerie* pCurveSeries8 = m_cChartCtrl.CreateLineSerie();

	pCurveSeries1->SetSeriesOrdering(poNoOrdering);
	pCurveSeries2->SetSeriesOrdering(poNoOrdering);
	pCurveSeries3->SetSeriesOrdering(poNoOrdering);
	pCurveSeries4->SetSeriesOrdering(poNoOrdering);
	pCurveSeries5->SetSeriesOrdering(poNoOrdering);
	pCurveSeries6->SetSeriesOrdering(poNoOrdering);
	pCurveSeries7->SetSeriesOrdering(poNoOrdering);
	pCurveSeries8->SetSeriesOrdering(poNoOrdering);

	// 曲线1
	if (m_cCurve1Cbx.GetCheck())
	{
		for (auto iter = m_vecCurve1.begin(); iter != m_vecCurve1.end(); ++iter)
		{
			pCurveSeries1->AddPoint(iter - m_vecCurve1.begin(), *iter);
		}

		// 数值1
		if (!m_vecCurve1.empty())
		{
			CString csCurveVal;
			csCurveVal.Format(_T("数值:%d"), m_vecCurve1[m_vecCurve1.size() - 1]);
			m_cCurve1Stic.SetWindowTextW(csCurveVal);
			m_cCurve1Stic.Invalidate(FALSE);
			m_cCurve1Stic.UpdateWindow();
		}
		
	}

	// 曲线2
	if (m_cCurve2Cbx.GetCheck())
	{
		for (auto iter = m_vecCurve2.begin(); iter != m_vecCurve2.end(); ++iter)
		{
			pCurveSeries2->AddPoint(iter - m_vecCurve2.begin(), *iter);
		}

		// 数值2
		if (!m_vecCurve2.empty())
		{
			CString csCurveVal;
			csCurveVal.Format(_T("数值:%d"), m_vecCurve2[m_vecCurve2.size() - 1]);
			m_cCurve2Stic.SetWindowTextW(csCurveVal);
			m_cCurve2Stic.Invalidate(FALSE);
			m_cCurve2Stic.UpdateWindow();
		}

	}

	// 曲线3
	if (m_cCurve3Cbx.GetCheck())
	{
		for (auto iter = m_vecCurve3.begin(); iter != m_vecCurve3.end(); ++iter)
		{
			pCurveSeries3->AddPoint(iter - m_vecCurve3.begin(), *iter);
		}

		// 数值3
		if (!m_vecCurve3.empty())
		{
			CString csCurveVal;
			csCurveVal.Format(_T("数值:%d"), m_vecCurve3[m_vecCurve3.size() - 1]);
			m_cCurve3Stic.SetWindowTextW(csCurveVal);
			m_cCurve3Stic.Invalidate(FALSE);
			m_cCurve3Stic.UpdateWindow();
		}

	}

	// 曲线4
	if (m_cCurve4Cbx.GetCheck())
	{
		for (auto iter = m_vecCurve4.begin(); iter != m_vecCurve4.end(); ++iter)
		{
			pCurveSeries4->AddPoint(iter - m_vecCurve4.begin(), *iter);
		}

		// 数值4
		if (!m_vecCurve4.empty())
		{
			CString csCurveVal;
			csCurveVal.Format(_T("数值:%d"), m_vecCurve4[m_vecCurve4.size() - 1]);
			m_cCurve4Stic.SetWindowTextW(csCurveVal);
			m_cCurve4Stic.Invalidate(FALSE);
			m_cCurve4Stic.UpdateWindow();
		}

	}

	// 曲线5
	if (m_cCurve5Cbx.GetCheck())
	{
		for (auto iter = m_vecCurve5.begin(); iter != m_vecCurve5.end(); ++iter)
		{
			pCurveSeries5->AddPoint(iter - m_vecCurve5.begin(), *iter);
		}

		// 数值5
		if (!m_vecCurve5.empty())
		{
			CString csCurveVal;
			csCurveVal.Format(_T("数值:%d"), m_vecCurve5[m_vecCurve5.size() - 1]);
			m_cCurve5Stic.SetWindowTextW(csCurveVal);
			m_cCurve5Stic.Invalidate(FALSE);
			m_cCurve5Stic.UpdateWindow();
		}

	}

	// 曲线6
	if (m_cCurve6Cbx.GetCheck())
	{
		for (auto iter = m_vecCurve6.begin(); iter != m_vecCurve6.end(); ++iter)
		{
			pCurveSeries6->AddPoint(iter - m_vecCurve6.begin(), *iter);
		}

		// 数值6
		if (!m_vecCurve6.empty())
		{
			CString csCurveVal;
			csCurveVal.Format(_T("数值:%d"), m_vecCurve6[m_vecCurve6.size() - 1]);
			m_cCurve6Stic.SetWindowTextW(csCurveVal);
			m_cCurve6Stic.Invalidate(FALSE);
			m_cCurve6Stic.UpdateWindow();
		}

	}

	// 曲线7
	if (m_cCurve7Cbx.GetCheck())
	{
		for (auto iter = m_vecCurve7.begin(); iter != m_vecCurve7.end(); ++iter)
		{
			pCurveSeries7->AddPoint(iter - m_vecCurve7.begin(), *iter);
		}

		// 数值7
		if (!m_vecCurve7.empty())
		{
			CString csCurveVal;
			csCurveVal.Format(_T("数值:%d"), m_vecCurve7[m_vecCurve7.size() - 1]);
			m_cCurve7Stic.SetWindowTextW(csCurveVal);
			m_cCurve7Stic.Invalidate(FALSE);
			m_cCurve7Stic.UpdateWindow();
		}

	}

	// 曲线8
	if (m_cCurve8Cbx.GetCheck())
	{
		for (auto iter = m_vecCurve8.begin(); iter != m_vecCurve8.end(); ++iter)
		{
			pCurveSeries8->AddPoint(iter - m_vecCurve8.begin(), *iter);
		}

		// 数值8
		if (!m_vecCurve8.empty())
		{
			CString csCurveVal;
			csCurveVal.Format(_T("数值:%d"), m_vecCurve8[m_vecCurve8.size() - 1]);
			m_cCurve8Stic.SetWindowTextW(csCurveVal);
			m_cCurve8Stic.Invalidate(FALSE);
			m_cCurve8Stic.UpdateWindow();
		}

	}

	m_cChartCtrl.EnableRefresh(true);
}

// CRealCurveDlg 接收定时器响应
void CRealCurveDlg::SerialPortRecvOnTimer()
{
	// 显示串口接收数据
	CString csRecvData;

	csRecvData.Format(_T("已接收:%ld"), m_dwRecvCount);
	m_cSerialInfoAllRecvStic.SetWindowTextW(csRecvData);
	m_cSerialInfoAllRecvStic.Invalidate(FALSE);
	m_cSerialInfoAllRecvStic.UpdateWindow();

}

// CRealCurveDlg 接收数据线程
unsigned int CALLBACK CRealCurveDlg::OnReceiveBuffer(LPVOID lpParameters)
{
	CRealCurveDlg* pUser = reinterpret_cast<CRealCurveDlg*>(lpParameters);

	while (true)
	{
		EnterCriticalSection(&pUser->m_cSerialPort.m_csCOMSync);
		if (!pUser->m_cSerialPort.m_bOpen)
		{
			LeaveCriticalSection(&pUser->m_cSerialPort.m_csCOMSync);
			break;
		}
		LeaveCriticalSection(&pUser->m_cSerialPort.m_csCOMSync);

		while (true)
		{
			EnterCriticalSection(&pUser->m_csThreadSafe);
			if (!pUser->m_bShareInfo)
			{
				pUser->m_bShareInfo = true;
				LeaveCriticalSection(&pUser->m_csThreadSafe);
				break;
			}
			LeaveCriticalSection(&pUser->m_csThreadSafe);
			Sleep(10);
		}

		if (pUser->m_cSerialPort.CCSerialPortBaseGetRecv())
		{
			memset(pUser->m_chRecvBuf, 0, sizeof(pUser->m_chRecvBuf));
			pUser->m_cSerialPort.CCSerialPortBaseGetRecvBuf(pUser->m_chRecvBuf, sizeof(pUser->m_chRecvBuf), pUser->m_dwRecvSize);
			pUser->m_cSerialPort.CCSerialPortBaseSetRecv(false);
			::PostMessageA(pUser->m_hWnd, WM_USER_MSG_ONRECEIVEBUFFER, (WPARAM)((LPVOID)(&pUser->m_chRecvBuf)), (LPARAM)0);
		}
		else
		{
			pUser->m_bShareInfo = false;
		}

	}

	return 0;
}

// CRealCurveDlg 接收数据消息响应
LRESULT CRealCurveDlg::OnRecvSerialPortBufferMsg(WPARAM wParam, LPARAM lParam)
{
	CThreadSafe ThreadSafe(&m_csThreadSafe);

	USES_CONVERSION;

	for (int i = 0; i < SERIALPORT_COMM_OUTPUT_BUFFER_SIZE - 21; ++i)
	{
		if (m_chRecvBuf[i] == 0xFF && m_chRecvBuf[i + 1] == 0x00 && m_chRecvBuf[i + 2] == 0x16 && m_chRecvBuf[i + 20] == 0xAA && m_chRecvBuf[i + 21] == 0x55)
		{
			INT16 uCurveVar[8] = { 0 };

			for (int j = 0, k = i + 4; j < 8; ++j, k += 2)
			{
				uCurveVar[j] |= (m_chRecvBuf[k] << 8);
				uCurveVar[j] |= (m_chRecvBuf[k + 1] & 0xFF);
			}

			//assert(m_chRecvBuf[i + 3] >= 0x00 && m_chRecvBuf[i + 3] <= 0x08);
			//SerialPortSetShowCurve(m_chRecvBuf[i + 3]);

			// 添加曲线点
			SerialPortCurveAddPoint(uCurveVar);

			// 曲线点计数
			m_dwRecvCount++;
			if (m_dwRecvCount > 65535)
			{
				m_dwRecvCount = 0;
			}

			i += 21;
		}

	}

	m_bShareInfo = false;
	return 0;
}

void CRealCurveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_CURVE, m_cChartCtrl);
	DDX_Control(pDX, IDC_COMBO_CURVE_PORT, m_cSerialPortNameCbx);
	DDX_Control(pDX, IDC_COMBO_CURVE_BOAD, m_cSerialPortBaudCbx);
	DDX_Control(pDX, IDC_COMBO_CURVE_DATABIT, m_cSerialPortDataBitCbx);
	DDX_Control(pDX, IDC_COMBO_CURVE_STOPBIT, m_cSerialPortStopBitCbx);
	DDX_Control(pDX, IDC_COMBO_CURVE_CHECKBIT, m_cSerialPortCheckBitCbx);
	DDX_Control(pDX, IDC_BUTTON_CURVE_REFRESH, m_cSerialPortRefreshBtn);
	DDX_Control(pDX, IDC_BUTTON_CURVE_OPEN, m_cSerialPortOpenBtn);
	DDX_Control(pDX, IDC_CHECK_CURVE1, m_cCurve1Cbx);
	DDX_Control(pDX, IDC_CHECK_CURVE2, m_cCurve2Cbx);
	DDX_Control(pDX, IDC_CHECK_CURVE3, m_cCurve3Cbx);
	DDX_Control(pDX, IDC_CHECK_CURVE4, m_cCurve4Cbx);
	DDX_Control(pDX, IDC_CHECK_CURVE5, m_cCurve5Cbx);
	DDX_Control(pDX, IDC_CHECK_CURVE6, m_cCurve6Cbx);
	DDX_Control(pDX, IDC_CHECK_CURVE7, m_cCurve7Cbx);
	DDX_Control(pDX, IDC_CHECK_CURVE8, m_cCurve8Cbx);
	DDX_Control(pDX, IDC_STATIC_CURVE1_VALUE, m_cCurve1Stic);
	DDX_Control(pDX, IDC_STATIC_CURVE2_VALUE, m_cCurve2Stic);
	DDX_Control(pDX, IDC_STATIC_CURVE3_VALUE, m_cCurve3Stic);
	DDX_Control(pDX, IDC_STATIC_CURVE4_VALUE3, m_cCurve4Stic);
	DDX_Control(pDX, IDC_STATIC_CURVE5_VALUE, m_cCurve5Stic);
	DDX_Control(pDX, IDC_STATIC_CURVE6_VALUE, m_cCurve6Stic);
	DDX_Control(pDX, IDC_STATIC_CURVE7_VALUE, m_cCurve7Stic);
	DDX_Control(pDX, IDC_STATIC_CURVE8_VALUE, m_cCurve8Stic);
	DDX_Control(pDX, IDC_BUTTON_CURVE_CLEAR, m_cCurveClearBtn);
	DDX_Control(pDX, IDC_EDIT_CURVE_INFO_MESSAGE, m_cSerialInfoEdt);
	DDX_Control(pDX, IDC_EDIT_CURVE_INFO_PORT, m_cSerialInfoPortEdt);
	DDX_Control(pDX, IDC_EDIT_CURVE_INFO_BAUD, m_cSerialInfoBaudEdt);
	DDX_Control(pDX, IDC_STATIC_CURVE_RECEIVE_ALL, m_cSerialInfoAllRecvStic);
	DDX_Control(pDX, IDC_BUTTON_CURVE_EXPORT_DATA, m_cCurveExportBtn);
	DDX_Control(pDX, IDC_BUTTON_CURVE_EXPORT_PICTURE, m_cCurvePictureBtn);
}


BEGIN_MESSAGE_MAP(CRealCurveDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CURVE_REFRESH, &CRealCurveDlg::OnBnClickedButtonCurveRefresh)
	ON_BN_CLICKED(IDC_BUTTON_CURVE_OPEN, &CRealCurveDlg::OnBnClickedButtonCurveOpen)
	ON_BN_CLICKED(IDC_BUTTON_CURVE_CLEAR, &CRealCurveDlg::OnBnClickedButtonCurveClear)
	ON_BN_CLICKED(IDC_BUTTON_CURVE_EXPORT_DATA, &CRealCurveDlg::OnBnClickedButtonCurveExportData)
	ON_MESSAGE(WM_USER_MSG_ONRECEIVEBUFFER, &CRealCurveDlg::OnRecvSerialPortBufferMsg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CURVE_EXPORT_PICTURE, &CRealCurveDlg::OnBnClickedButtonCurveExportPicture)
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
	::KillTimer(this->GetSafeHwnd(), WM_USER_TIMER_ONREFRESHRECVINFO);
	::KillTimer(this->GetSafeHwnd(), WM_USER_TIMER_ONREFRESHCURVECHART);
	DeleteCriticalSection(&m_csThreadSafe);
	CDialogEx::OnClose();
}


void CRealCurveDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case WM_USER_TIMER_ONREFRESHRECVINFO:
		SerialPortRecvOnTimer();
		break;
	case WM_USER_TIMER_ONREFRESHCURVECHART:
		SerialPortCurveDraw();
		break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

// CRealCurveDlg 刷新串口
void CRealCurveDlg::OnBnClickedButtonCurveRefresh()
{
	m_cSerialPort.EnumSerialPort();		// 枚举串口

	// 添加串口
	m_cSerialPortNameCbx.ResetContent();	// 清空Cbx
	for (auto iter = m_cSerialPort.m_mapEnumCOM.begin(); iter != m_cSerialPort.m_mapEnumCOM.end(); ++iter)
	{
		USES_CONVERSION;
		m_cSerialPortNameCbx.InsertString(iter->first, A2T((iter->second).c_str()));
	}

	m_cSerialPortNameCbx.SetCurSel(0);
}

// CRealCurveDlg 打开串口
void CRealCurveDlg::OnBnClickedButtonCurveOpen()
{
	// 串口打开状态
	if (!m_cSerialPort.CCSerialPortBaseGetStatus())
	{
		// 打开串口
		if (!SerialPortConfigCheck())
		{
			return;
		}

		bool bRet = false;
		S_SERIALPORT_PROPERTY sSerialPortProperty = { 0 };

		// 填充串口参数
		SerialPortSetValue(&sSerialPortProperty);

		// 打开串口函数
		bRet = m_cSerialPort.CCSerialPortBaseOpenPort(sSerialPortProperty);
		if (!bRet)
		{
			// 打开失败
			MessageBoxW(_T("串口打开失败"), _T("警告"), MB_OK | MB_ICONWARNING);
			return;
		}

		// 开启串口收发线程
		SerialPortStartRecvThread();

		// 窗口显示函数
		SerialPortSetOpenStatus();

		// 开启接收计数定时器
		::SetTimer(this->GetSafeHwnd(), WM_USER_TIMER_ONREFRESHRECVINFO, 100, NULL);

		// 开始绘制曲线图表
		::SetTimer(this->GetSafeHwnd(), WM_USER_TIMER_ONREFRESHCURVECHART, 100, NULL);
	}
	else
	{
		// 关闭接收计数定时器
		::KillTimer(this->GetSafeHwnd(), WM_USER_TIMER_ONREFRESHRECVINFO);

		// 关闭绘制曲线图表
		::KillTimer(this->GetSafeHwnd(), WM_USER_TIMER_ONREFRESHCURVECHART);

		// 关闭串口
		m_cSerialPort.CCSerialPortBaseClosePort();

		// 关闭串口收发线程
		SerialPortCloseRecvThread();

		// 窗口显示函数
		SerialPortSetCloseStatus();
	}

}

// CRealCurveDlg 清除曲线
void CRealCurveDlg::OnBnClickedButtonCurveClear()
{
	// 清除计数
	m_dwRecvCount = 0;

	// 清空曲线
	m_vecCurve1.clear();
	m_vecCurve2.clear();
	m_vecCurve3.clear();
	m_vecCurve4.clear();
	m_vecCurve5.clear();
	m_vecCurve6.clear();
	m_vecCurve7.clear();
	m_vecCurve8.clear();

	// 清空数值
	m_cCurve1Stic.SetWindowTextW(_T("数值:0"));
	m_cCurve1Stic.Invalidate(FALSE);
	m_cCurve1Stic.UpdateWindow();

	m_cCurve2Stic.SetWindowTextW(_T("数值:0"));
	m_cCurve2Stic.Invalidate(FALSE);
	m_cCurve2Stic.UpdateWindow();

	m_cCurve3Stic.SetWindowTextW(_T("数值:0"));
	m_cCurve3Stic.Invalidate(FALSE);
	m_cCurve3Stic.UpdateWindow();

	m_cCurve4Stic.SetWindowTextW(_T("数值:0"));
	m_cCurve4Stic.Invalidate(FALSE);
	m_cCurve4Stic.UpdateWindow();

	m_cCurve5Stic.SetWindowTextW(_T("数值:0"));
	m_cCurve5Stic.Invalidate(FALSE);
	m_cCurve5Stic.UpdateWindow();

	m_cCurve6Stic.SetWindowTextW(_T("数值:0"));
	m_cCurve6Stic.Invalidate(FALSE);
	m_cCurve6Stic.UpdateWindow();

	m_cCurve7Stic.SetWindowTextW(_T("数值:0"));
	m_cCurve7Stic.Invalidate(FALSE);
	m_cCurve7Stic.UpdateWindow();

	m_cCurve8Stic.SetWindowTextW(_T("数值:0"));
	m_cCurve8Stic.Invalidate(FALSE);
	m_cCurve8Stic.UpdateWindow();

	m_cChartCtrl.EnableRefresh(false);
	m_cChartCtrl.RemoveAllSeries();
	m_cChartCtrl.EnableRefresh(true);
}

// CRealCurveDlg 导出数据
void CRealCurveDlg::OnBnClickedButtonCurveExportData()
{
	SerialPortCurveExportData();
}

// CRealCurveDlg 导出图片
void CRealCurveDlg::OnBnClickedButtonCurveExportPicture()
{
	SerialPortCurveExportPicture();
}
