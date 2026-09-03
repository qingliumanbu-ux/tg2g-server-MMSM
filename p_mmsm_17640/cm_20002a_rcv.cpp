/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2014-12-13
Version:1.0
Description: 接收PES炼钢合金消耗实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/ 




/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 接收PES炼钢合金消耗实绩
/// <para>
/// 接收PES炼钢合金消耗实绩并处理
/// </para>
/// </summary>
/// <param name="tmmsm2a">炼钢合金消耗实绩</param>
/// <param name="PROC_DIV">处理标记</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE_TELE(cm_20002a_rcv)


int f_cm_20002a_rcv(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString ch_proc_div = "";
 	CString ch_time = "";
	CString sqlstr = "";
	CString delCondition = "";

	CModel tmmsm2a("TMMSM2A");

	try
	{
		ch_time = CDateTime::Now().ToString("yyyyMMddHHmmss");

		tmmsm2a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm2a.Print();

		ch_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();

		//Log::Trace("", __FUNCTION__, "ch_proc_div=[{0}]]",ch_proc_div);
			//Log::Trace("", __FUNCTION__, "tmmsm2a["PROC_COUNT"] =[{0}]]",tmmsm2a["PROC_COUNT"].ToDecimal());

		
		if(tmmsm2a["PROC_COUNT"].ToDecimal() == 0)
		{
			delCondition = "PROC_NO,MAT_CODE,HEAT_NO";
		}
		else
		{
			delCondition = "PROC_NO,MAT_CODE,HEAT_NO,PROC_COUNT";
		}

		tmmsm2a.Delete(delCondition);

		if (ch_proc_div != "D")/*电炉作业实绩新增*/
		{
			tmmsm2a["REC_CREATOR"] = "20002A";
			tmmsm2a["REC_CREATE_TIME"] = ch_time;

			tmmsm2a.Insert();
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



  
