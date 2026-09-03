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
BM2F_ENTERACE(mmsm81_rcv_inq)

int f_mmsm81_rcv_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm81("TMMSM81_RCV");

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
		if (bcls_rec->Tables[0].Columns.Contains("RECEIVE_DATA_TIME_FROM"))
			cs_receive_data_time_from = bcls_rec->Tables[0].Rows[0]["RECEIVE_DATA_TIME_FROM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("RECEIVE_DATA_TIME_TO"))
			cs_receive_data_time_to = bcls_rec->Tables[0].Rows[0]["RECEIVE_DATA_TIME_TO"].ToString().Trim();
		Log::Info("", __FUNCTION__, "cs_receive_data_time_from =[{0}]", cs_receive_data_time_from);
		Log::Info("", __FUNCTION__, "QUALITY_BATCH_NO =[{0}]", tmmsm81["QUALITY_BATCH_NO"].ToString());
		Log::Info("", __FUNCTION__, "COMBINE_YN =[{0}]", tmmsm81["COMBINE_YN"].ToString());
		//Log::Info("", __FUNCTION__, "MAT_TYPE =[{0}]", tmmsm81["MAT_TYPE"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM81_RCV "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM TMMSM81_RCV "
				"  WHERE 1=1  "
				;
			if (tmmsm81["WEIGH_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND WEIGH_NO		like '%'|| @tmmsm81.WEIGH_NO||'%'";
			}
			if (tmmsm81["QUALITY_BATCH_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND QUALITY_BATCH_NO		like '%'|| @tmmsm81.QUALITY_BATCH_NO||'%'";
			}
			if (tmmsm81["SHIP_NAME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND SHIP_NAME	like '%'|| @tmmsm81.SHIP_NAME||'%'";
			}
			if (tmmsm81["VEHICLE_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND VEHICLE_NO	like '%'|| @tmmsm81.VEHICLE_NO||'%'";
			}
			if (tmmsm81["VOUCHER_ID"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND VOUCHER_ID		like '%'|| @tmmsm81.VOUCHER_ID||'%'";
			}
			if (tmmsm81["LOT_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND LOT_NO		like '%'|| @tmmsm81.LOT_NO||'%'";
			}
			if (tmmsm81["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_CODE		like '%'|| @tmmsm81.MAT_CODE||'%'";
			}
			if (tmmsm81["MAT_NAME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_NAME		like '%'|| @tmmsm81.MAT_NAME||'%'";
			}
			if (tmmsm81["BUNKER_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND BUNKER_NO		like '%'|| @tmmsm81.BUNKER_NO||'%'";
			}
			if (cs_receive_data_time_from != "")
			{
				sqlstr_temp += " AND RECEIVE_DATA_TIME >= '" + cs_receive_data_time_from + "'";
			}
			if (cs_receive_data_time_to != "")
			{
				sqlstr_temp += " AND RECEIVE_DATA_TIME <= '" + cs_receive_data_time_to + "'";
			}

			if (tmmsm81["AUART"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND AUART		like '%'|| @tmmsm81.AUART||'%'";
			}
			if (tmmsm81["COMBINE_YN"].ToString() == "0")
			{
				sqlstr_temp += " AND COMBINE_YN		like '%'|| @tmmsm81.COMBINE_YN||'%'";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY RECEIVE_DATA_TIME DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.Parameters.Set("tmmsm81.WEIGH_NO", tmmsm81["WEIGH_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.AUART", tmmsm81["AUART"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.QUALITY_BATCH_NO", tmmsm81["QUALITY_BATCH_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.SHIP_NAME", tmmsm81["SHIP_NAME"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.VEHICLE_NO", tmmsm81["VEHICLE_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.VOUCHER_ID", tmmsm81["VOUCHER_ID"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.LOT_NO", tmmsm81["LOT_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.MAT_CODE", tmmsm81["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.MAT_NAME", tmmsm81["MAT_NAME"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.BUNKER_NO", tmmsm81["BUNKER_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm81.COMBINE_YN", tmmsm81["COMBINE_YN"].ToString());
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
