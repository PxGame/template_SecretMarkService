// MonProcByWMI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
/*
执行注入的关键代码在CServiceModule::Run中。主函数中是处理服务相关操作的。
*/
//////////////////////////////////////////////////////////////////////////


int main(int argc, char ** argv)
{
	// 初始化服务名
	Dp(_T("BaiYi#main"));
	ServiceModule()->Init(_T("WordProtectService"));
	ServiceModule()->m_bService = TRUE;
	
	if(argc >= 2)
	{
		CString strOpt = argv[1];
		strOpt.MakeLower();

		ServiceModule()->LogEvent(_T("服务程序启动参数【%s】"),strOpt);
		
		// 执行服务安装操作 启动
		if(_tcscmp(strOpt, _T("install")) == 0)
		{
			ServiceModule()->Install();
			return 0;
		}
		// 执行服务的卸载操作
		if(_tcscmp(strOpt, _T("uninstall")) == 0)
		{
			ServiceModule()->Uninstall();
			return 0;
		}
		//// 执行服务的启动操作
		//if(_tcscmp(strOpt, _T("-start")) == 0)
		//{
		//	ServiceModule()->StartSevice();
		//	return 0;
		//}
		// 调试模式
		if(_tcscmp(strOpt, _T("debug")) == 0)
		{
			ServiceModule()->m_bService = FALSE;
		}
	}
	// 服务的主函数
	OutputDebugString(_T("ServiceModule()->StartMain()"));
	ServiceModule()->StartMain();
	
	return 0; 
}

