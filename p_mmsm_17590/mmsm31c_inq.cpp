/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 炼钢转炉作业其它实绩查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsm31c_inq)

int f_mmsm31c_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int TotalRecordCount = 0;

	CString prod_time_f = "";
	CString prod_time_t = "";
	CString station_no = "";
	CString heat_no = "";
	CString pono = "";
	CString st_no = "";
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);

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
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_F"))
			prod_time_f = bcls_rec->Tables[0].Rows[0]["PROD_TIME_F"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_T"))
			prod_time_t = bcls_rec->Tables[0].Rows[0]["PROD_TIME_T"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_NO"))
			station_no = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "prod_time_f  =[{0}]", prod_time_f);
		Log::Info("", __FUNCTION__, "prod_time_t  =[{0}]", prod_time_t);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = 
				" SELECT COUNT(1) "
				"   FROM TMMSM31 t1 "
				"  WHERE 1=1 "
				;
			sqlstr = 
				" SELECT t1.*,tb.* ,t2.HEAT_NO AS NEXT_HEAT_NO  ,t3.HEAT_NO AS PREV_HEAT_NO"
				"        FROM  TMMSM31 t1 "
				"        LEFT JOIN TMMSM31B tb ON t1.HEAT_NO=tb.HEAT_NO AND   t1.PROC_NO=tb.PROC_NO"
				"        LEFT JOIN (SELECT * FROM TPSSM41 UNION SELECT * FROM TPSSM11) t2 "
				"        on t1.cast_no=t2.cast_no  AND t1.CAST_DIV_NO=t2.CAST_DIV_NO-1"
				"        LEFT JOIN (SELECT * FROM TPSSM41 UNION SELECT * FROM TPSSM11) t3 "
				"        on t1.cast_no=t3.cast_no  AND t1.CAST_DIV_NO=t3.CAST_DIV_NO+1 "
				" where 1=1 ";
				;
			if (heat_no != "")
			{
				sqlstr_temp += " AND t1.HEAT_NO LIKE '%" + heat_no + "%'";
			}
			if (pono != "")
			{
				sqlstr_temp += " AND t1.PONO LIKE '%" + pono + "%'";
			}
			if (station_no != "")
			{
				sqlstr_temp += " AND t1.STATION_NO = '" + station_no + "'";
			}
			if (st_no != "")
			{
				sqlstr_temp += " AND t1.ST_NO = '" + st_no + "'";
			}
			if (prod_time_f != "")
			{
				sqlstr_temp += " AND t1.START_TIME >= '" + prod_time_f + "'";
			}
			if (prod_time_t != "")
			{
				sqlstr_temp += " AND t1.START_TIME <= '" + prod_time_t + "'";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY t1.STATION_NO, t1.START_TIME DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr_count  =[{0}]", sqlstr_count);
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
