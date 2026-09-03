/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-09-11
Description: 炉成本查询
**************************************************/
//框架头文件
#include "stdafx.h"	 

BM2F_ENTERACE(mmsmlcbsum_inq)

int f_mmsmlcbsum_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_heatnr = "";
	CString v_cast_div_no_1 = "";
	CString v_assist_heatno = "";
	CString v_grade_type = "";
	CString v_grade1 = "";
	CString v_route = "";
	CString v_mat_type = "";
	CString v_mat_code = "";
	CString type_flag = "0";
	CString mat_type_flag = "0";
	CString v_mat_class_desc = "";
	CString zh = "";
	CString c_ss = "";
	
	CString v_from = "";//开始时刻
	CString v_to = "";//开始时刻
	CPageInfo pageInfo;



	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("HEATNR"))
		{
			v_heatnr = bcls_rec->Tables[0].Rows[0]["HEATNR"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CAST_DIV_NO_1"))
		{
			v_cast_div_no_1 = bcls_rec->Tables[0].Rows[0]["CAST_DIV_NO_1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("ASSIST_HEATNO"))
		{
			v_assist_heatno = bcls_rec->Tables[0].Rows[0]["ASSIST_HEATNO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE1"))
		{
			v_grade_type = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("F_ROUTE1"))
		{
			v_route = bcls_rec->Tables[0].Rows[0]["F_ROUTE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_TYPE_DESC"))
		{
			v_mat_type = bcls_rec->Tables[0].Rows[0]["MAT_TYPE_DESC"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE_DR"))
		{
			v_mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE_DR"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("GRADE_ID"))
		{
			v_grade1= bcls_rec->Tables[0].Rows[0]["GRADE_ID"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_from = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().SubstringNE(0, 8);
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_to = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		}

		if (bcls_rec->Tables[0].Columns.Contains("TYPE_FLAG"))
		{
			type_flag = bcls_rec->Tables[0].Rows[0]["TYPE_FLAG"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_TYPE_DESC1"))
		{
			mat_type_flag = bcls_rec->Tables[0].Rows[0]["MAT_TYPE_DESC1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CLASS_DESC"))
		{
			v_mat_class_desc = bcls_rec->Tables[0].Rows[0]["MAT_CLASS_DESC"].ToString().Trim();
		}

		if (bcls_rec->Tables[0].Columns.Contains("ZH"))
		{
			zh = bcls_rec->Tables[0].Rows[0]["ZH"].ToString();
		}
		if (bcls_rec->Tables[0].Columns.Contains("C_SS"))
		{
			c_ss = bcls_rec->Tables[0].Rows[0]["C_SS"].ToString().Trim();
		}
		Log::Trace("", __FUNCTION__, "2026[{0}]  ", c_ss);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			
			if (c_ss == "C")
			{
				if (zh == "ZH")
				{
					if (type_flag == "1")
					{
						sqlstr = "SELECT "
							" AOD_BOF_E_DTIME,"
							" HEATNR,"
							" GRADE_ID,"
							" GRADE_TYPE1,"
							" F_ROUTE1,"
							" QUALIFIED_WT_MAX AS QUALIFIED_WT,"
							" TOTAL_COST AS COST,"
							" TOTAL_COST_PERT AS COST_PERT,"
							" SG_GRADE_1,"
							" WT_PERT,"
							" CAST_DIV_NO_1,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(IRON_MAT_COST / QUALIFIED_WT_MAX, 2) END AS IRON_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(ALLOY_COST / QUALIFIED_WT_MAX, 2) END AS ALLOY_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(AUXI_MAT_COST / QUALIFIED_WT_MAX, 2) END AS AUXI_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(STEP_FEE_COST / QUALIFIED_WT_MAX, 2) END AS STEP_FEE_COST_PER"
							" FROM(SELECT"
							" h.*,"
							" x.SG_GRADE_1,"
							" t31.CAST_DIV_NO AS CAST_DIV_NO_1"
							" FROM(SELECT"
							" t.HEATNR,"
							" t.QUALIFIED_WT,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN t.COST ELSE 0 END) AS IRON_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '合金' THEN t.COST ELSE 0 END) AS ALLOY_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '辅料' THEN t.COST ELSE 0 END) AS AUXI_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '步骤费' THEN t.COST ELSE 0 END) AS STEP_FEE_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) ELSE 0 END) AS  WT_PERT,"
							" MAX(t.AOD_BOF_E_DTIME) AS AOD_BOF_E_DTIME,"
							" MAX(t.GRADE_ID) AS GRADE_ID,"
							" MAX(t.GRADE_TYPE1) AS GRADE_TYPE1,"
							" MAX(t.F_ROUTE1) AS F_ROUTE1,"
							" MAX(t.QUALIFIED_WT) AS QUALIFIED_WT_MAX,"
							" SUM(t.COST) AS TOTAL_COST,"
							" SUM(t.COST_PERT) AS TOTAL_COST_PERT,"
							" SUM(t.WEIGHT) AS TOTAL_WEIGHT"
							" FROM (select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY from (select * from tqmtscb03a_mx x where (x.GRADE_ID like '2%' or x.GRADE_ID like '3%' or x.GRADE_ID like '5%')) t left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr) t"
							" GROUP BY HEATNR, t.QUALIFIED_WT) h"
							" LEFT JOIN TQMTS0X x ON x.ST_NO = h.GRADE_ID"
							" LEFT JOIN tmmsm31 t31 ON t31.heat_no = h.HEATNR)"
							" where 1 = 1";
					}
					else
					{
						sqlstr = "SELECT "
							" AOD_BOF_E_DTIME,"
							" HEATNR,"
							" GRADE_ID,"
							" GRADE_TYPE1,"
							" F_ROUTE1,"
							" QUALIFIED_WT_MAX AS QUALIFIED_WT,"
							" TOTAL_COST AS COST,"
							" TOTAL_COST_PERT AS COST_PERT,"
							" SG_GRADE_1,"
							" WT_PERT,"
							" CAST_DIV_NO_1,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(IRON_MAT_COST / QUALIFIED_WT_MAX, 2) END AS IRON_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(ALLOY_COST / QUALIFIED_WT_MAX, 2) END AS ALLOY_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(AUXI_MAT_COST / QUALIFIED_WT_MAX, 2) END AS AUXI_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(STEP_FEE_COST / QUALIFIED_WT_MAX, 2) END AS STEP_FEE_COST_PER"
							" FROM(SELECT"
							" h.*,"
							" x.SG_GRADE_1,"
							" t31.CAST_DIV_NO AS CAST_DIV_NO_1"
							" FROM(SELECT"
							" t.HEATNR,"
							" t.QUALIFIED_WT,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN t.COST ELSE 0 END) AS IRON_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '合金' THEN t.COST ELSE 0 END) AS ALLOY_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '辅料' THEN t.COST ELSE 0 END) AS AUXI_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '步骤费' THEN t.COST ELSE 0 END) AS STEP_FEE_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) ELSE 0 END) AS  WT_PERT,"
							" MAX(t.AOD_BOF_E_DTIME) AS AOD_BOF_E_DTIME,"
							" MAX(t.GRADE_ID) AS GRADE_ID,"
							" MAX(t.GRADE_TYPE1) AS GRADE_TYPE1,"
							" MAX(t.F_ROUTE1) AS F_ROUTE1,"
							" MAX(t.QUALIFIED_WT) AS QUALIFIED_WT_MAX,"
							" SUM(t.COST) AS TOTAL_COST,"
							" SUM(t.COST_PERT) AS TOTAL_COST_PERT,"
							" SUM(t.WEIGHT) AS TOTAL_WEIGHT"
							" FROM (select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY from (select * from tqmtscb01a_mx x where (x.GRADE_ID like '2%' or x.GRADE_ID like '3%' or x.GRADE_ID like '5%')) t left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr) t"
							" GROUP BY HEATNR, t.QUALIFIED_WT) h"
							" LEFT JOIN TQMTS0X x ON x.ST_NO = h.GRADE_ID"
							" LEFT JOIN tmmsm31 t31 ON t31.heat_no = h.HEATNR)"
							" where 1 = 1";
					}
				}
				else
				{


					if (type_flag == "1")
					{
						sqlstr = "SELECT "
							" AOD_BOF_E_DTIME,"
							" HEATNR,"
							" GRADE_ID,"
							" GRADE_TYPE1,"
							" F_ROUTE1,"
							" QUALIFIED_WT_MAX AS QUALIFIED_WT,"
							" TOTAL_COST AS COST,"
							" TOTAL_COST_PERT AS COST_PERT,"
							" SG_GRADE_1,"
							" WT_PERT,"
							" CAST_DIV_NO_1,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(IRON_MAT_COST / QUALIFIED_WT_MAX, 2) END AS IRON_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(ALLOY_COST / QUALIFIED_WT_MAX, 2) END AS ALLOY_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(AUXI_MAT_COST / QUALIFIED_WT_MAX, 2) END AS AUXI_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(STEP_FEE_COST / QUALIFIED_WT_MAX, 2) END AS STEP_FEE_COST_PER"
							" FROM(SELECT"
							" h.*,"
							" x.SG_GRADE_1,"
							" t31.CAST_DIV_NO AS CAST_DIV_NO_1"
							" FROM(SELECT"
							" t.HEATNR,"
							" t.QUALIFIED_WT,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN t.COST ELSE 0 END) AS IRON_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '合金' THEN t.COST ELSE 0 END) AS ALLOY_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '辅料' THEN t.COST ELSE 0 END) AS AUXI_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '步骤费' THEN t.COST ELSE 0 END) AS STEP_FEE_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) ELSE 0 END) AS  WT_PERT,"
							" MAX(t.AOD_BOF_E_DTIME) AS AOD_BOF_E_DTIME,"
							" MAX(t.GRADE_ID) AS GRADE_ID,"
							" MAX(t.GRADE_TYPE1) AS GRADE_TYPE1,"
							" MAX(t.F_ROUTE1) AS F_ROUTE1,"
							" MAX(t.QUALIFIED_WT) AS QUALIFIED_WT_MAX,"
							" SUM(t.COST) AS TOTAL_COST,"
							" SUM(t.COST_PERT) AS TOTAL_COST_PERT,"
							" SUM(t.WEIGHT) AS TOTAL_WEIGHT"
							" FROM (select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY from (select * from tqmtscb03_mx x where (x.GRADE_ID like '2%' or x.GRADE_ID like '3%' or x.GRADE_ID like '5%')) t left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr) t"
							" GROUP BY HEATNR, t.QUALIFIED_WT) h"
							" LEFT JOIN TQMTS0X x ON x.ST_NO = h.GRADE_ID"
							" LEFT JOIN tmmsm31 t31 ON t31.heat_no = h.HEATNR)"
							" where 1 = 1";
					}
					else
					{
						sqlstr = "SELECT "
							" AOD_BOF_E_DTIME,"
							" HEATNR,"
							" GRADE_ID,"
							" GRADE_TYPE1,"
							" F_ROUTE1,"
							" QUALIFIED_WT_MAX AS QUALIFIED_WT,"
							" TOTAL_COST AS COST,"
							" TOTAL_COST_PERT AS COST_PERT,"
							" SG_GRADE_1,"
							" WT_PERT,"
							" CAST_DIV_NO_1,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(IRON_MAT_COST / QUALIFIED_WT_MAX, 2) END AS IRON_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(ALLOY_COST / QUALIFIED_WT_MAX, 2) END AS ALLOY_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(AUXI_MAT_COST / QUALIFIED_WT_MAX, 2) END AS AUXI_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(STEP_FEE_COST / QUALIFIED_WT_MAX, 2) END AS STEP_FEE_COST_PER"
							" FROM(SELECT"
							" h.*,"
							" x.SG_GRADE_1,"
							" t31.CAST_DIV_NO AS CAST_DIV_NO_1"
							" FROM(SELECT"
							" t.HEATNR,"
							" t.QUALIFIED_WT,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN t.COST ELSE 0 END) AS IRON_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '合金' THEN t.COST ELSE 0 END) AS ALLOY_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '辅料' THEN t.COST ELSE 0 END) AS AUXI_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '步骤费' THEN t.COST ELSE 0 END) AS STEP_FEE_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) ELSE 0 END) AS  WT_PERT,"
							" MAX(t.AOD_BOF_E_DTIME) AS AOD_BOF_E_DTIME,"
							" MAX(t.GRADE_ID) AS GRADE_ID,"
							" MAX(t.GRADE_TYPE1) AS GRADE_TYPE1,"
							" MAX(t.F_ROUTE1) AS F_ROUTE1,"
							" MAX(t.QUALIFIED_WT) AS QUALIFIED_WT_MAX,"
							" SUM(t.COST) AS TOTAL_COST,"
							" SUM(t.COST_PERT) AS TOTAL_COST_PERT,"
							" SUM(t.WEIGHT) AS TOTAL_WEIGHT"
							" FROM (select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY from (select * from tqmtscb01_mx x where (x.GRADE_ID like '2%' or x.GRADE_ID like '3%' or x.GRADE_ID like '5%')) t left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr) t"
							" GROUP BY HEATNR, t.QUALIFIED_WT) h"
							" LEFT JOIN TQMTS0X x ON x.ST_NO = h.GRADE_ID"
							" LEFT JOIN tmmsm31 t31 ON t31.heat_no = h.HEATNR)"
							" where 1 = 1";
					}
				}
			}
			else if (c_ss == "SS")
			{
				if (zh == "ZH")
				{
					if (type_flag == "1")
					{
						sqlstr = "SELECT "
							" AOD_BOF_E_DTIME,"
							" HEATNR,"
							" GRADE_ID,"
							" GRADE_TYPE1,"
							" F_ROUTE1,"
							" QUALIFIED_WT_MAX AS QUALIFIED_WT,"
							" TOTAL_COST AS COST,"
							" TOTAL_COST_PERT AS COST_PERT,"
							" SG_GRADE_1,"
							" WT_PERT,"
							" CAST_DIV_NO_1,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(IRON_MAT_COST / QUALIFIED_WT_MAX, 2) END AS IRON_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(ALLOY_COST / QUALIFIED_WT_MAX, 2) END AS ALLOY_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(AUXI_MAT_COST / QUALIFIED_WT_MAX, 2) END AS AUXI_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(STEP_FEE_COST / QUALIFIED_WT_MAX, 2) END AS STEP_FEE_COST_PER"
							" FROM(SELECT"
							" h.*,"
							" x.SG_GRADE_1,"
							" t31.CAST_DIV_NO AS CAST_DIV_NO_1"
							" FROM(SELECT"
							" t.HEATNR,"
							" t.QUALIFIED_WT,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN t.COST ELSE 0 END) AS IRON_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '合金' THEN t.COST ELSE 0 END) AS ALLOY_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '辅料' THEN t.COST ELSE 0 END) AS AUXI_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '步骤费' THEN t.COST ELSE 0 END) AS STEP_FEE_COST,"
							" SUM(CASE WHEN (t.MAT_TYPE_DESC = '钢铁料' or t.MAT_TYPE_DESC = '合金') THEN DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) ELSE 0 END) AS  WT_PERT,"
							" MAX(t.AOD_BOF_E_DTIME) AS AOD_BOF_E_DTIME,"
							" MAX(t.GRADE_ID) AS GRADE_ID,"
							" MAX(t.GRADE_TYPE1) AS GRADE_TYPE1,"
							" MAX(t.F_ROUTE1) AS F_ROUTE1,"
							" MAX(t.QUALIFIED_WT) AS QUALIFIED_WT_MAX,"
							" SUM(t.COST) AS TOTAL_COST,"
							" SUM(t.COST_PERT) AS TOTAL_COST_PERT,"
							" SUM(t.WEIGHT) AS TOTAL_WEIGHT"
							" FROM (select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY from (select * from tqmtscb03a_mx x where (x.GRADE_ID like '1%' or x.GRADE_ID like '4%')) t left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr) t"
							" GROUP BY HEATNR, t.QUALIFIED_WT) h"
							" LEFT JOIN TQMTS0X x ON x.ST_NO = h.GRADE_ID"
							" LEFT JOIN tmmsm31 t31 ON t31.heat_no = h.HEATNR)"
							" where 1 = 1";
					}
					else
					{
						sqlstr = "SELECT "
							" AOD_BOF_E_DTIME,"
							" HEATNR,"
							" GRADE_ID,"
							" GRADE_TYPE1,"
							" F_ROUTE1,"
							" QUALIFIED_WT_MAX AS QUALIFIED_WT,"
							" TOTAL_COST AS COST,"
							" TOTAL_COST_PERT AS COST_PERT,"
							" SG_GRADE_1,"
							" WT_PERT,"
							" CAST_DIV_NO_1,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(IRON_MAT_COST / QUALIFIED_WT_MAX, 2) END AS IRON_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(ALLOY_COST / QUALIFIED_WT_MAX, 2) END AS ALLOY_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(AUXI_MAT_COST / QUALIFIED_WT_MAX, 2) END AS AUXI_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(STEP_FEE_COST / QUALIFIED_WT_MAX, 2) END AS STEP_FEE_COST_PER"
							" FROM(SELECT"
							" h.*,"
							" x.SG_GRADE_1,"
							" t31.CAST_DIV_NO AS CAST_DIV_NO_1"
							" FROM(SELECT"
							" t.HEATNR,"
							" t.QUALIFIED_WT,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN t.COST ELSE 0 END) AS IRON_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '合金' THEN t.COST ELSE 0 END) AS ALLOY_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '辅料' THEN t.COST ELSE 0 END) AS AUXI_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '步骤费' THEN t.COST ELSE 0 END) AS STEP_FEE_COST,"
							" SUM(CASE WHEN (t.MAT_TYPE_DESC = '钢铁料' or t.MAT_TYPE_DESC = '合金') THEN DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) ELSE 0 END) AS  WT_PERT,"
							" MAX(t.AOD_BOF_E_DTIME) AS AOD_BOF_E_DTIME,"
							" MAX(t.GRADE_ID) AS GRADE_ID,"
							" MAX(t.GRADE_TYPE1) AS GRADE_TYPE1,"
							" MAX(t.F_ROUTE1) AS F_ROUTE1,"
							" MAX(t.QUALIFIED_WT) AS QUALIFIED_WT_MAX,"
							" SUM(t.COST) AS TOTAL_COST,"
							" SUM(t.COST_PERT) AS TOTAL_COST_PERT,"
							" SUM(t.WEIGHT) AS TOTAL_WEIGHT"
							" FROM (select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY from (select * from tqmtscb01a_mx x where (x.GRADE_ID like '1%' or x.GRADE_ID like '4%')) t left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr) t"
							" GROUP BY HEATNR, t.QUALIFIED_WT) h"
							" LEFT JOIN TQMTS0X x ON x.ST_NO = h.GRADE_ID"
							" LEFT JOIN tmmsm31 t31 ON t31.heat_no = h.HEATNR)"
							" where 1 = 1";
					}
				}
				else
				{


					if (type_flag == "1")
					{
						sqlstr = "SELECT "
							" AOD_BOF_E_DTIME,"
							" HEATNR,"
							" GRADE_ID,"
							" GRADE_TYPE1,"
							" F_ROUTE1,"
							" QUALIFIED_WT_MAX AS QUALIFIED_WT,"
							" TOTAL_COST AS COST,"
							" TOTAL_COST_PERT AS COST_PERT,"
							" SG_GRADE_1,"
							" WT_PERT,"
							" CAST_DIV_NO_1,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(IRON_MAT_COST / QUALIFIED_WT_MAX, 2) END AS IRON_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(ALLOY_COST / QUALIFIED_WT_MAX, 2) END AS ALLOY_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(AUXI_MAT_COST / QUALIFIED_WT_MAX, 2) END AS AUXI_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(STEP_FEE_COST / QUALIFIED_WT_MAX, 2) END AS STEP_FEE_COST_PER"
							" FROM(SELECT"
							" h.*,"
							" x.SG_GRADE_1,"
							" t31.CAST_DIV_NO AS CAST_DIV_NO_1"
							" FROM(SELECT"
							" t.HEATNR,"
							" t.QUALIFIED_WT,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN t.COST ELSE 0 END) AS IRON_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '合金' THEN t.COST ELSE 0 END) AS ALLOY_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '辅料' THEN t.COST ELSE 0 END) AS AUXI_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '步骤费' THEN t.COST ELSE 0 END) AS STEP_FEE_COST,"
							" SUM(CASE WHEN (t.MAT_TYPE_DESC = '钢铁料' or t.MAT_TYPE_DESC = '合金') THEN DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) ELSE 0 END) AS  WT_PERT,"
							" MAX(t.AOD_BOF_E_DTIME) AS AOD_BOF_E_DTIME,"
							" MAX(t.GRADE_ID) AS GRADE_ID,"
							" MAX(t.GRADE_TYPE1) AS GRADE_TYPE1,"
							" MAX(t.F_ROUTE1) AS F_ROUTE1,"
							" MAX(t.QUALIFIED_WT) AS QUALIFIED_WT_MAX,"
							" SUM(t.COST) AS TOTAL_COST,"
							" SUM(t.COST_PERT) AS TOTAL_COST_PERT,"
							" SUM(t.WEIGHT) AS TOTAL_WEIGHT"
							" FROM (select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY from (select * from tqmtscb03_mx x where (x.GRADE_ID like '1%' or x.GRADE_ID like '4%')) t left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr) t"
							" GROUP BY HEATNR, t.QUALIFIED_WT) h"
							" LEFT JOIN TQMTS0X x ON x.ST_NO = h.GRADE_ID"
							" LEFT JOIN tmmsm31 t31 ON t31.heat_no = h.HEATNR)"
							" where 1 = 1";
					}
					else
					{
						sqlstr = "SELECT "
							" AOD_BOF_E_DTIME,"
							" HEATNR,"
							" GRADE_ID,"
							" GRADE_TYPE1,"
							" F_ROUTE1,"
							" QUALIFIED_WT_MAX AS QUALIFIED_WT,"
							" TOTAL_COST AS COST,"
							" TOTAL_COST_PERT AS COST_PERT,"
							" SG_GRADE_1,"
							" WT_PERT,"
							" CAST_DIV_NO_1,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(IRON_MAT_COST / QUALIFIED_WT_MAX, 2) END AS IRON_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(ALLOY_COST / QUALIFIED_WT_MAX, 2) END AS ALLOY_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(AUXI_MAT_COST / QUALIFIED_WT_MAX, 2) END AS AUXI_MAT_COST_PER,"
							" CASE WHEN QUALIFIED_WT_MAX = 0 THEN NULL"
							" ELSE ROUND(STEP_FEE_COST / QUALIFIED_WT_MAX, 2) END AS STEP_FEE_COST_PER"
							" FROM(SELECT"
							" h.*,"
							" x.SG_GRADE_1,"
							" t31.CAST_DIV_NO AS CAST_DIV_NO_1"
							" FROM(SELECT"
							" t.HEATNR,"
							" t.QUALIFIED_WT,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '钢铁料' THEN t.COST ELSE 0 END) AS IRON_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '合金' THEN t.COST ELSE 0 END) AS ALLOY_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '辅料' THEN t.COST ELSE 0 END) AS AUXI_MAT_COST,"
							" SUM(CASE WHEN t.MAT_TYPE_DESC = '步骤费' THEN t.COST ELSE 0 END) AS STEP_FEE_COST,"
							" SUM(CASE WHEN (t.MAT_TYPE_DESC = '钢铁料' or t.MAT_TYPE_DESC = '合金') THEN DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) ELSE 0 END) AS  WT_PERT,"
							" MAX(t.AOD_BOF_E_DTIME) AS AOD_BOF_E_DTIME,"
							" MAX(t.GRADE_ID) AS GRADE_ID,"
							" MAX(t.GRADE_TYPE1) AS GRADE_TYPE1,"
							" MAX(t.F_ROUTE1) AS F_ROUTE1,"
							" MAX(t.QUALIFIED_WT) AS QUALIFIED_WT_MAX,"
							" SUM(t.COST) AS TOTAL_COST,"
							" SUM(t.COST_PERT) AS TOTAL_COST_PERT,"
							" SUM(t.WEIGHT) AS TOTAL_WEIGHT"
							" FROM (select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY from (select * from tqmtscb01_mx x where (x.GRADE_ID like '1%' or x.GRADE_ID like '4%')) t left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr) t"
							" GROUP BY HEATNR, t.QUALIFIED_WT) h"
							" LEFT JOIN TQMTS0X x ON x.ST_NO = h.GRADE_ID"
							" LEFT JOIN tmmsm31 t31 ON t31.heat_no = h.HEATNR)"
							" where 1 = 1";
					}
				}
			}
			else
			{
				strcpy(s.msg, "请选择碳钢、不锈钢其中之一!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (v_heatnr != ""){
				sqlstr += " AND HEATNR like @v_heatnr||'%'";
			}
			if (v_cast_div_no_1 != ""){
				sqlstr += " AND CAST_DIV_NO_1 = @v_cast_div_no_1";
			}
			if (v_grade_type != ""){
				sqlstr += " AND GRADE_TYPE1 = @v_grade_type";
			}
			if (v_route != ""){
				sqlstr += " AND F_ROUTE1 = @v_route";
			}
			if (v_grade1 != ""){
				sqlstr += " AND GRADE_ID = @v_grade1";
			}
			if (v_from.Trim() != "")
			{
				sqlstr += " AND AOD_BOF_E_DTIME>= @v_from";
			}
			if (v_to.Trim() != "")
			{
				sqlstr += " AND AOD_BOF_E_DTIME<= @v_to";
			}
			
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", v_from);
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", v_to);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_heatnr", v_heatnr);
			cmd_inq.Parameters.Set("v_cast_div_no_1", v_cast_div_no_1);
			cmd_inq.Parameters.Set("v_grade_type", v_grade_type);
			cmd_inq.Parameters.Set("v_route", v_route);
			cmd_inq.Parameters.Set("v_grade1", v_grade1);
			cmd_inq.Parameters.Set("v_from", v_from);
			cmd_inq.Parameters.Set("v_to", v_to);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}


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
