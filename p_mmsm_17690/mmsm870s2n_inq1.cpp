/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm870s2n_inq1)
 
int f_mmsm870s2n_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString mat_code = "";
	CString heat_no = "";
	CString bunker_no = " ";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm2a_yl("TMMSM2A_YL");

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

		Log::Info("", __FUNCTION__, "Tables =[{0}]", bcls_rec->Tables.get_Count());
		if (bcls_rec->Tables[2].Columns.Contains("L2_PROC_NO"))
			heat_no = bcls_rec->Tables[2].Rows[0]["L2_PROC_NO"].ToString().Trim();
		if (bcls_rec->Tables[2].Columns.Contains("STK_NO"))
			bunker_no = bcls_rec->Tables[2].Rows[0]["STK_NO"].ToString().Trim();


		Log::Info("", __FUNCTION__, "heat_no =[{0}],bunker_no =[{1}]", heat_no, bunker_no);
		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

		

		if (heat_no !="" )
		{
			sqlstr = "  SELECT * FROM  TMMSM2A_YL  WHERE 1=1   AND L2_PROC_NO = @L2_PROC_NO AND STK_NO = @BUNKER_NO  ORDER BY DEVO_TIME ASC ";
			Log::Trace("", "", "sqlstr1=[{0}]", sqlstr);
			
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("L2_PROC_NO", heat_no);
			cmd_inq.Parameters.Set("BUNKER_NO", bunker_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
			
		}

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
