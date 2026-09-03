/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-04-15
Description: 换砂轮倒渣斗记录报表查询
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(mmsmhsljl_inq)

int f_mmsmhsljl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString v_start_time = "";
	CString v_end_time = "";


	//系统的分页类信息。
	CPageInfo pageInfo;


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
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		}
		Log::Trace("", "", "v_start_time[{0}]", v_start_time);
		Log::Trace("", "", "v_end_time[{0}]", v_end_time);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM34 "
				"  WHERE 1=1 "
				;
			sqlstr = " select t.*,r.grade_name,r.grade_type"
				" from TMMSM34 t left join DA_GRADE_TYPE r"
				" on t.ST_NO = r.grade_id"
				" WHERE 1 = 1";
			
			if (v_start_time.Trim() != "")
			{
				sqlstr_temp += " AND REC_CREATE_TIME			>= @v_start_time";
			}
			if (v_end_time.Trim() != "")
			{
				sqlstr_temp += " AND REC_CREATE_TIME			<= @v_end_time";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY  REC_CREATE_TIME DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("v_end_time", v_end_time);



		cmd_inq.SetCommandText(sqlstr_count);
		//TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
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
