/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 炼钢实绩操作过程事项
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmlc99_inq)

int f_mmsmlc99_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString weigh_flag = "";
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm89("TMMSM89");

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
		tmmsm89.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
		weigh_flag = bcls_rec->Tables[0].Rows[0]["WEIGH_FLAG"].ToString();
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

		Log::Trace(" ", __FUNCTION__, "MAT_NAME=[{0}], MAT_CODE =[{1}],WEIGH_FLAG =[{2}]", tmmsm89["MAT_NAME"].ToString(), tmmsm89["MAT_CODE"].ToString(), weigh_flag);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			if (weigh_flag == "1")
			{
				sqlstr_count = " select count(1) from (SELECT WEIGH_NO,MAX(EVENT_TIME) EVENT_TIME, "
					" MAX(EVENT_DESC) EVENT_DESC, "
					" MAX(BUNKER_NO_ORIGINAL) BUNKER_NO_ORIGINAL, "
					" MAX(BUNKER_NO) BUNKER_NO, "
					" MAX(MAT_CODE) MAT_CODE, "
					" SUM(STOCK_WT) STOCK_WT, "
					" MAX(QUALITY_BATCH_NO) QUALITY_BATCH_NO, "
					" MAX(HEAT_NO) HEAT_NO, "
					" MAX(MAT_NAME) MAT_NAME, "
					" MAX(BUNKER_TYPE_ORIGINAL) BUNKER_TYPE_ORIGINAL, "
					" MAX(BUNKER_NAME_ORIGINAL) BUNKER_NAME_ORIGINAL, "
					" MAX(BUNKER_TYPE) BUNKER_TYPE, "
					" MAX(BUNKER_NAME) BUNKER_NAME, "
					" MAX(REC_CREATOR) REC_CREATOR, "
					" MAX(PROC_COUNT) PROC_COUNT, "
					" MAX(CLIENT_IP) CLIENT_IP, "
					" MAX(SEQ_NO) SEQ_NO, "
					" MAX(ID_2A) ID_2A, "
					" MAX(EVENT_CODE) EVENT_CODE, "
					" MAX(BUCKLE_WT) BUCKLE_WT, "
					" MAX(VEHICLE_NO) VEHICLE_NO, "
					" MAX(RESUME_SEQ_NO) RESUME_SEQ_NO, "
					" MAX(TARE_WT) TARE_WT, "
					" MAX(BACK_CODE_1) BACK_CODE_1 FROM TMMSM89 WHERE EVENT_CODE = 'IN' GROUP BY WEIGH_NO  ) WHERE 1=1 ";

				sqlstr = "  SELECT WEIGH_NO,MAX(EVENT_TIME) EVENT_TIME, "
					" MAX(EVENT_DESC) EVENT_DESC, "
					" MAX(BUNKER_NO_ORIGINAL) BUNKER_NO_ORIGINAL, "
					" MAX(BUNKER_NO) BUNKER_NO, "
					" MAX(MAT_CODE) MAT_CODE, "
					" SUM(STOCK_WT) STOCK_WT, "
					" MAX(QUALITY_BATCH_NO) QUALITY_BATCH_NO, "
					" MAX(HEAT_NO) HEAT_NO, "
					" MAX(MAT_NAME) MAT_NAME, "
					" MAX(BUNKER_TYPE_ORIGINAL) BUNKER_TYPE_ORIGINAL, "
					" MAX(BUNKER_NAME_ORIGINAL) BUNKER_NAME_ORIGINAL, "
					" MAX(BUNKER_TYPE) BUNKER_TYPE, "
					" MAX(BUNKER_NAME) BUNKER_NAME, "
					" MAX(REC_CREATOR) REC_CREATOR, "
					" MAX(PROC_COUNT) PROC_COUNT, "
					" MAX(CLIENT_IP) CLIENT_IP, "
					" MAX(SEQ_NO) SEQ_NO, "
					" MAX(ID_2A) ID_2A, "
					" MAX(EVENT_CODE) EVENT_CODE, "
					" MAX(BUCKLE_WT) BUCKLE_WT, "
					" MAX(VEHICLE_NO) VEHICLE_NO, "
					" MAX(RESUME_SEQ_NO) RESUME_SEQ_NO, "
					" MAX(TARE_WT) TARE_WT, "
					" MAX(BACK_CODE_1) BACK_CODE_1 FROM TMMSM89 WHERE EVENT_CODE = 'IN'  ";
			}
			else
			{
				sqlstr_count = " SELECT COUNT(1) "
					"   FROM tmmsm89 "
					"  WHERE 1=1 "
					;
				sqlstr = " SELECT * "
					"   FROM tmmsm89 "
					"  WHERE 1=1 "
					;
			}

			if (tmmsm89["WEIGH_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND WEIGH_NO	like '%'||trim(@weigh_no)||'%' ";
			}
			
			if (tmmsm89["MAT_NAME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND mat_name	like '%'||@mat_name||'%'";
			}
			if (tmmsm89["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND mat_code	like '%'|| @mat_code||'%'";
			}
			//if (weigh_flag == "1")	//过滤榜单信息为0的
			//{
			//	sqlstr_temp += " AND (（EVENT_CODE in ('IN','LIMEIN','MEIIN') and stock_wt != 0 and WEIGH_NO !=' ') or   EVENT_CODE not in ('IN','LIMEIN','MEIIN'))";
			//}
			Log::Info("", __FUNCTION__, "EVENT_CODE =[{0}]", tmmsm89["EVENT_CODE"].ToString());
			if (tmmsm89["EVENT_CODE"].ToString().Trim() != "")
			{
				//20250408addbywcm
				if (tmmsm89["EVENT_CODE"].ToString().Trim() == "ZHCX")
				{
					if (tmmsm89["BUNKER_NO_ORIGINAL"].ToString().Trim() != ""&&tmmsm89["BUNKER_NO"].ToString().Trim() == "")
					{
						sqlstr_temp += " AND BUNKER_NO_ORIGINAL	like '%'||trim(@bunker_no_original)||'%'";
					}
					if (tmmsm89["BUNKER_NO"].ToString().Trim() != ""&&tmmsm89["BUNKER_NO_ORIGINAL"].ToString().Trim() == "")
					{
						sqlstr_temp += " AND bunker_no	like '%'||trim(@bunker_no)||'%'";
					}
					if (tmmsm89["BUNKER_NO"].ToString().Trim() != ""&&tmmsm89["BUNKER_NO_ORIGINAL"].ToString().Trim()!= "")
					{
						sqlstr_temp += " AND (bunker_no	like '%'||trim(@bunker_no)||'%' or BUNKER_NO_ORIGINAL	like '%'||trim(@bunker_no_original)||'%')";
					}
				}
			  else
			  {
				 if (tmmsm89["BUNKER_NO_ORIGINAL"].ToString().Trim() != "")
				 {
					  sqlstr_temp += " AND BUNKER_NO_ORIGINAL	like '%'||trim(@bunker_no_original)||'%'";
				 }
				 if (tmmsm89["BUNKER_NO"].ToString().Trim() != "")
				 {
					  sqlstr_temp += " AND bunker_no	like '%'||trim(@bunker_no)||'%'";
				 }
				 if (tmmsm89["EVENT_CODE"].ToString().Trim() == "ALCHARGE")
				 {
					sqlstr_temp += " AND event_code	IN('CHARGE','EXTRA')";
				 }
				 else if (tmmsm89["EVENT_CODE"].ToString().Trim() == "ALLIN")
				 {
					sqlstr_temp += " AND event_code	IN('IN','LIMEIN','MEIIN')";
				 }
				 else if (tmmsm89["EVENT_CODE"].ToString().Trim() == "ALLMOVE")
				 {
					sqlstr_temp += " AND event_code	IN('MOVE','TRANSFER')";
				 }
				 else if (tmmsm89["EVENT_CODE"].ToString().Trim() == "ALLPK")
				 {
					sqlstr_temp += " AND event_code	IN('CORR','INVE')";
				 }
				 else
				 {
					sqlstr_temp += " AND event_code	=@event_code";
				 }
			  }
			}
			else
			{
				if (tmmsm89["BUNKER_NO_ORIGINAL"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND BUNKER_NO_ORIGINAL	like '%'||trim(@bunker_no_original)||'%'";
				}
				if (tmmsm89["BUNKER_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND bunker_no	like '%'||trim(@bunker_no)||'%'";
				}
			}
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND EVENT_TIME<=@end_time";
			}
			if (begin_time.Trim() != "")
			{
				sqlstr_temp += " AND EVENT_TIME>=@begin_time";
			}


			sqlstr_count = sqlstr_count + sqlstr_temp;
			if (weigh_flag == "1")
			{
				sqlstr_temp += "  GROUP BY WEIGH_NO  ORDER BY RESUME_SEQ_NO DESC,EVENT_TIME DESC";
			}
			else
			{
				sqlstr_temp += " ORDER BY EVENT_TIME DESC,  RESUME_SEQ_NO DESC";
			}
			
			sqlstr = sqlstr + sqlstr_temp;
			
			break;
		}
		cmd_inq.Parameters.Set("weigh_no", tmmsm89["WEIGH_NO"].ToString());
		cmd_inq.Parameters.Set("mat_name", tmmsm89["MAT_NAME"].ToString());
		cmd_inq.Parameters.Set("mat_code", tmmsm89["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("bunker_no", tmmsm89["BUNKER_NO"].ToString());
		cmd_inq.Parameters.Set("bunker_no_original", tmmsm89["BUNKER_NO_ORIGINAL"].ToString());
		cmd_inq.Parameters.Set("event_code", tmmsm89["EVENT_CODE"].ToString());
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);

		Log::Trace(" ", __FUNCTION__, "sqlstr_count= [{0}]", sqlstr_count);
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.Close();
		//分页获取

		Log::Trace(" ", __FUNCTION__, "sqlstr= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);

		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
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
