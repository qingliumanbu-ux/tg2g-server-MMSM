/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-04-18
Description: 成品改切记录报表查询
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsmysph_inq)

int f_mmsmysph_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_start_time = "";//开始时刻
	CString v_end_time = ""; //结束时刻
	CString v_heat_no = "";
	CString v_batch = "";
	CString v_st_no = "";
	CString v_bidui = "";
	CString v_print_no = "";

	CPageInfo pageInfo;

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr ="";
	CString sqlstr_union ="";
	CString sqlstr_temp = "";
	CString sqlstr_temp_1 = "";
	CString sqlstr_count = "";
	CString sqlstr_order = "";


	/* 数据库操作类定义 */
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

		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_FROM"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_TO"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("BATCH"))
			v_batch = bcls_rec->Tables[0].Rows[0]["BATCH"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("BIDUI"))
			v_bidui = bcls_rec->Tables[0].Rows[0]["BIDUI"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PRINT_NO"))
			v_print_no = bcls_rec->Tables[0].Rows[0]["PRINT_NO"].ToString().Trim();

		if (v_start_time == "" && v_end_time == "")
		{
			sprintf(s.msg, "开始时间和结束时间条件必须都有！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (v_start_time != ""){
			sqlstr_temp += " AND SLAB_CUT_TIME >= '" + v_start_time + "'";
			//sqlstr += " AND REC_CREATE_TIME >= @v_start_time";
		}
		if (v_end_time != "")
		{
			sqlstr_temp += " AND SLAB_CUT_TIME <= '" + v_end_time + "'";
		}
		if (v_heat_no != "")
		{
			sqlstr_temp += " AND HEAT_NO = '" + v_heat_no + "'";
		}
		if (v_batch != "")
		{
			sqlstr_temp += " AND BATCH = '" + v_batch + "'";
		}
		if (v_st_no != "")
		{
			sqlstr_temp += " AND ST_NO = '" + v_st_no + "'";
		}
		if (v_bidui != "")
		{
			sqlstr_temp_1 += " AND BIDUI = '" + v_bidui + "'";
		}
		if (v_print_no != "")
		{
			sqlstr_temp_1 += " AND PRINT_NO = '" + v_print_no + "'";
		}


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT * FROM(   "
				" SELECT HEAT_NO, BATCH, MAT_NO, ST_NO, SLAB_CUT_TIME, MAT_ACT_WT, MAT_DESTION, INITIAL_BATCH,PRINT_NO,BIDUI,SLAB_NO,MAT_ACT_LEN,MAT_ACT_WIDTH,MAT_ACT_THICK   "
				" FROM( "
				" SELECT HEAT_NO, BATCH, MAT_NO, ST_NO, SLAB_CUT_TIME, MAT_ACT_WT, MAT_DESTION, "
				" NVL((SELECT BATCH FROM TMMSM33 WHERE MAT_NO = T.MAT_NO), ' ') INITIAL_BATCH, PRINT_NO, "
				" CASE WHEN PRINT_NO <> SLAB_NO THEN '3' ELSE '2' END AS BIDUI, SLAB_NO, MAT_ACT_LEN, MAT_ACT_WIDTH, MAT_ACT_THICK "
				" FROM(SELECT HEAT_NO, BATCH, MAT_NO, ST_NO, SLAB_CUT_TIME, MAT_ACT_WT, MAT_DESTION, PRINT_NO, SLAB_NO, MAT_ACT_LEN, MAT_ACT_WIDTH, MAT_ACT_THICK  FROM TMMSM01 "
				" WHERE 1 = 1 " + sqlstr_temp +
				" UNION ALL SELECT HEAT_NO,BATCH,MAT_NO,ST_NO,SLAB_CUT_TIME,MAT_ACT_WT,MAT_DESTION,PRINT_NO,SLAB_NO,MAT_ACT_LEN,MAT_ACT_WIDTH,MAT_ACT_THICK  FROM HMMSM01  "
				" WHERE 1 = 1 " + sqlstr_temp + " ) T  where print_no in (select print_no from( "
				" SELECT  PRINT_NO "
				" FROM(SELECT PRINT_NO  FROM TMMSM01 "
				" WHERE 1 = 1 " + sqlstr_temp +
				" UNION ALL SELECT PRINT_NO  FROM HMMSM01  "
				" WHERE 1 = 1 " + sqlstr_temp + " )T )group by print_no having count(print_no)>1) AND PRINT_NO > '0' "
				" UNION ALL "
				" SELECT HEAT_NO, BATCH, MAT_NO, ST_NO, SLAB_CUT_TIME, MAT_ACT_WT, MAT_DESTION,   "
				" NVL((SELECT BATCH FROM TMMSM33 WHERE MAT_NO = T.MAT_NO), ' ') INITIAL_BATCH, PRINT_NO, "
				" CASE WHEN PRINT_NO <> SLAB_NO THEN '3' ELSE '0' END AS BIDUI, SLAB_NO, MAT_ACT_LEN, MAT_ACT_WIDTH, MAT_ACT_THICK "
				" FROM(SELECT HEAT_NO, BATCH, MAT_NO, ST_NO, SLAB_CUT_TIME, MAT_ACT_WT, MAT_DESTION, PRINT_NO, SLAB_NO, MAT_ACT_LEN, MAT_ACT_WIDTH, MAT_ACT_THICK  FROM TMMSM01  "
				" WHERE 1 = 1 " + sqlstr_temp +
				"  UNION ALL SELECT HEAT_NO,BATCH,MAT_NO,ST_NO,SLAB_CUT_TIME,MAT_ACT_WT,MAT_DESTION,PRINT_NO,SLAB_NO,MAT_ACT_LEN,MAT_ACT_WIDTH,MAT_ACT_THICK  FROM HMMSM01  "
				" WHERE 1 = 1 " + sqlstr_temp + ") T  where print_no in (select print_no from("
				" SELECT  PRINT_NO "
				" FROM(SELECT PRINT_NO  FROM TMMSM01 "
				" WHERE 1 = 1 " + sqlstr_temp +
				" UNION ALL SELECT PRINT_NO  FROM HMMSM01  "
				" WHERE 1 = 1 " + sqlstr_temp + ")T )group by print_no having count(print_no)=1) AND PRINT_NO > '0' "
				" UNION ALL "
				" SELECT HEAT_NO, BATCH, MAT_NO, ST_NO, SLAB_CUT_TIME, MAT_ACT_WT, MAT_DESTION, "
				" NVL((SELECT BATCH FROM TMMSM33 WHERE MAT_NO = T.MAT_NO), ' ') INITIAL_BATCH, PRINT_NO, '1' AS BIDUI, SLAB_NO, MAT_ACT_LEN, MAT_ACT_WIDTH, MAT_ACT_THICK "
				" FROM(SELECT HEAT_NO, BATCH, MAT_NO, ST_NO, SLAB_CUT_TIME, MAT_ACT_WT, MAT_DESTION, PRINT_NO, SLAB_NO, MAT_ACT_LEN, MAT_ACT_WIDTH, MAT_ACT_THICK  FROM TMMSM01 "
				" WHERE 1 = 1 " + sqlstr_temp + 
				" UNION ALL SELECT HEAT_NO,BATCH,MAT_NO,ST_NO,SLAB_CUT_TIME,MAT_ACT_WT,MAT_DESTION,PRINT_NO,SLAB_NO,MAT_ACT_LEN,MAT_ACT_WIDTH,MAT_ACT_THICK  FROM HMMSM01  "
				" WHERE 1 = 1 " + sqlstr_temp + " ) T  where print_no = ' ' "
				" ) ) WHERE 1=1 ";

			sqlstr_order += " ORDER BY  SLAB_CUT_TIME ASC";
			
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			
			
			sqlstr = sqlstr + sqlstr_temp_1 + sqlstr_order;


			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;

		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


