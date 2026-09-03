/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面维护
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmtlcf_inq1)

int f_mmsmtlcf_inq1(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int blkNum = 0;
	int doFlag = 0;
	CString s_formname = "";
	CString v_proc_div = "";
	CString sqlstr = "";
	CString sqlstr_temp = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tmmsm19("TMMSM19");
	CModel tmmsm20("TMMSM20");

	try
	{

		
		sqlstr = " SELECT * FROM TMMSMWQ_LL WHERE 1=1  ";
		if (bcls_rec->Tables[0].Rows[0]["DATE_FROM"].ToString().Trim() != "")
		{
			sqlstr+= " AND PROD_DATE >='" + bcls_rec->Tables[0].Rows[0]["DATE_FROM"].ToString() + "'";
		}
		if (bcls_rec->Tables[0].Rows[0]["DATE_TO"].ToString().Trim() != "")
		{
			sqlstr+= " AND PROD_DATE <='" + bcls_rec->Tables[0].Rows[0]["DATE_TO"].ToString() + "'";
		}
		if (bcls_rec->Tables[0].Rows[0]["C_STATE"].ToString().Trim() != "")
		{
			sqlstr+= " AND C_STATE='" + bcls_rec->Tables[0].Rows[0]["C_STATE"].ToString() + "'";
		}

		if (bcls_rec->Tables[0].Rows[0]["PROD_DATE"].ToString().Trim()!="")
		{
			sqlstr += " and PROD_DATE='" + bcls_rec->Tables[0].Rows[0]["PROD_DATE"].ToString() + "' ";
		}
		if (bcls_rec->Tables[0].Rows[0]["LOT_NO"].ToString().Trim() != "")
		{
			sqlstr += " and LOT_NO='" + bcls_rec->Tables[0].Rows[0]["LOT_NO"].ToString() + "' ";
		}
		sqlstr += " ORDER BY REC_CREATE_TIME DESC ";
		Log::Trace("", __FUNCTION__, "FROM =[{0}]", bcls_rec->Tables[0].Rows[0]["DATE_FROM"].ToString());
		Log::Trace("", __FUNCTION__, "TO =[{0}]", bcls_rec->Tables[0].Rows[0]["DATE_TO"].ToString());
		Log::Trace("", __FUNCTION__, "STATE =[{0}]", bcls_rec->Tables[0].Rows[0]["C_STATE"].ToString());
		Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		
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
