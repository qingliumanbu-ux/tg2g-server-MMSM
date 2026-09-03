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
BM2F_ENTERACE(mmsm82bd1_inq)

int f_mmsm82bd1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString delivy_end_time1 = "";


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm60("TMMSM60");

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
	/*	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		nowTime = nowTime.Substring(0, 4);*/

		tmmsm60["BUNKER_NO"] = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString().Trim();


		//--------------------------------
		//获取传入参数
		/*tmmsm60.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		delivy_end_time1 = bcls_rec->Tables[0].Rows[0]["DELIVY_END_TIME1"].ToString().Trim();*/

		/*	Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm60["BUNKER_NO"].ToString());
		Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm60["MAT_CODE"].ToString());*/

		//Log::Info("", __FUNCTION__, "BUNKER_NAME =[{0}]", tmmsm60["BUNKER_NAME"].ToString());
		//Log::Info("", __FUNCTION__, "BUNKER_TYPE =[{0}]", tmmsm60["BUNKER_TYPE"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr = " SELECT BUNKER_NO,BUNKER_NO_ORIGINAL,MAT_CODE,MAT_NAME,STOCK_WT,WEIGH_NO,SEQ_NO,UPLOAD_301 FROM TMMSM85 WHERE 1=1 and BUNKER_NO = @BUNKER_NO "
				;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);



		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm60["BUNKER_NO"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();



		Log::Info("", __FUNCTION__, "bcls_ret->Tables[0].Rows =[{0}]", bcls_ret->Tables[0].Rows.get_Count());
		//返回分页总数量信息 
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