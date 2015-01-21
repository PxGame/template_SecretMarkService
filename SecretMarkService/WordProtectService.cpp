// MonProcByWMI.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
/*
ִ��ע��Ĺؼ�������CServiceModule::Run�С����������Ǵ��������ز����ġ�
*/
//////////////////////////////////////////////////////////////////////////


int main(int argc, char ** argv)
{
	// ��ʼ��������
	Dp(_T("BaiYi#main"));
	ServiceModule()->Init(_T("WordProtectService"));
	ServiceModule()->m_bService = TRUE;
	
	if(argc >= 2)
	{
		CString strOpt = argv[1];
		strOpt.MakeLower();

		ServiceModule()->LogEvent(_T("�����������������%s��"),strOpt);
		
		// ִ�з���װ���� ����
		if(_tcscmp(strOpt, _T("install")) == 0)
		{
			ServiceModule()->Install();
			return 0;
		}
		// ִ�з����ж�ز���
		if(_tcscmp(strOpt, _T("uninstall")) == 0)
		{
			ServiceModule()->Uninstall();
			return 0;
		}
		//// ִ�з������������
		//if(_tcscmp(strOpt, _T("-start")) == 0)
		//{
		//	ServiceModule()->StartSevice();
		//	return 0;
		//}
		// ����ģʽ
		if(_tcscmp(strOpt, _T("debug")) == 0)
		{
			ServiceModule()->m_bService = FALSE;
		}
	}
	// �����������
	OutputDebugString(_T("ServiceModule()->StartMain()"));
	ServiceModule()->StartMain();
	
	return 0; 
}

