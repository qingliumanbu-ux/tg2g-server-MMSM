/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-13 17:13:56
Description: 鱼雷罐铁水包跟踪信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsm12c_inq)

int f_mmsm12c_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;

	CString ch_start_time_f = "";
	CString ch_start_time_t = "";


	//系统的分页类信息。
	CPageInfo pageInfo;

	CDbCommand cmd_inq(conn);

	CModel tmmsm12c("TMMSM12C");

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
		tmmsm12c.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//开始时刻
		if (bcls_rec->Tables[0].Columns.Contains("TIME_S"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["TIME_S"].ToString();
		//结束时刻
		if (bcls_rec->Tables[0].Columns.Contains("TIME_E"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["TIME_E"].ToString();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = "SELECT COUNT(1) FROM TMMSM12C WHERE 1=1 ";
			sqlstr = "SELECT * FROM TMMSM12C WHERE 1=1 ";

			//罐类型
			if (tmmsm12c["MOLD_TYPE"].ToString().Trim() != "")
			{
				sqlstr_temp += "AND MOLD_TYPE LIKE '%' || @tmmsm12c.MOLD_TYPE || '%' ";
			}

			//处理号
			if (tmmsm12c["ACTION_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += "AND ACTION_NO LIKE '%' || @tmmsm12c.ACTION_NO || '%' ";
			}

			//工位
			if (tmmsm12c["DEV_CODE"].ToString().Trim() != "")
			{
				sqlstr_temp += "AND DEV_CODE LIKE '%' || @tmmsm12c.DEV_CODE || '%' ";
			}

			//开始时刻
			if (ch_start_time_f.Trim() != "")
			{
				sqlstr_temp += "AND TIME_STAMPS >= @ch_start_time_f ";
			}
			//结束时刻
			if (ch_start_time_t.Trim() != "")
			{
				sqlstr_temp += "AND TIME_STAMPS <= @ch_start_time_t ";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += "ORDER BY  DEV_CODE";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		
		cmd_inq.Parameters.Set("tmmsm12c.ACTION_NO", tmmsm12c["ACTION_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm12c.MOLD_TYPE", tmmsm12c["MOLD_TYPE"].ToString());
		cmd_inq.Parameters.Set("tmmsm12c.DEV_CODE", tmmsm12c["DEV_CODE"].ToString());
		cmd_inq.Parameters.Set("ch_start_time_f", ch_start_time_f);
		cmd_inq.Parameters.Set("ch_start_time_t", ch_start_time_t);


		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();

		Log::Trace("", __FUNCTION__, "sqlstr = {0}", sqlstr);
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
