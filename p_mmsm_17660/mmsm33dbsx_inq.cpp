/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-04-11
Description: 物料代办事项查询
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsm33dbsx_inq)

int f_mmsm33dbsx_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_mat_no = "";//开始时刻
	CString v_event_id = ""; //结束时刻

	CPageInfo pageInfo;

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_temp;
	CString sqlstr_count = "";

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

		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("EVENT_ID"))
		{
			v_event_id = bcls_rec->Tables[0].Rows[0]["EVENT_ID"].ToString().Trim();
		}



		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT T.*,DECODE(T.EVENT_ID,'MM13','未收货','材料封锁') BACK_YUANYIN FROM TMMSM33DBSX T "
				" WHERE 1 = 1 ";

			if (v_mat_no != ""){
				sqlstr_temp += " AND MAT_NO = '" + v_mat_no + "'";
			}
			if (v_event_id != "")
			{
				sqlstr_temp += " AND EVENT_ID = '" + v_event_id + "'";
			}


			//sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY  MAT_NO";
			sqlstr = sqlstr + sqlstr_temp;
			break;

		}

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_mat_no", v_mat_no);
		cmd_inq.Parameters.Set("v_event_id", v_event_id);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		sqlstr_temp = " ";
		sqlstr = "WITH MM1 AS (SELECT *\
			FROM(\
				SELECT T.*, ROW_NUMBER() over(PARTITION BY MAT_NO ORDER BY PROD_TIME DESC) ROW_ID\
				FROM TMMSM3E T\
				WHERE RECEIVE_BACK_STATUS = 'E')\
			WHERE ROW_ID = 1)\
			SELECT T1.MAT_NO, T3.EVENT_ID, T3.EVENT_TYPE, T3.PROD_TIME, T3.RECEIVE_BACK_STATUS, T3.REMARK\
			FROM TMMSM01 T1\
			LEFT JOIN MM1 T3 ON T1.MAT_NO = T3.MAT_NO\
			WHERE T1.RCV_MAT_FLAG = 'E' ";

		if (v_mat_no != "") {
			sqlstr_temp += " AND t1.MAT_NO = '" + v_mat_no + "'";
		}
		if (v_event_id != "")
		{
			sqlstr_temp += " AND t3.EVENT_ID = '" + v_event_id + "'";
		}


		//sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr_temp += " ORDER BY  MAT_NO";
		sqlstr = sqlstr + sqlstr_temp;
		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
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


