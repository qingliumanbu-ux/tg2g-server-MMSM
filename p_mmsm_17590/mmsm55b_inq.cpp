/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料检验信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm55b_inq)

int f_mmsm55b_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString ch_start_time_f = "";
	CString ch_start_time_t = "";
	CString v_mat_kind = "";


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm55b("TMMSM55B");

	CDbCommand cmd_inq(conn);

	try
	{

		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}


		//--------------------------------
		//获取传入参数
		tmmsm55b.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("SAMPLE_TIME_S"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["SAMPLE_TIME_S"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SAMPLE_TIME_E"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["SAMPLE_TIME_S"].ToString().Trim();



		/* ***** 打印输入参数 ***** */

		Log::Info("", __FUNCTION__, "FACTORY_DIV  =[{0}]", tmmsm55b["FACTORY_DIV"].ToString());
		Log::Info("", __FUNCTION__, "MAT_TYPE  =[{0}]", tmmsm55b["MAT_TYPE"].ToString());
		Log::Info("", __FUNCTION__, "MAT_SAMPLE_NO  =[{0}]", tmmsm55b["MAT_SAMPLE_NO"].ToString());


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM tmmsm55b "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM tmmsm55b "
				"  WHERE 1=1 "
				;


			if (tmmsm55b["FACTORY_DIV"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND FACTORY_DIV	like '%'|| @FACTORY_DIV||'%'";
			}
			if (tmmsm55b["MAT_TYPE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_TYPE	like '%'|| @MAT_TYPE||'%'";
			}
			if (tmmsm55b["MAT_SAMPLE_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_SAMPLE_NO	like '%'|| @MAT_SAMPLE_NO||'%'";
			}
			if (tmmsm55b["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_CODE	like '%'|| @MAT_CODE||'%'";
			}
			if (tmmsm55b["MAT_NAME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_NAME	like '%'|| @MAT_NAME||'%'";
			}
			if (ch_start_time_f != "")
			{
				sqlstr_temp += " AND GET_SAMPLE_TIME >= '" + ch_start_time_f + "'";
			}
			if (ch_start_time_t != "")
			{
				sqlstr_temp += " AND GET_SAMPLE_TIME <= '" + ch_start_time_t + "'";
			}
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY MAT_CODE DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.Parameters.Set("FACTORY_DIV", tmmsm55b["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("MAT_SAMPLE_NO", tmmsm55b["MAT_SAMPLE_NO"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm55b["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("MAT_NAME", tmmsm55b["MAT_NAME"].ToString());
		cmd_inq.Parameters.Set("MAT_TYPE", tmmsm55b["MAT_TYPE"].ToString());

		//Log::Info("", __FUNCTION__, "sqlstr_count  =[{0}]", sqlstr_count);
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

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
