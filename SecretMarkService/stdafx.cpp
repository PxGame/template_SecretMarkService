// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// MonProcByWMI.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// ��ý��̵��û��ʺ���
BOOL QueryUserName(DWORD Pid, CString& UserName)
{
	HANDLE hToken;
	HANDLE hProcess;
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,Pid);
	if (!OpenProcessToken(hProcess,TOKEN_READ,&hToken) || hToken == NULL)
	{
		DWORD dwByte = GetLastError();
		return FALSE;
	}

	DWORD Size = 0;
	if(!GetTokenInformation(hToken, TokenUser, NULL, 0 , &Size))
	{
		if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return FALSE;
		PTOKEN_USER pTokenUser = (PTOKEN_USER)new UCHAR[Size];
		if(!pTokenUser)
			return FALSE;

		if(GetTokenInformation(hToken , TokenUser , pTokenUser , Size , &Size))
		{
			Size = MAX_PATH;
			TCHAR User[MAX_PATH];
			TCHAR Domain[MAX_PATH];
			SID_NAME_USE SidType;
			if(LookupAccountSid(NULL, pTokenUser->User.Sid, User, &Size, Domain, &Size, &SidType))
			{
				UserName += User;
			}
		}
		delete[] (PUCHAR)pTokenUser;
	}
	return TRUE;
}

// �ɽ�������ý���ID
bool GetProcessPid(TCHAR *pszProcessName, vector<DWORD>& dwPids)
{
	DWORD Pid = -1;
	if ( pszProcessName == NULL ) return false;

	HANDLE hFindhandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if ( hFindhandle ==INVALID_HANDLE_VALUE )
		return false;

	PROCESSENTRY32* ptagProcessInfo = new PROCESSENTRY32;
	ptagProcessInfo->dwSize = sizeof(PROCESSENTRY32);

	if ( Process32First(hFindhandle,ptagProcessInfo) ) 
	{
		TCHAR *p= NULL;
		p = ptagProcessInfo->szExeFile;
		printf("%d\t%S;\r\n",ptagProcessInfo->th32ProcessID,ptagProcessInfo->szExeFile);
		if ( 0 == _tcsicmp(pszProcessName,p) )
		{
			Pid = ptagProcessInfo->th32ProcessID;
			//CloseHandle(hFindhandle);//�رս��̷�J�����ִ���
			//if ( ptagProcessInfo ) 
			//{
			//	delete ptagProcessInfo;
			//	ptagProcessInfo = NULL;
			//}
			//return Pid;
			dwPids.push_back(Pid);
		}
		p = NULL;
		while( Process32Next(hFindhandle,ptagProcessInfo) != FALSE )
		{
			p = ptagProcessInfo->szExeFile;
			printf("%d\t%S;\r\n",ptagProcessInfo->th32ProcessID,ptagProcessInfo->szExeFile);
			if ( 0 == _tcsicmp(pszProcessName,p) )
			{
				Pid = ptagProcessInfo->th32ProcessID;
				dwPids.push_back(Pid);

			}
		}

	}

	CloseHandle(hFindhandle);//�رս��̷�J�����ִ���
	if ( ptagProcessInfo ) 
	{
		delete ptagProcessInfo;
		ptagProcessInfo = NULL;
	}

	return true;
}