/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-15
Description: 碳钢铸坯检验记录查询
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(mmsmtgjyjl_inq)

int f_mmsmtgjyjl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString	v_heat_no = "";
	CString v_start_time = "";
	CString v_end_time = "";


	//系统的分页类信息。
	CPageInfo pageInfo;


	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}


		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		Log::Trace("", "", "heat_no[{0}]", v_heat_no);
		/*if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("PRINT_NO"))
			v_print_no = bcls_rec->Tables[0].Rows[0]["PRINT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("QUERY_DIV"))
			queryDiv = bcls_rec->Tables[0].Rows[0]["QUERY_DIV"].ToString();
		tableName = queryDiv.Trim();*/

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr =  " select t.*,r.grade_name,r.grade_type"
				" from TMMSM33TGJY t left join DA_GRADE_TYPE r"
				" on t.ST_NO = r.grade_id"
				" WHERE 1 = 1";
			//sqlstr = "SELECT *  FROM TMMSM33TGJY  WHERE 1 = 1 ";

			//sqlstr_count = " SELECT COUNT(1) FROM " + tableName + " WHERE 1=1 ";
			//sqlstr = " SELECT * FROM " + tableName + " WHERE 1=1 ";

			if (v_start_time.Trim() != "")
			{
				sqlstr_temp += " AND DATE_TIME			>= '" + v_start_time + "'";
			}
			if (v_end_time.Trim() != "")
			{
				sqlstr_temp += " AND DATE_TIME			<= '" + v_end_time + "'";
			}
			if (v_heat_no.Trim() != "")
			{
				sqlstr_temp += " AND HEAT_NO	= '" + v_heat_no + "'";
			}
			/*if (tmmsm33tgjy["MAT_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND MAT_NO	= @v_mat_no";
			}
			if (tmmsm33tgjy["PRINT_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND PRINT_NO	= @v_print_no";
			}*/

			/*tmmsm33tgjy["DEV_CODE"] = tmmsm01["DEV_CODE"];
			tmmsm33tgjy["MAT_TARG_WIDTH"] = tmmsm01["MAT_TARG_WIDTH"];
			tmmsm33tgjy["SLAB_CUT_TIME"] = tmmsm01["SLAB_CUT_TIME"];*/

			sqlstr_temp += " ORDER BY  DATE_TIME";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

		
		/*cmd_inq.Parameters.Set("v_mat_no", v_mat_no);
		cmd_inq.Parameters.Set("v_print_no", v_print_no);*/



		
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
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
