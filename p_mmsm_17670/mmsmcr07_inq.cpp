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
BM2F_ENTERACE(mmsmcr07_inq)

int f_mmsmcr07_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString heat_no1 = " ";
	CString heat_no2 = " ";
	CString heat_no3 = " ";
	CString heat_no4 = " ";
	CString heat_no5 = " ";
	CString heat_no6 = " ";
	CString heat_no7 = " ";
	CString heat_no8 = " ";
	CString heat_no9 = " ";
	CString heat_no10 = " ";
	CString heat_no11 = " ";
	CString heat_no12 = " ";
	CString heat_no13 = " ";
	CString heat_no14 = " ";
	CString heat_no15 = " ";
	CString heat_no16 = " ";
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME_1"].ToString().TrimOrBlank().ToUpper();
		sqlstr = " SELECT HEAT_NO1,HEAT_NO2,HEAT_NO3,HEAT_NO4,HEAT_NO5,HEAT_NO6,HEAT_NO7,HEAT_NO8,HEAT_NO9,HEAT_NO10,HEAT_NO11,HEAT_NO12,HEAT_NO13,HEAT_NO14,HEAT_NO15,HEAT_NO16  FROM TMMSMZXHBB_LH  WHERE 1=1 ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			heat_no1 = cmd_inq.GetString(1);
			heat_no2 = cmd_inq.GetString(2);
			heat_no3 = cmd_inq.GetString(3);
			heat_no4 = cmd_inq.GetString(4);
			heat_no5 = cmd_inq.GetString(5);
			heat_no6 = cmd_inq.GetString(6);
			heat_no7 = cmd_inq.GetString(7);
			heat_no8 = cmd_inq.GetString(8);
			heat_no9 = cmd_inq.GetString(9);
			heat_no10 = cmd_inq.GetString(10);
			heat_no11 = cmd_inq.GetString(11);
			heat_no12 = cmd_inq.GetString(12);
			heat_no13 = cmd_inq.GetString(13);
			heat_no14 = cmd_inq.GetString(14);
			heat_no15 = cmd_inq.GetString(15);
			heat_no16 = cmd_inq.GetString(16);

		}
		else
		{
			strcpy(s.msg, "未查询到炉号数据请先执行F3按钮生成数据");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();

		if (end_time.Trim() != "")end_time += "000000";
		if (end_time_1.Trim() != "")end_time_1 += "235959";
		Log::Info("", __FUNCTION__, "end_time   =[{0}]", end_time);
		Log::Info("", __FUNCTION__, "end_time_1   =[{0}]", end_time_1);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT MAX(TIME_1) AS DATE_1,A.MAT_CODE,B.MAT_NAME, SUM(DEVO_WT) AS DEVO_WT ,SUM(DEVO_WT*NI_VALUE) AS SUM_NI_IN, SUM(DEVO_WT*CR_VALUE)  AS SUM_CR_IN,"
				" ROUND(DECODE(SUM(DEVO_WT),0,0,SUM(DEVO_WT*NI_VALUE)/SUM(DEVO_WT)),4) AS NI, ROUND(DECODE(SUM(DEVO_WT),0,0,SUM(DEVO_WT*CR_VALUE)/SUM(DEVO_WT)),4) AS CR "
				" FROM TMMSMZXHBB A "
				" LEFT JOIN TMMSM50 B ON A.MAT_CODE=B.MAT_CODE  "
				" where 1=1 ";
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND TIME_1 >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
				sqlstr_temp += " AND TIME_1 <= @end_time_1";
			}
			sqlstr_temp = " and ((A.HEAT_NO >=@heat_no1  AND A.HEAT_NO <= @heat_no2) OR (A.HEAT_NO >= @heat_no3 AND A.HEAT_NO <= @heat_no4) OR (A.HEAT_NO >= @heat_no5  AND "
				"	A.HEAT_NO <= @heat_no6) OR(A.HEAT_NO >= @heat_no7 AND A.HEAT_NO <= @heat_no8) OR(A.HEAT_NO >= @heat_no9 "
				"	AND A.HEAT_NO <= @heat_no10) OR(A.HEAT_NO >= @heat_no11 AND A.HEAT_NO <= @heat_no12) OR(A.HEAT_NO >= @heat_no13 AND A.HEAT_NO <= @heat_no14) "
				"	OR(A.HEAT_NO >= @heat_no15 AND A.HEAT_NO <= @heat_no16)) AND SUBSTR(A.ST_NO,1,1) IN('1','4')  ";
			sql_group = " group by A.MAT_CODE,B.MAT_NAME  ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time_1", end_time_1);
			cmd_inq.Parameters.Set("heat_no1", heat_no1);
			cmd_inq.Parameters.Set("heat_no2", heat_no2);
			cmd_inq.Parameters.Set("heat_no3", heat_no3);
			cmd_inq.Parameters.Set("heat_no4", heat_no4);
			cmd_inq.Parameters.Set("heat_no5", heat_no5);
			cmd_inq.Parameters.Set("heat_no6", heat_no6);
			cmd_inq.Parameters.Set("heat_no7", heat_no7);
			cmd_inq.Parameters.Set("heat_no8", heat_no8);
			cmd_inq.Parameters.Set("heat_no9", heat_no9);
			cmd_inq.Parameters.Set("heat_no10", heat_no10);
			cmd_inq.Parameters.Set("heat_no11", heat_no11);
			cmd_inq.Parameters.Set("heat_no12", heat_no12);
			cmd_inq.Parameters.Set("heat_no13", heat_no13);
			cmd_inq.Parameters.Set("heat_no14", heat_no14);
			cmd_inq.Parameters.Set("heat_no15", heat_no15);
			cmd_inq.Parameters.Set("heat_no16", heat_no16);
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
