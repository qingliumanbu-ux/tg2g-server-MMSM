/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2024-01-10 17:13:56
Description: 板坯物料改判信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsmgp_inq)

int f_mmsmgp_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CModel tmmsm01("TMMSM01");

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
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM01 "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM TMMSM01 "
				"  WHERE  INITIAL_ST_NO != ST_NO "
				"  AND PROD_TIME			>= @v_start_time"
				"  AND PROD_TIME			<= @v_end_time"
				;


			//if (tmmsm01["ST_NO"].ToString().Trim() != "")
			//{
			//	sqlstr_temp += " AND CC_MACH_NO	= @tmmsm33czxs.CC_MACH_NO";
			//}
			//if (tmmsm01["STRAND_NO"].ToString().Trim() != "")
			//{
			//	sqlstr_temp += " AND STRAND_NO	= @tmmsm33czxs.STRAND_NO";
			//}
			

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY MAT_NO";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		//cmd_inq.Parameters.Set("tmmsm01.ST_NO", tmmsm01["ST_NO"].ToString());
		//cmd_inq.Parameters.Set("tmmsm01.INITIAL_ST_NO", tmmsm01["INITIAL_ST_NO"].ToString());
		cmd_inq.Parameters.Set("v_start_time", v_start_time);
		cmd_inq.Parameters.Set("v_end_time", v_end_time);


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
