/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2015-2-13
Version:1.0
Description: 接收PES炼钢测温实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/ 



/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 接收PES炼钢测温实绩
/// <para>
/// 接收PES炼钢测温实绩并处理
/// </para>
/// </summary>
/// <param name="tmmsm2b">炼钢测温实绩</param>
/// <param name="PROC_DIV">处理标记</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE_TELE(cm_20002b_rcv)


int f_cm_20002b_rcv(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString ch_proc_div = "";
 	CString ch_time = "";
	CString sqlstr = "";

	CModel tmmsm2b("TMMSM2B");

	try
	{
		ch_time = CDateTime::Now().ToString("yyyyMMddHHmmss");

		tmmsm2b.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		ch_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();

		tmmsm2b.Delete("PROC_NO,PROC_COUNT");

		if (ch_proc_div != "D")/*电炉作业实绩新增*/
		{
			tmmsm2b["REC_CREATOR"] = "20002B";
			tmmsm2b["REC_CREATE_TIME"] = ch_time;

			tmmsm2b.Insert();
		}

 
	 }
     catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}



  
