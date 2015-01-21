# include "stdafx.h"
# include "HookWordApi.h"
# include "FileOperations.h"

using namespace std;

struct TracedHookType{
	HOOK_TRACE_INFO		HookHandle;
	ULONG				AclEntries;
};

TracedHookType	TracedHookArry[10];
static PVOID RealFunctionAddressArray[10] = {0};

//CreateFileW---------------------------------------------------------------------------------
typedef
HANDLE (WINAPI *CreateFileWPtr)(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
	);

HANDLE (WINAPI HookCreateFileW)(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
	)
{
	OutputDebugString(L"BaiYi#执行HookCreateFileW");

	WCHAR msg[6+MAX_PATH] = L"BaiYi#";
	StrCatW(msg, lpFileName);
	OutputDebugString(msg);

	//if (JudgeFileIsOk(lpFileName))
	//{
	//	if (!JudgeFileHeadIsExist(lpFileName))
	//	{
	//		if (!RemoveHeadToFile(lpFileName))
	//		{
	//			OutputDebugString(L"BaiYi#RemoveHeadToFile is error !");
	//		}
	//	}
	//}


	HANDLE handle = ((CreateFileWPtr)(RealFunctionAddressArray[0]))(
		lpFileName
		,dwDesiredAccess
		,dwShareMode
		,lpSecurityAttributes
		,dwCreationDisposition
		,dwFlagsAndAttributes
		,hTemplateFile);

	return handle;
}

typedef
NTSTATUS(WINAPI *NtCreateFilePtr)(
	_Out_     PHANDLE FileHandle,
	_In_      ACCESS_MASK DesiredAccess,
	_In_      POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_     PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_  PLARGE_INTEGER AllocationSize,
	_In_      ULONG FileAttributes,
	_In_      ULONG ShareAccess,
	_In_      ULONG CreateDisposition,
	_In_      ULONG CreateOptions,
	_In_      PVOID EaBuffer,
	_In_      ULONG EaLength
	);

NTSTATUS (WINAPI HookNtCreateFile)(
	_Out_     PHANDLE FileHandle,
	_In_      ACCESS_MASK DesiredAccess,
	_In_      POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_     PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_  PLARGE_INTEGER AllocationSize,
	_In_      ULONG FileAttributes,
	_In_      ULONG ShareAccess,
	_In_      ULONG CreateDisposition,
	_In_      ULONG CreateOptions,
	_In_      PVOID EaBuffer,
	_In_      ULONG EaLength
	)
{
	OutputDebugString(L"BaiYi#执行HookNtCreateFile");
	

	NTSTATUS ntStatus = ((NtCreateFilePtr)(RealFunctionAddressArray[1]))(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer,
		EaLength);
	return ntStatus;
}
//OpenFile---------------------------------------------------------------------------------

typedef
HFILE (WINAPI *OpenFilePtr)(
	_In_   LPCSTR lpFileName,
	_Out_  LPOFSTRUCT lpReOpenBuff,
	_In_   UINT uStyle
	);

HFILE (WINAPI HookOpenFile) (
	_In_   LPCSTR lpFileName,
	_Out_  LPOFSTRUCT lpReOpenBuff,
	_In_   UINT uStyle
	)
{
	OutputDebugString(L"BaiYi#执行HookOpenFile");

	CHAR msg[6+MAX_PATH] = "BaiYi#";
	StrCatA(msg, lpFileName);
	OutputDebugStringA(msg);

	HFILE hFile = (OpenFilePtr(RealFunctionAddressArray[2]))(
		lpFileName,
		lpReOpenBuff,
		uStyle);

	return hFile;
}

typedef
NTSTATUS(WINAPI *NtOpenFilePtr)(
	_Out_  PHANDLE FileHandle,
	_In_   ACCESS_MASK DesiredAccess,
	_In_   POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_  PIO_STATUS_BLOCK IoStatusBlock,
	_In_   ULONG ShareAccess,
	_In_   ULONG OpenOptions
	);

NTSTATUS WINAPI HookNtOpenFile(
	_Out_  PHANDLE FileHandle,
	_In_   ACCESS_MASK DesiredAccess,
	_In_   POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_  PIO_STATUS_BLOCK IoStatusBlock,
	_In_   ULONG ShareAccess,
	_In_   ULONG OpenOptions
	)
{
	OutputDebugString(L"BaiYi#执行HookNtOpenFile");
	NTSTATUS ntStatus = ((NtOpenFilePtr)(RealFunctionAddressArray[3]))(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		ShareAccess,
		OpenOptions);
	return ntStatus;
}

// ReadFile


//---------------------------------------------------------------------------------
bool SetWordHook()
{
	OutputDebugString(L"BaiYi#执行SetWordHook");

	HMODULE		hKLib = LoadLibrary(L"Kernel32.dll");
	HMODULE		hNLib = LoadLibrary(L"NtDll.dll");
	NTSTATUS	Status;

	CHAR msg[6+MAX_PATH];

	if (hKLib == NULL || hNLib == NULL)
	{
		OutputDebugString(L"BaiYi#SetWordHook:LoadLibrary is NULL");
		return false;
	}

	RealFunctionAddressArray[0] = GetProcAddress(hKLib, "CreateFileW");
	RealFunctionAddressArray[1] = GetProcAddress(hNLib, "NtCreateFile");
	RealFunctionAddressArray[2] = GetProcAddress(hKLib, "OpenFile");
	RealFunctionAddressArray[3] = GetProcAddress(hNLib, "NtOpenFile");

	for (int i = 0; i < 1 ; i++)
	{
		if (RealFunctionAddressArray[i] != NULL)
		{
			PVOID funAddr = 0;
			switch (i)
			{
			case 0:
				funAddr = HookCreateFileW;
				break;
			case 1:
				funAddr = HookNtCreateFile;
				break;
			case 2:
				funAddr = HookOpenFile;
				break;
			case 3:
				funAddr = HookNtOpenFile;
				break;
			default:
				break;
			}

			Status = LhInstallHook(
				RealFunctionAddressArray[i],
				funAddr,
				NULL,
				&TracedHookArry[i].HookHandle);

			if (Status >= 0)
			{
				// 启用HOOK点
				Status = LhSetExclusiveACL(
					&TracedHookArry[i].AclEntries,
					1,
					&TracedHookArry[i].HookHandle);
			}
			else
			{
				sprintf_s(msg,6+MAX_PATH, "BaiYi#函数%d:LhInstallHook < 0", i);
				OutputDebugStringA(msg);
			}
		}
		else
		{
			sprintf_s(msg,6+MAX_PATH, "BaiYi#函数%d:GetProcAddress is NULL", i);
			OutputDebugStringA(msg);
		}
	}

	FreeLibrary(hKLib);
	FreeLibrary(hNLib);
	return true;
}