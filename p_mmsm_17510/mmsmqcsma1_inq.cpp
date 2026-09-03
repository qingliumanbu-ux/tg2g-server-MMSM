/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 特棒外购料信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 特棒外购料信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


//外部函数声明

BM2F_ENTERACE(mmsmqcsma1_inq)

int f_mmsmqcsma1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString process_flag("");
	CDecimal cd_count = 0;
	CString product_flag("");
	int	record_count_per_page = 0;	/* 每页记录数 */
	int	current_page_no = 0;	/* 需查询的页号,从0开始计数 */
	int	start_row = 0;	/* 将要压入outBlock的起始行 */


	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];	/* 每页记录数 */
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];		/* 需查询的页号 */
		process_flag = bcls_rec->Tables[0].Rows[0]["AFFIRM_FLAG"].ToString().Trim();/* 确认标志 */
		product_flag = bcls_rec->Tables[0].Rows[0]["IMPORT_TYPE"].ToString().Trim();/* 确认标志 */

		Log::Trace("", __FUNCTION__, "record_count_per_page		= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "current_page_no				= [{0}]", current_page_no);
		Log::Trace("", __FUNCTION__, "process_flag				= [{0}]", process_flag);
		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT COUNT(*) "
				"  FROM TMMSMQC_FP "
				" WHERE 1 = 1 ";

			break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		if (process_flag.Trim() != "")
		{
			sqlstr += " and process_flag = @process_flag ";
		}
		if (product_flag.Trim() != "")
		{
			sqlstr += " and product_flag = @product_flag ";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		if (process_flag.Trim() != "")
		{
			cmd_inq.Parameters.Set("process_flag", process_flag);
		}
		if (product_flag.Trim() != "")
		{
			cmd_inq.Parameters.Set("product_flag", product_flag);
		}
		cd_count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "总记录数cd_count = [{0}]", cd_count.ToInt32());
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToInt32())
		{
			start_row = 0;
		}
		Log::Trace("", __FUNCTION__, "start_row = [{0}]", start_row);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT * "
				"  FROM TMMSMQC_FP "
				" WHERE 1 = 1 ";
			break;
		}
		if (process_flag.Trim() != "")
		{
			sqlstr += " and process_flag = @process_flag ";
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();	
		if (process_flag.Trim() != "")
		{
			cmd_inq.Parameters.Set("process_flag", process_flag);
		}
		Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		/* 替换数据列名 */
		if (bcls_ret->Tables[0].Columns.Contains("PROCESS_FLAG"))
			bcls_ret->Tables[0].Columns.SetColumnName(bcls_ret->Tables[0].Columns.IndexOf("PROCESS_FLAG"), "PRO_FLAG");//处理标志
		if (bcls_ret->Tables[0].Columns.Contains("PROCESS_DESC"))
			bcls_ret->Tables[0].Columns.SetColumnName(bcls_ret->Tables[0].Columns.IndexOf("PROCESS_DESC"), "DESCRIPTION");//处理说明
		if (bcls_ret->Tables[0].Columns.Contains("ST_NO_ERP"))
			bcls_ret->Tables[0].Columns.SetColumnName(bcls_ret->Tables[0].Columns.IndexOf("ST_NO_ERP"), "ST_NO");//ERP钢号
		if (bcls_ret->Tables[0].Columns.Contains("SURFACE_DECIDE_CODE_ERP"))
			bcls_ret->Tables[0].Columns.SetColumnName(bcls_ret->Tables[0].Columns.IndexOf("SURFACE_DECIDE_CODE_ERP"), "SURFACE_DECIDE_CODE");//ERP表面判定
		if (bcls_ret->Tables[0].Columns.Contains("MAT_WT_SP"))
			bcls_ret->Tables[0].Columns.SetColumnName(bcls_ret->Tables[0].Columns.IndexOf("MAT_WT_SP"), "MAT_WT");//实盘重量		
		if (bcls_ret->Tables[0].Columns.Contains("STOCK_PLACE_NO_ERP"))
			bcls_ret->Tables[0].Columns.SetColumnName(bcls_ret->Tables[0].Columns.IndexOf("STOCK_PLACE_NO_ERP"), "STOCK_PLACE_NO");//ERP库位号
		if (bcls_ret->Tables[0].Columns.Contains("DIFFERENT_DESC"))
			bcls_ret->Tables[0].Columns.SetColumnName(bcls_ret->Tables[0].Columns.IndexOf("DIFFERENT_DESC"), "ARCHIVE_STAMP_NO");//差别类型
		
		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");//增加块
		blkNum = bcls_ret->Tables.IndexOf("PAGEINFO");
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");		//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
