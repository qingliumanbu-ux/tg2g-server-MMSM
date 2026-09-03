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
BM2F_ENTERACE(mmsmylgxhz_inq)

int f_mmsmylgxhz_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;
	int	 union_flag = 0;
	int	 empty_flag = 0;
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlstr2 = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString STATION_ID = "";
	CString HEAT_NO_A1_S = "";
	CString	HEAT_NO_A1_F = "";
	CString	HEAT_NO_A2_S = "";
	CString	HEAT_NO_A2_F = "";
	CString	HEAT_NO_A0_S = "";
	CString	HEAT_NO_A0_F = "";
	CString	HEAT_NO_A6_S = "";
	CString	HEAT_NO_A6_F = "";
	CString	HEAT_NO_B1_S = "";
	CString	HEAT_NO_B1_F = "";
	CString	HEAT_NO_B2_S = "";
	CString	HEAT_NO_B2_F = "";
	CString	HEAT_NO_B0_S = "";
	CString	HEAT_NO_B0_F = "";
	CString	HEAT_NO_E1_S = "";
	CString	HEAT_NO_E1_F = "";
	CString	HEAT_NO_E2_S = "";
	CString	HEAT_NO_E2_F = "";
	CString	HEAT_NO_F1_S = "";
	CString	HEAT_NO_F1_F = "";
	CString	HEAT_NO_F2_S = "";
	CString	HEAT_NO_F2_F = "";
	CString	HEAT_NO_F3_S = "";
	CString	HEAT_NO_F3_F = "";
	CString	HEAT_NO_F4_S = "";
	CString	HEAT_NO_F4_F = "";
	CString	HEAT_NO_F5_S = "";
	CString	HEAT_NO_F5_F = "";
	CString	HEAT_NO_F6_S = "";
	CString	HEAT_NO_F6_F = "";
	CString	HEAT_NO_F7_S = "";
	CString	HEAT_NO_F7_F = "";
	CString	HEAT_NO_F8_S = "";
	CString	HEAT_NO_F8_F = "";
	
	//系统的分页类信息。
	CPageInfo pageInfo;
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
		STATION_ID = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString();
		HEAT_NO_A1_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A1_S"].ToString();
		HEAT_NO_A1_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A1_F"].ToString();
		HEAT_NO_A2_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A2_S"].ToString();
		HEAT_NO_A2_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A2_F"].ToString();
		HEAT_NO_A0_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A0_S"].ToString();
		HEAT_NO_A0_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A0_F"].ToString();
		HEAT_NO_A6_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A6_S"].ToString();
		HEAT_NO_A6_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_A6_F"].ToString();
		HEAT_NO_B1_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_B1_S"].ToString();
		HEAT_NO_B1_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_B1_F"].ToString();
		HEAT_NO_B2_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_B2_S"].ToString();
		HEAT_NO_B2_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_B2_F"].ToString();
		HEAT_NO_B0_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_B0_S"].ToString();
		HEAT_NO_B0_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_B0_F"].ToString();
		HEAT_NO_E1_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_E1_S"].ToString();
		HEAT_NO_E1_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_E1_F"].ToString();
		HEAT_NO_E2_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_E2_S"].ToString();
		HEAT_NO_E2_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_E2_F"].ToString();
		HEAT_NO_F1_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F1_S"].ToString();
		HEAT_NO_F1_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F1_F"].ToString();
		HEAT_NO_F2_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F2_S"].ToString();
		HEAT_NO_F2_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F2_F"].ToString();
		HEAT_NO_F3_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F3_S"].ToString();
		HEAT_NO_F3_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F3_F"].ToString();
		HEAT_NO_F4_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F4_S"].ToString();
		HEAT_NO_F4_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F4_F"].ToString();
		HEAT_NO_F5_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F5_S"].ToString();
		HEAT_NO_F5_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F5_F"].ToString();
		HEAT_NO_F6_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F6_S"].ToString();
		HEAT_NO_F6_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F6_F"].ToString();
		HEAT_NO_F7_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F7_S"].ToString();
		HEAT_NO_F7_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F7_F"].ToString();
		HEAT_NO_F8_S = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F8_S"].ToString();
		HEAT_NO_F8_F = bcls_rec->Tables[0].Rows[0]["HEAT_NO_F8_F"].ToString();



		Log::Info("", __FUNCTION__, "station_id =[{0}]", STATION_ID);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:




			sqlstr1 = " SELECT  MAT_CODE,MAT_NAME,STATION_ID,SUM(DEVO_WT)/1000 AS STOCK_WT FROM TMMSM2A_YL WHERE 1=1 ";
			sqlstr2 = " SELECT  MAT_CODE,SUM(DEVO_WT)/1000 AS STOCK_WT FROM TMMSM2A_YL WHERE 1=1 ";
			//AOD
			if (HEAT_NO_A1_S.Trim() != ""&& HEAT_NO_A1_F.Trim() != "")
			{
				sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_A1_S + "' AND L2_PROC_NO<='"+HEAT_NO_A1_F + "')";
				union_flag = 1;
			}
			else if (HEAT_NO_A1_S.Trim() != ""|| HEAT_NO_A1_F.Trim() != "")
			{
				if (HEAT_NO_A1_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_A1_S + "'";
				if (HEAT_NO_A1_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_A1_F + "'";
				union_flag = 1;
			}
			else
			{
				union_flag = 0;
			}

			if (union_flag)
			{
				if (HEAT_NO_A2_S.Trim() != ""&& HEAT_NO_A2_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_A2_S + "' AND L2_PROC_NO<='" + HEAT_NO_A2_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_A2_S.Trim() != ""|| HEAT_NO_A2_F.Trim() != "")
				{
					if (HEAT_NO_A2_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_A2_S + "'";
					if (HEAT_NO_A2_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_A2_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					empty_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_A2_S.Trim() != ""&& HEAT_NO_A2_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_A2_S + "' AND L2_PROC_NO<='" + HEAT_NO_A2_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_A2_S.Trim() != ""|| HEAT_NO_A2_F.Trim() != "")
				{
					if (HEAT_NO_A2_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_A2_S + "'";
					if (HEAT_NO_A2_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_A2_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_A0_S.Trim() != ""&& HEAT_NO_A0_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_A0_S + "' AND L2_PROC_NO<='" + HEAT_NO_A0_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_A0_S.Trim() != ""|| HEAT_NO_A0_F.Trim() != "")
				{
					if (HEAT_NO_A0_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_A0_S + "'";
					if (HEAT_NO_A0_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_A0_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					empty_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_A0_S.Trim() != ""&& HEAT_NO_A0_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_A0_S + "' AND L2_PROC_NO<='" + HEAT_NO_A0_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_A0_S.Trim() != "" || HEAT_NO_A0_F.Trim() != "")
				{
					if (HEAT_NO_A0_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_A0_S + "'";
					if (HEAT_NO_A0_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_A0_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_A6_S.Trim() != ""&& HEAT_NO_A6_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_A6_S + "' AND L2_PROC_NO<='" + HEAT_NO_A6_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_A6_S.Trim() != ""|| HEAT_NO_A6_F.Trim() != "")
				{
					if (HEAT_NO_A6_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_A6_S + "'";
					if (HEAT_NO_A6_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_A6_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					empty_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_A6_S.Trim() != ""&& HEAT_NO_A6_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_A6_S + "' AND L2_PROC_NO<='" + HEAT_NO_A6_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_A6_S.Trim() != "" || HEAT_NO_A6_F.Trim() != "")
				{
					if (HEAT_NO_A6_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_A6_S + "'";
					if (HEAT_NO_A6_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_A6_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}

			}
			
			//BOF
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_B1_S.Trim() != ""&& HEAT_NO_B1_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_B1_S + "' AND L2_PROC_NO<='" + HEAT_NO_B1_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_B1_S.Trim() != ""|| HEAT_NO_B1_F.Trim() != "")
				{
					if (HEAT_NO_B1_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_B1_S + "'";
					if (HEAT_NO_B1_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_B1_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					empty_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_B1_S.Trim() != ""&& HEAT_NO_B1_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_B1_S + "' AND L2_PROC_NO<='" + HEAT_NO_B1_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_B1_S.Trim() != "" || HEAT_NO_B1_F.Trim() != "")
				{
					if (HEAT_NO_B1_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_B1_S + "'";
					if (HEAT_NO_B1_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_B1_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_B2_S.Trim() != ""&& HEAT_NO_B2_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_B2_S + "' AND L2_PROC_NO<='" + HEAT_NO_B2_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_B2_S.Trim() != ""|| HEAT_NO_B2_F.Trim() != "")
				{
					if (HEAT_NO_B2_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_B2_S + "'";
					if (HEAT_NO_B2_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_B2_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_B2_S.Trim() != ""&& HEAT_NO_B2_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_B2_S + "' AND L2_PROC_NO<='" + HEAT_NO_B2_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_B2_S.Trim() != "" || HEAT_NO_B2_F.Trim() != "")
				{
					if (HEAT_NO_B2_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_B2_S + "'";
					if (HEAT_NO_B2_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_B2_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}

			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_B0_S.Trim() != ""&& HEAT_NO_B0_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_B0_S + "' AND L2_PROC_NO<='" + HEAT_NO_B0_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_B0_S.Trim() != ""|| HEAT_NO_B0_F.Trim() != "")
				{
					if (HEAT_NO_B0_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_B0_S + "'";
					if (HEAT_NO_B0_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_B0_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_B0_S.Trim() != ""&& HEAT_NO_B0_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_B0_S + "' AND L2_PROC_NO<='" + HEAT_NO_B0_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_B0_S.Trim() != "" || HEAT_NO_B0_F.Trim() != "")
				{
					if (HEAT_NO_B0_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_B0_S + "'";
					if (HEAT_NO_B0_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_B0_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			
			//EAF
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_E1_S.Trim() != ""&& HEAT_NO_E1_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_E1_S + "' AND L2_PROC_NO<='" + HEAT_NO_E1_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_E1_S.Trim() != ""|| HEAT_NO_E1_F.Trim() != "")
				{
					if (HEAT_NO_E1_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_E1_S + "'";
					if (HEAT_NO_E1_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_E1_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;

				}
			}
			else
			{
				if (HEAT_NO_E1_S.Trim() != ""&& HEAT_NO_E1_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_E1_S + "' AND L2_PROC_NO<='" + HEAT_NO_E1_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_E1_S.Trim() != "" || HEAT_NO_E1_F.Trim() != "")
				{
					if (HEAT_NO_E1_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_E1_S + "'";
					if (HEAT_NO_E1_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_E1_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;

				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_E2_S.Trim() != ""&& HEAT_NO_E2_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_E2_S + "' AND L2_PROC_NO<='" + HEAT_NO_E2_F + "')";
					union_flag = 1;
				}
				else if(HEAT_NO_E2_S.Trim() != ""|| HEAT_NO_E2_F.Trim() != "")
				{
					if (HEAT_NO_E2_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_E2_S + "'";
					if (HEAT_NO_E2_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_E2_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_E2_S.Trim() != ""&& HEAT_NO_E2_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_E2_S + "' AND L2_PROC_NO<='" + HEAT_NO_E2_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_E2_S.Trim() != "" || HEAT_NO_E2_F.Trim() != "")
				{
					if (HEAT_NO_E2_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_E2_S + "'";
					if (HEAT_NO_E2_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_E2_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			
			//IF
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_F1_S.Trim() != ""&& HEAT_NO_F1_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_F1_S + "' AND L2_PROC_NO<='" + HEAT_NO_F1_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F1_S.Trim() != ""|| HEAT_NO_F1_F.Trim() != "")
				{
					if (HEAT_NO_F1_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_F1_S + "'";
					if (HEAT_NO_F1_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_F1_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_F1_S.Trim() != ""&& HEAT_NO_F1_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_F1_S + "' AND L2_PROC_NO<='" + HEAT_NO_F1_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F1_S.Trim() != "" || HEAT_NO_F1_F.Trim() != "")
				{
					if (HEAT_NO_F1_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_F1_S + "'";
					if (HEAT_NO_F1_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_F1_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_F2_S.Trim() != ""&& HEAT_NO_F2_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_F2_S + "' AND L2_PROC_NO<='" + HEAT_NO_F2_F + "')";
					union_flag = 1;
				}
				else if(HEAT_NO_F2_S.Trim() != ""|| HEAT_NO_F2_F.Trim() != "")
				{
					if (HEAT_NO_F2_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_F2_S + "'";
					if (HEAT_NO_F2_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_F2_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_F2_S.Trim() != ""&& HEAT_NO_F2_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_F2_S + "' AND L2_PROC_NO<='" + HEAT_NO_F2_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F2_S.Trim() != "" || HEAT_NO_F2_F.Trim() != "")
				{
					if (HEAT_NO_F2_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_F2_S + "'";
					if (HEAT_NO_F2_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_F2_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_F3_S.Trim() != ""&& HEAT_NO_F3_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_F3_S + "' AND L2_PROC_NO<='" + HEAT_NO_F3_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F3_S.Trim() != ""||HEAT_NO_F3_F.Trim() != "")
				{
					if (HEAT_NO_F3_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_F3_S + "'";
					if (HEAT_NO_F3_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_F3_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_F3_S.Trim() != ""&& HEAT_NO_F3_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_F3_S + "' AND L2_PROC_NO<='" + HEAT_NO_F3_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F3_S.Trim() != "" || HEAT_NO_F3_F.Trim() != "")
				{
					if (HEAT_NO_F3_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_F3_S + "'";
					if (HEAT_NO_F3_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_F3_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_F4_S.Trim() != ""&& HEAT_NO_F4_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_F4_S + "' AND L2_PROC_NO<='" + HEAT_NO_F4_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F4_S.Trim() != ""|| HEAT_NO_F4_F.Trim() != "")
				{
					if (HEAT_NO_F4_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_F4_S + "'";
					if (HEAT_NO_F4_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_F4_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_F4_S.Trim() != ""&& HEAT_NO_F4_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_F4_S + "' AND L2_PROC_NO<='" + HEAT_NO_F4_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F4_S.Trim() != "" || HEAT_NO_F4_F.Trim() != "")
				{
					if (HEAT_NO_F4_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_F4_S + "'";
					if (HEAT_NO_F4_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_F4_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_F5_S.Trim() != ""&& HEAT_NO_F5_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_F5_S + "' AND L2_PROC_NO<='" + HEAT_NO_F5_F + "')";
					union_flag = 1;
				}
				else if(HEAT_NO_F5_S.Trim() != ""|| HEAT_NO_F5_F.Trim() != "")
				{
					if (HEAT_NO_F5_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_F5_S + "'";
					if (HEAT_NO_F5_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_F5_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_F5_S.Trim() != ""&& HEAT_NO_F5_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_F5_S + "' AND L2_PROC_NO<='" + HEAT_NO_F5_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F5_S.Trim() != "" || HEAT_NO_F5_F.Trim() != "")
				{
					if (HEAT_NO_F5_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_F5_S + "'";
					if (HEAT_NO_F5_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_F5_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_F6_S.Trim() != ""&& HEAT_NO_F6_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_F6_S + "' AND L2_PROC_NO<='" + HEAT_NO_F6_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F6_S.Trim() != ""|| HEAT_NO_F6_F.Trim() != "")
				{
					if (HEAT_NO_F6_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_F6_S + "'";
					if (HEAT_NO_F6_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_F6_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_F6_S.Trim() != ""&& HEAT_NO_F6_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_F6_S + "' AND L2_PROC_NO<='" + HEAT_NO_F6_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F6_S.Trim() != "" || HEAT_NO_F6_F.Trim() != "")
				{
					if (HEAT_NO_F6_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_F6_S + "'";
					if (HEAT_NO_F6_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_F6_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_F7_S.Trim() != ""&& HEAT_NO_F7_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_F7_S + "' AND L2_PROC_NO<='" + HEAT_NO_F7_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F7_S.Trim() != ""|| HEAT_NO_F7_F.Trim() != "")
				{
					if (HEAT_NO_F7_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_F7_S + "'";
					if (HEAT_NO_F7_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_F7_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_F7_S.Trim() != ""&& HEAT_NO_F7_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_F7_S + "' AND L2_PROC_NO<='" + HEAT_NO_F7_F + "')";
					union_flag = 1;
				}
				else if (HEAT_NO_F7_S.Trim() != "" || HEAT_NO_F7_F.Trim() != "")
				{
					if (HEAT_NO_F7_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_F7_S + "'";
					if (HEAT_NO_F7_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_F7_F + "'";
					union_flag = 1;
				}
				else
				{
					union_flag = 0;
				}
			}
			if (union_flag || empty_flag)
			{
				if (HEAT_NO_F8_S.Trim() != ""&& HEAT_NO_F8_F.Trim() != "")
				{
					sqlstr_temp += " OR (L2_PROC_NO>='" + HEAT_NO_F8_S + "' AND L2_PROC_NO<='" + HEAT_NO_F8_F + "')";
				}
				else if (HEAT_NO_F8_S.Trim() != ""|| HEAT_NO_F8_F.Trim() != "")
				{
					if (HEAT_NO_F8_S.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO>='" + HEAT_NO_F8_S + "'";
					if (HEAT_NO_F8_F.Trim() != "")  sqlstr_temp += " OR L2_PROC_NO<='" + HEAT_NO_F8_F + "'";
				}
				else
				{
					union_flag = 0;
					union_flag = 1;
				}
			}
			else
			{
				if (HEAT_NO_F8_S.Trim() != ""&& HEAT_NO_F8_F.Trim() != "")
				{
					sqlstr_temp += " AND (L2_PROC_NO>='" + HEAT_NO_F8_S + "' AND L2_PROC_NO<='" + HEAT_NO_F8_F + "')";
				}
				else if (HEAT_NO_F8_S.Trim() != "" || HEAT_NO_F8_F.Trim() != "")
				{
					if (HEAT_NO_F8_S.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO>='" + HEAT_NO_F8_S + "'";
					if (HEAT_NO_F8_F.Trim() != "")  sqlstr_temp += " AND L2_PROC_NO<='" + HEAT_NO_F8_F + "'";
				}
				else
				{
					union_flag = 0;
				}

			}
			
			if (STATION_ID.Trim() != "")
			{
				sqlstr_temp += " AND STATION_ID='" + STATION_ID + "'";
			}

			sqlstr1 = sqlstr1 + sqlstr_temp + "  GROUP BY MAT_CODE,MAT_NAME,STATION_ID  ";;
			sqlstr2 = sqlstr2 + sqlstr_temp + "  GROUP BY MAT_CODE ";
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr1 =[{0}]", sqlstr1);
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr2);


		cmd_inq.Close();
		//分页获取

		cmd_inq.SetCommandText(sqlstr1);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		bcls_ret->Tables.Add();
		cmd_inq.SetCommandText(sqlstr2);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();

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