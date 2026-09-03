/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-04-23
Description: 成品平衡产量报表
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsmphcl_inq)

int f_mmsmphcl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_start_time = "";//开始时刻
	CString v_end_time = ""; //结束时刻
	//CString v_heat_no = "";

	CPageInfo pageInfo;

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{

		/*if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		}*/
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr = " SELECT *  FROM VMMSM37  WHERE 1=1 ";


			if (v_start_time != ""){
				sqlstr += " AND MOVEMENT_C >= @v_start_time";
				//sqlstr += " AND REC_CREATE_TIME >= @START_TIME";
			}
			if (v_end_time != "")
			{
				sqlstr += " AND MOVEMENT_C <= @v_end_time";
				//sqlstr += " AND REC_CREATE_TIME <= @END_TIME";
			}
			/*if (tmmsm36["HEAT_NO"].ToString().Trim() != "")
			{
			sqlstr += " AND HEAT_NO			= @v_heat_no";
			}*/
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

			sqlstr += " ORDER BY  MOVEMENT_C";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_start_time", v_start_time);
			cmd_inq.Parameters.Set("v_end_time", v_end_time);

			//cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
			//cmd_inq.Parameters.Set("PROD_SHIFT_NO", tmmsm36["PROD_SHIFT_NO"].ToString());
			//cmd_inq.Parameters.Set("PROD_SHIFT_GROUP", tmmsm36["PROD_SHIFT_GROUP"].ToString());
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


