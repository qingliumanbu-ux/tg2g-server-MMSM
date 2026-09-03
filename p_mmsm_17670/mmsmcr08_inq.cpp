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
BM2F_ENTERACE(mmsmcr08_inq)

int f_mmsmcr08_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString end_time = " ";
	CString end_time_1 = " ";

	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME_1"].ToString().TrimOrBlank().ToUpper();
		end_time += "000000";
		end_time_1 += "235959";
		Log::Info("", __FUNCTION__, "end_time_1   =[{0}]", end_time_1);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT A.HEAT_NO,MAX(A.REC_CREATOR) DAMIN,MAX(A.DEVO_TIME) DATE_1,SUM(A.DEVO_WT) DEVO_WT, "
				" MAX(T.DEVO_WT_MES) DEVO_WT_MES,SUM(A.DEVO_WT)-MAX(T.DEVO_WT_MES) CY FROM TMMSMGY08 A  "
				" LEFT JOIN(SELECT sum(DEVO_WT) DEVO_WT_MES, HEAT_NO FROM tmmsm56 group by HEAT_NO) T ON T.HEAT_NO=A.HEAT_NO  "
			
			    " where 1=1 ";
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME <= @end_time_1";
			}
			sql_group = " group by  A.HEAT_NO  ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time_1", end_time_1);
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
