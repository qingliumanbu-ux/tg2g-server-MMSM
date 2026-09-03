/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:
Date:     2023/3/11
Description: 导入查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmdr_inq)

int f_mmsmdr_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int TotalRecordCount = 0;

	CString table_name = "";
	CString rec_create_time = "";
	CString rec_create_time_1 = "";

	CString v_st_no = "";
	CString v_grade_type2 = "";
	CString v_heat_no = "";
	CString v_order_no = "";

	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);

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
		if (bcls_rec->Tables[0].Columns.Contains("table_name"))
			table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("REC_CREATE_TIME"))
			rec_create_time = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("REC_CREATE_TIME_1"))
			rec_create_time_1 = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_1"].ToString().Trim();
		if (table_name == "TWMSMCZTS" || table_name == "TWMSMCZTS_GC")
		{
			if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
				v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
			if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE2"))
				v_grade_type2 = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE2"].ToString().Trim();
		}
		if (table_name == "TQMTS0RDR")
		{
			if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
				v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
			if (bcls_rec->Tables[0].Columns.Contains("ORDER_NO"))
				v_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();
		}
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "rec_create_time  =[{0}]", rec_create_time);
		//Log::Info("", __FUNCTION__, "prod_time_t  =[{0}]", prod_time_t);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM " + table_name + " "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM " + table_name + " "
				"  WHERE 1=1 "
				;
			if (rec_create_time != ""){
				sqlstr_temp += " AND REC_CREATE_TIME>='" + rec_create_time + "' ";
			}
			if (rec_create_time_1 != ""){
				sqlstr_temp += " AND REC_CREATE_TIME<='" + rec_create_time_1 + "' ";
			}
			if (table_name == "TWMSMCZTS" || table_name == "TWMSMCZTS_GC")
			{
				if (v_st_no != ""){
					sqlstr_temp += " AND ST_NO LIKE '%" + v_st_no + "%' ";
				}
				if (v_grade_type2 != ""){
					sqlstr_temp += " AND GRADE_TYPE2 LIKE '%" + v_grade_type2 + "%' ";
				}
				sqlstr_temp += "ORDER BY C_DIV,GRADE_TYPE2,FACTORY_2";
			}
			if (table_name == "TWMSMCZTS_GYHX")
			{
				sqlstr_temp += "ORDER BY C_DIV,FACTORY_2,DEV_CODE";
			}
			if (table_name == "TQMTS0RDR")
			{
				if (v_heat_no != ""){
					sqlstr_temp += " AND HEAT_NO LIKE '%" + v_heat_no + "%' ";
				}
				if (v_order_no != ""){
					sqlstr_temp += " AND ORDER_NO LIKE '%" + v_order_no + "%' ";
				}
				sqlstr_temp += " ORDER BY  REC_CREATE_TIME DESC";

			}

			if (table_name == "TQMTSBWDY"){
				sqlstr_temp += " ORDER BY  DATE_CODE";
			}

			if (table_name == "TWMSMSGAL"){
				sqlstr_temp += " ORDER BY  REC_CREATE_TIME DESC";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
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
