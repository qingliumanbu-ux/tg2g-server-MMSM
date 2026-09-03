/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   孟凡杰
Version:    1.0
Date:     2024-04-25 9:13:56
Description: 成品废品不锈钢改切明细查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsmfpmx_inq4)

int f_mmsmfpmx_inq4(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "c_div =[{0}]", v_c_div);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			/*if (bcls_rec->Tables[0].Columns.Contains("GRID_TAB"))
			v_grid_tab = bcls_rec->Tables[0].Rows[0]["GRID_TAB"].ToString().Trim();*/

			sqlstr = "  SELECT * FROM (SELECT T.*,CASE WHEN SUBSTR(T.ST_NO,0,1) = '2' THEN '0' ELSE '1' END AS C_DIV, "
				" CASE SUBSTR(T.ST_NO,2,1) WHEN  'F' THEN '铬钢' WHEN 'M' THEN '铬钢' WHEN 'A' THEN '镍钢' WHEN 'D' THEN '镍钢' ELSE ' ' END AS GRADE_ID FROM ( "
				" SELECT T.PROD_TIME, T.PROD_SHIFT_GROUP, T.RECUT_GROUP, T.PROD_SHIFT_NO, T.HEAT_NO, T.MAT_NO, T.BATCH, T.ST_NO, R1.SG_GRADE_1, R2.CODE_DESC_1_CONTENT AS STEEL_TYPE,   "
				" T.CUT_BEFORE_WIDTH, T.CUT_AFTER_WIDTH, T.GRINDING_FLAG, T.CUTTING_TYPE, T.RECUT_DATE, T.CUT_BEFORE_LEN, T.CUT_AFTER_THICK, T.OTHER_CUT_LEN, "
				" T.CUT_BEFORE_WT, T.CUT_AFTER_WT, T.CUT_SCRAP_WT, T.REMARK, "
				" CASE WHEN NVL(SUM_35, 0) = 0 THEN 0 ELSE 1 END AS SF_GQZB, nvl(MAT_LEN_1, 0) MAT_LEN_1, nvl(MAT_LEN_2, 0) MAT_LEN_2, "
				" nvl(MAT_LEN_3, 0) MAT_LEN_3, nvl(MAT_LEN_4, 0) MAT_LEN_4, nvl(MAT_LEN_5, 0) MAT_LEN_5, nvl(MAT_LEN_6, 0) MAT_LEN_6, "
				" nvl(MAT_LEN_7, 0) MAT_LEN_7, nvl(MAT_LEN_8, 0) MAT_LEN_8, nvl(MAT_LEN_9, 0) MAT_LEN_9, NVL(MAT_NO_1, ' ') MAT_NO_1, "
				" NVL(MAT_NO_2, ' ') MAT_NO_2, NVL(MAT_NO_3, ' ') MAT_NO_3, NVL(MAT_NO_4, ' ') MAT_NO_4, NVL(MAT_NO_5, ' ') MAT_NO_5, "
				" NVL(MAT_NO_6, ' ') MAT_NO_6, NVL(MAT_NO_7, ' ') MAT_NO_7, NVL(MAT_NO_8, ' ') MAT_NO_8, NVL(MAT_NO_9, ' ') MAT_NO_9 FROM TMMSM39 T LEFT JOIN( "
				" SELECT MAX(MAT_LEN_1) MAT_LEN_1, MAX(MAT_LEN_2) MAT_LEN_2, MAX(MAT_LEN_3) MAT_LEN_3, MAX(MAT_LEN_4) MAT_LEN_4, MAX(MAT_LEN_5) MAT_LEN_5, MAX(MAT_LEN_6) MAT_LEN_6, MAX(MAT_LEN_7) MAT_LEN_7, MAX(MAT_LEN_8)MAT_LEN_8, MAX(MAT_LEN_9)MAT_LEN_9, IN_MAT_NO, COUNT(1) AS SUM_35, "
				" MAX(MAT_NO_1) MAT_NO_1, MAX(MAT_NO_2) MAT_NO_2, MAX(MAT_NO_3) MAT_NO_3, MAX(MAT_NO_4) MAT_NO_4, MAX(MAT_NO_5) MAT_NO_5, "
				" MAX(MAT_NO_6) MAT_NO_6, MAX(MAT_NO_7) MAT_NO_7, MAX(MAT_NO_8) MAT_NO_8, MAX(MAT_NO_9) MAT_NO_9 FROM( "
				" SELECT CASE WHEN PX_OR = '1' THEN MAT_LEN ELSE 0 END MAT_LEN_1, CASE WHEN PX_OR = '2' THEN MAT_LEN ELSE 0 END MAT_LEN_2, "
				" CASE WHEN PX_OR = '3' THEN MAT_LEN ELSE 0 END MAT_LEN_3, CASE WHEN PX_OR = '4' THEN MAT_LEN ELSE 0 END MAT_LEN_4, "
				" CASE WHEN PX_OR = '5' THEN MAT_LEN ELSE 0 END MAT_LEN_5, CASE WHEN PX_OR = '6' THEN MAT_LEN ELSE 0 END MAT_LEN_6, "
				" CASE WHEN PX_OR = '7' THEN MAT_LEN ELSE 0 END MAT_LEN_7, CASE WHEN PX_OR = '8' THEN MAT_LEN ELSE 0 END MAT_LEN_8, "
				" CASE WHEN PX_OR = '9' THEN MAT_LEN ELSE 0 END MAT_LEN_9, IN_MAT_NO, "
				" CASE WHEN PX_OR = '1' THEN MAT_NO ELSE ' ' END MAT_NO_1, CASE WHEN PX_OR = '2' THEN MAT_NO ELSE ' ' END MAT_NO_2, "
				" CASE WHEN PX_OR = '3' THEN MAT_NO ELSE ' ' END MAT_NO_3, CASE WHEN PX_OR = '4' THEN MAT_NO ELSE ' ' END MAT_NO_4, "
				" CASE WHEN PX_OR = '5' THEN MAT_NO ELSE ' ' END MAT_NO_5, CASE WHEN PX_OR = '6' THEN MAT_NO ELSE ' ' END MAT_NO_6, "
				" CASE WHEN PX_OR = '7' THEN MAT_NO ELSE ' ' END MAT_NO_7, CASE WHEN PX_OR = '8' THEN MAT_NO ELSE ' ' END MAT_NO_8, "
				" CASE WHEN PX_OR = '9' THEN MAT_NO ELSE ' ' END MAT_NO_9 "
				" FROM(select row_number()  over(partition by IN_MAT_NO ORDER BY MAT_NO) AS PX_OR, IN_MAT_NO, MAT_LEN, mat_no FROM TMMSM35))GROUP BY IN_MAT_NO) "
				" T2 ON T.MAT_NO = T2.IN_MAT_NO "
				" LEFT JOIN TQMTS0X R1 "
				" ON T.ST_NO = R1.ST_NO "
				" LEFT JOIN(SELECT * FROM TWMSMZD02 WHERE CODE_CLASS = 'STEEL_TYPE')R2 "
				" ON T.ST_NO = R2.CODE )T WHERE CUTTING_TYPE <> '8' AND CUTTING_TYPE <> '9' AND CUT_SCRAP_WT <> '0' "
				" )WHERE 1 = 1 AND C_DIV = '1'";

			//if (v_c_div.Trim() != "")
			//{
				Log::Info("", __FUNCTION__, "c_div111 =[{0}]", v_c_div);

				//查询碳钢，即 C_DIV = 0
				//sqlstr += " AND  C_DIV =  '" + v_c_div + "'";
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
			//}
			sqlstr += " ORDER BY  PROD_TIME";
			break;
		}
		cmd_inq.Parameters.Set("v_c_div", v_c_div);
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
