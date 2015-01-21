// EventSink.h
#ifndef __EVENTSINK_H__
#define __EVENTSINK_H__
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#include <comutil.h>
#pragma comment(lib, "wbemuuid.lib")

_COM_SMARTPTR_TYPEDEF(IWbemClassObject, __uuidof(IWbemClassObject));


class EventSink : public IWbemObjectSink
{
	LONG m_lRef;
	bool bDone;

public:
	EventSink() { m_lRef = 0; }
	~EventSink() { bDone = true; }

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();        
	virtual HRESULT 
		STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

	virtual HRESULT STDMETHODCALLTYPE Indicate( 
		LONG lObjectCount,
		IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
		);

	virtual HRESULT STDMETHODCALLTYPE SetStatus( 
		/* [in] */ LONG lFlags,
		/* [in] */ HRESULT hResult,
		/* [in] */ BSTR strParam,
		/* [in] */ IWbemClassObject __RPC_FAR *pObjParam
		);
};

typedef struct _FISB_PROCINFO{
	DWORD	PID;
	TCHAR	szProcPath[MAX_PATH];
}FISB_PROCINFO,*PFISB_PROCINFO;

void GetProcessFullNameByPID(DWORD processID, TCHAR *pathname);

#endif    // end of EventSink.h