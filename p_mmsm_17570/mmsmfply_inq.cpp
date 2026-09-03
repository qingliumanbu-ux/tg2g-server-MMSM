/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   孟凡杰
Version:    1.0
Date:     2024-01-04 9:13:56
Description: 废钢利用增值品查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsmfply_inq)

int f_mmsmfply_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_heat_no = "";
	CString v_start_time = "";
	CString v_end_time = "";

	//CModel tmmsmfp("TMMSMFP");

	CDbCommand cmd_inq(conn);

	try
	{
		//--------------------------------
		//获取传入参数
		//if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		//v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		//v_heat_no_num = CDecimal::Parse(v_heat_no.Substring(3, 5))+1;
		//Log::Info("", __FUNCTION__, "v_heat_no_num =[{0}]", v_heat_no_num);
		
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


				sqlstr = " SELECT T1.*,T2.SG_GRADE_1 ,T3.CODE_DESC_1_CONTENT AS STEEL_TYPE FROM TMMSMFPLY T1 LEFT JOIN TQMTS0X T2 ON T1.ST_NO = T2.ST_NO "
					" LEFT JOIN(SELECT * FROM TWMSMZD02 WHERE CODE_CLASS = 'STEEL_TYPE') T3 ON T1.ST_NO = T3.CODE WHERE 1=1 ";
				if (v_heat_no != "")
				{
					sqlstr += " AND  T1.HEAT_NO	> '" + v_heat_no + "'";
				}
				if (v_start_time != "")
				{
					sqlstr += " AND  T1.REC_CREATE_TIME	>= '" + v_start_time + "'";
				}
				if (v_end_time != "")
				{
					sqlstr += " AND  T1.REC_CREATE_TIME	<= '" + v_end_time + "'";
				}
				
			sqlstr += " ORDER BY  T1.REC_CREATE_TIME";
			break;
		}
		cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
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