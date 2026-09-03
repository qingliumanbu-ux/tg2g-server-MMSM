/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:
Date:     2023/3/11
Description: 北区工艺参数查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmgycs_inq)

int f_mmsmgycs_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString cost_center = "";
	CString rec_create_time = "";
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
		if (bcls_rec->Tables[0].Columns.Contains("COST_CENTER"))
			cost_center = bcls_rec->Tables[0].Rows[0]["COST_CENTER"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			rec_create_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			rec_create_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "cost_center  =[{0}]", cost_center);
		//Log::Info("", __FUNCTION__, "prod_time_t  =[{0}]", prod_time_t);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			if (cost_center == "EGBB"){
				//sqlstr_count = " SELECT COUNT(1) "
					//"  FROM TMMSM21 "
					//"  WHERE 1=1 "
					//;
				sqlstr = " SELECT 'EGBB' AS COST_CENTER,T1.*,T2.MAT_ACT_WT,ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) AS MELT_DURATION_1 "
					"  FROM TMMSM21 T1 "
					"  LEFT JOIN "
					" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
					" GROUP BY HEAT_NO)T2 "
					" ON T1.HEAT_NO = T2.HEAT_NO WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND END_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND END_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY END_TIME ";

			}

			if (cost_center == "EGBC"){
				
				sqlstr = " SELECT 'EGBC' AS COST_CENTER,T1.*,T2.MAT_ACT_WT,ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) AS MELT_DURATION_1 "
					"  FROM TMMSM24 T1 "
					"  LEFT JOIN "
					" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
					" GROUP BY HEAT_NO)T2 "
					" ON T1.HEAT_NO = T2.HEAT_NO WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND END_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND END_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY END_TIME ";
			}

			if (cost_center == "EGBD"){
				
				sqlstr = " SELECT 'EGBD' AS COST_CENTER,T1.*,T2.MAT_ACT_WT,ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) AS MELT_DURATION_1 "
					"  FROM TMMSM23 T1 "
					"  LEFT JOIN "
					" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
					" GROUP BY HEAT_NO)T2 "
					" ON T1.HEAT_NO = T2.HEAT_NO WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND END_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND END_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY END_TIME ";
			}

			if (cost_center == "EGBF"){
				
				sqlstr = " SELECT 'EGBF' AS COST_CENTER,T1.*,T2.MAT_ACT_WT,ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) AS MELT_DURATION_1 "
					"  FROM TMMSM20 T1 "
					"  LEFT JOIN "
					" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
					" GROUP BY HEAT_NO)T2 "
					" ON T1.HEAT_NO = T2.HEAT_NO WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND END_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND END_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY END_TIME ";
			}

			if (cost_center == "EGBG"){
				
				sqlstr = " SELECT 'EGBG' AS COST_CENTER,T3.*,T4.MAT_ACT_WT, "
					" ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') - TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60) / 60, 0) AS MELT_DURATION_1 "
					" FROM(SELECT T1.START_TIME, T1.END_TIME, T1.L2_PROC_NO, T2.HEAT_NO, T1.ST_NO, T5.RET_HEAT_NO, NVL(T5.RET_HEAT_NO, T2.HEAT_NO) AS HEAT_NO1,T1.POWER_CONSUME "
					" FROM TMMSM19 T1 "
					" LEFT JOIN TMMSMGY06 T2 ON T1.L2_PROC_NO = T2.L2_PROC_NO "
					" LEFT JOIN TPSSM35 T5 ON T1.HEAT_NO = T5.HEAT_NO) T3 "
					" LEFT JOIN "
					" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
					" GROUP BY HEAT_NO)T4 "
					" ON T3.HEAT_NO1 = T4.HEAT_NO WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND END_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND END_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY END_TIME ";
			}

			if (cost_center == "EGBH"){
				
				sqlstr = " SELECT 'EGBH' AS COST_CENTER,T1.*,T2.MAT_ACT_WT,ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) AS MELT_DURATION_1 "
					"  FROM TMMSM27 T1 "
					"  LEFT JOIN "
					" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
					" GROUP BY HEAT_NO)T2 "
					" ON T1.HEAT_NO = T2.HEAT_NO WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND END_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND END_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY END_TIME ";
			}

			if (cost_center == "EGBI"){
				
				sqlstr = " SELECT 'EGBI' AS COST_CENTER,T1.*,T2.MAT_ACT_WT,ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) AS MELT_DURATION_1 "
					"  FROM TMMSM25 T1 "
					"  LEFT JOIN "
					" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
					" GROUP BY HEAT_NO)T2 "
					" ON T1.HEAT_NO = T2.HEAT_NO WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND END_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND END_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY END_TIME ";
			}

			if (cost_center == "EGBK"){
				
				sqlstr = " SELECT 'EGBK' AS COST_CENTER,T3.END_TIME,T3.HEAT_NO,T3.ST_NO,T4.MAT_ACT_WT, "
					" COUNT(*) OVER(PARTITION BY CAST_DIV_NO, TD_NO_1, CAST_DIV_NO_1) AS LJLS FROM "
					" (SELECT T1.END_TIME, T1.HEAT_NO, T1.ST_NO, T2.CAST_DIV_NO, T2.TD_NO_1, T2.CAST_DIV_NO_1 "
					" FROM TMMSM31 T1 "
					" LEFT JOIN TMMSMGY05 T2 ON T1.HEAT_NO = T2.HEAT_NO WHERE SUBSTR(T1.ST_NO, 0, 1) = '2')T3 "
					" LEFT JOIN "
					" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
					" GROUP BY HEAT_NO)T4 "
					" ON T3.HEAT_NO = T4.HEAT_NO WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND END_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND END_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY END_TIME ";
			}

			if (cost_center == "EGBL"){
				
				sqlstr = " SELECT 'EGBL' AS COST_CENTER,T1.END_TIME,T1.HEAT_NO,T1.ST_NO,T2.MAT_ACT_WT, "
					" COUNT(*) OVER(PARTITION BY DEV_CODE, CAST_DIV_NO, TD_NO_1 ORDER BY DEV_CODE, CAST_DIV_NO, LADLE_LEAVE_TIME, TD_NO_1) AS LJLS "
					" FROM(SELECT * FROM TMMSM31 WHERE SUBSTR(ST_NO, 0, 1) = '1') T1 "
					" LEFT JOIN "
					" (SELECT HEAT_NO, SUM(MAT_ACT_WT) AS MAT_ACT_WT FROM VMMSMCPCL_BB1 "
					" GROUP BY HEAT_NO)T2 "
					" ON T1.HEAT_NO = T2.HEAT_NO WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND END_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND END_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY END_TIME ";
			}


			
			if (cost_center == "EGBM"){
				
				sqlstr = " SELECT * FROM( "
					" SELECT T3.*, ROW_NUMBER() OVER(PARTITION BY HEAT_NO ORDER BY END_TIME) AS RN FROM( "
					" SELECT  'EGBM' AS COST_CENTER, GRINDING_START_TIME AS END_TIME,T.HEAT_NO,ST_NO,T2.MEND_BEFORE_WEIGHT,T2.MEND_AFTER_WEIGHT "
					" FROM (SELECT GRINDING_START_TIME,HEAT_NO,ST_NO FROM TMMSM34 WHERE 1=1 ";
				if (rec_create_time != ""){
					sqlstr_temp += " AND GRINDING_START_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND GRINDING_START_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_temp += " ) T  LEFT JOIN (SELECT T1.HEAT_NO, sum(mend_before_weight) as mend_before_weight, sum(mend_after_weight) as mend_after_weight FROM TMMSM34 T1  WHERE 1 = 1  ";
				if (rec_create_time != ""){
					sqlstr_temp += " AND GRINDING_START_TIME>='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND GRINDING_START_TIME<='" + rec_create_time_1 + "' ";
				}
				sqlstr_temp += " GROUP BY T1.HEAT_NO)T2  ON T.HEAT_NO = T2.HEAT_NO ";

				sqlstr_order = " ORDER BY GRINDING_START_TIME )T3) WHERE RN = '1' ORDER BY END_TIME";
			}
			


			//sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr = sqlstr + sqlstr_temp + sqlstr_order;
			break;
		}

		//cmd_inq.SetCommandText(sqlstr_count);
		//TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
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
