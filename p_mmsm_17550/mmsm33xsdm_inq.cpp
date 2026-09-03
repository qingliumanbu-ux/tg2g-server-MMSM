/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-02-29
Description: 板坯称重系数断面查询
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(mmsm33xsdm_inq)

int f_mmsm33xsdm_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CDecimal v_width = 0;
	CDecimal v_maxwidth = 0;
	CDecimal v_minwidth = 0;

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm33xsdm("TMMSM33XSDM");

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
		tmmsm33xsdm.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Columns.Contains("WIDTH"))
			v_width = bcls_rec->Tables[0].Rows[0]["WIDTH"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("MAXWIDTH"))
			v_maxwidth = bcls_rec->Tables[0].Rows[0]["MAXWIDTH"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("MINWIDTH"))
			v_minwidth = bcls_rec->Tables[0].Rows[0]["MINWIDTH"].ToDecimal();
		Log::Info("", __FUNCTION__, "WIDTH =[{0}]", tmmsm33xsdm["WIDTH"].ToString());
		Log::Info("", __FUNCTION__, "MAXWIDTH =[{0}]", tmmsm33xsdm["MAXWIDTH"].ToString());
		Log::Info("", __FUNCTION__, "MINWIDTH =[{0}]", tmmsm33xsdm["MINWIDTH"].ToString());



		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM33XSDM "
				"  WHERE 1=1 "
				;

			sqlstr = " SELECT * "
				" FROM TMMSM33XSDM "
				" WHERE 1=1 "
				;

			if (tmmsm33xsdm["WIDTH"].ToDecimal()!= 0)
			{
				sqlstr_temp += " AND WIDTH	= @v_width";
			}
			if (tmmsm33xsdm["MAXWIDTH"].ToDecimal() != 0)
			{
				sqlstr_temp += " AND MAXWIDTH = @v_maxwidth";
			}
			if (tmmsm33xsdm["MINWIDTH"].ToDecimal() != 0)
			{
				sqlstr_temp += " AND MINWIDTH = @v_minwidth";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY  WIDTH";
			sqlstr = sqlstr + sqlstr_temp;
			break; 
		}


		cmd_inq.Parameters.Set("v_width", v_width);
		cmd_inq.Parameters.Set("v_maxwidth", v_maxwidth);
		cmd_inq.Parameters.Set("v_minwidth", v_minwidth);




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
