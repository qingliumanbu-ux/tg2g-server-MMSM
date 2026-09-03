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
BM2F_ENTERACE(mmsmcr05_inq)

int f_mmsmcr05_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sql = "";
	CString sql_temp = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString end_time = " ";
	CString end_time_1 = " ";
	CDecimal SUM_CR = 0;
	CDecimal SUM_NI =0;
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME_1"].ToString().TrimOrBlank().ToUpper();
		if (end_time.Trim() != "")end_time += "000000";
		if (end_time_1.Trim() != "")end_time_1 += "235959";
		Log::Info("", __FUNCTION__, "end_time_1   =[{0}]", end_time_1);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT MAX(REC_CREATE_TIME) AS DATE_1 ,MAX(PROD_DATE) AS PROD_DATE, MIN(REC_CREATOR) AS DAMIN, MAX(BUNKER_NO) AS BUNKER_NO,MAT_CODE,MAT_NAME,"
				" ROUND(SUM(NET_WT)/1000,3) AS DEVO_WT,NVL(MAX(CR),0) AS CR, NVL(MAX(NI),0) AS NI,ROUND(SUM(NET_WT)/1000,3)*NVL(MAX(CR),0) AS SUM_CR,ROUND(SUM(NET_WT)/1000,3)*NVL(MAX(NI),0) AS SUM_NI"
                  " FROM"
                  " (SELECT REC_CREATE_TIME, SUBSTR(REC_CREATE_TIME,1,8) PROD_DATE,REC_CREATOR, MAT_CODE,MAT_NAME,NET_WT,BUNKER_NO FROM TMMSM81_S WHERE MAT_CODE LIKE 'F06%'"
                  " UNION"
                  " SELECT LOAD_END_TIME,SUBSTR(LOAD_END_TIME,1,8) PROD_DATE ,REC_CREATOR,MATERIAL_CODE,(SELECT MAT_NAME FROM TMMSM50 WHERE MAT_CODE=A.MATERIAL_CODE) MAT_NAME,"
                  "        MAT_WT,LOAD_CODE_FACTORY  FROM TWMSM61 A  WHERE SUBSTR(PLAN_NO,1,4)='21JF') A LEFT JOIN"
                  "  ZJ_SCRAP_ELEMENT  B ON A.MAT_CODE=B.MAT_ID"
				  " where 1=1 ";
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND A.REC_CREATE_TIME >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
				sqlstr_temp += " AND A.REC_CREATE_TIME <= @end_time_1";
			}
			sql_group = " GROUP BY  MAT_CODE,MAT_NAME  ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time_1", end_time_1);
			cmd_inq.SetCommandText(sqlstr);
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();


			sqlstr_count = "WITH  T AS (" + sqlstr + ") SELECT SUM(SUM_CR),SUM(SUM_NI) FROM T ";
			cmd_inq.SetCommandText(sqlstr_count);
			Log::Info("", __FUNCTION__, "sqlstr_count   =[{0}]", sqlstr_count);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				SUM_CR = cmd_inq.GetDecimal(1);
				SUM_NI = cmd_inq.GetDecimal(2);
			}
			cmd_inq.Close();

			Log::Info("", __FUNCTION__, "SUM_CR   =[{0}] SUM_CR   =[{1}]", SUM_CR, SUM_NI);
			sql = " SELECT MAX(REC_CREATE_TIME) AS DATE_1 ,MAX(PROD_DATE) AS PROD_DATE, MIN(REC_CREATOR) AS DAMIN, MAX(BUNKER_NO) AS BUNKER_NO,MAT_CODE,MAT_NAME,"
				" ROUND(SUM(NET_WT)/1000,3) AS DEVO_WT,NVL(MAX(CR),0) AS CR, NVL(MAX(NI),0) AS NI,TO_NUMBER(" + SUM_CR.ToString() + ") AS  SUM_CR,TO_NUMBER(" + SUM_NI.ToString() + ") AS  SUM_NI, "
				" ROUND(SUM(NET_WT) / 1000, 3)*NVL(MAX(CR), 0) AS SUM_CR_1, ROUND(SUM(NET_WT) / 1000, 3)*NVL(MAX(NI), 0) AS SUM_NI_1 "
				" FROM"
				" (SELECT REC_CREATE_TIME, SUBSTR(REC_CREATE_TIME,1,8) PROD_DATE,REC_CREATOR, MAT_CODE,MAT_NAME,NET_WT,BUNKER_NO FROM TMMSM81_S WHERE MAT_CODE LIKE 'F06%'"
				" UNION"
				" SELECT LOAD_END_TIME,SUBSTR(LOAD_END_TIME,1,8) PROD_DATE ,REC_CREATOR,MATERIAL_CODE,(SELECT MAT_NAME FROM TMMSM50 WHERE MAT_CODE=A.MATERIAL_CODE) MAT_NAME,"
				"        MAT_WT,LOAD_CODE_FACTORY  FROM TWMSM61 A  WHERE SUBSTR(PLAN_NO,1,4)='21JF') A LEFT JOIN"
				"  ZJ_SCRAP_ELEMENT  B ON A.MAT_CODE=B.MAT_ID"
				" where 1=1 ";
			if (end_time.Trim() != "")
			{
				sql_temp += " AND A.REC_CREATE_TIME >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
				sql_temp += " AND A.REC_CREATE_TIME <= @end_time_1";
			}
			sql_group = " GROUP BY  MAT_CODE,MAT_NAME  ";
			sql = sql + sql_temp + sql_group;
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time_1", end_time_1);
			cmd_inq.SetCommandText(sql);
			Log::Info("", __FUNCTION__, "sql   =[{0}]", sql);
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
