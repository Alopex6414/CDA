#include "CSerialPortBase.h"

bool CCSerialPortBase::m_sbExit = false;

// CCSerialPortBase 构造函数
CCSerialPortBase::CCSerialPortBase()
{
	m_hCOM = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;

	InitializeCriticalSection(&m_csCOMSync);
}

// CCSerialPortBase 析构函数
CCSerialPortBase::~CCSerialPortBase()
{
	if (INVALID_HANDLE_VALUE != m_hListenThread)
	{
		::CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}

	if (INVALID_HANDLE_VALUE != m_hCOM)
	{
		::CloseHandle(m_hCOM);
		m_hCOM = INVALID_HANDLE_VALUE;
	}

	DeleteCriticalSection(&m_csCOMSync);
}

// CCSerialPortBase 枚举串口(注册表)
void CCSerialPortBase::EnumSerialPort()
{
	HKEY hKey;
	LSTATUS ls;

	ls = RegOpenKeyExW(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"), NULL, KEY_READ, &hKey);
	if (ERROR_SUCCESS == ls)
	{
		TCHAR szPortName[256] = { 0 };
		TCHAR szComName[256] = { 0 };
		DWORD dwLong = 0;
		DWORD dwSize = 0;
		int nCount = 0;

		m_mapEnumCOM.clear();

		while (true)
		{
			LSTATUS ls2;

			dwLong = dwSize = 256;
			ls2 = RegEnumValueW(hKey, nCount, szPortName, &dwLong, NULL, NULL, (PUCHAR)szComName, &dwSize);
			if (ERROR_NO_MORE_ITEMS == ls2)
			{
				break;
			}

			int iLen = WideCharToMultiByte(CP_ACP, 0, szComName, -1, NULL, 0, NULL, NULL);
			char* chRtn = new char[iLen + 1];
			memset(chRtn, 0, iLen + 1);
			WideCharToMultiByte(CP_ACP, 0, szComName, -1, chRtn, iLen, NULL, NULL);

			string str(chRtn);
			m_mapEnumCOM.insert(pair<int, string>(nCount, str));
			SafeDeleteArray(chRtn);
			nCount++;
		}

		RegCloseKey(hKey);
	}

}

// CCSerialPortBase 打开串口
bool CCSerialPortBase::CCSerialPortBaseCreate(const char* szPort)
{
	DWORD dwError;

	EnterCriticalSection(&m_csCOMSync);

	m_hCOM = CreateFileA(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (INVALID_HANDLE_VALUE == m_hCOM)
	{
		dwError = GetLastError();
		LeaveCriticalSection(&m_csCOMSync);
		return false;
	}

	LeaveCriticalSection(&m_csCOMSync);
	return true;
}

// CCSerialPortBase 配置串口
bool CCSerialPortBase::CCSerialPortBaseConfig(S_SERIALPORT_PROPERTY sCommProperty)
{
	BOOL bRet = FALSE;

	EnterCriticalSection(&m_csCOMSync);

	// 设置输入输出缓冲区
	bRet = SetupComm(m_hCOM, SERIALPORT_COMM_INPUT_BUFFER_SIZE, SERIALPORT_COMM_OUTPUT_BUFFER_SIZE);
	if (!bRet)
	{
		LeaveCriticalSection(&m_csCOMSync);
		return false;
	}

	// 设置DCB结构体
	DCB dcb = { 0 };
	
	bRet = GetCommState(m_hCOM, &dcb);
	if (!bRet)
	{
		LeaveCriticalSection(&m_csCOMSync);
		return false;
	}

	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = sCommProperty.dwBaudRate;
	dcb.ByteSize = sCommProperty.byDataBits;
	dcb.StopBits = sCommProperty.byStopBits;
	dcb.Parity = sCommProperty.byCheckBits;
	bRet = SetCommState(m_hCOM, &dcb);
	if (!bRet)
	{
		LeaveCriticalSection(&m_csCOMSync);
		return false;
	}

	// 设置串口超时时间
	COMMTIMEOUTS ct = { 0 };
	ct.ReadIntervalTimeout = 0;
	ct.ReadTotalTimeoutMultiplier = 0;
	ct.ReadTotalTimeoutConstant = 0;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;
	bRet = SetCommTimeouts(m_hCOM, &ct);
	if (!bRet)
	{
		LeaveCriticalSection(&m_csCOMSync);
		return false;
	}

	// 清空串口缓冲区
	bRet = PurgeComm(m_hCOM, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!bRet)
	{
		LeaveCriticalSection(&m_csCOMSync);
		return false;
	}

	LeaveCriticalSection(&m_csCOMSync);
	return true;
}

// CCSerialPortBase 初始化串口
bool CCSerialPortBase::CCSerialPortBaseInit(S_SERIALPORT_PROPERTY sCommProperty)
{
	bool bRet = false;

	bRet = CCSerialPortBaseCreate(sCommProperty.chPort);
	if (!bRet)
	{
		return false;
	}

	bRet = CCSerialPortBaseConfig(sCommProperty);
	if (!bRet)
	{
		return false;
	}

	return true;
}

// CCSerialPortBase 初始化串口监听
bool CCSerialPortBase::CCSerialPortBaseInitListen()
{
	if (INVALID_HANDLE_VALUE != m_hListenThread)
	{
		return false;
	}

	m_sbExit = false;

	unsigned int uThreadID;

	m_hListenThread = (HANDLE)::_beginthreadex(NULL, 0, (_beginthreadex_proc_type)OnReceiveBuffer, this, 0, &uThreadID);
	if (!m_hListenThread)
	{
		return false;
	}

	BOOL bRet = FALSE;
	bRet = ::SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL);
	if (!bRet)
	{
		return false;
	}

	return true;
}

// CCSerialPortBase 关闭串口
void CCSerialPortBase::CCSerialPortBaseClose()
{
	if (INVALID_HANDLE_VALUE != m_hCOM)
	{
		::CloseHandle(m_hCOM);
		m_hCOM = INVALID_HANDLE_VALUE;
	}
}

// CCSerialPortBase 关闭串口监听
void CCSerialPortBase::CCSerialPortBaseCloseListen()
{
	if (INVALID_HANDLE_VALUE != m_hListenThread)
	{
		m_sbExit = true;
		::WaitForSingleObject(m_hListenThread, INFINITE);
		::CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}
}

// CCSerialPortBase 串口接收线程
unsigned int CALLBACK CCSerialPortBase::OnReceiveBuffer(LPVOID lpParameters)
{
	CCSerialPortBase* pCSerialPortBase = reinterpret_cast<CCSerialPortBase*>(lpParameters);

	return 0;
}
