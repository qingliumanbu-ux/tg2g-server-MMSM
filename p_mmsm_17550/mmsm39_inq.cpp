/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   孟凡杰
Version:    1.0
Date:     2024-01-04 9:13:56
Description: 废钢查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsm39_inq)

int f_mmsm39_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_mat_no = "";
	CString v_heat_no = "";
	CString v_grid_tab = "";	//Grid区分
	CString tableName = "";
	CString v_start_time = "";
	CString v_end_time = "";


	CModel tmmsm39("TMMSM39");
	CModel tmmsm01("TMMSM01");

	CDbCommand cmd_inq(conn);

	try
	{
		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "mat_no =[{0}]", v_mat_no);
	

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			if (bcls_rec->Tables[0].Columns.Contains("GRID_TAB"))
				v_grid_tab = bcls_rec->Tables[0].Rows[0]["GRID_TAB"].ToString();

			tableName = v_grid_tab.Trim();

			sqlstr = " SELECT * FROM " + tableName + " WHERE 1=1 ";

			if (tableName == "TMMSM01")
			{
				if (v_heat_no != "")
				{
					sqlstr += " AND  HEAT_NO	= @v_heat_no";
				}
				if (v_mat_no != "")
				{
					sqlstr += " AND  MAT_NO	= @v_mat_no";
				}
				if (v_start_time.Trim() != "")
				{
					sqlstr += " AND RECV_MAT_TIME		>= @v_start_time";
				}
				if (v_end_time.Trim() != "")
				{
					sqlstr += " AND RECV_MAT_TIME		<= @v_end_time";
				}

				//添加条件，不能查出已装车的坯子  mfj  20240309
				sqlstr += "  AND (C_STATESIGN = '0' OR C_STATESIGN =' ' OR C_STATESIGN ='6')";
			}
			
			if (tableName == "TMMSM39")
			{
				if (v_heat_no != "")
				{
					sqlstr += " AND  HEAT_NO	= @v_heat_no";
				}
				if (v_mat_no != "")
				{
					sqlstr += " AND  MAT_NO	= @v_mat_no";
				}
				if (v_start_time.Trim() != "")
				{
					sqlstr += " AND RECUT_DATE		>= @v_start_time";
				}
				if (v_end_time.Trim() != "")
				{
					sqlstr += " AND RECUT_DATE		<= @v_end_time";
				}

				//添加排序规则
				sqlstr += "  order by  RECUT_DATE desc";
			}
			
			
			break;
		}
		cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
		cmd_inq.Parameters.Set("v_mat_no", v_mat_no);
		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("v_end_time", v_end_time);
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);    
		cmd_inq.Close();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
