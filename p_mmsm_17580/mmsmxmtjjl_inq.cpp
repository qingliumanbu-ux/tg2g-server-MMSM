/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2016-01-22
Description: 设备停机实绩查询
**************************************************/
//框架头文件
#include "stdafx.h"

//业务头文件

BM2F_ENTERACE(mmsmxmtjjl_inq)

int f_mmsmxmtjjl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	CString v_start_time = "";
	CString v_end_time = "";
	/*CString v_dev_code = "";
	CString v_prod_shift_group = "";
	CString v_stop_reason_code = "";*/
	//CDecimal stop_time = 0;//停机时间min

	CPageInfo pageInfo;

	/* 业务变量 */ 
	//CModel tmmsm2e("TMMSM2E");

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";


	/* 数据库操作类定义 */
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
		/*if (bcls_rec->Tables[0].Columns.Contains("DEV_CODE"))
			v_dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_GROUP"))
			v_prod_shift_group = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_GROUP"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("STOP_REASON_CODE"))
			v_stop_reason_code = bcls_rec->Tables[0].Rows[0]["STOP_REASON_CODE"].ToString();*/

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr = "SELECT * "
				//"DEV_CODE,PROD_SHIFT_GROUP,IS_PLAN,STOP_START_TIME,STOP_END_TIME,"
				//"DEV_STOP_TIME,STOP_REASON_CODE,STOP_REMARK,REC_CREATOR "
				"FROM TMMSM2E"
				"	WHERE 1 = 1 ";
			/*if (v_dev_code.Trim() != "")
			{
				sqlstr_temp += " AND DEV_CODE			= @v_dev_code";
			}
			if (v_prod_shift_group.Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_GROUP			= @v_prod_shift_group";
			}*/
			if (v_start_time.Trim() != "")
			{
				sqlstr_temp += " AND REC_CREATE_TIME			>= '" + v_start_time + "'";
			}
			if (v_end_time.Trim() != "")
			{
				sqlstr_temp += " AND REC_CREATE_TIME			<= '" + v_end_time + "'";
			}
			/*if (v_stop_reason_code.Trim() != "")
			{
				sqlstr_temp += " AND STOP_REASON_CODE			= @v_stop_reason_code";
			}*/

			sqlstr_temp += " ORDER BY  REC_CREATE_TIME";
			sqlstr = sqlstr + sqlstr_temp;
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("v_end_time", v_end_time);
		/*cmd_inq.Parameters.Set("v_dev_code", v_dev_code);
		cmd_inq.Parameters.Set("v_prod_shift_group", v_prod_shift_group);
		cmd_inq.Parameters.Set("v_stop_reason_code", v_stop_reason_code);*/
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


