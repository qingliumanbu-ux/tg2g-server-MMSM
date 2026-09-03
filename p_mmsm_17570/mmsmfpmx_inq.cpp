/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   孟凡杰
Version:    1.0
Date:     2024-04-25 9:13:56
Description: 成品废品明细查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsmfpmx_inq)

int f_mmsmfpmx_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_heat_no = "";
	CString v_st_no = "";
	CString v_grid_tab = "";	//Grid区分
	CString v_c_div = "";//碳锈区分  1  不锈钢  2碳钢
	CString v_start_time = "";
	CString v_end_time = "";
	CString v_app_code = "";
	CString v_table_name = "";


	CModel tmmsmfp("TMMSMFP");

	CDbCommand cmd_inq(conn);

	try
	{
		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();
		Log::Info("", __FUNCTION__, "v_start_time =[{0}]", v_start_time);

		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
		Log::Info("", __FUNCTION__, "v_end_time =[{0}]", v_end_time);

		if (bcls_rec->Tables[0].Columns.Contains("C_DIV"))
			v_c_div = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
			v_table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "c_div =[{0}]", v_c_div);
		if (bcls_rec->Tables[0].Columns.Contains("APP_CODE"))
			v_app_code = bcls_rec->Tables[0].Rows[0]["APP_CODE"].ToString().Trim();


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			if (v_table_name == "TMMSMFP")//废品明细
			{


				/*if (bcls_rec->Tables[0].Columns.Contains("GRID_TAB"))
					v_grid_tab = bcls_rec->Tables[0].Rows[0]["GRID_TAB"].ToString().Trim();*/
				if (v_c_div.Trim() == "0"){

					sqlstr = " SELECT T.*,T2.CODE_DESC_1_CONTENT AS STEEL_TYPE FROM (\
						SELECT T1.RESUME_SEQ_NO,T1.PROD_TIME, T1.PROD_SHIFT_GROUP, T1.MAT_ACT_THICK, T1.MAT_ACT_WIDTH, T1.MATERIAL_DESC_BACK,\
						T1.HEAT_NO1, T1.MAT_LEN_1, T1.MATERIAL_DESC_FORE, T1.HEAT_NO2, T1.MAT_LEN_2,T1.CUT_SCRAP_WT, T1.CUT_SCRAP_REASON, \
						T1.ST_NO, T1.APP_CODE\
						FROM TMMSMFP T1\
						WHERE 1 = 1 AND C_DIV = '0'";
					
					if (v_app_code != "")
					{
						sqlstr += " AND  T1.APP_CODE	=  '" + v_app_code + "'";
					}
					if (v_heat_no != "")
					{
						sqlstr += " AND  T1.HEAT_NO	=  '" + v_heat_no + "'";
					}
					if (v_st_no != "")
					{
						sqlstr += " AND  T1.ST_NO	= '" + v_st_no + "'";
					}
					if (v_start_time.Trim() != "")
					{
						sqlstr += " AND PROD_TIME		>= '" + v_start_time + "'";
					}
					if (v_end_time.Trim() != "")
					{
						sqlstr += " AND PROD_TIME		<= '" + v_end_time + "'";
					}
					
					//sqlstr += "UNION ALL\
					//	SELECT ' ' AS RESUME_SEQ_NO,T.PROD_TIME, T.PROD_SHIFT_GROUP, T.MAT_ACT_THICK, T.MAT_ACT_WIDTH, T.SG_GRADE_1 AS MATERIAL_DESC_BACK,\
					//	T.MAT_NO AS HEAT_NO1, T.MAT_ACT_LEN AS MAT_LEN_1, ' ' AS MATERIAL_DESC_FORE, ' ' AS HEAT_NO2, 0 AS MAT_LEN_2,\
					//	T.L3_CALTHEROY_WT AS CUT_SCRAP_WT, T.DELETE_REASON AS CUT_SCRAP_REASON,\
					//	T.ST_NO, '无系统-删除' AS APP_CODE\
					//	FROM(SELECT * FROM HMMSM96 WHERE 1 = 1\
					//	)T\
					//	WHERE C_DIV = '2' AND\
					//	((EVENT_ID = 'MM04' AND FUNC_ID = 'mmsm01a1f5_del')\
					//	OR(EVENT_ID = 'MM05' AND FUNC_ID = 'mmsm01a1f6_pro_new'))";

					////查询碳钢，即 C_DIV = 0
					////sqlstr += " AND  T1.C_DIV =  '" + v_c_div + "'";
					///*if (v_app_code != "")
					//{
					//	sqlstr += " AND  APP_CODE	=  '" + v_app_code + "'";
					//}*/
					//if (v_heat_no != "")
					//{
					//	sqlstr += " AND  HEAT_NO	=  '" + v_heat_no + "'";
					//}
					//if (v_st_no != "")
					//{
					//	sqlstr += " AND  ST_NO	= '" + v_st_no + "'";
					//}
					//if (v_start_time.Trim() != "")
					//{
					//	sqlstr += " AND PROD_TIME		>= '" + v_start_time + "'";
					//}
					//if (v_end_time.Trim() != "")
					//{
					//	sqlstr += " AND PROD_TIME		<= '" + v_end_time + "'";
					//}
					sqlstr += ") T\
						LEFT JOIN(SELECT * FROM TWMSMZD02 WHERE CODE_CLASS = 'STEEL_TYPE') T2 ON T.ST_NO = T2.CODE where 1=1 "; 

					Log::Info("", __FUNCTION__, "c_div =[{0}]", v_c_div);

					
				}
				else if (v_c_div.Trim() == "1"){
					sqlstr = " SELECT T.*,T2.CODE_DESC_1_CONTENT AS STEEL_TYPE FROM (\
						SELECT T1.RESUME_SEQ_NO,T1.PROD_TIME, T1.PROD_SHIFT_GROUP, T1.MATERIAL_DESC, T1.HEAT_NO, T1.MAT_ACT_THICK, T1.MAT_ACT_WIDTH,\
						T1.MAT_LEN, T1.CUT_SCRAP_WT, T1.SCRAP_REASON,\
						T1.ST_NO, T1.APP_CODE, T1.TOTAL_CYCLE, T1.GRADE_ID, 0 AS is_duplicate\
						FROM TMMSMFP T1\
						WHERE 1 = 1 AND T1.C_DIV<>'0'";
					if (v_app_code != "")
					{
						sqlstr += " AND  T1.APP_CODE	=  '" + v_app_code + "'";
					}
					if (v_heat_no != "")
					{
						sqlstr += " AND  T1.HEAT_NO	=  '" + v_heat_no + "'";
					}
					if (v_st_no != "")
					{
						sqlstr += " AND  T1.ST_NO	= '" + v_st_no + "'";
					}
					if (v_start_time.Trim() != "")
					{
						sqlstr += " AND PROD_TIME		>= '" + v_start_time + "'";
					}
					if (v_end_time.Trim() != "")
					{
						sqlstr += " AND PROD_TIME		<= '" + v_end_time + "'";
					}
					//sqlstr += "UNION ALL\
					//	SELECT ' ' AS RESUME_SEQ_NO,T.PROD_TIME, T.PROD_SHIFT_GROUP, ' ' AS MATERIAL_DESC, T.HEAT_NO, T.MAT_ACT_THICK, T.MAT_ACT_WIDTH,\
					//	T.MAT_LEN, T.L3_CALTHEROY_WT AS CUT_SCRAP_WT, T.DELETE_REASON AS SCRAP_REASON,\
					//	T.ST_NO, '无系统-删除' AS APP_CODE, ' ' AS TOTAL_CYCLE, ' ' AS GRADE_ID,\
					//	CASE WHEN COUNT(t.mat_no) OVER(PARTITION BY t.mat_no) > 1 THEN 1 ELSE 0 END AS is_duplicate\
					//	FROM(SELECT * FROM HMMSM96 WHERE 1 = 1\
					//	)T\
					//	WHERE C_DIV = '1' AND\
					//	((EVENT_ID = 'MM04' AND FUNC_ID = 'mmsm01a1f5_del')\
					//	OR(EVENT_ID = 'MM05' AND FUNC_ID = 'mmsm01a1f6_pro_new'))";
					////sqlstr += " AND  T1.C_DIV <> '0'";

					///*if (v_app_code != "")
					//{
					//	sqlstr += " AND  APP_CODE	=  '" + v_app_code + "'";
					//}*/
					//if (v_heat_no != "")
					//{
					//	sqlstr += " AND  HEAT_NO	=  '" + v_heat_no + "'";
					//}
					//if (v_st_no != "")
					//{
					//	sqlstr += " AND  ST_NO	= '" + v_st_no + "'";
					//}
					//if (v_start_time.Trim() != "")
					//{
					//	sqlstr += " AND PROD_TIME		>= '" + v_start_time + "'";
					//}
					//if (v_end_time.Trim() != "")
					//{
					//	sqlstr += " AND PROD_TIME		<= '" + v_end_time + "'";
					//}
					sqlstr += ") T \
						LEFT JOIN(SELECT * FROM TWMSMZD02 WHERE CODE_CLASS = 'STEEL_TYPE') T2 ON T.ST_NO = T2.CODE where 1=1 ";

					Log::Info("", __FUNCTION__, "c_div =[{0}]", v_c_div);
					
				}
			}
			else if (v_table_name == "TMMSMPF")//2026.03.12 判废明细
			{
				if (v_c_div.Trim() == "2"){

					sqlstr = " SELECT * FROM (SELECT T.ST_NO,T.PROD_TIME,T.PROD_SHIFT_GROUP,T.MAT_ACT_WIDTH,T.SG_GRADE_1 AS MATERIAL_DESC_BACK,\
						T.MAT_NO AS HEAT_NO1, T.MAT_ACT_LEN AS MAT_LEN_1, ' ' AS MATERIAL_DESC_FORE, ' ' AS HEAT_NO2, 0 AS MAT_LEN_2,\
						' ' AS SCRAP_REASON, \
						CASE WHEN EVENT_ID = 'QM05' THEN T.L3_CALTHEROY_WT\
						WHEN EVENT_ID = 'QM06' THEN - T.L3_CALTHEROY_WT\
						ELSE T.L3_CALTHEROY_WT\
						END AS CUT_SCRAP_WT,\
						' ' AS TOTAL_CYCLE, \
						' ' AS APP_CODE, \
						T.C_DIV \
						FROM(SELECT * FROM HMMSM96 WHERE 1 = 1\
						)T\
						WHERE \
						((EVENT_ID = 'QM05' AND EVENT_NAME = '材料判废')\
						OR(EVENT_ID = 'QM06' AND EVENT_NAME = '材料判废取消'))\
						UNION ALL\
						SELECT T1.ST_NO, T1.PROD_TIME, T1.PROD_SHIFT_GROUP, T1.MAT_ACT_WIDTH, T1.MATERIAL_DESC_BACK,\
						T1.HEAT_NO1, T1.MAT_LEN_1, T1.MATERIAL_DESC_FORE, T1.HEAT_NO2, T1.MAT_LEN_2,\
						' ' AS SCRAP_REASON,\
						T1.CUT_SCRAP_WT,\
						T1.TOTAL_CYCLE,\
						T1.APP_CODE, \
						T1.C_DIV \
					    FROM TMMSMPF T1) WHERE 1 = 1 \
						";

					Log::Info("", __FUNCTION__, "c_div =[{0}]", v_c_div);

					//查询碳钢，即 C_DIV = 0
					sqlstr += " AND  C_DIV =  '" + v_c_div + "'";
					/*if (v_app_code != "")
					{
						sqlstr += " AND  T1.APP_CODE	=  '" + v_app_code + "'";
					}*/
					if (v_heat_no != "")
					{
						sqlstr += " AND  HEAT_NO1	=  '" + v_heat_no + "'";
					}
					/*if (v_st_no != "")
					{
						sqlstr += " AND  T1.ST_NO	= '" + v_st_no + "'";
					}*/
					if (v_start_time.Trim() != "")
					{
						sqlstr += " AND PROD_TIME		>= '" + v_start_time + "'";
					}
					if (v_end_time.Trim() != "")
					{
						sqlstr += " AND PROD_TIME		<= '" + v_end_time + "'";
					}
				}
				else if (v_c_div.Trim() == "1"){
					sqlstr = " SELECT T1.*,T2.CODE_DESC_1_CONTENT AS STEEL_TYPE FROM (\
						SELECT T.ST_NO, T.PROD_TIME, T.PROD_SHIFT_GROUP, ' ' AS MATERIAL_DESC, T.HEAT_NO, T.MAT_ACT_WIDTH,\
						T.MAT_LEN, CASE WHEN EVENT_ID = 'QM05' THEN T.L3_CALTHEROY_WT\
						WHEN EVENT_ID = 'QM06' THEN - T.L3_CALTHEROY_WT\
						ELSE T.L3_CALTHEROY_WT\
						END AS CUT_SCRAP_WT,\
						' ' AS SCRAP_REASON, \
						' ' AS TOTAL_CYCLE, \
						' ' AS APP_CODE, \
						T.C_DIV \
						FROM(SELECT * FROM HMMSM96 WHERE 1 = 1\
						)T\
						WHERE \
						((EVENT_ID = 'QM05' AND EVENT_NAME = '材料判废')\
						OR(EVENT_ID = 'QM06' AND EVENT_NAME = '材料判废取消'))\
						UNION ALL\
						SELECT T.ST_NO, T.PROD_TIME, T.PROD_SHIFT_GROUP, T.MATERIAL_DESC, T.HEAT_NO, T.MAT_ACT_WIDTH,\
						T.MAT_LEN, T.CUT_SCRAP_WT, T.SCRAP_REASON,\
						T.TOTAL_CYCLE, \
						T.APP_CODE,\
						T.C_DIV \
						FROM TMMSMPF T WHERE 1 = 1\
						)T1 LEFT JOIN(SELECT * FROM TWMSMZD02 WHERE CODE_CLASS = 'STEEL_TYPE') T2 ON T1.ST_NO = T2.CODE\
						WHERE 1 = 1 ";

					Log::Info("", __FUNCTION__, "c_div =[{0}]", v_c_div);
					sqlstr += " AND  C_DIV = '" + v_c_div + "'";

					/*if (v_app_code != "")
					{
						sqlstr += " AND  T1.APP_CODE	=  '" + v_app_code + "'";
					}*/
					if (v_heat_no != "")
					{
						sqlstr += " AND  HEAT_NO	=  '" + v_heat_no + "'";
					}
					if (v_st_no != "")
					{
						sqlstr += " AND  ST_NO	= '" + v_st_no + "'";
					}
					if (v_start_time.Trim() != "")
					{
						sqlstr += " AND PROD_TIME		>= '" + v_start_time + "'";
					}
					if (v_end_time.Trim() != "")
					{
						sqlstr += " AND PROD_TIME		<= '" + v_end_time + "'";
					}
				}
			}

			sqlstr += " ORDER BY  PROD_TIME";
			break;
		}
		cmd_inq.Parameters.Set("v_c_div", v_c_div);
		cmd_inq.Parameters.Set("v_app_code", v_app_code);
		cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
		cmd_inq.Parameters.Set("v_st_no", v_st_no);
		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("v_end_time", v_end_time);
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
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
