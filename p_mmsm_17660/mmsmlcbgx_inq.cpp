/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-09-11
Description: 炉成本查询
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明
//int f_mmsm36f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsmlcbgx_inq)

int f_mmsmlcbgx_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_heatnr = "";
	CString v_cast_seq = "";
	CString v_assist_heatno = "";
	CString v_grade_type = "";
	CString v_route = "";
	CString v_mat_type = "";
	CString v_mat_code = "";

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
		if (bcls_rec->Tables[0].Columns.Contains("HEATNR"))
		{
			v_heatnr = bcls_rec->Tables[0].Rows[0]["HEATNR"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CAST_SEQ"))
		{
			v_cast_seq = bcls_rec->Tables[0].Rows[0]["CAST_SEQ"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("ASSIST_HEATNO"))
		{
			v_assist_heatno = bcls_rec->Tables[0].Rows[0]["ASSIST_HEATNO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE1"))
		{
			v_grade_type = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("F_ROUTE1"))
		{
			v_route = bcls_rec->Tables[0].Rows[0]["F_ROUTE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_TYPE_DESC"))
		{
			v_mat_type = bcls_rec->Tables[0].Rows[0]["MAT_TYPE_DESC"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE_DR"))
		{
			v_mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE_DR"].ToString().Trim();
		}

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_from = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_to = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		}


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "select * from tqmtscb01_mx t where 1=1";

			if (v_heatnr != ""){
				sqlstr += " AND HEATNR = @v_heatnr";
			}
			if (v_cast_seq != ""){
				sqlstr += " AND CAST_SEQ = @v_cast_seq";
			}
			if (v_assist_heatno != ""){
				sqlstr += " AND ASSIST_HEATNO = @v_assist_heatno";
			}
			if (v_grade_type != ""){
				sqlstr += " AND GRADE_TYPE1 = @v_grade_type";
			}
			if (v_route != ""){
				sqlstr += " AND F_ROUTE1 = @v_route";
			}
			if (v_mat_type != ""){
				sqlstr += " AND MAT_TYPE_DESC = @v_mat_type";
			}
			if (v_mat_code != ""){
				sqlstr += " AND MAT_CODE_DR = @v_mat_code";
			}

			if (v_from.Trim() != "")
			{
				sqlstr += " AND to_char(to_date(decode(trim(AOD_BOF_E_DTIME),null,'1999-01-01 00:01:01',AOD_BOF_E_DTIME),'yyyy-mm-dd hh24:mi:ss'),'yyyyMMddhhmiss')>= @v_from";
			}
			if (v_to.Trim() != "")
			{
				sqlstr += " AND to_char(to_date(decode(trim(AOD_BOF_E_DTIME),null,'1999-01-01 00:01:01',AOD_BOF_E_DTIME),'yyyy-mm-dd hh24:mi:ss'),'yyyyMMddhhmiss')<= @v_to";
			}

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_heatnr", v_heatnr);
			cmd_inq.Parameters.Set("v_cast_seq", v_cast_seq);
			cmd_inq.Parameters.Set("v_assist_heatno", v_assist_heatno);
			cmd_inq.Parameters.Set("v_grade_type", v_grade_type);
			cmd_inq.Parameters.Set("v_route", v_route);
			cmd_inq.Parameters.Set("v_mat_type", v_mat_type);
			cmd_inq.Parameters.Set("v_mat_code", v_mat_code);

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
