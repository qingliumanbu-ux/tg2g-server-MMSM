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
BM2F_ENTERACE(mmsm60_inq)

int f_mmsm60_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;


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


		//--------------------------------
		//获取传入参数
		tmmsm60.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm60["BUNKER_NO"].ToString());
		Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm60["MAT_CODE"].ToString());
		Log::Info("", __FUNCTION__, "BUNKER_TYPE =[{0}]", tmmsm60["BUNKER_TYPE"].ToString());
		Log::Info("", __FUNCTION__, "BACK_C4 =[{0}]", tmmsm60["BACK_C4"].ToString());
		//Log::Info("", __FUNCTION__, "BUNKER_NAME =[{0}]", tmmsm60["BUNKER_NAME"].ToString());
		//Log::Info("", __FUNCTION__, "BUNKER_TYPE =[{0}]", tmmsm60["BUNKER_TYPE"].ToString());

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM tmmsm60 "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM tmmsm60 "
				"  WHERE 1=1 "
				;
			//if (tmmsm60["MAT_CODE"].ToString().Trim() != "")
			//{
			//	//sqlstr = " SELECT decode(t.MAT_CODE, '" + tmmsm60["MAT_CODE"].ToString().Replace("	", "").Trim() + "', 0, 1)  AS  FLAG, t.*FROM tmmsm60 t WHERE 1 = 1 ";
			//	sqlstr_temp += " AND MAT_CODE			like '%'|| @tmmsm60.MAT_CODE||'%'";

			//}
			if (tmmsm60["BUNKER_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND BUNKER_NO		like '%'|| @tmmsm60.BUNKER_NO||'%'";
			}
			if (tmmsm60["BUNKER_NAME"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND BUNKER_NAME	like '%'|| @tmmsm60.BUNKER_NAME||'%'";
			}
			if (tmmsm60["BACK_C4"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND BACK_C4	like '%'|| @tmmsm60.BACK_C4||'%'";
			}
			if (tmmsm60["BUNKER_TYPE"].ToString().Trim() != "")
			{
				if (tmmsm60["BUNKER_TYPE"].ToString().Trim() == "A")
				{
					sqlstr_temp += " AND BUNKER_TYPE  in ('0','2') ";
					count = 1;
				}
				else
				{
					if (tmmsm60["BUNKER_TYPE"].ToString().Trim()=="SCRAPALLOY")
					{
						sqlstr_temp += " AND BUNKER_NO	NOT IN('E19','E20','E21','E22','F19','F20','F21','F22')";
					}
					else
					{
						sqlstr_temp += " AND BUNKER_TYPE		= @tmmsm60.BUNKER_TYPE";
					}
					
				}

			}
			if (tmmsm60["BUNKER_TYPE"].ToString().Trim() != "")
			{
				if (count == 0)
				{
					sqlstr_temp += " AND BUNKER_TYPE		= @tmmsm60.BUNKER_TYPE";
				}
			}

			if (tmmsm60["STK_NO"].ToString().Trim() != "")
			{
				if (count == 0)
				{
					sqlstr_temp += " AND STK_NO		= @tmmsm60.STK_NO";
				}
			}
			
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY BACK_N_1 ";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		
		cmd_inq.Parameters.Set("tmmsm60.MAT_CODE", tmmsm60["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("tmmsm60.BUNKER_NO", tmmsm60["BUNKER_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm60.BUNKER_NAME", tmmsm60["BUNKER_NAME"].ToString());
		cmd_inq.Parameters.Set("tmmsm60.BUNKER_TYPE", tmmsm60["BUNKER_TYPE"].ToString());
		cmd_inq.Parameters.Set("tmmsm60.BACK_C4", tmmsm60["BACK_C4"].ToString());
		cmd_inq.Parameters.Set("tmmsm60.STK_NO", tmmsm60["STK_NO"].ToString());

		//cmd_inq.SetCommandText(sqlstr_count);
		//TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		
		if ( !bcls_ret->Tables[0].Columns.Contains("FLAG"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"FLAG");
		}

		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			if (tmmsm60["MAT_CODE"].ToString().Trim()!="")
			{
				if (tmmsm60["MAT_CODE"].ToString().Trim() == bcls_ret->Tables[0].Rows[i]["MAT_CODE"].ToString().Trim())
				{
					bcls_ret->Tables[0].Rows[i]["FLAG"] = "1";
				}
				else
				{
					bcls_ret->Tables[0].Rows[i]["FLAG"] = "0";
				}
			}
		}
		
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