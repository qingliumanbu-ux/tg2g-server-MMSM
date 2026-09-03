/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-28
Version:1.0
Description: 精整相关信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 可供精整炼钢板坯信息查询
/// <para>
/// 查询可供精整炼钢板坯信息
/// </para>
/// </summary>
/// <param name="MAT_NO">材料号</param>
/// <param name="PONO">制造命令号</param>
/// <param name="HSF_END_TIME_F">精整结束开始时刻</param>
/// <param name="HSF_END_TIME_T">精整结束结束时刻</param>
/// <param name="MACH_CLEAR_FLAG">精整标记</param>
/// <returns>板坯信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm33tgjypop_inq)


int f_mmsm33tgjypop_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int rowCount = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString heat_no = "";
	CString mat_no = "";
	CString print_no = "";
	int		TotalRecordCount = 0;

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


		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PRINT_NO"))
			print_no = bcls_rec->Tables[0].Rows[0]["PRINT_NO"].ToString();

		//Log::Trace("", "", "ch_hsf_end_time_f={0}", ch_hsf_end_time_f);
		//Log::Trace("", "", "ch_hsf_end_time_t={0}", ch_hsf_end_time_t);
		//Log::Trace("", "", "tableName={0}", tableName);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr_count = " SELECT COUNT(1) "
				" FROM TMMSM01 "
				" WHERE 1=1 ";
			sqlstr = " SELECT * "
				" FROM TMMSM01 "
				" WHERE 1=1 ";


			if (heat_no.Trim() != "")
			{
				sqlstr_temp += " AND HEAT_NO = @heat_no";
			}
			if (mat_no.Trim() != "")
			{
					sqlstr_temp += " AND MAT_NO = @mat_no";
			}
			if (print_no.Trim() != "")
			{
				sqlstr_temp += " AND PRINT_NO = @print_no";
			}
			
				sqlstr_temp += " AND MAT_LINE_TYPE = 'SM'  AND LOGISTICS_STATUS IN('0','1','4')  AND (C_STATESIGN = '0' OR C_STATESIGN =' ' OR C_STATESIGN ='6')";//AND FIX_SLAB_NUM >=2 
				//AND IN_FLAG = '1' AND ORDER_NO =' ' AND HOLD_FLAG='2'



			sqlstr_count = sqlstr_count + sqlstr_temp;

			
				sqlstr_temp += " ORDER BY MAT_NO ";
			


			sqlstr = sqlstr + sqlstr_temp;

			Log::Trace("", "", "sqlstr={0}", sqlstr);
			break;
		}
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.Parameters.Set("mat_no", mat_no);
		cmd_inq.Parameters.Set("print_no", print_no);

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
