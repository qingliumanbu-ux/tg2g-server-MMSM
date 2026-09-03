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
BM2F_ENTERACE(mmsm868s2n_inq)

int f_mmsm868s2n_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm85("TMMSM85");

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
		if (bcls_rec->Tables.get_Count()>0)
		{
			if (bcls_rec->Tables[0].Columns.Contains("BUNKER_NO"))
				mat_code = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString().Trim();
		}


		/*tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm85["BUNKER_NO"].ToString());
		Log::Info("", __FUNCTION__, "MAT_NAME =[{0}]", tmmsm85["MAT_NAME"].ToString());
		Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm85["MAT_CODE"].ToString());*/

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

		
			sqlstr = " SELECT * "
				"   FROM TMMSM60 "
				"  WHERE 1=1  AND BUNKER_NO	in ('WIRE') OR BUNKER_TYPE  = 'M' "
				;

			break;
		}

		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();

		if (mat_code !="" )
		{
			sqlstr = "  SELECT * FROM  TMMSM85  WHERE 1=1   AND BUNKER_NO = @BUNKER_NO   ORDER BY SEQ_NO ASC ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("BUNKER_NO", mat_code);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
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
