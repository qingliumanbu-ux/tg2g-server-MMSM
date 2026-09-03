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
BM2F_ENTERACE(mmsmyljlls_inq)

int f_mmsmyljlls_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	CDecimal cd_count = 0;
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString devo_time1 = "";
	int		TotalRecordCount = 0;
	CString s_bunker_no = "";
	CString s_L2_PROC_NO = "";
	CString s_sm_plan_no = "";
	CString s_stk_no = "";
	int i_idx = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm2a_yl("TMMSM2A_YL");

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
		//--------------------------------
		//获取传入参数
		tmmsm2a_yl.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		devo_time1 = bcls_rec->Tables[0].Rows[0]["DEVO_TIME1"].ToString().Trim();


		Log::Info("", __FUNCTION__, "L2_PROC_NO =[{0}]", tmmsm2a_yl["L2_PROC_NO"].ToString());
		Log::Info("", __FUNCTION__, "SM_PLAN_NO =[{0}]", tmmsm2a_yl["SM_PLAN_NO"].ToString());
		Log::Info("", __FUNCTION__, "STK_NO =[{0}]", tmmsm2a_yl["STK_NO"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) FROM ( "
				" SELECT SUBSTR(MAX(DEVO_TIME),1,8) AS PROD_DATE,A.L2_PROC_NO ,MAX(A.HEAT_NO) HEAT_NO,MAX(A.LAYERNO) LAYERNO, MAX(DEVO_TIME) AS DEVO_TIME,SUM(DEVO_WT) DEVO_WT,"
				" MAX(C.SM_PLAN_NOL2) AS SM_PLAN_NO,A.MAT_CODE,MAT_NAME,WEIGH_NO,MAX(SPLIT_INDICATION) AS SPLIT_INDICATION,A.STK_NO,MAX(C.ST_NO) ST_NO,"
				" MAX(CASE WHEN SUBSTR(C.L2_PROC_NO, 1, 1) = 'E' THEN C.ACTRESULT WHEN SUBSTR(C.L2_PROC_NO, 1, 1) = 'B' THEN C.ACTRESULT WHEN SUBSTR(C.L2_PROC_NO, 1, 1) = 'A' THEN C.ACTRESULT ELSE 0 END)  STEEL_WT,MAX(D.SHIFT_GROUP) SHIFT_GROUP,MAX(D.SHIFT_NO) SHIFT_NO,MAX(EVENT_TIME) EVENT_TIME FROM"
				" (SELECT * FROM(SELECT ACTRESULT, ST_NO, L2_PROC_NO, SM_PLAN_NOL2 FROM TMMSM20 WHERE  SUBSTR(L2_PROC_NO, 1, 1) = 'E' UNION"
				" SELECT ACTRESULT, ST_NO, L2_PROC_NO,SM_PLAN_NOL2 FROM TMMSM21 WHERE  SUBSTR(L2_PROC_NO, 1, 1) = 'B' UNION"
				" SELECT ACTRESULT, ST_NO, L2_PROC_NO,SM_PLAN_NOL2 FROM TMMSM27 WHERE  SUBSTR(L2_PROC_NO, 1, 1) = 'A' )) C"
				" LEFT JOIN (SELECT * FROM TMMSM2A_YL WHERE STK_NO IN(SELECT BUNKER_NO  FROM TMMSM60 WHERE BUNKER_TYPE IN('EAFBOX', 'BOFBOX', 'AODBOX'))) A"
				" ON A.L2_PROC_NO = C.L2_PROC_NO"
				" LEFT JOIN "
				" (SELECT A.L2_PROC_NO,MAX(A.HEAT_NO) HEAT_NO,A.MAT_CODE,MAX(B.EVENT_TIME) EVENT_TIME,MAX(B.SHIFT_GROUP) SHIFT_GROUP,MAX(B.SHIFT_NO) SHIFT_NO FROM"
				" (SELECT MAT_CODE,STOCK_WT,WEIGH_NO, RECEIVE_DATA_TIME,SHIFT_GROUP,SHIFT_NO,EVENT_TIME,L2_PROC_NO,HEAT_NO,BUNKER_NO FROM TMMSM89"
				" WHERE EVENT_DESC LIKE '%加料'  AND BUNKER_TYPE LIKE '%BOX')A LEFT JOIN"
				" (SELECT MAT_CODE,STOCK_WT,WEIGH_NO, RECEIVE_DATA_TIME,SHIFT_GROUP,SHIFT_NO,EVENT_TIME"
				" FROM TMMSM89 WHERE EVENT_DESC='移库' AND  (FUNC_ID='mmsm831_upd2' OR FUNC_ID='mmsm831_upd3') AND  BUNKER_TYPE LIKE '%BOX') B "
				" ON A.MAT_CODE=B.MAT_CODE AND A.WEIGH_NO=B.WEIGH_NO  AND A.RECEIVE_DATA_TIME=B.RECEIVE_DATA_TIME GROUP BY A.L2_PROC_NO,A.MAT_CODE) 	D  "
				" ON A.L2_PROC_NO = D.L2_PROC_NO AND  A.MAT_CODE = D.MAT_CODE "
				" WHERE 1= 1 AND A.WEIGH_NO<>' '"
				;

			sqlstr =" SELECT SUBSTR(MAX(DEVO_TIME),1,8) AS PROD_DATE,A.L2_PROC_NO ,MAX(A.HEAT_NO) HEAT_NO,MAX(A.LAYERNO) LAYERNO, MAX(DEVO_TIME) AS DEVO_TIME,SUM(DEVO_WT) DEVO_WT,"
                    " MAX(C.SM_PLAN_NOL2) AS SM_PLAN_NO,A.MAT_CODE,MAT_NAME,WEIGH_NO,MAX(SPLIT_INDICATION) AS SPLIT_INDICATION,A.STK_NO,MAX(C.ST_NO) ST_NO,"
                    " MAX(CASE WHEN SUBSTR(C.L2_PROC_NO, 1, 1) = 'E' THEN C.ACTRESULT WHEN SUBSTR(C.L2_PROC_NO, 1, 1) = 'B' THEN C.ACTRESULT WHEN SUBSTR(C.L2_PROC_NO, 1, 1) = 'A' THEN C.ACTRESULT ELSE 0 END)  STEEL_WT,MAX(D.SHIFT_GROUP) SHIFT_GROUP,MAX(D.SHIFT_NO) SHIFT_NO,MAX(EVENT_TIME) EVENT_TIME FROM"
                    " (SELECT * FROM(SELECT ACTRESULT, ST_NO, L2_PROC_NO, SM_PLAN_NOL2 FROM TMMSM20 WHERE  SUBSTR(L2_PROC_NO, 1, 1) = 'E' UNION"
                    " SELECT ACTRESULT, ST_NO, L2_PROC_NO,SM_PLAN_NOL2 FROM TMMSM21 WHERE  SUBSTR(L2_PROC_NO, 1, 1) = 'B' UNION"
                    " SELECT ACTRESULT, ST_NO, L2_PROC_NO,SM_PLAN_NOL2 FROM TMMSM27 WHERE  SUBSTR(L2_PROC_NO, 1, 1) = 'A' )) C"
                    " LEFT JOIN (SELECT * FROM TMMSM2A_YL WHERE STK_NO IN(SELECT BUNKER_NO  FROM TMMSM60 WHERE BUNKER_TYPE IN('EAFBOX', 'BOFBOX', 'AODBOX'))) A"
                    " ON A.L2_PROC_NO = C.L2_PROC_NO"
                    " LEFT JOIN "
					" (SELECT A.L2_PROC_NO,MAX(A.HEAT_NO) HEAT_NO,A.MAT_CODE,MAX(B.EVENT_TIME) EVENT_TIME,MAX(B.SHIFT_GROUP) SHIFT_GROUP,MAX(B.SHIFT_NO) SHIFT_NO FROM"
					" (SELECT MAT_CODE,STOCK_WT,WEIGH_NO, RECEIVE_DATA_TIME,SHIFT_GROUP,SHIFT_NO,EVENT_TIME,L2_PROC_NO,HEAT_NO,BUNKER_NO FROM TMMSM89"
					" WHERE EVENT_DESC LIKE '%加料'  AND BUNKER_TYPE LIKE '%BOX')A LEFT JOIN"
					" (SELECT MAT_CODE,STOCK_WT,WEIGH_NO, RECEIVE_DATA_TIME,SHIFT_GROUP,SHIFT_NO,EVENT_TIME"
					" FROM TMMSM89 WHERE EVENT_DESC='移库' AND  (FUNC_ID='mmsm831_upd2' OR FUNC_ID='mmsm831_upd3') AND  BUNKER_TYPE LIKE '%BOX') B "
					" ON A.MAT_CODE=B.MAT_CODE AND A.WEIGH_NO=B.WEIGH_NO  AND A.RECEIVE_DATA_TIME=B.RECEIVE_DATA_TIME  GROUP BY A.L2_PROC_NO,A.MAT_CODE ) 	D  "
					" ON A.L2_PROC_NO = D.L2_PROC_NO AND  A.MAT_CODE = D.MAT_CODE "
					" WHERE 1= 1 AND A.WEIGH_NO<>' '"
					;

			if (tmmsm2a_yl["L2_PROC_NO"].ToString().Trim() != "")

			{
				sqlstr_temp += " AND A.L2_PROC_NO like '%'|| @L2_PROC_NO||'%'";
			}
			if (tmmsm2a_yl["SM_PLAN_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND A.SM_PLAN_NO like '%'|| @SM_PLAN_NO||'%'";
			}
			if (tmmsm2a_yl["STK_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND A.STK_NO like '%'|| @STK_NO||'%'";
			}
			if (tmmsm2a_yl["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND A.MAT_CODE like '%'|| @MAT_CODE||'%'";
			}

			if (tmmsm2a_yl["DEVO_TIME"].ToString() != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME >= @DEVO_TIME";
			}
			if (devo_time1 != "")
			{
				sqlstr_temp += " AND A.DEVO_TIME <= @DEVO_TIME1";
			}
			sqlstr_count = sqlstr_count + sqlstr_temp + "  GROUP BY A.L2_PROC_NO, A.STK_NO, A.MAT_CODE, A.MAT_NAME, A.WEIGH_NO ORDER BY DEVO_TIME DESC, A.L2_PROC_NO DESC)  ";
			sqlstr = sqlstr + sqlstr_temp + "  GROUP BY A.L2_PROC_NO,A.STK_NO,A.MAT_CODE,A.MAT_NAME,A.WEIGH_NO ORDER BY DEVO_TIME DESC,A.L2_PROC_NO DESC ";
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.Parameters.Set("L2_PROC_NO", tmmsm2a_yl["L2_PROC_NO"].ToString());
		cmd_inq.Parameters.Set("SM_PLAN_NO", tmmsm2a_yl["SM_PLAN_NO"].ToString());
		cmd_inq.Parameters.Set("STK_NO", tmmsm2a_yl["STK_NO"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm2a_yl["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("DEVO_TIME", tmmsm2a_yl["DEVO_TIME"].ToString());
		cmd_inq.Parameters.Set("DEVO_TIME1", devo_time1);

		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		Log::Info("", __FUNCTION__, "TotalRecordCount =[{0}]", TotalRecordCount);
		//分页获取
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
