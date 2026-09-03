/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:songwei
Date:2023-11-28
Version:1.0
Description: 接收资源系统
物料主数据**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE_TELE(cm_a02101_rcv)

int f_cm_a02101_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CModel tmmsm50("TMMSM50");
	//CString div_flag = "";
	//CString matPlmsCode = "";
	//CString matCode = "";
	//CDbCommand cmd_inq(conn);
	//CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
