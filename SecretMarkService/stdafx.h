// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlstr.h>
#include <Tlhelp32.h>
#include <psapi.h>
#include <vector>
#pragma comment(lib,"psapi.lib")
using namespace std;
extern "C"
{
#include "../include/easystruct.h"
#include "../include/easyhook.h"
};

#include "Func.h"
#include "EventSink.h"
#include "ServiceModule.h"

BOOL	QueryUserName(DWORD Pid, CString& UserName);
int		RegisterMonitorFun();
bool GetProcessPid(TCHAR *pszProcessName, vector<DWORD>& dwPids);

#ifndef _DEBUG
#define Dp(x) WriteToLog(x)
#else
#define Dp(x) OutputDebugString(x)
#endif

// Explorer����ID
extern DWORD	 WinWordPid;