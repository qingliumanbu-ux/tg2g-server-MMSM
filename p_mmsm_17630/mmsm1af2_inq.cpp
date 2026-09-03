/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 实绩查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"



/* ***** 静态函数申明 ***** */


// service入口
BM2F_ENTERACE(mmsm1af2_inq)

int f_mmsm1af2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;

	CString ch_start_time_f = "";
	CString ch_start_time_t = "";


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm1a("TMMSM1A");

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
		tmmsm1a.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString();

		if (ch_start_time_f != "")
		{
			ch_start_time_f = ch_start_time_f.SubstringNE(0, 8) + "000000";
		}

		if (ch_start_time_t != "")
		{
			ch_start_time_t = ch_start_time_t.SubstringNE(0, 8) + "235959";
		}

		/* ***** 打印输入参数 ***** */


		CString  c_sql_where = "  WHERE  1 = 1 "; //查询条件。

		CString c_sql_condition = " SELECT *        FROM tmmsm1a  t";
		CString c_sql_condition2 = " SELECT COUNT(1) FROM tmmsm1a  t";
		CString c_sql_orderBY = " ORDER BY t.IRON_NO DESC";


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM tmmsm1a "
				"   WHERE 1 =1";
			sqlstr = " SELECT * "
				"   FROM tmmsm1a "
				"   WHERE 1 =1";

			
			if (tmmsm1a["IRON_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND IRON_NO = @tmmsm1a.IRON_NO";
			}

			if (tmmsm1a["IRON_LADLE_NO_01"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND IRON_LADLE_NO_01 = @tmmsm1a.IRON_LADLE_NO_01";
			}

		
			

			sqlstr_count = sqlstr_count + sqlstr_temp;

			sqlstr_temp += " ORDER BY IRON_NO DESC";

			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		cmd_inq.Parameters.Set("tmmsm1a.IRON_NO", tmmsm1a["IRON_NO"]);
		cmd_inq.Parameters.Set("tmmsm1a.IRON_LADLE_NO_01", tmmsm1a["IRON_LADLE_NO_01"]);
		cmd_inq.Parameters.Set("ch_start_time_f", ch_start_time_f);
		cmd_inq.Parameters.Set("ch_start_time_t", ch_start_time_t);


		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

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
