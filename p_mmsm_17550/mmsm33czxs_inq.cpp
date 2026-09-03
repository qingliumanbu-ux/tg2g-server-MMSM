/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2024-01-09 17:13:56
Description: 板坯修正系数信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsm33czxs_inq)

int f_mmsm33czxs_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_strand_no = "";
	CString v_st_no = "";
	int		TotalRecordCount = 0;



	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm33czxs("TMMSM33CZXS");

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
		tmmsm33czxs.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Columns.Contains("STRAND_NO"))
			v_strand_no = bcls_rec->Tables[0].Rows[0]["STRAND_NO"].ToString().ToUpper();
		Log::Trace("", "", "strand_no[{0}]", v_strand_no);

		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		Log::Trace("", "", "st_no[{0}]", v_st_no);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM33CZXS "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM TMMSM33CZXS " 
				"  WHERE 1=1 "
				;

			/*if (tmmsm33czxs["CC_MACH_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND CC_MACH_NO	= @v_cc_mach_no";
			}*/
			if (tmmsm33czxs["STRAND_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND STRAND_NO	= @v_strand_no";
			}
			if (tmmsm33czxs["ST_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND ST_NO	= @v_st_no";
			}



			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY  ST_NO";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		//cmd_inq.Parameters.Set("tmmsm33czxs.CC_MACH_NO", tmmsm33czxs["CC_MACH_NO"].ToString());
		cmd_inq.Parameters.Set("v_strand_no", v_strand_no);
		cmd_inq.Parameters.Set("v_st_no", v_st_no);


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
