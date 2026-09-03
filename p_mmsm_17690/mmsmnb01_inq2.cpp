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
BM2F_ENTERACE(mmsmnb01_inq2)

int f_mmsmnb01_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString start_time = "";
	CString end_time = "";
	CDbCommand cmd_inq(conn);
	CModel tmmsmnb01("TMMSMNB01");
	//系统的分页类信息。
	CPageInfo pageInfo;
	try
	{

		//tmmsmnb01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsmnb01["MAT_CODE"] = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();	//物料代码
		tmmsmnb01["WEIGH_NO"] = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString().Trim();	//计量单号
		tmmsmnb01["TICODE"] = bcls_rec->Tables[0].Rows[0]["TICODE"].ToString().Trim();		//调拨单号
		start_time = bcls_rec->Tables[0].Rows[0]["RECEIVE_DATA_TIME_S"].ToString().Trim();	//开始时间
		end_time = bcls_rec->Tables[0].Rows[0]["RECEIVE_DATA_TIME_E"].ToString().Trim();	//结束时间
		/*tmmsmnb01["PROD_DATE"] = tmmsmnb01["PROD_DATE"].ToString().SubstringNE(0, 8);*/
		//Log::Trace("", __FUNCTION__, "PROD_DATE[{0}]  ", tmmsmnb01["PROD_DATE"].ToString());
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSMNB01 "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * FROM TMMSMNB01 WHERE 1=1 and STATUS1 = '1'and STATUS = '1' ";


			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;
		}
		if (tmmsmnb01["TICODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND TICODE		like '%'|| @TICODE||'%'";
		}
		if (tmmsmnb01["WEIGH_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND WEIGH_NO		like '%'|| @WEIGH_NO||'%'";
		}
		if (tmmsmnb01["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_CODE like '%'|| @MAT_CODE||'%'";
		}
		if (start_time != "")
		{
			sqlstr_temp += " AND REC_CREATE_TIME >= @START_TIME";
		}
		if (end_time != "")
		{
			sqlstr_temp += " AND REC_CREATE_TIME <= @END_TIME";
		}
		sqlstr_count = sqlstr_count + sqlstr_temp;
		//sqlstr_temp += " ORDER BY PROD_DATE DESC";
		sqlstr = sqlstr + sqlstr_temp;
		Log::Info("", __FUNCTION__, "sqlstr1 =[{0}]", sqlstr);
		cmd_inq.Parameters.Set("TICODE", tmmsmnb01["TICODE"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", tmmsmnb01["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("START_TIME", start_time);
		cmd_inq.Parameters.Set("END_TIME", end_time);
		cmd_inq.Parameters.Set("WEIGH_NO", tmmsmnb01["WEIGH_NO"].ToString());
		//cmd_inq.SetCommandText(sqlstr_count);
		//TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		////分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		////返回分页总数量信息 
		//bcls_ret->Tables.Add("PageInfo");
		//bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		//bcls_ret->Tables["PageInfo"].Rows.Add();
		//bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;
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
