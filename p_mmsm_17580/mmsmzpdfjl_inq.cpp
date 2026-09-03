/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-04-11
Description: 不锈钢无修磨检验记录报表查询
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明
//int f_mmsm36f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsmzpdfjl_inq)

int f_mmsmzpdfjl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_start_time = "";//开始时刻
	CString v_end_time = ""; //结束时刻
	CString v_heat_no = "";
	CString v_mat_no = "";

	CPageInfo pageInfo;

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		/*if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("QUERY_DIV"))
		{
		qy_table = bcls_rec->Tables[0].Rows[0]["QUERY_DIV"].ToString();
		Log::Trace("", __FUNCTION__, "qy_table[{0}]  ", qy_table);
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


			sqlstr = " select t.*,T.WATER_EXPLOSION_MIDDLE_DEPRESSION AS MIDDLE_DEPRESSION ,T1.slab_cut_time,T2.CAST_DIV_NO_1,r.grade_name,r.grade_type "
				" from TMMSM36 t "
				" left join TMMSM33 T1 "
				" on t.mat_no = T1.mat_no "
				" left join TMMSMGY05 T2 "
				" ON T.HEAT_NO = T2.HEAT_NO "
				" left join DA_GRADE_TYPE r "
				" on t.ST_NO = r.grade_id "
				" WHERE 1 = 1 ";
				//sqlstr = "SELECT *  FROM TMMSM36  WHERE 1 = 1 ";

			if (v_start_time != ""){
				sqlstr_temp += " AND T.REC_CREATE_TIME >= '" + v_start_time + "'";
			}
			if (v_end_time != "")
			{
				sqlstr_temp += " AND T.REC_CREATE_TIME <=  '" + v_end_time + "'";
			}

			sqlstr_temp += " ORDER BY  T.REC_CREATE_TIME ";
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


