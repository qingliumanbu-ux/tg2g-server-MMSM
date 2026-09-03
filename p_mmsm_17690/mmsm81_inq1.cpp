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
BM2F_ENTERACE(mmsm81_inq1)

int f_mmsm81_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString s_bunker_no = "";
	CString s_weigh_no = "";
	int i_idx = 0;
	CDecimal cd_count = 0;
	
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm81("TMMSM81");

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
		tmmsm81.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		s_weigh_no = tmmsm81["WEIGH_NO"].ToString().Trim();
		s_bunker_no = tmmsm81["BUNKER_NO"].ToString().Trim();
		
		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM") && bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
				current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
			}
		}
		else {
			record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		}
		if (s_bunker_no.GetLength()>0)
		{
			i_idx = s_bunker_no.Find('-');
			if (i_idx>0)
			{
				s_bunker_no = s_bunker_no.Substring(0, i_idx);
				tmmsm81["BUNKER_NO"] = s_bunker_no;

			}
		}
		Log::Info("", __FUNCTION__, "WEIGH_NO =[{0}]", tmmsm81["WEIGH_NO"].ToString());
		Log::Info("", __FUNCTION__, "MAT_NAME =[{0}]", tmmsm81["MAT_NAME"].ToString());
		Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm81["MAT_CODE"].ToString());
		Log::Trace("", __FUNCTION__, "record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "current_page_no			= [{0}]", current_page_no);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM81 "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM TMMSM81 "
				"  WHERE 1=1 "
				;
			if (tmmsm81["WEIGH_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND WEIGH_NO			= @tmmsm81.WEIGH_NO";
			}
			/*if (tmmsm81["MAT_NAME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_NAME			= @tmmsm81.MAT_NAME";
			}*/
			/*if (tmmsm81["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_CODE			= @tmmsm81.MAT_CODE";
			}*/
			
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY RECEIVE_DATA_TIME DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.Parameters.Set("tmmsm81.WEIGH_NO", tmmsm81["WEIGH_NO"].ToString());
	/*	cmd_inq.Parameters.Set("tmmsm81.MAT_NAME", tmmsm81["MAT_NAME"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.MAT_CODE", tmmsm81["MAT_CODE"].ToString());*/
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.Close();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();


		//返回分页信息
		bcls_ret->Tables.Add("PAGEINFO");//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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
