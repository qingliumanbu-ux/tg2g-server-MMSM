
/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-04-30
Description: 连铸初判与成品判定匹配查询报表
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(mmsmccmpp_inq)

int f_mmsmccmpp_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_start_time = "";//开始时刻
	CString v_end_time = ""; //结束时刻
	CString v_mat_no = "";
	CString v_heat_no = "";

	CPageInfo pageInfo;

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_temp;
	//CString sqlstr_count = "";

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
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		}


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT P.*, CASE WHEN (PM_JUDGMENT = CCM_JUDGMENT) THEN '1' ELSE '0' END AS IS_SAME "
				" FROM( "
				" SELECT * FROM "
				" (SELECT T.SLAB_NO, T.ST_NO, T.STRAND_NO, T.SLAB_CUT_TIME, T.CASTING_PRE_JUDGMENT AS PM_JUDGMENT, "
				" RE.CK_RESULT AS CCM_JUDGMENT,T.MAT_NO,T.HEAT_NO FROM VMMSM01 T "
				" LEFT JOIN TMMSM3F RE ON T.SLAB_NO = RE.SLAB_NO) "
				" WHERE PM_JUDGMENT != ' ' AND CCM_JUDGMENT != ' '"
				" )P "
				" WHERE 1 = 1";

			if (v_start_time != ""){
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
			if (v_mat_no != "")
			{
				sqlstr_temp += " AND MAT_NO = '" + v_mat_no + "'";
			}


			//sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY  SLAB_CUT_TIME";
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
