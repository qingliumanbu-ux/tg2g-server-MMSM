/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      MFJ
Version:     1.0
Date:        2024.3.27
Description: 自循环废钢查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(mmsm68_inq)


int f_mmsm68_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString prod_time_f = "";
	CString prod_time_t = "";
	CDbCommand cmd(conn);
	try
	{
		CString mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		CString mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME"].ToString().Trim();
		CString ship_name = bcls_rec->Tables[0].Rows[0]["SHIP_NAME"].ToString().Trim();
		//2025.09.18 查询条件增加日期PROD_DATE
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_F"))
		prod_time_f = bcls_rec->Tables[0].Rows[0]["PROD_TIME_F"].ToString().Substring(0,8);
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_T"))
		prod_time_t = bcls_rec->Tables[0].Rows[0]["PROD_TIME_T"].ToString().Substring(0,8);


		Log::Trace("", "", "mat_code = {0}", mat_code);
		Log::Trace("", "", "mat_name = {0}", mat_name);
		Log::Trace("", "", "ship_name = {0}", ship_name);

		sqlstr = "SELECT * FROM  TMMSM68 WHERE 1=1 ";
		if (mat_code != "")
		{
			sqlstr += " AND  MAT_CODE = @mat_code";
		}
		if (mat_name != "")
		{
			sqlstr += " AND  MAT_NAME = @mat_name";
		}
		if (ship_name != "")
		{
			sqlstr += " AND  SHIP_NAME = @ship_name";
		}
		if (prod_time_f.Trim() != "")
		{
			sqlstr += " AND PROD_DATE >= '" + prod_time_f + "'";
		}
		if (prod_time_t.Trim() != "")
		{
			sqlstr += " AND PROD_DATE <= '" + prod_time_t + "'";
		}

		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("mat_code", mat_code);
		cmd.Parameters.Set("mat_name", mat_name);
		cmd.Parameters.Set("ship_name", ship_name);
		cmd.Parameters.Set("prod_time_f", prod_time_f);
		cmd.Parameters.Set("prod_time_t", prod_time_t);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


