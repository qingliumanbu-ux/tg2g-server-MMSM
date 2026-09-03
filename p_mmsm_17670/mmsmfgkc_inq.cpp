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
BM2F_ENTERACE(mmsmfgkc_inq)

int f_mmsmfgkc_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString sqlstr_temp1 = "";
	int		TotalRecordCount = 0;
	CString begin_time = "";
	CString end_time = "";
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm85("TMMSM85");

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
		tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);
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
			record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
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

			sqlstr_count = " SELECT COUNT(1) FROM (SELECT  T2.BUNKER_NO, T2.BACK_N_1 ,T1.SEQ_NO , T2.BASE_NAME,T2.mat_name,T2.BACK_C5,T2.mat_code,T2.STOCK_WT,  "   
				"     T1.WEIGH_NO,T1.TIME_INSTOCK,T1.MAT_CODE,T1.MAT_CODE_LOT_NO,T1.QUALITY_BATCH_NO,T1.MAT_NAME as MAT_NAME1,T1.STOCK_WT as STOCK_WT1     "
				"     FROM  TMMSM85 T1 "
				"    LEFT JOIN TMMSM60 T2 ON T1.BUNKER_NO= T2.BUNKER_NO  WHERE T2.BACK_C4 IN ('MMSM518S2N','MMSM518_1S2N')  "
				
				;
			sqlstr = " WITH T3	 AS (SELECT  *  FROM (SELECT   QUALITY_BATCH_NO, ELM_NAME,ELM_VALUE  FROM  ( SELECT QUALITY_BATCH_NO, ELM_NAME,ELM_VALUE  FROM TMMSM81AL A ))   "
				"    pivot(MAX(ELM_VALUE)   FOR ELM_NAME IN('C' AS C_VALUE,'Si'AS SI_VALUE,'Mn'AS MN_VALUE,'P'AS P_VALUE,'S'AS S_VALUE,'Cr'AS CR_VALUE,'Ni'AS NI_VALUE,  "
				"    'Mo'AS MO_VALUE,'Co'AS CO_VALUE,'Nb'AS NB_VALUE,'Ti'AS TI_VALUE,'V'AS V_VALUE  )))  "
				"   SELECT  T2.BUNKER_NO, T2.BACK_N_1 ,T1.SEQ_NO , "
				"   T2.BASE_NAME,T2.mat_name,T2.BACK_C5,T2.mat_code,T2.STOCK_WT, "
				"   T1.WEIGH_NO,T1.TIME_INSTOCK,T1.MAT_CODE,T1.MAT_CODE_LOT_NO,T1.QUALITY_BATCH_NO,T1.MAT_NAME as MAT_NAME1,T1.STOCK_WT as STOCK_WT1, "
				"   T3.C_VALUE,T3.SI_VALUE,T3.MN_VALUE,T3.P_VALUE,T3.S_VALUE,T3.CR_VALUE,T3.NI_VALUE,T3.MO_VALUE,T3.CO_VALUE,T3.NB_VALUE,T3.TI_VALUE,T3.V_VALUE "
				"   FROM  TMMSM85 T1 LEFT JOIN TMMSM60 T2 ON T1.BUNKER_NO= T2.BUNKER_NO  "
				"   LEFT JOIN T3 ON T1.QUALITY_BATCH_NO=T3.QUALITY_BATCH_NO WHERE T2.BACK_C4 IN ('MMSM518S2N','MMSM518_1S2N')  "
				
				;

			if (begin_time.Trim() != "")
			{
				sqlstr_temp += " AND TIME_INSTOCK>=@begin_time";
			}
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND TIME_INSTOCK<=@end_time";
			}
			
			
			sqlstr_count = sqlstr_count + sqlstr_temp + " )";
			sqlstr_temp1 = "  ORDER BY  T2.BACK_N_1 ,T1.SEQ_NO  ";
			sqlstr = sqlstr + sqlstr_temp + sqlstr_temp1;
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