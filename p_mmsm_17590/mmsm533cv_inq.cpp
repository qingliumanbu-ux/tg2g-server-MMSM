/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   mfj
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料退货装车确认查询1
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm533cv_inq)

int f_mmsm533cv_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString v_plan_no_y = "";//计划号
	CString v_mat_code = "";//物料号
	CString v_start_time = "";//计划退货日期从:
	CString v_end_time = "";//计划退货日期到:
	CString v_sql_column = "";//字段名
	CString v_status = "";//状态    V为车辆信息   1为退货装货确认 



	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm84("TMMSM84");
	CModel tmmsm54("TMMSM54");

	CDbCommand cmd_inq(conn);

	try
	{
		

		if (bcls_rec->Tables[0].Columns.Contains("PLAN_NO_Y"))
			v_plan_no_y = bcls_rec->Tables[0].Rows[0]["PLAN_NO_Y"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			v_mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();

		Log::Info("", __FUNCTION__, "[{0}]  [{1}]  [{2}]  [{3}]", v_plan_no_y, v_mat_code, v_start_time, v_end_time);

		//Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm84["MAT_CODE"].ToString());

		v_sql_column = " ";

		//查询退货计划
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT  * "
				"   FROM TMMSM54 "
				"  WHERE 1=1 ";



			if (v_plan_no_y.Trim() != "")
			{
				sqlstr += " AND PLAN_NO_Y = '" + v_plan_no_y + "'";
			}
			if (v_mat_code.Trim() != "")
			{
				sqlstr += " AND MAT_CODE = '" + v_mat_code + "'";
			}
			if (v_start_time.Trim() != "")
			{
				sqlstr += " AND PLAN_SEND_TIME >= '" + v_start_time + "'";
			}
			if (v_end_time.Trim() != "")
			{
				sqlstr += " AND PLAN_SEND_TIME <= '" + v_end_time + "'";
			}


			break;
		}

		Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		bcls_ret->Tables.Add();

		//查询退货进厂车辆信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT * "
				"   FROM TMMSM84 "
				"  WHERE STATUS = 'V' " ;

			if (v_plan_no_y.Trim() != "")
			{
				sqlstr += " AND PLAN_NO = '" + v_plan_no_y + "'";
			}
			/*if (v_mat_code.Trim() != "")
			{
			sqlstr += " AND MAT_CODE = '" + v_mat_code + "'";
			}
			if (v_start_time.Trim() != "")
			{
			sqlstr += " AND PLAN_MAKE_TIME >= '" + v_start_time.Substring(0, 8) + "'";
			}
			if (v_end_time.Trim() != "")
			{
			sqlstr += " AND PLAN_MAKE_TIME <= '" + v_end_time.Substring(0, 8) + "'";
			}*/


			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr222[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		bcls_ret->Tables.Add();

		//查询退货装货确认信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT * "
				"   FROM TMMSM84 "
				"  WHERE STATUS = '1' ";

			if (v_plan_no_y.Trim() != "")
			{
				sqlstr += " AND PLAN_NO = '" + v_plan_no_y + "'";
			}
			/*if (v_mat_code.Trim() != "")
			{
			sqlstr += " AND MAT_CODE = '" + v_mat_code + "'";
			}
			if (v_start_time.Trim() != "")
			{
			sqlstr += " AND PLAN_MAKE_TIME >= '" + v_start_time.Substring(0, 8) + "'";
			}
			if (v_end_time.Trim() != "")
			{
			sqlstr += " AND PLAN_MAKE_TIME <= '" + v_end_time.Substring(0, 8) + "'";
			}*/


			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr3333[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2], pageInfo.RecordFrom, pageInfo.PageSize);
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
