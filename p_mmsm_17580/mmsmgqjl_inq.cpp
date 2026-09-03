/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   mfj
Version:    1.0
Date:     2024-04-01 17:13:56
Description:
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"





// service入口
BM2F_ENTERACE(mmsmgqjl_inq)

int f_mmsmgqjl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString v_mat_no = "";
	CString v_heat_no = "";
	CString v_st_no = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = " ";
	int		TotalRecordCount = 0;
	CString v_start_time = "";
	CString v_end_time = "";


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

		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();



		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " SELECT T.PROD_TIME,T.PROD_SHIFT_GROUP,T.RECUT_GROUP,T.PROD_SHIFT_NO,T.HEAT_NO,T.MAT_NO,T.BATCH,T.ST_NO,R.SG_GRADE_1,  "
				"	T.CUT_BEFORE_WIDTH, T.CUT_AFTER_WIDTH, T.GRINDING_FLAG, T.CUTTING_TYPE, T.RECUT_DATE, T.CUT_BEFORE_LEN, T.CUT_AFTER_THICK, T.OTHER_CUT_LEN,  	"
				"	T.CUT_BEFORE_WT, T.CUT_AFTER_WT, T.CUT_SCRAP_WT, T.REMARK, T.C_DIV, T.REC_CREATOR,re.cname as resp,												"
				"	CASE WHEN NVL(SUM_35, 0) = 0 THEN 0 ELSE 1 END AS SF_GQZB, nvl(MAT_LEN_1, 0) MAT_LEN_1, nvl(MAT_LEN_2, 0) MAT_LEN_2,							"
				"	nvl(MAT_LEN_3, 0) MAT_LEN_3, nvl(MAT_LEN_4, 0) MAT_LEN_4, nvl(MAT_LEN_5, 0) MAT_LEN_5, nvl(MAT_LEN_6, 0) MAT_LEN_6,								"
				"	nvl(MAT_LEN_7, 0) MAT_LEN_7, nvl(MAT_LEN_8, 0) MAT_LEN_8, nvl(MAT_LEN_9, 0) MAT_LEN_9, NVL(MAT_NO_1, ' ') MAT_NO_1								"
				"	, NVL(MAT_NO_2, ' ') MAT_NO_2, NVL(MAT_NO_3, ' ') MAT_NO_3, NVL(MAT_NO_4, ' ') MAT_NO_4, NVL(MAT_NO_5, ' ') MAT_NO_5							"
				"	, NVL(MAT_NO_6, ' ') MAT_NO_6, NVL(MAT_NO_7, ' ') MAT_NO_7, NVL(MAT_NO_8, ' ') MAT_NO_8, NVL(MAT_NO_9, ' ') MAT_NO_9 FROM TMMSM39 T LEFT JOIN(	"
				"	SELECT MAX(MAT_LEN_1) MAT_LEN_1, MAX(MAT_LEN_2) MAT_LEN_2, MAX(MAT_LEN_3) MAT_LEN_3, MAX(MAT_LEN_4) MAT_LEN_4, MAX(MAT_LEN_5) MAT_LEN_5, MAX(MAT_LEN_6) MAT_LEN_6, MAX(MAT_LEN_7) MAT_LEN_7, MAX(MAT_LEN_8)MAT_LEN_8, MAX(MAT_LEN_9)MAT_LEN_9, IN_MAT_NO, COUNT(1) AS SUM_35 "
				"	, MAX(MAT_NO_1) MAT_NO_1, MAX(MAT_NO_2) MAT_NO_2, MAX(MAT_NO_3) MAT_NO_3, MAX(MAT_NO_4) MAT_NO_4, MAX(MAT_NO_5) MAT_NO_5,						"
				"	MAX(MAT_NO_6) MAT_NO_6, MAX(MAT_NO_7) MAT_NO_7, MAX(MAT_NO_8) MAT_NO_8, MAX(MAT_NO_9) MAT_NO_9 FROM(										   "
				"	SELECT CASE WHEN PX_OR = '1' THEN MAT_LEN ELSE 0 END MAT_LEN_1, CASE WHEN PX_OR = '2' THEN MAT_LEN ELSE 0 END MAT_LEN_2,					   "
				"	CASE WHEN PX_OR = '3' THEN MAT_LEN ELSE 0 END MAT_LEN_3, CASE WHEN PX_OR = '4' THEN MAT_LEN ELSE 0 END MAT_LEN_4,							   "
				"	CASE WHEN PX_OR = '5' THEN MAT_LEN ELSE 0 END MAT_LEN_5, CASE WHEN PX_OR = '6' THEN MAT_LEN ELSE 0 END MAT_LEN_6,							   "
				"	CASE WHEN PX_OR = '7' THEN MAT_LEN ELSE 0 END MAT_LEN_7, CASE WHEN PX_OR = '8' THEN MAT_LEN ELSE 0 END MAT_LEN_8,							   "
				"	CASE WHEN PX_OR = '9' THEN MAT_LEN ELSE 0 END MAT_LEN_9, IN_MAT_NO,																			   "
				"	CASE WHEN PX_OR = '1' THEN MAT_NO ELSE ' ' END MAT_NO_1, CASE WHEN PX_OR = '2' THEN MAT_NO ELSE ' ' END MAT_NO_2,							   "
				"	CASE WHEN PX_OR = '3' THEN MAT_NO ELSE ' ' END MAT_NO_3, CASE WHEN PX_OR = '4' THEN MAT_NO ELSE ' ' END MAT_NO_4,							   "
				"	CASE WHEN PX_OR = '5' THEN MAT_NO ELSE ' ' END MAT_NO_5, CASE WHEN PX_OR = '6' THEN MAT_NO ELSE ' ' END MAT_NO_6,							   "
				"	CASE WHEN PX_OR = '7' THEN MAT_NO ELSE ' ' END MAT_NO_7, CASE WHEN PX_OR = '8' THEN MAT_NO ELSE ' ' END MAT_NO_8,							   "
				"	CASE WHEN PX_OR = '9' THEN MAT_NO ELSE ' ' END MAT_NO_9																						   "
				"	FROM(select row_number()  over(partition by IN_MAT_NO ORDER BY MAT_NO) AS PX_OR, IN_MAT_NO, MAT_LEN, mat_no FROM TMMSM35))GROUP BY IN_MAT_NO)  "
				"	T2 ON T.MAT_NO = T2.IN_MAT_NO "
				"   LEFT JOIN TQMTS0X R "
				"   ON T.ST_NO = R.ST_NO "
				"   left join tesuserinfo re "
				"   on t.rec_creator = re.ename";


			sqlstr_temp += " where 1 =1 ";

			if (v_mat_no != "")
			{
				sqlstr_temp += " AND  T.MAT_NO = '" + v_mat_no + "'  ";
			}
			if (v_heat_no != "")
			{
				sqlstr_temp += " AND  T.HEAT_NO = '" + v_heat_no + "'  ";
			}
			if (v_st_no != "")
			{
				sqlstr_temp += " AND  T.ST_NO = '" + v_st_no + "'  ";
			}
			if (v_start_time != "")
			{
				sqlstr_temp += " AND  T.RECUT_DATE >= '" + v_start_time + "'  ";
			}
			if (v_end_time != "")
			{
				sqlstr_temp += " AND  T.RECUT_DATE <= '" + v_end_time + "'  ";
			}

			sqlstr += sqlstr_temp;
		}

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

		/*cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();*/
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		//返回分页总数量信息 
		/*	bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;*/

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
