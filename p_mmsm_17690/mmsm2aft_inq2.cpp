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
BM2F_ENTERACE(mmsm2aft_inq2)

int f_mmsm2aft_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString start_time = "";
	CString end_time = "";
	CString yue_date = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm2a("TMMSM2A_SEND");

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

		tmmsm2a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		//--------------------------------
		//获取传入参数
		
		start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();	//开始时间
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();	//结束时间
		yue_date = bcls_rec->Tables[0].Rows[0]["STAR_TIME"].ToString().SubstringNE(0, 6);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " select * from TMMSM2A_SEND  t where 1=1 and t.send_flag = '1' and t.rtn_flag != '1' AND T.HANDLE_DIV = 'F'";
			Log::Info("", __FUNCTION__, "yue =[{0}]", yue_date);
			if (yue_date != "")
			{
				sqlstr += " and STAT_DATE = @STAR_TIME";
			}
			if (start_time != "")
			{
				sqlstr += " AND REC_CREATE_TIME >= @START_TIME";
			}
			if (end_time != "")
			{
				sqlstr += " AND REC_CREATE_TIME <= @END_TIME";
			}
			if (tmmsm2a["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr += " and MAT_CODE = @MAT_CODE";
			}

			sqlstr = sqlstr + sqlstr_temp;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("STAR_TIME", yue_date);
		cmd_inq.Parameters.Set("START_TIME", start_time);
		cmd_inq.Parameters.Set("END_TIME", end_time);
		cmd_inq.Parameters.Set("MAT_CODE", tmmsm2a["MAT_CODE"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();


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
