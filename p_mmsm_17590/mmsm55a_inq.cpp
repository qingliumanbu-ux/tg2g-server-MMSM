/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料入库信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm55a_inq)

int f_mmsm55a_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CModel tmmsm55a("TMMSM55A");

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
		tmmsm55a.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("SAMPLE_TIME_S"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["SAMPLE_TIME_S"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SAMPLE_TIME_E"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["SAMPLE_TIME_E"].ToString().Trim();


		if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND"))//材料类型 有可能是1，2拼在一块传到后台
			v_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();



		/* ***** 打印输入参数 ***** */

		//Log::Info("", __FUNCTION__, "FACTORY_DIV  =[{0}]", tmmsm55a["FACTORY_DIV"].ToString());
		//Log::Info("", __FUNCTION__, "MAT_TYPE  =[{0}]", tmmsm55a["MAT_TYPE"].ToString());
		Log::Info("", __FUNCTION__, "MAT_KIND  =[{0}]", v_mat_kind);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM tmmsm55a "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT  "
				"   IN_STOCK_NO,MAT_CODE,MAX(MAT_NAME) MAT_NAME, \
				SUM(CASE WHEN ELM_NAME = '001' THEN ELM_VALUE ELSE 0 END) A001, 	\
				SUM(CASE WHEN ELM_NAME = '006' THEN ELM_VALUE ELSE 0 END) A006,		\
				SUM(CASE WHEN ELM_NAME = '004' THEN ELM_VALUE ELSE 0 END) A004,		\
				SUM(CASE WHEN ELM_NAME = '005' THEN ELM_VALUE ELSE 0 END) A005,		\
				SUM(CASE WHEN ELM_NAME = '002' THEN ELM_VALUE ELSE 0 END) A002,		\
				SUM(CASE WHEN ELM_NAME = '010' THEN ELM_VALUE ELSE 0 END) A010,		\
				SUM(CASE WHEN ELM_NAME = '020' THEN ELM_VALUE ELSE 0 END) A020,		\
				SUM(CASE WHEN ELM_NAME = '003' THEN ELM_VALUE ELSE 0 END) A003,		\
				SUM(CASE WHEN ELM_NAME = '013' THEN ELM_VALUE ELSE 0 END) A013,		\
				SUM(CASE WHEN ELM_NAME = '047' THEN ELM_VALUE ELSE 0 END) A047,		\
				SUM(CASE WHEN ELM_NAME = '021' THEN ELM_VALUE ELSE 0 END) A021,		\
				SUM(CASE WHEN ELM_NAME = '025' THEN ELM_VALUE ELSE 0 END) A024,		\
				SUM(CASE WHEN ELM_NAME = '022' THEN ELM_VALUE ELSE 0 END) A022,		\
				SUM(CASE WHEN ELM_NAME = '023' THEN ELM_VALUE ELSE 0 END) A023,		\
				SUM(CASE WHEN ELM_NAME = '017' THEN ELM_VALUE ELSE 0 END) A017,		\
				SUM(CASE WHEN ELM_NAME = '025' THEN ELM_VALUE ELSE 0 END) A025,		\
				SUM(CASE WHEN ELM_NAME = '007' THEN ELM_VALUE ELSE 0 END) A007,		\
				SUM(CASE WHEN ELM_NAME = '009' THEN ELM_VALUE ELSE 0 END) A009		\
				FROM tmmsm55a WHERE 1 = 1 "
				;

			/*if (tmmsm55a["FACTORY_DIV"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND FACTORY_DIV	like '%'|| @FACTORY_DIV||'%'";
			}*/
			if (tmmsm55a["IN_STOCK_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND IN_STOCK_NO	like '%'|| @IN_STOCK_NO||'%'";
			}
			if (tmmsm55a["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_CODE	like '%'|| @MAT_CODE||'%'";
			}
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " GROUP BY IN_STOCK_NO,MAT_CODE";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		//cmd_inq.Parameters.Set("FACTORY_DIV", tmmsm55a["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("MAT_SAMPLE_NO", tmmsm55a["MAT_SAMPLE_NO"].ToString());
		cmd_inq.Parameters.Set("IN_STOCK_NO", tmmsm55a["IN_STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm55a["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("MAT_NAME", tmmsm55a["MAT_NAME"].ToString());
		//cmd_inq.Parameters.Set("MAT_TYPE", tmmsm55a["MAT_TYPE"].ToString());

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
