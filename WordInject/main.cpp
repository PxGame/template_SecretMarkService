// subinject.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlstr.h>
#include <Tlhelp32.h>
#include <locale.h>


extern "C"
{
#include "..\include\easystruct.h"
#include "..\include\easyhook.h"
};

#include <share.h>
bool WriteToLog(LPCTSTR str)
{

	FILE* pfile;
	_wsetlocale(LC_ALL, L"chs");//设置中文
	TCHAR	szLogPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szLogPath, MAX_PATH);
	PathRemoveFileSpec(szLogPath);
	PathRemoveBackslash(szLogPath);
	wcscat_s(szLogPath, 260, L"\\log.log");

	pfile = _wfsopen(szLogPath, L"a+", _SH_DENYNO);
	if (pfile == NULL)
	{
		return FALSE;
	}
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	fwprintf_s(pfile, L"[%d-%02d-%02d %02d:%02d:%02d] %s\n",
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

BOOL  EnableSpecificPrivilege(IN BOOL bEnable, IN WCHAR * Name)
{
	BOOL bResult = FALSE;
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES TokenPrivileges;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken) == 0)
	{
		return FALSE;
	}

	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
	bResult = LookupPrivilegeValue(NULL, Name, &TokenPrivileges.Privileges[0].Luid);

	if (!bResult)
	{
		CloseHandle(hToken);
		return FALSE;
	}

	bResult = AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);

	if (GetLastError() != ERROR_SUCCESS || !bResult)
	{
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
}



BOOL SetAllPrivileges(IN BOOL bEnable)
{
	EnableSpecificPrivilege(bEnable, SE_ASSIGNPRIMARYTOKEN_NAME);
	EnableSpecificPrivilege(bEnable, SE_AUDIT_NAME);
	EnableSpecificPrivilege(bEnable, SE_BACKUP_NAME);
	EnableSpecificPrivilege(bEnable, SE_CHANGE_NOTIFY_NAME);
	EnableSpecificPrivilege(bEnable, SE_CREATE_PAGEFILE_NAME);
	EnableSpecificPrivilege(bEnable, SE_CREATE_PERMANENT_NAME);
	EnableSpecificPrivilege(bEnable, SE_CREATE_TOKEN_NAME);
	EnableSpecificPrivilege(bEnable, SE_DEBUG_NAME);
	EnableSpecificPrivilege(bEnable, SE_INC_BASE_PRIORITY_NAME);
	EnableSpecificPrivilege(bEnable, SE_INCREASE_QUOTA_NAME);
	EnableSpecificPrivilege(bEnable, SE_LOAD_DRIVER_NAME);
	EnableSpecificPrivilege(bEnable, SE_LOCK_MEMORY_NAME);
	EnableSpecificPrivilege(bEnable, SE_PROF_SINGLE_PROCESS_NAME);
	EnableSpecificPrivilege(bEnable, SE_REMOTE_SHUTDOWN_NAME);
	EnableSpecificPrivilege(bEnable, SE_RESTORE_NAME);
	EnableSpecificPrivilege(bEnable, SE_SECURITY_NAME);
	EnableSpecificPrivilege(bEnable, SE_SHUTDOWN_NAME);
	EnableSpecificPrivilege(bEnable, SE_SYSTEM_ENVIRONMENT_NAME);
	EnableSpecificPrivilege(bEnable, SE_SYSTEM_PROFILE_NAME);
	EnableSpecificPrivilege(bEnable, SE_SYSTEMTIME_NAME);
	EnableSpecificPrivilege(bEnable, SE_TAKE_OWNERSHIP_NAME);
	EnableSpecificPrivilege(bEnable, SE_TCB_NAME);
	EnableSpecificPrivilege(bEnable, SE_UNSOLICITED_INPUT_NAME);
	EnableSpecificPrivilege(bEnable, SE_MACHINE_ACCOUNT_NAME);

	return TRUE;
}

// 注入操作跟服务中的一样
VOID Inject(ULONG hPid)
{
	long status;
	BOOL   bCurretX64, bTargetX64;

	RhIsX64Process(GetCurrentProcessId(), &bCurretX64);
	RhIsX64Process(hPid, &bTargetX64);

	SetAllPrivileges(TRUE);
	TCHAR	szPath[2048] = { 0 };
	GetModuleFileName(NULL, szPath, 2048);

	CString strFileName(szPath);
	strFileName = strFileName.Left(strFileName.ReverseFind(_T('\\')) + 1);

	CString strFileName32 = strFileName + _T("WordProjectx86.dll");
	CString strFileName64 = strFileName + _T("WordProjectx64.dll");
	
	status = RhInjectLibrary(hPid, 0, EASYHOOK_INJECT_DEFAULT, 
		strFileName32.GetBuffer(strFileName32.GetLength()), 
		strFileName64.GetBuffer(strFileName64.GetLength()), 
		NULL, 0);

	if (status == 0)
	{
		WriteToLog(_T("Inject success!"));
	}
	else
	{
		WriteToLog(_T("Inject error!"));
	}
}

// 第一个参数是被注入进程的ID
int _tmain(int argc, _TCHAR* argv[])
{
	//WriteToLog(_T("Main !"));
	/*if( argc < 1 )
	{
	ShellExecuteW(NULL,  _T("open"),_T("explorer.exe"), NULL, NULL, SW_SHOW);
	//WriteToLog(_T("explorer !"));
	CString name = _T("explorer.exe");

	CString ExeFile;
	PROCESSENTRY32 pe;
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL PF=FALSE;
	BOOL result = FALSE;
	for(PF=Process32First(hSnapshot,&pe);PF;PF=Process32Next(hSnapshot,&pe))
	{
	ExeFile = pe.szExeFile;
	if(!ExeFile.CompareNoCase(name))
	{
	id = pe.th32ProcessID;
	Inject(id);
	//break;
	}
	}
	CloseHandle(hSnapshot);
	WriteToLog(_T("explorer Inject!"));

	return 0;
	}

	*/

	ULONG  pid = _wtol(argv[0]);
	//ULONG  pid = _wtol(argv[1]);//Debug

	if (pid)
		Inject(pid);

	return 0;
}
