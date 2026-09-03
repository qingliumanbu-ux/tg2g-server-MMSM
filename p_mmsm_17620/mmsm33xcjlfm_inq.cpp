/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wsl
Version:    1.0
Date:       2024-01-06
Description: 炼钢校秤记录查询砝码重量
**************************************************/
//框架头文件
#include "stdafx.h"

//业务头文件

BM2F_ENTERACE(mmsm33xcjlfm_inq)

int f_mmsm33xcjlfm_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_prod_date = "";
	CString v_prod_date_to = "";
	/* 业务变量 */
	CModel tmmsm33xcjl("TMMSM33XCJL");
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_temp;
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{

		/*tmmsm33xcjl.MergeFrom(bcls_rec->Tables[0].Rows[0]);


		if (bcls_rec->Tables[0].Columns.Contains("PROD_DATE"))
			v_prod_date = bcls_rec->Tables[0].Rows[0]["PROD_DATE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_DATE_TO"))
			v_prod_date_to = bcls_rec->Tables[0].Rows[0]["PROD_DATE_TO"].ToString().Trim();*/


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select code as MAT_WT,CODE_DESC_1_CONTENT as c_div  from TWMSMZD02 where CODE_CLASS ='MMXCJLFM' ";

			
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

			cmd_inq.SetCommandText(sqlstr);

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


