/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:    
Description: 获取消耗工序基表
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmwx_inq)

int f_mmsmwx_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_temp = "";
	CString v_table_type = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CDbCommand cmd_inq(conn);

	try
	{
	
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			v_mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_ID"))
			v_mat_id = bcls_rec->Tables[0].Rows[0]["MAT_ID"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DESCRIPTION"))
			v_mat_name = bcls_rec->Tables[0].Rows[0]["DESCRIPTION"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DEV_CODE"))
			v_dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().Trim();
		
		if (v_table_type.Trim() == "")
		{
			sprintf(s.msg, "【表名称】不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "v_table_type[{0}]  ", v_table_type);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT *  FROM " + v_table_type + " WHERE 1=1 ";
			 
			if (v_mat_code.Trim() != "" && v_table_type == "TMMSMW4")
			{
				sqlstr_temp += " AND MAT_CODE LIKE  '%'|| @MAT_CODE||'%'";
			}

			if (v_dev_code.Trim() != "" && (v_table_type == "TMMSMW1" || v_table_type == "TMMSMW2"))
			{
				sqlstr_temp += " AND DEV_CODE LIKE  '%'|| @DEV_CODE||'%'";
			}
			if (v_mat_id.Trim() != "" && (v_table_type == "ZJ_MAT_ELEMENT" || v_table_type == "ZJ_SCRAP_ELEMENT" || v_table_type == "ZJ_JISHUKE_MAT"))
			{
				sqlstr_temp += " AND MAT_ID LIKE  '%'|| @MAT_ID||'%'";
			}
			if (v_mat_name.Trim() != "" && (v_table_type == "ZJ_MAT_ELEMENT" || v_table_type == "ZJ_SCRAP_ELEMENT" || v_table_type == "ZJ_JISHUKE_MAT"))
			{
				sqlstr_temp += " AND DESCRIPTION  LIKE '%'|| @DESCRIPTION||'%'";
			}
			sqlstr = sqlstr + sqlstr_temp;

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;
		}

		cmd_inq.Parameters.Set("MAT_CODE", v_mat_code);
		cmd_inq.Parameters.Set("MAT_ID", v_mat_id);
		cmd_inq.Parameters.Set("DEV_CODE", v_dev_code);
		cmd_inq.Parameters.Set("DESCRIPTION", v_mat_name);
		cmd_inq.SetCommandText(sqlstr);
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
