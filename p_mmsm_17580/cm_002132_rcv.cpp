/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author: lizhen
Date:2015-01-22
Version:1.0
Description: empty 注意这是空配置程序，无任何作用！
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"



// Service 入口 这部分的宏定义将来还可能修改
BM2F_ENTERACE_TELE(cm_002132_rcv);
int f_cm_002132_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
	int  doFlag = 0;
	int ret = 0;
	CString sqlstr("");
	try
	{

		sprintf(s.msg, "注意这是空配置程序，无任何作用！");

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)
	{
		doFlag = ex.GetCode();
		strcpy(s.msg, (const char*)ex.GetMsg());
	}
	catch (const CException& ex)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)ex.GetMsg());
	}

	s.flag = doFlag;

	return(doFlag);
}
