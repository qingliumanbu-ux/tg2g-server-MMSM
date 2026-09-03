/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-28
Version:1.0
Description: 精整相关信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
通过关键字，查询小代码信息
/// <returns>板坯信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsmfetch_shift_group)


int f_mmsmfetch_shift_group(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int rowCount = 0;
	CString sqlstr = "";
	CString div = "";
	CString time = "";
	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";
	CDbCommand cmd_sql(conn); //与DB 建立连接。


	try
	{	
		if (bcls_rec->Tables[0].Columns.Contains("DIV"))
		{
			div = bcls_rec->Tables[0].Rows[0]["DIV"].ToString();
		}

		if (bcls_rec->Tables[0].Columns.Contains("REC_CREATE_TIME"))
		{

			time = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME"].ToString();
			Log::Trace("", "", "time={0}", time);


		}
		if (div!=""&&time!=""){
			//库区号 小代码			
			f_epep_get_shift_group(div, time, PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);

			bcls_ret->Tables[0].Columns.Add(DT_STRING, "PROD_SHIFT_NO");
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "PROD_SHIFT_GROUP");

			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
			bcls_ret->Tables[0].Rows[0]["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;

		}
		else
		{
			Log::Trace("", "", "time={0}", "aa");
		}
			
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
