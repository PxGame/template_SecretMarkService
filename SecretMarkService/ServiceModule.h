#pragma once
#include <Winsvc.h>
#pragma comment(lib, "Advapi32.lib")

class CServiceModule  
{
public:
	
	virtual ~CServiceModule();

public:
	TCHAR m_szServiceName[256];
	TCHAR m_szLogFilePath[256];
	SERVICE_STATUS_HANDLE m_hServiceStatus;
	SERVICE_STATUS m_status;

	BOOL m_bService;
	BOOL m_bExit;
	HANDLE m_hStopNotify;

public:
	void Init(LPCTSTR lpszServiceName);
	void StartMain();
	void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	void Handler(DWORD dwOpcode);
	void LogEvent(LPCTSTR pszFormat, ...);
	void LogEvent2(LPCTSTR pszFormat, ...);
	void SetServiceStatus(DWORD dwState);
	void Run(DWORD dwArgc, LPTSTR* lpszArgv);
	int  RegisterMonitorFun();

	BOOL IsInstalled();
	BOOL Install();
	BOOL Uninstall();
	BOOL StartSevice();//暂时没用  因为Install 就已Start

	bool WriteToLog(LPCTSTR str);
	static CServiceModule* Instance()
	{
		static CServiceModule _Obj;
		return &_Obj;
	}
private:
	CServiceModule();
private:
	static void WINAPI _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	static void WINAPI _Handler(DWORD dwOpcode);
	
};

#define  ServiceModule()	CServiceModule::Instance()