/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料履历信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */
int f_t8f003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm58_inq)

int f_mmsm58_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_mat_type = "";
	CString v_mat_code = "";
	CString ch_start_time_f = "";
	CString ch_start_time_t = "";
	int		TotalRecordCount = 0;

	CString s_shift_group = " ", s_shift_no = " ", s_operate_time = " ";
	//系统的分页类信息。
	CPageInfo pageInfo;
	CModel tmmsm58("TMMSM58");
	CModel tmmsm89("TMMSM89");
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
		//--------------------------------
		//获取传入参数
		tmmsm58.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("SUPPLIER_ID"))
			v_mat_type = bcls_rec->Tables[0].Rows[0]["SUPPLIER_ID"].ToString();

		Log::Info("", __FUNCTION__, "SUPPLIER_ID =[{0}]", tmmsm58["SUPPLIER_ID"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM tmmsm58 "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM tmmsm58 "
				"  WHERE 1=1 "
				;
			if (tmmsm58["SUPPLIER_ID"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND SUPPLIER_ID	like '%'|| @SUPPLIER_ID||'%' ";
			}
			if (tmmsm58["SUPPLIER_SIMPLIFY_NAME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND SUPPLIER_SIMPLIFY_NAME	like '%'|| @SUPPLIER_SIMPLIFY_NAME||'%'";
			}
			if (tmmsm58["SUPPLIER_FULLNAME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND SUPPLIER_FULLNAME	like '%'|| @SUPPLIER_FULLNAME||'%'";
			}
			if (tmmsm58["SUPPLIER_NATION"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND SUPPLIER_NATION	 like '%'|| @SUPPLIER_NATION||'%'";
			}
			if (tmmsm58["SUPPLIER_PROVINCE"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND SUPPLIER_PROVINCE	like '%'|| @SUPPLIER_PROVINCE||'%'";
			}
			if (tmmsm58["SUPPLIER_CITY"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND SUPPLIER_CITY	like '%'|| @SUPPLIER_CITY||'%'";
			}
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY SUPPLIER_ID DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.Parameters.Set("SUPPLIER_ID", tmmsm58["SUPPLIER_ID"].ToString());
		cmd_inq.Parameters.Set("SUPPLIER_SIMPLIFY_NAME", tmmsm58["SUPPLIER_SIMPLIFY_NAME"].ToString());
		cmd_inq.Parameters.Set("SUPPLIER_FULLNAME", tmmsm58["SUPPLIER_FULLNAME"].ToString());
		cmd_inq.Parameters.Set("SUPPLIER_NATION", tmmsm58["SUPPLIER_NATION"].ToString());
		cmd_inq.Parameters.Set("SUPPLIER_PROVINCE", tmmsm58["SUPPLIER_PROVINCE"].ToString());
		cmd_inq.Parameters.Set("SUPPLIER_CITY", tmmsm58["SUPPLIER_CITY"].ToString());

		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		Log::Info("", __FUNCTION__, "TotalRecordCount =[{0}]", TotalRecordCount);
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
