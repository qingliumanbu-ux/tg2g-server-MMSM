/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:
Date:     2023/3/11
Description: 查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmfptj_inq)

int f_mmsmfptj_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString sqlstr_order = "";
	int TotalRecordCount = 0;

	CString table_name = "";
	CString v_heat_no = "";
	CString v_st_no = "";
	CString v_dev_code = "";
	CString v_mat_no = "";
	CString rec_create_time = "";
	CString v_prod_time_from = "";
	CString v_prod_time_to = "";

	CString vapply = "";
	CString vmatno = "";
	CString vstatus = "";
	CString vcarno = "";
	CString v_area = "";


	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);
	CString rec_create_time_1 = "";


	try
	{
		try
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
			table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			rec_create_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			rec_create_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DEV_CODE"))
			v_dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_FROM"))
			v_prod_time_from = bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_TO"))
			v_prod_time_to = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("PURCHASEDOCID"))
			vapply = bcls_rec->Tables[0].Rows[0]["PURCHASEDOCID"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			vmatno = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STATUS"))
			vstatus = bcls_rec->Tables[0].Rows[0]["STATUS"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("CAR_NO"))
			vcarno = bcls_rec->Tables[0].Rows[0]["CAR_NO"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("AREA"))
			v_area = bcls_rec->Tables[0].Rows[0]["AREA"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "rec_create_time  =[{0}]", rec_create_time);
		//Log::Info("", __FUNCTION__, "prod_time_t  =[{0}]", prod_time_t);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//2026.04.08 查询废品统计周报表初始数据
			if (table_name == "TMMSMFPTJ"){
				sqlstr = " SELECT A.*,B.DATE_TIME\
				    FROM ( SELECT DECODE(T1.STEEL_TYPE_CNAME,'碳素废钢','碳钢','特殊废钢','碳钢',' ',' ','不锈钢') AS STEEL_TYPE,T1.*,T2.STOCK_INI_WT FROM (\
					SELECT NVL(STEEL_TYPE_CNAME, ' ') AS STEEL_TYPE_CNAME,\
					SUM(CASE WHEN APP_CODE = '有系统' THEN CUT_SCRAP_WT ELSE 0 END) AS IN_STOCK_WT1,\
					SUM(CASE WHEN APP_CODE = '无系统' THEN CUT_SCRAP_WT ELSE 0 END) AS IN_STOCK_WT2,\
					SUM(CUT_SCRAP_WT) AS WIPQUATITY\
					FROM (SELECT T.* FROM (\
					SELECT T.PROD_TIME, T.CUT_SCRAP_WT, T.GRADE_ID, T.STEEL_TYPE_1, T.APP_CODE, DECODE(W.CODE_DESC_1_CONTENT, '特殊废钢', '特殊废钢', '碳素废钢') as STEEL_TYPE_CNAME FROM TMMSMFP_TG T\
					LEFT JOIN(SELECT * FROM TWMSMZD02 WHERE CODE_CLASS = 'STEEL_TYPE') W ON T.GRADE_ID = W.CODE\
					UNION ALL\
					SELECT T.PROD_TIME, T.CUT_SCRAP_WT, T.GRADE_ID, T.STEEL_TYPE_1, T.APP_CODE, W.CODE_DESC_1_CONTENT AS STEEL_TYPE_CNAME FROM TMMSMFP_BX T\
					LEFT JOIN(SELECT * FROM TWMSMZD02 WHERE CODE_CLASS = 'STEEL_TYPE') W ON T.GRADE_ID = W.CODE\
					WHERE 1=1 ";

				if (rec_create_time != ""){
					sqlstr_temp += " AND PROD_TIME >= '" + rec_create_time + "'";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND PROD_TIME <= '" + rec_create_time_1 + "'";
				}

				sqlstr_order = " )T LEFT JOIN (SELECT * FROM TWMSMZD02 WHERE CODE_CLASS='STEEL_TYPE') W ON T.GRADE_ID = W.CODE\
					order by PROD_TIME)\
					GROUP BY STEEL_TYPE_CNAME)T1\
					LEFT JOIN(SELECT s.STEEL_TYPE_CNAME, NVL(t.stock_end_wt, 0) AS stock_ini_wt\
					FROM(SELECT DISTINCT STEEL_TYPE_CNAME FROM tmmsmfptj) s\
					LEFT JOIN(SELECT STEEL_TYPE_CNAME, stock_end_wt, ROW_NUMBER() OVER(PARTITION BY STEEL_TYPE_CNAME ORDER BY rec_create_time DESC) rn\
					FROM tmmsmfptj\
					WHERE TO_DATE(rec_create_time, 'yyyyMMddhh24miss') >= ADD_MONTHS(TRUNC(SYSDATE, 'mm'), -1)\
					AND TO_DATE(rec_create_time, 'yyyyMMddhh24miss') < TRUNC(SYSDATE, 'mm')\
					) t ON s.STEEL_TYPE_CNAME = t.STEEL_TYPE_CNAME AND t.rn = 1 ORDER BY s.STEEL_TYPE_CNAME)T2\
					ON T1.STEEL_TYPE_CNAME = T2.STEEL_TYPE_CNAME ORDER BY T1.STEEL_TYPE_CNAME)A\
					CROSS JOIN (SELECT MAX(DATE_TIME) AS DATE_TIME FROM TMMSMFPTJ)B";
				sqlstr = sqlstr + sqlstr_temp + sqlstr_order;
			}
			//查询废品统计周报表最终结果
			if (table_name == "TMMSMFPTJ_BB"){
				sqlstr = "SELECT * FROM tmmsmfptj WHERE rec_create_time = (SELECT MAX(rec_create_time) FROM tmmsmfptj) ORDER BY STEEL_TYPE_CNAME";

				sqlstr = sqlstr + sqlstr_temp + sqlstr_order;
			}
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

		//cmd_inq.SetCommandText(sqlstr_count);
		//TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
