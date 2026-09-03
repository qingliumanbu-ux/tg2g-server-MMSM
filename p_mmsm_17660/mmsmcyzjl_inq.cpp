/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-04-18
Description: 成品储运站记录报表查询
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsmcyzjl_inq)

int f_mmsmcyzjl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_start_time = "";//开始时刻
	CString v_end_time = ""; //结束时刻
	CString v_heat_no = "";
	CString v_load_up_shift_group = "";//装车班组
	CString v_emp_name = "";//装车人
	CString v_unload_code = "";//卸点

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
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim().ToUpper();
		}
		if (bcls_rec->Tables[0].Columns.Contains("LOAD_UP_SHIFT_GROUP"))
		{
			v_load_up_shift_group = bcls_rec->Tables[0].Rows[0]["LOAD_UP_SHIFT_GROUP"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("EMP_NAME"))
		{
			v_emp_name = bcls_rec->Tables[0].Rows[0]["EMP_NAME"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("UNLOAD_CODE"))
		{
			v_unload_code = bcls_rec->Tables[0].Rows[0]["UNLOAD_CODE"].ToString().Trim();
		}


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select a.PROD_TIME,a.PROD_SHIFT_GROUP,a.HEAT_NO,a.MAT_NO,a.ST_NO,a.MAT_LEN,a.MAT_WIDTH,a.MAT_THICK,a.MAT_WT, "
				" decode(b.SHIFT_GROUP, null, c.shift_group, b.SHIFT_GROUP)          LOAD_UP_SHIFT_GROUP,decode(b.OUT_STOCK_TIME, null, c.OUT_STOCK_TIME, b.OUT_STOCK_TIME) LOAD_UP_TIME, "
				" CASE WHEN substr(b.OUT_STOCK_TIME,9,4)>='0800' AND substr(b.OUT_STOCK_TIME,9,4)<='2000' THEN '白班'"
				" ELSE '夜班' END SHIFT_NO, "
				" a.HAND_OVER_GROUP AS OUT_STOCK_SHIFT_GROUP, c.LOAD_END_TIME AS OUT_STOCK_TIME, DECODE(TRAN_END_TIME,' ',TRAN_TIME,TRAN_END_TIME)TRAN_TIME ,B.OPERATOR EMP_NAME,"
				" a.LGORT,  decode(b.UNLOAD_CODE, null, c.UNLOAD_CODE, b.UNLOAD_CODE)          UNLOAD_CODE, "
				" decode(b.TRUCK_NO, null, c.TRUCK_NO, b.TRUCK_NO)                   TRUCK_NO, D.CODE_DESC_1_CONTENT, b.OPERATOR, GUIDE_DEST, STOCK_L2, C.REMARK, a.UNIT_CODE,a.c_div"
				" from vmmsm01 a"
				" left join vwmsm12 b on a.LOAD_SCHEME_NO = b.LOAD_SCHEME_NO and a.MAT_NO = b.MAT_NO"
				" left join twmsm61 c on a.PRACTICE_NO = c.PRACTICE_NO and a.MAT_NO = c.MAT_NO"
				" left join twmsmzd02 d on d.CODE = b.TRUCK_NO and d.code_class='WM01' "
				" where 1=1 AND A.LOAD_SCHEME_NO!=' ' ";
			 
			if (v_start_time != "")
			{
				sqlstr_temp += " AND b.out_stock_time >= '" + v_start_time + "'";
			}
			if (v_end_time != "")
			{
				sqlstr_temp += " AND b.out_stock_time <= '" + v_end_time + "'";
			}
			if (v_heat_no != "")
			{
				sqlstr += " AND a.HEAT_NO = '" + v_heat_no + "'";
			}
			if (v_load_up_shift_group != "")
			{
				sqlstr += " AND b.SHIFT_GROUP = '" + v_load_up_shift_group + "'";
			}
			if (v_emp_name != "")
			{
				sqlstr += " AND OPERATOR = '" + v_emp_name + "'";
			}
			if (v_unload_code != "")
			{
				sqlstr += " AND b.UNLOAD_CODE = '" + v_unload_code + "'";
			}
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY HEAT_NO ";
			sqlstr = sqlstr + sqlstr_temp;
			break;

		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("v_end_time", v_end_time);
		cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
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


