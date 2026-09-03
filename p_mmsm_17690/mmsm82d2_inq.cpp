/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm82d2_inq)

int f_mmsm82d2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int		TotalRecordCount = 0;
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";

	CModel tmmsmyl("TMMSMGY06");
	CDbCommand cmd_inq(conn);
	//系统的分页类信息。
	CPageInfo pageInfo;
	try
	{
		
		
		tmmsmyl.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		Log::Info("", __FUNCTION__, "STAT_DATE =[{0}]", tmmsmyl["STAT_DATE"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr_count = " SELECT COUNT(1) FROM  (SELECT T.STAT_DATE,T.L2_PROC_NO,T.ST_NO,T.HEAT_NO,R.RATIO_NUM,COUNT(*) OVER (PARTITION BY T.L2_PROC_NO) AS RECORD_CNT FROM TMMSMGY06 T LEFT JOIN TMMSMGY09 R ON T.L2_PROC_NO = R.L2_PROC_NO and  T.Heat_No = R.Heat_No WHERE T.L2_PROC_NO != T.HEAT_NO) TMP WHERE TMP.RECORD_CNT >= 2";
			sqlstr = " SELECT STAT_DATE,L2_PROC_NO,ST_NO,HEAT_NO,RATIO_NUM FROM (SELECT T.STAT_DATE,T.L2_PROC_NO,T.ST_NO,T.HEAT_NO,R.RATIO_NUM,COUNT(*) OVER (PARTITION BY T.L2_PROC_NO) AS RECORD_CNT FROM TMMSMGY06 T LEFT JOIN TMMSMGY09 R ON T.L2_PROC_NO = R.L2_PROC_NO and  T.Heat_No = R.Heat_No WHERE T.L2_PROC_NO != T.HEAT_NO) TMP WHERE TMP.RECORD_CNT >= 2";
			break;
		}

		if (tmmsmyl.GetFields().Contains("STAT_DATE") && tmmsmyl["STAT_DATE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND TMP.STAT_DATE = @STAT_DATE";
			cmd_inq.Parameters.Set("STAT_DATE", tmmsmyl["STAT_DATE"].ToString());
		}
		if (tmmsmyl.GetFields().Contains("L2_PROC_NO") && tmmsmyl["L2_PROC_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND TMP.L2_PROC_NO = @L2_PROC_NO";
			cmd_inq.Parameters.Set("L2_PROC_NO", tmmsmyl["L2_PROC_NO"].ToString());
		}
		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr_temp += " ORDER BY TMP.L2_PROC_NO DESC";
		sqlstr = sqlstr + sqlstr_temp;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;
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
