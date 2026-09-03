/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 转炉废钢成品评价基表
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmupd_inq)

int f_mmsmupd_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int		TotalRecordCount = 0;
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_table_type = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CString v_start_time = "";
	CString v_end_time = "";
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;
	try
	{

		v_table_type=bcls_rec->Tables[0].Rows[0]["TABLE"].ToString();
		if (v_table_type.Trim() == "")
		{
			sprintf(s.msg, "【表名称】不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		CModel tmmsmupd(v_table_type);
		v_start_time = bcls_rec->Tables[0].Rows[0]["PROD_DATE_S"].ToString();
		v_end_time = bcls_rec->Tables[0].Rows[0]["PROD_DATE_E"].ToString();
		tmmsmupd.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr_count = " SELECT COUNT(1)  FROM " + v_table_type + " WHERE 1=1 ";
			sqlstr = " SELECT *  FROM " + v_table_type + " WHERE 1=1 ";
			break;
		}
		if (tmmsmupd["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_CODE		like '%'|| @MAT_CODE||'%'";
		}
		if (v_start_time.Trim() != "")
		{
			sqlstr_temp += " AND DATE_C >= @v_start_time";
		}
		if (v_end_time.Trim() != "")
		{
			sqlstr_temp += " AND DATE_C <= @v_end_time";
		}
		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr_temp += "  ORDER BY  DATE_C DESC";
		sqlstr = sqlstr + sqlstr_temp;
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		cmd_inq.Parameters.Set("MAT_CODE", tmmsmupd["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("v_end_time", v_end_time);
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
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
