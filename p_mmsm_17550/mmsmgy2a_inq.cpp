/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-12-21
Description: 查预溶液画面的2a分组数据
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


BM2F_ENTERACE(mmsmgy2a_inq)


int f_mmsmgy2a_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CDecimal FURNACE_AGE_SM = 0;
	int i = 0;
	int fetchRowCount = 0;
	CString v_proc_no = "";
	CString v_heat_no = "";
	CString sqlstr_temp = "";
	CString dev_code = "";

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tmmsm27("TMMSM27");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("L2_PROC_NO"))
			v_proc_no = bcls_rec->Tables[0].Rows[0]["L2_PROC_NO"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("DEV_CODE"))
			dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().TrimOrBlank().ToUpper();
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT MAT_CODE, L2_PROC_NO, DEV_CODE, MAT_NAME, SUM(DEVO_WT) DEVO_WT "
				" FROM TMMSM2A "
				" WHERE 1 = 1 ";
			if (v_heat_no.Trim() != "")
			{
				sqlstr += " AND HEAT_NO = @v_heat_no";
			}
			if (v_proc_no.Trim() != "")
			{
				sqlstr += " AND L2_PROC_NO = @v_proc_no";
			}
			if (dev_code.Trim() != "")
			{
				sqlstr += " AND DEV_CODE = @dev_code";
			}

			sqlstr_temp += " GROUP BY MAT_CODE, L2_PROC_NO, DEV_CODE, MAT_NAME, HEAT_NO";

			sqlstr = sqlstr + sqlstr_temp;

			break;
		}
		cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
		cmd_inq.Parameters.Set("v_proc_no", v_proc_no);
		cmd_inq.Parameters.Set("dev_code", dev_code);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetMsg() };
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
