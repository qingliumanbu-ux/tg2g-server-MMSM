/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-04-18
Description: 成品改切记录报表查询
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsmbpqg_inq)

int f_mmsmbpqg_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_start_time = "";//开始时刻
	CString v_end_time = ""; //结束时刻
	CString v_heat_no = "";
	CString v_slab_no = "";
	CString v_strand_no = "";

	CPageInfo pageInfo;

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr_union = "";
	CString sqlstr_temp = "";
	CString sqlstr_count = "";
	CString sqlstr_order = "";


	/* 数据库操作类定义 */
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

		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_FROM"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_TO"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SLAB_NO"))
			v_slab_no = bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STRAND_NO"))
			v_strand_no = bcls_rec->Tables[0].Rows[0]["STRAND_NO"].ToString().Trim();

		if (v_start_time == "" && v_end_time == "")
		{
			sprintf(s.msg, "开始时间和结束时间条件必须都有！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT * FROM TMMSM33  WHERE 1 = 1 ";



			if (v_start_time != "")
			{
				sqlstr_temp += " AND SLAB_CUT_TIME >= '" + v_start_time + "'";
			}
			if (v_end_time != "")
			{
				sqlstr_temp += " AND SLAB_CUT_TIME <= '" + v_end_time + "'";
			}
			if (v_heat_no != "")
			{
				sqlstr_temp += " AND HEAT_NO = '" + v_heat_no + "'";
			}
			if (v_slab_no != "")
			{
				sqlstr_temp += " AND SLAB_NO = '" + v_slab_no + "'";
			}
			if (v_strand_no != "")
			{
				sqlstr_temp += " AND STRAND_NO = '" + v_strand_no + "'";
			}


			sqlstr_order += " ORDER BY  SLAB_CUT_TIME DESC";

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);


			sqlstr = sqlstr + sqlstr_temp + sqlstr_order;


			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;

		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

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


