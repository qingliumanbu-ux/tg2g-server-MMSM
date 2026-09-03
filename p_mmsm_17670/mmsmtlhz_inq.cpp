/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 铬镍收得率明细表
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmtlhz_inq)

int f_mmsmtlhz_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sqlstr_temp1 = " ";
	CString sql_group = " ";
	CString sql_group1 = " ";
	CString end_time = " ";
	CString end_time1 = " ";
	CString devo_time = " ";
	CString heat_no = "";
	CString mat_code = "";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME1"))
			end_time1 = bcls_rec->Tables[0].Rows[0]["END_TIME1"].ToString().TrimOrBlank().ToUpper();
		/*if (bcls_rec->Tables[0].Columns.Contains("DEVO_TIME"))
			devo_time = bcls_rec->Tables[0].Rows[0]["DEVO_TIME"].ToString().TrimOrBlank().ToUpper();*/
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		Log::Info("", __FUNCTION__, "heat_no   =[{0}]", heat_no);
		Log::Info("", __FUNCTION__, "end_time1   =[{0}]", end_time1);
		Log::Info("", __FUNCTION__, "end_time   =[{0}]", end_time);
		Log::Info("", __FUNCTION__, "mat_code   =[{0}]", mat_code);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select a.heat_no,a.mat_code,A.DEVO_WT ,a.DEVO_TIME,A.DEV_CODE,MAX(B.SM_PLAN_NOL2) SM_PLAN_NOL2,max(b.st_no) ST_NO ,"
				"   SUM(CASE WHEN  A.MAT_CODE='TS0000' THEN DEVO_WT ELSE 0 END) MOLTIRON_WT, MAX(CASE WHEN  A.MAT_CODE='TS0000' THEN '普通铁水' ELSE a.MAT_NAME END) MAT_NAME FROM TMMSMGY08 A  "
				" left join(select SM_PLAN_NOL2, heat_no,st_no from tmmsm21) b on b.heat_no = a.heat_no  "
				" where 1 = 1 and A.HEAT_NO<>' ' AND A.MAT_CODE not in ( SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG = '1') "
			    ;
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME >= @end_time";
			}
			if (end_time1.Trim() != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME <= @end_time1";
			}
			if (heat_no.Trim() != "")
			{
				sqlstr_temp += " AND A.HEAT_NO  =@heat_no"; 
			}
			if (mat_code.Trim() != "")
			{
				sqlstr_temp += " AND A.MAT_CODE like '%'|| @mat_code||'%'";
			}
			sql_group = " GROUP BY a.heat_no,a.mat_code,a.MAT_NAME,A.DEVO_WT ,a.DEVO_TIME,A.DEV_CODE  ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time1", end_time1);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("mat_code", mat_code);
			cmd_inq.SetCommandText(sqlstr);
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		
		//物料代码汇总
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr1 = " SELECT MAT_CODE,MAT_NAME, SUM(STOCK_WT)  DEVO_WT  FROM tmmsm89   WHERE  event_code	IN('CHARGE','EXTRA')   ";
			
			if (end_time.Trim() != "")
			{
				Log::Info("", __FUNCTION__, "end_time   =[{0}]", end_time);
				sqlstr_temp1 += " AND EVENT_TIME >= @end_time";
			}
			if (end_time1.Trim() != "")
			{
				sqlstr_temp1 += " AND EVENT_TIME <= @end_time1";
			}
			if (mat_code.Trim() != "")
			{
				sqlstr_temp1 += " AND MAT_CODE like '%'|| @mat_code||'%'";
			}
			sql_group1 = " GROUP BY MAT_CODE,MAT_NAME ";
			sqlstr1 = sqlstr1 + sqlstr_temp1 + sql_group1;
			bcls_ret->Tables.Add();
			cmd_inq1.Parameters.Clear();
			cmd_inq1.Parameters.Set("end_time", end_time);
			cmd_inq1.Parameters.Set("end_time1", end_time1);
			cmd_inq1.Parameters.Set("mat_code", mat_code);
			cmd_inq1.SetCommandText(sqlstr1);
			Log::Info("", __FUNCTION__, "sqlstr1   =[{0}]", sqlstr1);
			
			cmd_inq1.ExecuteQuery(bcls_ret->Tables[1]);
			
			cmd_inq1.Close();
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
