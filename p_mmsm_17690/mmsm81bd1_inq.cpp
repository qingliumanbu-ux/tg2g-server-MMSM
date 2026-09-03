/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm81bd1_inq)

int f_mmsm81bd1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString mat_code = "";
	int		TotalRecordCount = 0;
	CString delivy_end_time1 = "";


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm60("TMMSM81V");

	CDbCommand cmd_inq(conn);

	try
	{
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		if (bcls_rec->Tables.get_Count()>0)
		{
			if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
				mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		}

		Log::Info("", __FUNCTION__, "nowTime =[{0}]", nowTime);
		Log::Info("", __FUNCTION__, "mat_code =[{0}]", mat_code);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

		
			sqlstr = " SELECT * "
				"   FROM TMMSM81V "
				"  WHERE 1=1 AND C_STATE='1' AND  DATE_START <= @DATE_START ";

			if (mat_code.Trim()!="")
			{
				sqlstr_temp = " and MAT_CODE = @MAT_CODE ";
			}

			sqlstr = sqlstr + sqlstr_temp;
			sqlstr = sqlstr + " ORDER BY DATE_START DESC";
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);



		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("DATE_START",nowTime );
		cmd_inq.Parameters.Set("MAT_CODE",mat_code );
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