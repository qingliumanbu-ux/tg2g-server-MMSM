/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 能源消耗查询数据
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmt8m0s1_inq)

int f_mmsmt8m0s1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString mete_code_2 = " ";
	CString factory_1 = " ";
	CString date_time = " ";
	CString div = " ";
	CString mete_name = " ";
	CString date_time_1 = " ";

	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[1].Columns.Contains("DIV"))
			div = bcls_rec->Tables[1].Rows[0]["DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("METE_CODE_2"))
			mete_code_2 = bcls_rec->Tables[0].Rows[0]["METE_CODE_2"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_1"))
			factory_1 = bcls_rec->Tables[0].Rows[0]["FACTORY_1"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("DATE_TIME"))
			date_time = bcls_rec->Tables[0].Rows[0]["DATE_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("METE_NAME"))
			mete_name = bcls_rec->Tables[0].Rows[0]["METE_NAME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("DATE_TIME_1"))
			date_time_1 = bcls_rec->Tables[0].Rows[0]["DATE_TIME_1"].ToString().TrimOrBlank().ToUpper();
		Log::Info("", __FUNCTION__, "date_time   =[{0}]", date_time);
		Log::Info("", __FUNCTION__, "factory_1   =[{0}]", factory_1);
		if (div == "H"){
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = " select A.FACTORY_1,B.METE_CODE_2,B.METE_NAME,B.DATE_TIME,B.MIRL,B.MIRT from TMMSMT8S1 B LEFT JOIN TMMSMT8METE A ON A.METE_CODE_2=B.METE_CODE_2 "
					" WHERE 1 = 1  ";
				if (mete_code_2.Trim() != "")
				{
					sqlstr_temp += " AND B.METE_CODE_2 = @METE_CODE_2";
				}
				if (factory_1.Trim() != "")
				{
					sqlstr_temp += " AND A.FACTORY_1 = @FACTORY_1";
				}
				if (date_time.Trim() != "")
				{
					sqlstr_temp += " AND B.DATE_TIME >= @DATE_TIME";
				}
				if (date_time_1.Trim() != "")
				{
					sqlstr_temp += " AND B.DATE_TIME <= @date_time_1";
				}
				if (mete_name.Trim() != ""){
					sqlstr_temp += " AND B.METE_NAME  like '%" + mete_name + "%' ";
				}
				sql_group = "   ";
				sqlstr = sqlstr + sqlstr_temp + sql_group;
				cmd_inq.Parameters.Set("METE_CODE_2", mete_code_2);
				cmd_inq.Parameters.Set("FACTORY_1", factory_1);
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				//cmd_inq.Parameters.Set("METE_NAME", mete_name);
				cmd_inq.Parameters.Set("date_time_1", date_time_1);
				cmd_inq.SetCommandText(sqlstr);
				Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();
			}
		}
		if (div == "CF"){
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = " select distinct B.DATE_TIME from TMMSMT8S1 B LEFT JOIN TMMSMT8METE A   ON A.METE_CODE_2=B.METE_CODE_2 WHERE 1=1  ";
				if (mete_code_2.Trim() != "")
				{
					sqlstr_temp += " AND B.METE_CODE_2 = @METE_CODE_2";
				}
				if (factory_1.Trim() != "")
				{
					sqlstr_temp += " AND A.FACTORY_1 = @FACTORY_1";
				}
				if (date_time.Trim() != "")
				{
					sqlstr_temp += " AND B.DATE_TIME >= @DATE_TIME";
				}
				if (date_time_1.Trim() != "")
				{
					sqlstr_temp += " AND B.DATE_TIME <= @date_time_1";
				}
				sql_group = "   ";
				sqlstr = sqlstr + sqlstr_temp + sql_group;
				cmd_inq.Parameters.Set("METE_CODE_2", mete_code_2);
				cmd_inq.Parameters.Set("FACTORY_1", factory_1);
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.Parameters.Set("date_time_1", date_time_1);
				cmd_inq.SetCommandText(sqlstr);
				Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();
			}
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
