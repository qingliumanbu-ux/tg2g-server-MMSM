/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   孟凡杰
Version:    1.0
Date:     2024-01-04 9:13:56
Description: 板坯查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsmfpmx_inq2)

int f_mmsmfpmx_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_mat_no = "";
	CString v_heat_no = "";
	CString v_start_time = "";
	CString v_end_time = "";
	CString v_st_no = "";
	CString v_dev_code = "";
	CString v_app_code = "";

	CDbCommand cmd_inq(conn);

	try
	{
		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("DEV_CODE"))
			v_dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("APP_CODE"))
			v_app_code = bcls_rec->Tables[0].Rows[0]["APP_CODE"].ToString();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "mat_no =[{0}]", v_mat_no);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr = " SELECT T1.*,T2.RECUT_GROUP,T2.RECUT_DATE FROM VMMSM01 T1 "
				 " LEFT JOIN TMMSM39 T2 ON T1.MAT_NO = T2.MAT_NO WHERE 1 = 1  ";
				//" SELECT T1.*,T2.CODE_DESC_1_CONTENT FROM VMMSM01 T1 "
				//" LEFT JOIN(SELECT * FROM TWMSMZD02 WHERE CODE_CLASS = 'STEEL_TYPE') T2 ON T1.ST_NO = T2.CODE WHERE 1 = 1 ";
			if (v_app_code != "")
			{
				if (v_app_code == "02"){
					sqlstr += " AND T1.COMPLEX_DECIDE_CODE = '9' ";
				}
				else
				{
					sqlstr += " AND T1.COMPLEX_DECIDE_CODE <> '9' ";
				}
			}
			if (v_st_no != "")
			{
				sqlstr += " AND  T1.ST_NO	= @v_st_no";
			}
			if (v_dev_code != "")
			{
				sqlstr += " AND  T1.DEV_CODE	= @v_dev_code";
			}
			if (v_heat_no != "")
			{
				sqlstr += " AND  T1.HEAT_NO	= @v_heat_no";
			}
			if (v_mat_no != "")
			{
				sqlstr += " AND  T1.MAT_NO	= @v_mat_no";
			}
			if (v_start_time.Trim() != "")
			{
				sqlstr += " AND T1.SLAB_CUT_TIME		>= @v_start_time";
			}
			if (v_end_time.Trim() != "")
			{
				sqlstr += " AND T1.SLAB_CUT_TIME		<= @v_end_time";
			}


			sqlstr += "  order by  T1.SLAB_CUT_TIME ";

			break;
		}
		cmd_inq.Parameters.Set("v_app_code", v_app_code);
		cmd_inq.Parameters.Set("v_st_no", v_st_no);
		cmd_inq.Parameters.Set("v_dev_code", v_dev_code);
		cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
		cmd_inq.Parameters.Set("v_mat_no", v_mat_no);
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
