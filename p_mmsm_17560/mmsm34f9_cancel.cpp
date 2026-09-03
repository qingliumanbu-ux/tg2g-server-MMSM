/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-8-13
Version:1.0
Description: 炼钢板坯修磨实绩删除
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 




/* ***** 静态函数申明 ***** */

//修改板坯主档信息
int f_mmsm3401_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);

/*<remark>=========================================================
/// <summary>
/// 炼钢板坯修磨实绩删除
/// <para>
/// 删除炼钢板坯修磨实绩
/// </para>
/// </summary>
/// <param name="MAT_NO">材料号</param>
/// <param name="HSF_END_TIME">修磨结束时间</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm34f9_cancel)


int f_mmsm34f9_cancel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cutFinFlag = "";
  
	CModel tmmsm34("TMMSM34");
	CModel tmmsm01("TMMSM01");

	try
	{
		tmmsm34.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (!bcls_rec->Tables.Contains("MMSM34"))
		{
			bcls_rec->Tables.Add("MMSM34");
		}
		if (!bcls_rec->Tables["MMSM34"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSM34"].Columns.Add(DT_STRING, "PROC_DIV");
		}
		tmmsm34.MergeTo(bcls_rec->Tables["MMSM34"], false);
		bcls_rec->Tables["MMSM34"].Rows[0]["PROC_DIV"] = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();/*I:新增 U:修改 D:删除*/
		doFlag = f_mmsm3401_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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
