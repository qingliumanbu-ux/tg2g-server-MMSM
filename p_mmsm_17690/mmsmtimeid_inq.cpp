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
BM2F_ENTERACE(mmsmtimeid_inq)

int f_mmsmtimeid_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString end_time = " ";
	CString start_time = " ";
	CModel tmmsmzxhbb_lh("TMMSMZXHBB_LH");
	CModel tmmsmzxhbb_lh1("TMMSMZXHBB_LH");

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("TIME_STAMPS"))
			start_time = bcls_rec->Tables[0].Rows[0]["TIME_STAMPS"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("TIME_STAMPS_1"))
			end_time = bcls_rec->Tables[0].Rows[0]["TIME_STAMPS_1"].ToString();

		Log::Info("", __FUNCTION__, "start_time =[{0}] - end_time =[{1}]", start_time, end_time);
		if (start_time.Trim() == "")
		{
			return 0;
		}
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT TIME_STAMPS||'-'||USER_ID AS CX_FLAG FROM TMMSMZXHBB_LH WHERE 1=1  ";
			if (start_time.Trim()!="")
			{
				sqlstr_temp += " AND TIME_STAMPS>=@start_time";
			}
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND TIME_STAMPS<=@end_time";
			}
			cmd_inq.Parameters.Set("start_time", start_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			sqlstr_temp += " ORDER BY TIME_STAMPS DESC";
			sqlstr = sqlstr + sqlstr_temp;
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
