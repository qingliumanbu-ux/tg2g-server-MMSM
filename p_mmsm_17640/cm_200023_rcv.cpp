/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2015-01-13
Version:1.0
Description: 接收PESRH作业实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/ 



/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 接收PESRH作业实绩
/// <para>
/// 接收PESRH作业实绩并处理
/// </para>
/// </summary>
/// <param name="tmmsm23">RH作业实绩</param>
/// <param name="PROC_DIV">处理标记</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE_TELE(cm_200023_rcv)


int f_cm_200023_rcv(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString ch_proc_div = "";
 	CString ch_time = "";
	CString sqlstr = "";

	CModel tmmsm23("TMMSM23");

	try
	{
		ch_time = CDateTime::Now().ToString("yyyyMMddHHmmss");

		tmmsm23.MergeFrom(bcls_rec->Tables[0].Rows[0]);
  	//Log::Trace("",__FUNCTION__,"tmmsm23["PROC_NO"] = [{0}]",(const char*)tmmsm23["PROC_NO"].ToString());
		ch_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		//Log::Trace("",__FUNCTION__,"ch_proc_div= [{0}]",(const char*)ch_proc_div);

		tmmsm23.Delete("PROC_NO");

		if (ch_proc_div != "D")
		{
			tmmsm23["REC_CREATOR"] = "200023";
			tmmsm23["REC_CREATE_TIME"] = ch_time;

			//tmmsm23.Print();

			tmmsm23.Insert();
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,"数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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



 
