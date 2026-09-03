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
BM2F_ENTERACE(mmsm85_bunker_inqnbk)

int f_mmsm85_bunker_inqnbk(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString mat_code = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString s_bunker_no = "";
	int i_idx = 0;
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
		tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		s_bunker_no = tmmsm85["BUNKER_NO"].ToString().Trim();
		mat_code = tmmsm85["MAT_CODE"].ToString().Trim();
		if (s_bunker_no.GetLength()>0)
		{
			i_idx = s_bunker_no.Find('-');
			if (i_idx>0)
			{
				s_bunker_no = s_bunker_no.Substring(0, i_idx);
				tmmsm85["BUNKER_NO"] = s_bunker_no;

			}
		}

		if (mat_code.GetLength()>0)
		{
			int n_idx = mat_code.Find('_');
			if (n_idx>0)
			{
				mat_code = mat_code.Substring(0, n_idx);
				tmmsm85["MAT_CODE"] = mat_code;

			}
		}
		Log::Info("", __FUNCTION__, "i_idx =[{0}],s_bunker_no =[{1}],MAT_CODE =[{2}]", i_idx, s_bunker_no, tmmsm85["MAT_CODE"].ToString());

		//Log::Info("", __FUNCTION__, "BUNKER_TYPE =[{0}]", tmmsm85["BUNKER_TYPE"].ToString());
		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm85["BUNKER_NO"].ToString());
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM60 "
				"  WHERE 1=1 "
				;
			sqlstr = "  SELECT DISTINCT  MAT_CODE ||'_'|| MAT_NAME MAT_CODE  FROM  TMMSM85    "
				"  WHERE 1=1  "
				;
			/*if (tmmsm85["BUNKER_TYPE"].ToString().Trim() != "")
			{
				if (tmmsm85["BUNKER_TYPE"].ToString().Trim() == "all")
				{
					sqlstr_temp += " AND 1 = 1";
				}
				else
				{
					sqlstr_temp += " AND BUNKER_TYPE			= @tmmsm85.BUNKER_TYPE";

				}
			}
			if (tmmsm85["BUNKER_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND BUNKER_NO			= @tmmsm85.BUNKER_NO";
			}*/
			if (tmmsm85["BUNKER_NO"].ToString().Trim() != "")
			{
				if (tmmsm85["BUNKER_NO"].ToString().Trim() == "all" )
				{
					sqlstr_temp += "  AND BUNKER_NO IN (SELECT BUNKER_NO FROM TMMSM60 WHERE BUNKER_TYPE = 'NICKEL' )  ";
				}
				else
				{
					sqlstr_temp += " AND BUNKER_NO			= @tmmsm85.BUNKER_NO";
				}
			}
			/*	sqlstr_temp += " AND MAT_CODE			=  'AT000385'";
			sqlstr_temp += " order by MAT_CODE ";*/
			/*sqlstr_temp = sqlstr_temp + " order by  BUNKER_NO";*/
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		/*cmd_inq.Parameters.Set("tmmsm85.BUNKER_TYPE", tmmsm85["BUNKER_TYPE"].ToString());
		cmd_inq.Parameters.Set("tmmsm85.BUNKER_NO", tmmsm85["BUNKER_NO"].ToString());*/
		cmd_inq.Parameters.Set("tmmsm85.BUNKER_NO", tmmsm85["BUNKER_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr_count);
		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		
		bcls_ret->Tables.Add();

		Log::Info("", __FUNCTION__, "count =[{0}]", bcls_ret->Tables[0].Rows.get_Count());
		if (tmmsm85["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr = "  SELECT DISTINCT  QUALITY_BATCH_NO  FROM  TMMSM85    "
				"  WHERE 1=1  "
				;
			if (tmmsm85["BUNKER_NO"].ToString().Trim() != "")
			{
				if (tmmsm85["BUNKER_NO"].ToString().Trim() == "all")
				{
					sqlstr += "  AND BUNKER_NO IN (SELECT BUNKER_NO FROM TMMSM60 WHERE BUNKER_TYPE = 'NICKEL' )  ";
				}
				else if (tmmsm85["BUNKER_NO"].ToString().Trim() == "-")
				{

				}
				else
				{
					sqlstr += " AND BUNKER_NO			= @tmmsm85.BUNKER_NO";
				}
			}

			sqlstr += " AND MAT_CODE			= @tmmsm85.MAT_CODE";
			

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tmmsm85.MAT_CODE", tmmsm85["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("tmmsm85.BUNKER_NO", tmmsm85["BUNKER_NO"].ToString());
			Log::Info("", __FUNCTION__, "sqlstr1 =[{0}]", sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
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
