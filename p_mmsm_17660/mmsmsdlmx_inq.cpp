/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-09-11
Description: 炉成本查询
**************************************************/
//框架头文件
#include "stdafx.h"	 

BM2F_ENTERACE(mmsmsdlmx_inq)

int f_mmsmsdlmx_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	
	CString type_flag = "";
	
	CString v_from = "";//开始时刻
	CString v_to = "";//开始时刻
	CPageInfo pageInfo;



	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_from = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().SubstringNE(0, 8);
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_to = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		}

		if (bcls_rec->Tables[0].Columns.Contains("BUSI_TYPE"))
		{
			type_flag = bcls_rec->Tables[0].Rows[0]["BUSI_TYPE"].ToString().Trim();
		}
		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			
			sqlstr = "select * from TQMTSCB04_MX t where 1=1";
			
			if (type_flag != ""){
				sqlstr += " AND t.BUSI_TYPE = @type_flag";
			}

			if (v_from.Trim() != "")
			{
				sqlstr += " AND t.DATE_C>= @v_from";
			}
			if (v_to.Trim() != "")
			{
				sqlstr += " AND t.DATE_C<= @v_to";
			}
			sqlstr += " order by t.date_c desc";
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", v_from);
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", v_to);
			cmd_inq.SetCommandText(sqlstr);
			
			cmd_inq.Parameters.Set("type_flag", type_flag);
			cmd_inq.Parameters.Set("v_from", v_from);
			cmd_inq.Parameters.Set("v_to", v_to);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}


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