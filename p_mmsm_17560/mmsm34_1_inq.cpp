/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-28
Version:1.0
Description: 精整相关信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 可供精整炼钢板坯信息查询
/// <para>
/// 查询可供精整炼钢板坯信息
/// </para>
/// </summary>
/// <param name="MAT_NO">材料号</param>
/// <param name="PONO">制造命令号</param>
/// <param name="HSF_END_TIME_F">精整结束开始时刻</param>
/// <param name="HSF_END_TIME_T">精整结束结束时刻</param>
/// <param name="MACH_CLEAR_FLAG">精整标记</param>
/// <returns>板坯信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm34_1_inq)


int f_mmsm34_1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int rowCount = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString ch_hsf_end_time_f = "";
	CString ch_hsf_end_time_t = "";
	CString heat_no = "";
	CString mat_no = "";
	CString in_mat_no = "";
	CString prod_shift_no = "";
	CString prod_shift_group = "";
	CString queryDiv = "";
	CString tableName = "";
	CString factoryDiv = "";
	CString st_no = "";
	CString cc_mach_no = "";
	CString finish_flag = "";
	CString station_id = "";
	CString mend_flag = "";//修磨标记
	int		TotalRecordCount = 0;

	//2024-03-13
	CString unit_code = "";

	/* 实体类定义 */
	CModel tmmsm34_1("TMMSM34_1");

	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);

	try
	{
		CDateTime datetime = CDateTime::Now();
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}
		
		
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_GROUP"))
			prod_shift_group = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_GROUP"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_NO"))
			prod_shift_no = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_F"))
			ch_hsf_end_time_f = bcls_rec->Tables[0].Rows[0]["PROD_TIME_F"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_T"))
			ch_hsf_end_time_t = bcls_rec->Tables[0].Rows[0]["PROD_TIME_T"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			factoryDiv = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
			//LG1---表示二钢，只对四级起作用
			//factoryDiv = "LG1";
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CC_MACH_NO"))
			cc_mach_no = bcls_rec->Tables[0].Rows[0]["CC_MACH_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("FINISH_FLAG"))
			finish_flag = bcls_rec->Tables[0].Rows[0]["FINISH_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MEND_FLAG"))
			mend_flag = bcls_rec->Tables[0].Rows[0]["MEND_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("QUERY_DIV"))
			queryDiv = bcls_rec->Tables[0].Rows[0]["QUERY_DIV"].ToString();

		//2024-03-13
		if (bcls_rec->Tables[0].Columns.Contains("UNIT_CODE"))
			unit_code = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"].ToString();

		tableName = queryDiv.Trim();

		Log::Trace("", "", "heat_no={0}", heat_no);
		Log::Trace("", "", "mat_no={0}", mat_no);
		Log::Trace("", "", "mend_flag={0}", mend_flag);
		Log::Trace("", "", "ch_hsf_end_time_f={0}", ch_hsf_end_time_f);
		Log::Trace("", "", "ch_hsf_end_time_t={0}", ch_hsf_end_time_t);
		Log::Trace("", "", "station_id={0}", station_id);
		Log::Trace("", "", "tableName={0}", tableName);
		Log::Trace("", "", "tableName2={0}", bcls_rec->Tables[0].Rows[0]["QUERY_DIV"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			
			sqlstr_count = " SELECT COUNT(1) FROM " + tableName + " WHERE 1=1 ";
			sqlstr = " SELECT * FROM " + tableName + " WHERE 1=1 ";

			if (station_id.Trim() != "")
			{
				if (tableName == "TMMSM01")
				{
					sqlstr_temp += " AND IC_CC_FLAG = @station_id";
				}
				else if (tableName == "TMMSM34")
				{
					sqlstr_temp += " AND STATION_ID = @station_id";
				}
				else if (tableName == "TPSSM81")
				{

				}
			}

			if (heat_no.Trim() != "")
			{
				sqlstr_temp += " AND HEAT_NO = @heat_no";
			}
			if (prod_shift_no.Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_NO = @prod_shift_no";
			}
			if (prod_shift_group.Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_GROUP = @prod_shift_group";
			}
			if (mat_no.Trim() != "")
			{
				sqlstr_temp += " AND MAT_NO = @mat_no";
			}
			if (ch_hsf_end_time_f.Trim() != "")
			{
				//ch_hsf_end_time_f = ch_hsf_end_time_f.Trim() + "000000";
				sqlstr_temp += " AND PROD_TIME >= @ch_hsf_end_time_f";
			}
			if (ch_hsf_end_time_t.Trim() != "")
			{
				//ch_hsf_end_time_t = ch_hsf_end_time_t.Trim() + "235959";
				sqlstr_temp += " AND PROD_TIME <= @ch_hsf_end_time_t";
			}
			if (factoryDiv.Trim() != "")
			{
				sqlstr_temp += " AND factory_div = @factoryDiv";
			}
			if (st_no.Trim() != "")
			{
				sqlstr_temp += " AND st_no = @st_no";
			
			}
			if (cc_mach_no.Trim() != "")
			{
				if (tableName == "TMMSM01")
				{
					sqlstr_temp += " AND unit_code = @cc_mach_no";
				}
				else if (tableName == "TMMSM34")
				{
					sqlstr_temp += " AND cc_mach_no = @cc_mach_no";
				}
			}
			if (finish_flag.Trim() != "")
			{
			
				sqlstr_temp += " AND finish_flag = @finish_flag";
			
			}
			if (mend_flag.Trim() != "")
			{

				sqlstr_temp += " AND MEND_FLAG = @mend_flag";

			}
			if (tableName == "TMMSM34")
			{
				if (unit_code.Trim() != "")
				{

					sqlstr_temp += " AND MEND_SET = @unit_code ";
				}
			}

			//COMPLEX_DECIDE_CODE----综判标记
			if (tableName == "TMMSM01")
			{
				//sqlstr_temp += " AND MAT_LINE_TYPE = 'SM' AND IN_FLAG = '1'";
				//修磨条件：收货标记为 rcv_mat_flag = S
				//添加条件，不能查出已装车的坯子  mfj  20240309
				
				sqlstr_temp += " AND MAT_LINE_TYPE = 'SM' AND IN_FLAG = '1' AND rcv_mat_flag = 'S' AND  LOGISTICS_STATUS IN('0','1','4')  AND (C_STATESIGN = '0' OR C_STATESIGN =' ' OR C_STATESIGN ='6')";

			}

			if (tableName == "TPSSM81")
			{
				
				sqlstr_temp += "  AND PLAN_TYPE = 'SMJ'  AND PLAN_STATUS = '08' ";
			
			}


			sqlstr_count = sqlstr_count + sqlstr_temp;

			if (tableName == "TMMSM01")
			{

				sqlstr_temp += " ORDER BY MAT_NO ";
			}
			else if (tableName == "TMMSM34")
			{
				sqlstr_temp += " ORDER BY REC_CREATE_TIME DESC,MAT_NO ASC ";
			}
			else if (tableName == "TPSSM81")
			{
				sqlstr_temp += " ORDER BY REC_CREATE_TIME DESC,MAT_NO ASC ";
			}


			sqlstr = sqlstr + sqlstr_temp;

			Log::Trace("", "", "sqlstr={0}", sqlstr);
			break;
		}
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.Parameters.Set("prod_shift_no", prod_shift_no);
		cmd_inq.Parameters.Set("prod_shift_group", prod_shift_group);
		cmd_inq.Parameters.Set("mat_no", mat_no);
		cmd_inq.Parameters.Set("ch_hsf_end_time_f", ch_hsf_end_time_f);
		cmd_inq.Parameters.Set("ch_hsf_end_time_t", ch_hsf_end_time_t);
		cmd_inq.Parameters.Set("factoryDiv", factoryDiv);
		cmd_inq.Parameters.Set("st_no", st_no);
		cmd_inq.Parameters.Set("cc_mach_no", cc_mach_no);
		cmd_inq.Parameters.Set("finish_flag", finish_flag);
		cmd_inq.Parameters.Set("mend_flag", mend_flag);
		if (tableName == "TMMSM34")
		{
			cmd_inq.Parameters.Set("unit_code", unit_code);
		}
		cmd_inq.Parameters.Set("station_id", station_id);

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

		//返回提示栏信息
		CFormattable arguments[] = { TotalRecordCount }; // 定义参数列表的数组
		CMessageFormat::Format(s.msg, "查询到数据[{0}]条。", arguments, 1); //查询到[{0}]条记录。
		CString ts = ((CDecimal)(CDateTime::Now() - datetime).TotalMilliseconds()).Round(0).ToString();
		CFormattable arguments2[] = { ts };
		CMessageFormat::Format(s.sysmsg, "SVC用时[{0}ms]", arguments2, 1);
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
