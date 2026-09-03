/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-04-11
Description: 成品修磨记录报表查询
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明
//int f_mmsm36f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsmxmjl_inq)

int f_mmsmxmjl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_start_time = "";//开始时刻
	CString v_end_time = ""; //结束时刻

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

			/*sqlstr = " SELECT T.*,R.GRADE_TYPE, "
					 " ROUND((((DECODE(MEND_AFTER_WEIGHT, null, 0, MEND_AFTER_WEIGHT) * DECODE(MEND_RATE,NULL,0,MEND_RATE) / 100 * 1000) *  (18.3 / DECODE(MEND_AFTER_WEIGHT, 0, NULL, MEND_AFTER_WEIGHT)) / 238 *  "
					 " (10.25 / DECODE(decode(trim(MEND_METAL_RATE), null, 0, MEND_METAL_RATE), 0, null, MEND_METAL_RATE))) * DECODE(MEND_AFTER_WEIGHT, null, 0, MEND_AFTER_WEIGHT)), 3) AS MEND_WEIGHT1 "
				     " FROM TMMSM34 t "
					 " LEFT JOIN DA_GRADE_TYPE R "
					 " ON T.ST_NO = R.GRADE_ID"
				     " WHERE 1=1";*/
			sqlstr = " SELECT E.*,R.GRADE_TYPE,"
				     " ROUND((((DECODE(MEND_AFTER_WEIGHT, null, 0, MEND_AFTER_WEIGHT) * DECODE(MEND_RATE, NULL, 0, MEND_RATE) / 100 * 1000) *  (18.3 / DECODE(MEND_AFTER_WEIGHT, 0, NULL, MEND_AFTER_WEIGHT)) / 238 *"
					 " (10.25 / DECODE(decode(trim(MEND_METAL_RATE1), null, 0, MEND_METAL_RATE1), 0, null, MEND_METAL_RATE1))) * DECODE(MEND_AFTER_WEIGHT, null, 0, MEND_AFTER_WEIGHT)), 3) AS MEND_WEIGHT1"
					 " FROM"
					 " (SELECT T.*, Q.CODE_DESC_1_CONTENT AS MEND_METAL_RATE1"
					 " FROM TMMSM34 T"
					 " LEFT JOIN (select * from TWMSMZD02 WHERE CODE_CLASS = 'METALRATE') Q  ON T.ST_NO = Q.CODE )E "
					 " LEFT JOIN DA_GRADE_TYPE R  ON E.ST_NO = R.GRADE_ID"
					 " WHERE 1 = 1 ";

			if (v_start_time != ""){
				sqlstr_temp += " AND START_TIME >= '" + v_start_time + "'";
			}
			if (v_end_time != "")
			{
				sqlstr_temp += " AND START_TIME <= '" + v_end_time + "'";
			}
		

			//sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY  REC_CREATE_TIME";
			sqlstr = sqlstr + sqlstr_temp;
			break;

		}

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("v_end_time", v_end_time);
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


