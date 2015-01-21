// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
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

// Explorer进程ID
extern DWORD	 WinWordPid;