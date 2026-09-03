/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料履历信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm52_inq)

int f_mmsm52_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_mat_type = "";
	CString v_log_type = "";
	CString v_heat_no = "";
	CString ch_start_time_f = "";
	CString ch_start_time_t = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;
	CModel tmmsm52("TMMSM52");

	CDbCommand cmd_inq(conn);

	try
	{
		try{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}
		//--------------------------------
		//获取传入参数
		tmmsm52.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("LOG_TYPE"))
			v_log_type = bcls_rec->Tables[0].Rows[0]["LOG_TYPE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();

		if (bcls_rec->Tables[0].Columns.Contains("LOG_TIME_S"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["LOG_TIME_S"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("LOG_TIME_E"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["LOG_TIME_E"].ToString().Trim();



		Log::Info("", __FUNCTION__, "ch_start_time_f =[{0}]", ch_start_time_f);
		Log::Info("", __FUNCTION__, "ch_start_time_t =[{0}]", ch_start_time_t);
		Log::Info("", __FUNCTION__, "v_log_type =[{0}]", tmmsm52["LOG_TYPE"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM tmmsm52 "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM tmmsm52 "
				"  WHERE 1=1 "
				;
			if (tmmsm52["HEAT_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND HEAT_NO		like '%'|| @tmmsm52.HEAT_NO||'%' ";
			}
			if (tmmsm52["STATION_ID"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND STATION_ID	    like '%'|| @tmmsm52.STATION_ID||'%'";
			}
			if (tmmsm52["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_CODE		like '%'|| @tmmsm52.MAT_CODE||'%' ";
			}
			if (tmmsm52["MAT_TYPE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_TYPE		like '%'|| @tmmsm52.MAT_TYPE||'%'";
			}
			if (tmmsm52["LOG_TYPE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND LOG_TYPE		like '%'|| @tmmsm52.LOG_TYPE||'%'";
			}
			if (tmmsm52["FACTORY_DIV"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND FACTORY_DIV		like '%'|| @tmmsm52.FACTORY_DIV||'%'";
			}
			if (ch_start_time_f.Trim() != "")
			{
				sqlstr_temp += " AND LOG_TIME			>= @ch_start_time_f";
			}
			if (ch_start_time_t.Trim() != "")
			{
				sqlstr_temp += " AND LOG_TIME			<= @ch_start_time_t";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY MAT_CODE DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.Parameters.Set("tmmsm52.HEAT_NO", tmmsm52["HEAT_NO"]);
		cmd_inq.Parameters.Set("tmmsm52.STATION_ID", tmmsm52["STATION_ID"]);
		cmd_inq.Parameters.Set("tmmsm52.MAT_CODE", tmmsm52["MAT_CODE"]);
		cmd_inq.Parameters.Set("tmmsm52.MAT_TYPE", tmmsm52["MAT_TYPE"]);
		cmd_inq.Parameters.Set("tmmsm52.LOG_TYPE", tmmsm52["LOG_TYPE"]);
		cmd_inq.Parameters.Set("tmmsm52.FACTORY_DIV", tmmsm52["FACTORY_DIV"]);
		cmd_inq.Parameters.Set("ch_start_time_f", ch_start_time_f);
		cmd_inq.Parameters.Set("ch_start_time_t", ch_start_time_t);

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
