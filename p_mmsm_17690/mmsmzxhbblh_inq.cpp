/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 获取消耗工序基表
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmzxhbblh_inq)

int f_mmsmzxhbblh_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int		TotalRecordCount = 0;
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_table_type = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CDbCommand cmd_inq(conn);
	CModel tmmsmzxhbb_lh("TMMSMZXHBB_LH");
	//系统的分页类信息。
	CPageInfo pageInfo;
	try
	{

		tmmsmzxhbb_lh.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsmzxhbb_lh["PROD_DATE"] = tmmsmzxhbb_lh["PROD_DATE"].ToString().SubstringNE(0, 8);
		Log::Trace("", __FUNCTION__, "PROD_DATE[{0}]  ", tmmsmzxhbb_lh["PROD_DATE"].ToString());
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSMZXHBB_LH "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * FROM TMMSMZXHBB_LH WHERE 1=1 ";


			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;
		}
		if (tmmsmzxhbb_lh["PROD_DATE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND PROD_DATE		like '%'|| @PROD_DATE||'%'";
		}
		if (tmmsmzxhbb_lh["START_TIME"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND START_TIME >= @START_TIME";
		}
		if (tmmsmzxhbb_lh["END_TIME"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND END_TIME <= @END_TIME";
		}
		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr_temp += " ORDER BY PROD_DATE DESC";
		sqlstr = sqlstr + sqlstr_temp;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.Parameters.Set("PROD_DATE", tmmsmzxhbb_lh["PROD_DATE"].ToString());
		cmd_inq.Parameters.Set("START_TIME", tmmsmzxhbb_lh["START_TIME"].ToString());
		cmd_inq.Parameters.Set("END_TIME", tmmsmzxhbb_lh["END_TIME"].ToString());
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
