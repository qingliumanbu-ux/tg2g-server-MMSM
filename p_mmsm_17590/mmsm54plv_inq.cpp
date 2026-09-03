/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:   1.0
Date:     2016-01-29 17:13:56
Description: 原辅料计划管理
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsm54plv_inq)

int f_mmsm54plv_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_mat_kind = "";
	int		TotalRecordCount = 0;
	CString v_start_time = "";//开始时刻
	CString v_end_time = "";//结束时刻
	CString v_table_type = "";//画面名
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm54("TMMSM54");

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

		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();

		Log::Info("", __FUNCTION__, "START_TIME =[{0}]  END_TIME =[{1}]", v_start_time.Trim(), v_end_time.Trim());


		if (v_table_type.Trim() == "MMSM54F1PLV")
		{
			
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				//sql条件
				if (v_start_time.Trim() != "")//因CC_REQ_TIME产品化数据库中无数据，暂用plan_date
				{
					sqlstr_temp += " AND   PLAN_DATE >= '" + v_start_time.Substring(0,8).Trim() + "' ";
				}
				if (v_end_time.Trim() != "")
				{
					sqlstr_temp += " AND PLAN_DATE <= '" + v_end_time.Substring(0,8).Trim() + "' ";
				}

				sqlstr = "SELECT t.CAST_LOT_NO,MAX(t.CC_MACH_NO) CC_MACH_NO,MAX(t.st_no) st_no,MAX(T.CAST_LOT_SUM)CAST_LOT_SUM,  "
					" sum(t.PLAN_TAP_WT) PLAN_TAP_WT, MAX(t.PLAN_DATE)PLAN_DATE, MAX(t.sg_sign) sg_sign "
					" FROM TPSSM01 t WHERE t.CAST_LOT_NO <> ' ' " + sqlstr_temp + " GROUP BY t.CAST_LOT_NO ";
				
				break;
			}

			Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
			cmd_inq.Close();

		}
		else if (v_table_type.Trim() == "MMSM54F2PLV")
		{
			

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				//sql条件
				
				sqlstr_temp += " AND   PLAN_DATE >= '" + CDateTime::Now().AddDays(-10).ToString("yyyyMMddHHmmss").SubstringNE(0,8) + "' ";
				
				sqlstr_temp += " AND PLAN_DATE <= '" + CDateTime::Now().AddDays(10).ToString("yyyyMMddHHmmss").SubstringNE(0, 8) + "' ";
				

				sqlstr = "SELECT sum(t.PLAN_TAP_WT) PLAN_TAP_WT,t.PLAN_DATE ,count(1) CAST_LOT_SUM "
					" FROM TPSSM01 t WHERE t.CAST_LOT_NO <> ' '  " + sqlstr_temp + " GROUP BY t.PLAN_DATE ";

				break;
			}

			Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
			cmd_inq.Close();
		}




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
