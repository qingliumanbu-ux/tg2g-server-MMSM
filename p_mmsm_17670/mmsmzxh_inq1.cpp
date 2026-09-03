/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 总消耗查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmzxh_inq1)

int f_mmsmzxh_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString start_time = " ";
	CString end_time_1 = " ";
	CString end_time = " ";
	CString heat_no1 = " ";
	CString heat_no2 = " ";
	CString heat_no3 = " ";
	CString heat_no4 = " ";
	CString heat_no5 = " ";
	CString heat_no6 = " ";
	CString heat_no7 = " ";
	CString heat_no8 = " ";
	CString heat_no9 = " ";
	CString heat_no10 = " ";
	CString heat_no11 = " ";
	CString heat_no12 = " ";
	CString heat_no13 = " ";
	CString heat_no14 = " ";
	CString heat_no15 = " ";
	CString heat_no16 = " ";
	CString nian = "";
	CString yue = "";
	CString ri = "";
	CDecimal cd_count = 0;
	CDecimal devo_wt = 0;
	CString biaoji = "0";
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	int i_idx = 0;
	int i_count = 0;
	CDecimal ni_wt = 0; // 成分*同炉同物料汇总重量Ni
	CDecimal cr_wt = 0; // 成分*同炉同物料汇总重量Cr
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CModel tmmsmzxhbb("TMMSMZXHBB");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass EItables;
	CPageInfo pageInfo;

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

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_1"))
			heat_no1 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_1"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_2"))
			heat_no2 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_2"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_3"))
			heat_no3 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_3"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_4"))
			heat_no4 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_4"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_5"))
			heat_no5 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_5"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_6"))
			heat_no6 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_6"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_7"))
			heat_no7 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_7"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_8"))
			heat_no8 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_8"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_9"))
			heat_no9 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_9"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_10"))
			heat_no10 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_10"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_11"))
			heat_no11 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_11"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_12"))
			heat_no12 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_12"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_13"))
			heat_no13 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_13"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_14"))
			heat_no14 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_14"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_15"))
			heat_no15 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_15"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_16"))
			heat_no16 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_16"].ToString();
		/*end_time += "000000"; end_time = CDateTime::Now().AddHours(-4).ToString("yyyyMMddHH")+"5959";
		end_time_1 += "999999";*/
		/*if (start_time.Trim()!="")
		{
		if (start_time.GetLength()==14)
		{
		start_time = (bcls_rec->Tables[0].Rows[0]["START_TIME"].ToDateTime().AddDays(-2)).ToString("yyyyMMddHHmmss");
		}
		}



		if (end_time.Trim() != "")
		{
		if (end_time.GetLength() == 14)
		{
		end_time = (bcls_rec->Tables[0].Rows[0]["END_TIME"].ToDateTime().AddDays(+3)).ToString("yyyyMMddHHmmss");
		}
		}*/

		Log::Info("", __FUNCTION__, "start_time   =[{0}]", start_time);
		Log::Info("", __FUNCTION__, "end_time   =[{0}]", end_time);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr_count = "  SELECT COUNT(1) FROM TMMSMZXHBB SM56 WHERE 1=1 ";

			sqlstr = " SELECT TIME_1 DATE_TIME, USER_NAME ADMIN ,TAP_END_TIME END_TIME,HEAT_NO,PROC_NO,PROD_OUT_WT SCL,ORIGIN_SYS_CODE LY,DEV_CODE,SHIFT_GROUP,ST_NO,ST_NO_DESC,ST_NO_SMALL_CLASS,ST_NO_BIG_CLASS, BACKLOG_CODE ROUTELIST, "
				" TYPE_CODE1 TYPE_DL,MAT_CODE,MAT_NAME,NI_VALUE NI,CR_VALUE CR,DEVO_WT,MAT1_USE DH,PRICE JG,COST_HJ CB  FROM TMMSMZXHBB SM56 WHERE 1=1 "
				;
			/*if (start_time.Trim() != "")
			{
			sqlstr_temp += " AND A.END_TIME >= @start_time";
			}
			if (end_time.Trim() != "")
			{
			sqlstr_temp += " AND A.END_TIME <= @end_time";
			}*/

			sqlstr_temp += " and ((SM56.HEAT_NO >=@heat_no1  AND SM56.HEAT_NO <= @heat_no2) OR (SM56.HEAT_NO >= @heat_no3 AND SM56.HEAT_NO <= @heat_no4) OR (SM56.HEAT_NO >= @heat_no5  AND "
				"	SM56.HEAT_NO <= @heat_no6) OR(SM56.HEAT_NO >= @heat_no7 AND SM56.HEAT_NO <= @heat_no8) OR(SM56.HEAT_NO >= @heat_no9 "
				"	AND SM56.HEAT_NO <= @heat_no10) OR(SM56.HEAT_NO >= @heat_no11 AND SM56.HEAT_NO <= @heat_no12) OR(SM56.HEAT_NO >= @heat_no13 AND SM56.HEAT_NO <= @heat_no14) "
				"	OR(SM56.HEAT_NO >= @heat_no15 AND SM56.HEAT_NO <= @heat_no16)) ";

			/*if (heat_no1.Trim() != "" && heat_no2.Trim() == "")
			{
			sqlstr_temp += " AND (( A.HEAT_NO >= @heat_no1 ) ";
			}
			if (heat_no1.Trim() == "" && heat_no2.Trim() != "")
			{
			sqlstr_temp += " AND (( A.HEAT_NO <= @heat_no2 )";
			}
			if (heat_no1.Trim() != "" && heat_no2.Trim() != "")
			{
			sqlstr_temp += " AND (( A.HEAT_NO >= @heat_no1 and A.HEAT_NO <= @heat_no2 ) ";
			}
			if (heat_no1.Trim() == "" && heat_no2.Trim() == "" )
			{
			if (heat_no3.Trim() != "" && heat_no4.Trim() == "")
			{
			sqlstr_temp += " AND ((A.HEAT_NO >= @heat_no3)";
			}
			if (heat_no3.Trim() == "" && heat_no4.Trim() != "")
			{
			sqlstr_temp += " and ((A.HEAT_NO <= @heat_no4)";
			}

			if (heat_no3.Trim() != "" && heat_no4.Trim() != "")
			{
			sqlstr_temp += " AND (( A.HEAT_NO >= @heat_no3 and A.HEAT_NO <= @heat_no4 ) ";
			}

			}
			else
			{
			if (heat_no3.Trim() != "" && heat_no4.Trim() == "")
			{
			sqlstr_temp += " or (A.HEAT_NO >= @heat_no3)";
			}
			if (heat_no3.Trim() == "" && heat_no4.Trim() != "")
			{
			sqlstr_temp += " or (A.HEAT_NO <= @heat_no4)";
			}

			if (heat_no3.Trim() != "" && heat_no4.Trim() != "")
			{
			sqlstr_temp += " or ( A.HEAT_NO >= @heat_no3 and A.HEAT_NO <= @heat_no4 ) ";
			}
			}

			*/

			sql_group = "   ORDER BY SM56.HEAT_NO   ";
			sqlstr_count = sqlstr_count + sqlstr_temp + "   ORDER BY SM56.HEAT_NO   ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;

			cmd_inq.Parameters.Set("start_time", start_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("heat_no1", heat_no1);
			cmd_inq.Parameters.Set("heat_no2", heat_no2);
			cmd_inq.Parameters.Set("heat_no3", heat_no3);
			cmd_inq.Parameters.Set("heat_no4", heat_no4);
			cmd_inq.Parameters.Set("heat_no5", heat_no5);
			cmd_inq.Parameters.Set("heat_no6", heat_no6);
			cmd_inq.Parameters.Set("heat_no7", heat_no7);
			cmd_inq.Parameters.Set("heat_no8", heat_no8);
			cmd_inq.Parameters.Set("heat_no9", heat_no9);
			cmd_inq.Parameters.Set("heat_no10", heat_no10);
			cmd_inq.Parameters.Set("heat_no11", heat_no11);
			cmd_inq.Parameters.Set("heat_no12", heat_no12);
			cmd_inq.Parameters.Set("heat_no13", heat_no13);
			cmd_inq.Parameters.Set("heat_no14", heat_no14);
			cmd_inq.Parameters.Set("heat_no15", heat_no15);
			cmd_inq.Parameters.Set("heat_no16", heat_no16);
			Log::Trace(" ", __FUNCTION__, "sqlstr_count==[{0}]", sqlstr_count);
			cmd_inq.SetCommandText(sqlstr_count);
			cd_count = cmd_inq.ExecuteScalar();
			start_row = record_count_per_page * (current_page_no - 1);
			if (start_row > cd_count.ToDouble())
			{
				start_row = 0;
			}
			cmd_inq.Close();
			//分页获取

			Log::Trace(" ", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);

			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
			cmd_inq.Close();

			//返回分页总数量信息 
			bcls_ret->Tables.Add("PAGEINFO");	//增加块
			bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");//总记录数
			bcls_ret->Tables["PAGEINFO"].Rows.Add();
			bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
		}
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
