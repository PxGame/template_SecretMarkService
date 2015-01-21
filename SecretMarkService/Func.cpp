#include "StdAfx.h"
#include "Func.h"
#include <atlbase.h>
#include <atlstr.h>
#include <Userenv.h>


CFunc::CFunc(void)
{
}


CFunc::~CFunc(void)
{
}

// ����������Ȩ
BOOL CFunc::SetAllPrivileges(IN BOOL bEnable)
{
	EnableSpecificPrivilege(bEnable,SE_ASSIGNPRIMARYTOKEN_NAME);
	EnableSpecificPrivilege(bEnable,SE_AUDIT_NAME);
	EnableSpecificPrivilege(bEnable,SE_BACKUP_NAME);
	EnableSpecificPrivilege(bEnable,SE_CHANGE_NOTIFY_NAME);
	EnableSpecificPrivilege(bEnable,SE_CREATE_PAGEFILE_NAME);
	EnableSpecificPrivilege(bEnable,SE_CREATE_PERMANENT_NAME);
	EnableSpecificPrivilege(bEnable,SE_CREATE_TOKEN_NAME);
	EnableSpecificPrivilege(bEnable,SE_DEBUG_NAME);
	EnableSpecificPrivilege(bEnable,SE_INC_BASE_PRIORITY_NAME);
	EnableSpecificPrivilege(bEnable,SE_INCREASE_QUOTA_NAME);
	EnableSpecificPrivilege(bEnable,SE_LOAD_DRIVER_NAME);
	EnableSpecificPrivilege(bEnable,SE_LOCK_MEMORY_NAME);
	EnableSpecificPrivilege(bEnable,SE_PROF_SINGLE_PROCESS_NAME);
	EnableSpecificPrivilege(bEnable,SE_REMOTE_SHUTDOWN_NAME);
	EnableSpecificPrivilege(bEnable,SE_RESTORE_NAME);
	EnableSpecificPrivilege(bEnable,SE_SECURITY_NAME);
	EnableSpecificPrivilege(bEnable,SE_SHUTDOWN_NAME);
	EnableSpecificPrivilege(bEnable,SE_SYSTEM_ENVIRONMENT_NAME);
	EnableSpecificPrivilege(bEnable,SE_SYSTEM_PROFILE_NAME);
	EnableSpecificPrivilege(bEnable,SE_SYSTEMTIME_NAME);
	EnableSpecificPrivilege(bEnable,SE_TAKE_OWNERSHIP_NAME);
	EnableSpecificPrivilege(bEnable,SE_TCB_NAME);
	EnableSpecificPrivilege(bEnable,SE_UNSOLICITED_INPUT_NAME);
	EnableSpecificPrivilege(bEnable,SE_MACHINE_ACCOUNT_NAME);

	return TRUE;
}

// ����ĳһ����Ȩ
BOOL CFunc::EnableSpecificPrivilege(IN BOOL bEnable, IN WCHAR * Name)
{
	BOOL bResult = FALSE;
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES TokenPrivileges;

	if( OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,&hToken) == 0 )
	{
		return FALSE;
	}

	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
	bResult = LookupPrivilegeValue(NULL,Name,&TokenPrivileges.Privileges[0].Luid);

	if( !bResult )
	{
		CloseHandle(hToken);
		return FALSE;
	}

	bResult = AdjustTokenPrivileges(hToken,FALSE,&TokenPrivileges,sizeof(TOKEN_PRIVILEGES),NULL,NULL);

	if(GetLastError() != ERROR_SUCCESS || !bResult)
	{
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
}

// ִ��ע�����
VOID CFunc::Inject( ULONG hPid )
{
	
//	long status;
	BOOL   bTargetX64;
	// ���Խ����Ƿ�64λ����
	RhIsX64Process( hPid, &bTargetX64 );
	TCHAR	szPath[2048] = {0};
	GetModuleFileName(NULL,szPath,2048);
	CString strPid;
	strPid.Format(_T("%d"), hPid);
	ServiceModule()->WriteToLog(strPid+_T(" inject"));
	// ��õ�ǰ��������Ŀ¼
	CString strFileName(szPath);
	CString strSubExe;
	strFileName = strFileName.Left(strFileName.ReverseFind(_T('\\'))+1);

	// ���ݽ��̲�ͬ���ò�ͬ��ע�����
	if( bTargetX64 )
	{
		strSubExe = strFileName + _T("WordInjectx64.exe");
	}
	else
	{
		strSubExe = strFileName + _T("WordInjectx86.exe");
	}

	// ִ��ע�����
	STARTUPINFO si;    
	PROCESS_INFORMATION pi;    
	ZeroMemory(&si,sizeof(STARTUPINFO));    
	ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));    
	si.cb = sizeof(STARTUPINFO);      
	si.lpReserved = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;
	if (!CreateProcess(strSubExe.GetBuffer(),strPid.GetBuffer(),NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))   
	{   
		int error2 = GetLastError();   
		return;   
	} 
	/*
	CString strFileName32 = strFileName + _T("FileProtect32.dll");
	CString strFileName64 = strFileName + _T("FileProtect64.dll");

	NTSTATUS status  = RhInjectLibrary(hPid, 0, EASYHOOK_INJECT_DEFAULT,strFileName32.GetBuffer(0), strFileName64.GetBuffer(0),NULL,0);
	CString sCode;
	sCode.Format(_T("RhInjectLibrary return %l code %d"), status, GetLastError());
	ServiceModule()->WriteToLog(sCode);*/
}