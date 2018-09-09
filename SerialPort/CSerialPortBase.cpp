#include "CSerialPortBase.h"

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