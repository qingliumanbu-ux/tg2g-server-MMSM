/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料进料汇总查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmylcf_inq)

int f_mmsmylcf_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString begin_time = "";
	CString end_time = "";
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
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
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
			//record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			//current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		}
		Log::Info("", __FUNCTION__, "BEGIN_TIME =[{0}]", begin_time);
		Log::Info("", __FUNCTION__, "END_TIME =[{0}]", end_time);
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
				"   FROM tmmsm81 "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT A.WEIGH_NO,A.LOT_NO,A.QUALITY_BATCH_NO,A.MAT_CODE,A.MAT_NAME,A.STOCK_WT,A.MAT_RCV_TIME,   "
				"   (SELECT NVL(ELM_VALUE,0)  FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='C' fetch first 1 rows only) C, "
				"   (SELECT NVL(ELM_VALUE,0)  FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Si' fetch first 1 rows only) SI, "
				"   (SELECT NVL(ELM_VALUE,0)  FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Mn' fetch first 1 rows only) MN, "
				"   (SELECT NVL(ELM_VALUE,0)  FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='P' fetch first 1 rows only) P, "
				"   (SELECT NVL(ELM_VALUE,0)  FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='S' fetch first 1 rows only) S, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Cr' fetch first 1 rows only) CR, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Ni' fetch first 1 rows only) NI, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Ti' fetch first 1 rows only) TI, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Cu' fetch first 1 rows only) CU, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Nb' fetch first 1 rows only) NB, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Al' fetch first 1 rows only) AL, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='B' fetch first 1 rows only) B, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='W' fetch first 1 rows only) W, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Mo' fetch first 1 rows only) MO, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='MoO2' fetch first 1 rows only) MOO2, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='MoO3' fetch first 1 rows only) MOO3, "
				"   (SELECT NVL(ELM_VALUE,0) FROM TMMSM81AL  WHERE QUALITY_BATCH_NO = A.QUALITY_BATCH_NO AND MAT_CODE=A.MAT_CODE  AND ELM_NAME='Co' fetch first 1 rows only) CO "
				"    FROM TMMSM81 A where 1 = 1  "
				;

			if (begin_time.Trim() != "")
			{
				sqlstr_temp += " AND MAT_RCV_TIME>=@begin_time";
			}
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND MAT_RCV_TIME<=@end_time";
			}
			
			
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += "  ORDER BY mat_code  ";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		
		
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);

		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.Close();
		//分页获取

		Log::Trace(" ", __FUNCTION__, "sqlstr={[0]}", sqlstr);
		cmd_inq.SetCommandText(sqlstr);

		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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