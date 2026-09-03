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
BM2F_ENTERACE(mmsmcr06_inq)

int f_mmsmcr06_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString bunker_no = " ";
	CString now_time = " ";

	CModel tmmsm60("TMMSM60");
	CDbCommand cmd_inq(conn);

	try
	{
		/*if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME_1"].ToString().TrimOrBlank().ToUpper();*/
		if (bcls_rec->Tables[0].Columns.Contains("BUNKER_NO"))
		    bunker_no = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("NOW_TIME"))
			now_time = bcls_rec->Tables[0].Rows[0]["NOW_TIME"].ToString();
		if (now_time.GetLength() != 14)
		{
			now_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
		}
		/*end_time += "000000";
		end_time_1 += "235959";
		Log::Info("", __FUNCTION__, "end_time_1   =[{0}]", end_time_1);*/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT A.MAT_CODE,C.MAT_NAME,SUM(A.STOCK_WT) DEVO_WT,'" + now_time+"' PROD_DATE,MAX(Z.REC_CREATE_TIME) REC_CREATE_TIME, B.BUNKER_NO, MAX(B.LASTACTDATE) AS DATE_1 FROM (SELECT A.MAT_CODE,A.BUNKER_NO_ORIGINAL,A.STOCK_WT,A.EVENT_TIME,MAX(A.EVENT_TIME) OVER(PARTITION BY A.BUNKER_NO_ORIGINAL, A.MAT_CODE) MAX_EVENT FROM TMMSM89 A WHERE A.EVENT_CODE IN('CHARGE', 'EXTRA')) A "
				" LEFT JOIN TMMSM50 C ON C.MAT_CODE=A.MAT_CODE "
				" LEFT JOIN (SELECT * FROM (SELECT MAX(REC_CREATE_TIME) REC_CREATE_TIME,BUNKER_NO FROM TMMSM83 WHERE 1=1 GROUP BY BUNKER_NO)) Z ON Z.BUNKER_NO=A.BUNKER_NO_ORIGINAL  "
				" LEFT JOIN TMMSM60 B ON B.BUNKER_NO=A.BUNKER_NO_ORIGINAL WHERE B.FLAG1='H' AND EVENT_TIME>=(SELECT LASTACTDATE FROM TMMSM60 WHERE BUNKER_NO=A.BUNKER_NO_ORIGINAL) "
			    ;
			/*if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME <= @end_time_1";
			}*/
			if (bunker_no.Trim() != "")
			{
				sqlstr_temp += " AND A.BUNKER_NO_ORIGINAL like '%'|| @bunker_no||'%'";
			}
			sql_group = " group by  A.MAT_CODE,C.MAT_NAME,B.BUNKER_NO  ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			/*cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time_1", end_time_1);*/
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
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
