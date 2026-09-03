/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-12-21
Description: 查预溶液的熔炼号重复数据
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


BM2F_ENTERACE(mmsmaod_inq)


int f_mmsmaod_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CDecimal FURNACE_AGE_SM = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tmmsm27("TMMSM27");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		CString cs_station_no = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString().Trim();
		CString l2_proc_no = bcls_rec->Tables[0].Rows[0]["L2_PROC_NO"].ToString().Trim();
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		CString start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		CString end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		CString div = bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("", "", "cs_station_no = [{0}]", cs_station_no);
		Log::Trace("", "", "div = [{0}]", div);
		/* 设置开始时刻和结束时刻 */
		if (start_time.Trim() != "")
		{
			start_time = start_time.Substring(0, 8);
			start_time += "000000";
		}
		if (end_time.Trim() != "")
		{
			end_time = end_time.Substring(0, 8);
			end_time += "235959";
		}

		if (div == "ZC")//正常查询
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * FROM TMMSM27 where 1=1 ";
				if (heat_no != "")
				{
					sqlstr += " AND HEAT_NO = @heat_no";
				}
				if (l2_proc_no != "")
				{
					sqlstr += " AND L2_PROC_NO = @l2_proc_no";
				}
				if (start_time != "")
				{
					sqlstr += " AND START_TIME >=@start_time";
				}
				if (end_time != "")
				{
					sqlstr += " AND END_TIME <= @end_time";
				}
				break;
			}
			cmd_inq.Parameters.Set("cs_station_no", cs_station_no);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("l2_proc_no", l2_proc_no);
			cmd_inq.Parameters.Set("start_time", start_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		if (div=="CF")//重复数据
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT DISTINCT  HEATNO_PREMELT1 FROM( "
					"SELECT HEATNO_PREMELT1 HEATNO_PREMELT1 "
					"FROM TMMSM27 "
					"WHERE STATION_NO = @cs_station_no AND HEATNO_PREMELT1 <> ' ' "
					"UNION ALL "
					"SELECT HEATNO_PREMELT2 HEATNO_PREMELT1 "
					"FROM TMMSM27 "
					"WHERE STATION_NO = @cs_station_no AND HEATNO_PREMELT2 <> ' ' "
					"UNION ALL "
					"SELECT HEATNO_PREMELT3 HEATNO_PREMELT1 "
					"FROM TMMSM27 "
					"WHERE STATION_NO = @cs_station_no AND HEATNO_PREMELT3 <> ' ' "
					"UNION ALL "
					"SELECT HEATNO_PREMELT4 HEATNO_PREMELT1 "
					"FROM TMMSM27 "
					"WHERE STATION_NO = @cs_station_no AND HEATNO_PREMELT4 <> ' ' "
					"UNION ALL "
					"SELECT HEATNO_PREMELT5 HEATNO_PREMELT1 "
					"FROM TMMSM27 "
					"WHERE STATION_NO = @cs_station_no AND HEATNO_PREMELT5 <> ' ' "
					"UNION ALL  "
					"SELECT HEATNO_PREMELT6 HEATNO_PREMELT1 "
					"FROM TMMSM27 "
					"WHERE STATION_NO = @cs_station_no AND HEATNO_PREMELT6 <> ' ' "
					")  GROUP BY HEATNO_PREMELT1 HAVING  COUNT(HEATNO_PREMELT1)  > 1 ";
				Log::Trace("", "", "sqlstr IN:---sqlstr = [{0}]", (const char*)sqlstr);
				break;
			}
			cmd_inq.Parameters.Set("cs_station_no", cs_station_no);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
			Log::Trace("", "", "sqlstr IN:---sqlstr = [{0}]", bcls_ret->Tables[0].Rows.get_Count());
		}
		if (div == "TH"){//查询跳号数据
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "WITH table1 AS (select substr(heatno_premelt1,2,7) as 预溶液号 from tmmsm27 where heatno_premelt1 like 'F1%' "
				" union "
				" select substr(heatno_premelt2, 2, 7) as 预溶液号 from tmmsm27 where heatno_premelt2 like 'F1%' "
				" union "
				" select substr(heatno_premelt3, 2, 7) as 预溶液号 from tmmsm27 where heatno_premelt3 like 'F1%') "
				" select t.*, (t.X_HEAT_NO - D_HEAT_NO) as ZHI from(SELECT 预溶液号 as D_HEAT_NO, LEAD(预溶液号, 1, 0) over(ORDER BY 预溶液号 ASC) AS X_HEAT_NO from table1) t ";
				Log::Trace("", "", "sqlstr IN:---sqlstr = [{0}]", (const char*)sqlstr);
				break;
			}

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
			Log::Trace("", "", "sqlstr IN:---sqlstr = [{0}]", bcls_ret->Tables[0].Rows.get_Count());
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetMsg() };
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
