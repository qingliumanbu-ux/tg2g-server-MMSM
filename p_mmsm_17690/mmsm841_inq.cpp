/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2023-09-21 17:13:56
Description: 高位料仓查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsm841_inq)

int f_mmsm841_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString sqlstr1 = "";
	CString sqlstr_count1 = "";
	CString sqlstr_temp1 = "";
	CString mat_code = "";
	int TotalRecordCount = 0;
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CModel tmmsm60("TMMSM60");

	Log::Info("", __FUNCTION__, "1111 =[{0}]", 222);

	Log::Info("", __FUNCTION__, "Tables =[{0}]", bcls_rec->Tables.get_Count());
	if (bcls_rec->Tables.get_Count()>0)
	{
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		tmmsm60.MergeFrom(bcls_rec->Tables[0].Rows[0]);
	}


	try
	{
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr_count = " SELECT COUNT(*) "
				"   FROM TMMSM60 WHERE BUNKER_TYPE IN ('CARBON','STAINLESS','VS','COOL')  ";
			sqlstr = " select *  "
				" from TMMSM60 WHERE BUNKER_TYPE IN ('CARBON','STAINLESS','VS','COOL') ";

			if (tmmsm60["MAT_NAME"].ToString().Trim() != "")
			{
				sqlstr += " AND MAT_NAME like '%'|| @MAT_NAME||'%'";
			}
			if (tmmsm60["BUNKER_NO"].ToString().Trim() != "")
			{
				sqlstr += " AND BUNKER_NO like '%'|| @BUNKER_NO||'%'";
			}
			if (tmmsm60["BUNKER_TYPE"].ToString().Trim() != "")
			{
				sqlstr += " AND BUNKER_NAME	like '%'|| @BUNKER_NAME||'%'";
			}
			if (tmmsm60["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr += " AND MAT_CODE like '%'|| @MAT_CODE||'%'";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY BUNKER_NO ";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_NAME", tmmsm60["MAT_NAME"].ToString());
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm60["BUNKER_NO"].ToString());
		cmd_inq.Parameters.Set("BUNKER_NAME", tmmsm60["BUNKER_TYPE"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm60["MAT_CODE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
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
