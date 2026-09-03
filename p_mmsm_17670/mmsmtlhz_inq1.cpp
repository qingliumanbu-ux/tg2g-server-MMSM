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
BM2F_ENTERACE(mmsmtlhz_inq1)

int f_mmsmtlhz_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString end_time = " ";
	CString end_time1 = " ";
	CString devo_time = " ";
	CString devo_time1 = " ";
	CString devo_time2 = " ";
	CString heat_no = "";
	CDbCommand cmd_inq(conn);

	try
	{
		
		if (bcls_rec->Tables.get_Count()>0)
		{
			if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
				heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
			
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME1"))
			end_time1 = bcls_rec->Tables[0].Rows[0]["END_TIME1"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
	
		Log::Info("", __FUNCTION__, "heat_no   =[{0}]", heat_no);
		Log::Info("", __FUNCTION__, "end_time1   =[{0}]", end_time1);
		Log::Info("", __FUNCTION__, "end_time   =[{0}]", end_time);
		
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT A.MAT_CODE,MAX(CASE WHEN  A.MAT_CODE='TS0000' THEN '普通铁水' ELSE a.MAT_NAME END) MAT_NAME,SUM(A.DEVO_WT) DEVO_WT FROM TMMSMGY08 A  "
				
				" where 1 = 1  AND HEAT_NO = @HEAT_NO  AND A.MAT_CODE not in ( SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG = '1') "
			    ;
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME >= @end_time";
			}
			if (end_time1.Trim() != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME <= @end_time1";
			}
			sql_group = " GROUP BY A.MAT_CODE  ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			cmd_inq.Parameters.Set("HEAT_NO", heat_no);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time1", end_time1);
			cmd_inq.SetCommandText(sqlstr);
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
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
