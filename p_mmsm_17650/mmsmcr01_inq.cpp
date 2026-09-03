/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 铬镍收得率明细表
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmcr01_inq)

int f_mmsmcr01_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString end_time = " ";
	CString end_time_1 = " ";
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

	CDbCommand cmd_inq(conn);

	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
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
		

		sqlstr = " SELECT BOF0_S,BOF0_E,BOF1_S,BOF1_E,BOF2_S,BOF2_E,BOF9_S,BOF9_E,AOD0_S,AOD0_E,AOD1_S,AOD1_E,AOD2_S,AOD2_E,AOD6_S,AOD6_E   FROM TMMSMZXHBB_LH  WHERE 1 = 1 ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			heat_no1 = cmd_inq.GetString(1);
			heat_no2 = cmd_inq.GetString(2);
			heat_no3 = cmd_inq.GetString(3);
			heat_no4 = cmd_inq.GetString(4);
			heat_no5 = cmd_inq.GetString(5);
			heat_no6 = cmd_inq.GetString(6);
			heat_no7 = cmd_inq.GetString(7);
			heat_no8 = cmd_inq.GetString(8);
			heat_no9 = cmd_inq.GetString(9);
			heat_no10 = cmd_inq.GetString(10);
			heat_no11 = cmd_inq.GetString(11);
			heat_no12 = cmd_inq.GetString(12);
			heat_no13 = cmd_inq.GetString(13);
			heat_no14 = cmd_inq.GetString(14);
			heat_no15 = cmd_inq.GetString(15);
			heat_no16 = cmd_inq.GetString(16);

		}
		else
		{
			strcpy(s.msg, "未查询到炉号数据请先执行F3按钮生成数据");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();

		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no1, heat_no2);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no3, heat_no4);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no5, heat_no6);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no7, heat_no8);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no9, heat_no10);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no11, heat_no12);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no13, heat_no14);
		Log::Info("", __FUNCTION__, "heat_no1   =[{0}],heat_no2   =[{1}]", heat_no15, heat_no16);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " select count(1) "
				"	FROM  (SELECT  ZXH.HEAT_NO, ZXH.ST_NO, MAX(TIME_1) TIME_1, MAX(USER_NAME) USER_NAME, MAX(TAP_END_TIME) TAP_END_TIME, MAX(PROC_NO)PROC_NO, MAX(PROD_OUT_WT) PROD_OUT_WT, "
				"	MAX(ORIGIN_SYS_CODE) ORIGIN_SYS_CODE, MAX(ST_NO_DESC) ST_NO_DESC, MAX(ST_NO_SMALL_CLASS1)ST_NO_SMALL_CLASS, MAX(ST_NO_BIG_CLASS) ST_NO_BIG_CLASS, SUM(NI_VALUE*DEVO_WT) NI_VALUE, "
				"	SUM(CR_VALUE*DEVO_WT) CR_VALUE, SUM(DEVO_WT) DEVO_WT, NVL(MAX(B0.Ni), 0) Ni, NVL(MAX(B0.CR), 0) CR, MAX(MAT_ACT_WT) MAT_ACT_WT FROM "
				"	(SELECT * FROM TMMSMZXHBB WHERE 1=1 and((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16)) AND SUBSTR(ST_NO,1,1) IN('1','4') ) ZXH LEFT JOIN (SELECT * FROM VMMSMCPCL_BB1 WHERE 1=1 and ((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))) BB1 ON ZXH.HEAT_NO = BB1.HEAT_NO AND ZXH.ST_NO = BB1.ST_NO LEFT JOIN "
				"	(SELECT NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END,0) Cr, NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END,0) Ni, HEAT_NO, ST_NO FROM TQMTSB0 WHERE 1=1 and((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))) B0 	ON ZXH.HEAT_NO = B0.HEAT_NO  WHERE 1 = 1 ";

			sqlstr = "SELECT ZXH.TIME_1 DATE_1,ZXH.USER_NAME DAMIN,ZXH.TAP_END_TIME,ZXH.HEAT_NO,ZXH.PROC_NO,ZXH.PROD_OUT_WT MAT_WT,ZXH.ORIGIN_SYS_CODE DEV_CODE, "
				"	NVL(MAT_ACT_WT, 0) HGCL, ZXH.ST_NO, ZXH.ST_NO_DESC ST_NO_MS, ZXH.ST_NO_SMALL_CLASS1 ST_NO_LB, ZXH.ST_NO_BIG_CLASS ST_NO_DL, "
				"	ROUND(NVL(CR*MAT_ACT_WT, 0), 3) AT_CR, ROUND(NVL((CASE WHEN ZXH.ST_NO = (SELECT ST_NO FROM TMMSMW8 WHERE ST_NO =ZXH.ST_NO  ) "
				"   THEN 0 ELSE NI END)*MAT_ACT_WT, 0), 3) AT_NI, ROUND(ZXH.NI_VALUE, 3) DAVO_NI, "
				"	ROUND(ZXH.CR_VALUE, 3) DAVO_CR, "
				"	CASE WHEN ZXH.ST_NO = (SELECT ST_NO FROM TMMSMW8 WHERE ST_NO =ZXH.ST_NO  ) THEN 0 ELSE ROUND(nvl(CASE WHEN MAT_ACT_WT*Ni = 0 OR ZXH.NI_VALUE = 0 "
				"	THEN 0 ELSE  (MAT_ACT_WT*Ni)/(ZXH.NI_VALUE)   END, 0), 5) * 100 END  NI, "
				"	ROUND(nvl(CASE WHEN MAT_ACT_WT*CR = 0 OR ZXH.CR_VALUE = 0 THEN 0 ELSE(MAT_ACT_WT*CR)/(ZXH.CR_VALUE)   END, 0), 5) * 100 CR "
				"	FROM  (SELECT  ZXH.HEAT_NO, ZXH.ST_NO, MAX(TIME_1) TIME_1, MAX(USER_NAME) USER_NAME, MAX(TAP_END_TIME) TAP_END_TIME, MAX(PROC_NO)PROC_NO, MAX(PROD_OUT_WT) PROD_OUT_WT, "
				"	MAX(ORIGIN_SYS_CODE) ORIGIN_SYS_CODE, MAX(ST_NO_DESC) ST_NO_DESC, MAX(ST_NO_SMALL_CLASS1)ST_NO_SMALL_CLASS1, MAX(ST_NO_BIG_CLASS) ST_NO_BIG_CLASS, SUM(NI_VALUE*DEVO_WT) NI_VALUE, "
				"	SUM(CR_VALUE*DEVO_WT) CR_VALUE, SUM(DEVO_WT) DEVO_WT, NVL(MAX(B0.Ni), 0) Ni, NVL(MAX(B0.CR), 0) CR, MAX(MAT_ACT_WT) MAT_ACT_WT FROM "
				"	(SELECT * FROM TMMSMZXHBB WHERE 1=1 and((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16)) AND SUBSTR(ST_NO,1,1) IN('1','4') ) ZXH LEFT JOIN (SELECT * FROM VMMSMCPCL_BB1 WHERE 1=1 and ((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))) BB1 ON ZXH.HEAT_NO = BB1.HEAT_NO AND ZXH.ST_NO = BB1.ST_NO LEFT JOIN "
				"	(SELECT NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END,0) Cr, NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END,0) Ni, HEAT_NO, ST_NO FROM TQMTSB0 WHERE 1=1 and((HEAT_NO >= @heat_no1  AND HEAT_NO <= @heat_no2) OR(HEAT_NO >= @heat_no3 AND HEAT_NO <= @heat_no4) OR(HEAT_NO >= @heat_no5  AND "
				"	HEAT_NO <= @heat_no6) OR(HEAT_NO >= @heat_no7 AND HEAT_NO <= @heat_no8) OR(HEAT_NO >= @heat_no9 "
				"	AND HEAT_NO <= @heat_no10) OR(HEAT_NO >= @heat_no11 AND HEAT_NO <= @heat_no12) OR(HEAT_NO >= @heat_no13 AND HEAT_NO <= @heat_no14) "
				"	OR(HEAT_NO >= @heat_no15 AND HEAT_NO <= @heat_no16))) B0 	ON ZXH.HEAT_NO = B0.HEAT_NO  WHERE 1 = 1 ";
			/*if (end_time.Trim() != "")
			{
			sqlstr_temp += " AND ZXH.TAP_END_TIME >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
			sqlstr_temp += " AND ZXH.TAP_END_TIME <= @end_time_1";
			}*/

			sqlstr_temp = " and ((ZXH.HEAT_NO >=@heat_no1  AND ZXH.HEAT_NO <= @heat_no2) OR (ZXH.HEAT_NO >= @heat_no3 AND ZXH.HEAT_NO <= @heat_no4) OR (ZXH.HEAT_NO >= @heat_no5  AND "
				"	ZXH.HEAT_NO <= @heat_no6) OR(ZXH.HEAT_NO >= @heat_no7 AND ZXH.HEAT_NO <= @heat_no8) OR(ZXH.HEAT_NO >= @heat_no9 "
				"	AND ZXH.HEAT_NO <= @heat_no10) OR(ZXH.HEAT_NO >= @heat_no11 AND ZXH.HEAT_NO <= @heat_no12) OR(ZXH.HEAT_NO >= @heat_no13 AND ZXH.HEAT_NO <= @heat_no14) "
				"	OR(ZXH.HEAT_NO >= @heat_no15 AND ZXH.HEAT_NO <= @heat_no16)) ";
			sql_group = " GROUP BY ZXH.HEAT_NO,ZXH.ST_NO  order by ZXH.HEAT_NO ) ZXH ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			sqlstr_count = sqlstr_count + sqlstr_temp + sql_group;
			//cmd_inq.Parameters.Set("end_time", end_time);
			//cmd_inq.Parameters.Set("end_time_1", end_time_1);
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

			cmd_inq.SetCommandText(sqlstr);
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			//cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
			Log::Info("", __FUNCTION__, "Rows   =[{0}]", bcls_ret->Tables[0].Rows.get_Count());
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
