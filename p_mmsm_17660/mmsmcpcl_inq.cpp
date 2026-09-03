/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-15
Description: 成品产量报表查询
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(mmsmcpcl_inq)

int f_mmsmcpcl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString	v_heat_no = "";
	/*CString	v_mat_no = "";
	CString	v_print_no = "";*/
	CString v_start_time = "";
	CString v_end_time = "";
	//CString tableName = "";
	//CString queryDiv = "";


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

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		}



		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select A.*,B.GRADE_TYPE3 from VMMSMCPCL_BB A LEFT JOIN tqmtscb09_dr B ON A.ST_NO=B.STEEL_GRADE where 1 = 1 ";

			if (v_start_time != ""){
				sqlstr_temp += " AND SLAB_CUT_TIME >= '" + v_start_time.Substring(0,8).Trim()+"000000" + "'";
				//sqlstr += " AND REC_CREATE_TIME >= @v_start_time";
			}
			if (v_end_time != "")
			{
				sqlstr_temp += " AND SLAB_CUT_TIME <= '" + v_end_time.Substring(0, 8).Trim() + "235959" + "'";
			}
			if (v_heat_no != "")
			{
				sqlstr_temp += " AND HEAT_NO = @v_heat_no";
			}
			

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY HEAT_NO";
			sqlstr = sqlstr + sqlstr_temp;

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;

		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("v_end_time", v_end_time);
		cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
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
