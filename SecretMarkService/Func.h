#pragma once


class CFunc
{
public:
	static CFunc* Instance(){
		static CFunc _Obj;
		return &_Obj;
	}
	~CFunc(void);
private:
	CFunc(void);
public:
	BOOL SetAllPrivileges(IN BOOL bEnable);
	BOOL EnableSpecificPrivilege(IN BOOL bEnable,IN WCHAR * Name);
	VOID Inject( ULONG hPid );
};

#define  Func()		CFunc::Instance()

