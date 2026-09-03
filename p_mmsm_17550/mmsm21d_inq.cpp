/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 炼钢转炉作业铸余实绩查询
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

// service入口
BM2F_ENTERACE(mmsm21d_inq)

int f_mmsm21d_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int TotalRecordCount = 0  ;

	CString steel_mix_remain_time_f = "";
	CString steel_mix_remain_time_t = "";
	CString mix_remain_no = "";
	CString heat_no = "";
	CString prod_shift_no = "";
	CString prod_shift_group = "";
	//系统的分页类信息。
	CPageInfo pageInfo; 
	CDbCommand cmd_inq(conn);

	try
	{
		try
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch(CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize   = 1000;
		}
		//获取传入参数
		if(bcls_rec->Tables[0].Columns.Contains("STEEL_MIX_REMAIN_TIME_F"))
			steel_mix_remain_time_f = bcls_rec->Tables[0].Rows[0]["STEEL_MIX_REMAIN_TIME_F"].ToString().Trim();
		if(bcls_rec->Tables[0].Columns.Contains("STEEL_MIX_REMAIN_TIME_T"))
			steel_mix_remain_time_t = bcls_rec->Tables[0].Rows[0]["STEEL_MIX_REMAIN_TIME_T"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MIX_REMAIN_NO"))
			mix_remain_no = bcls_rec->Tables[0].Rows[0]["MIX_REMAIN_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_NO"))
			prod_shift_no = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_GROUP"))
			prod_shift_group = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_GROUP"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		//Log::Info("", __FUNCTION__, "steel_mix_remain_time_f  =[{0}]", steel_mix_remain_time_f);
		//Log::Info("", __FUNCTION__, "steel_mix_remain_time_t  =[{0}]", steel_mix_remain_time_t);
	
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TMMSM21D "
					"  WHERE 1=1 "
					;
				sqlstr	 = " SELECT * "
					"   FROM TMMSM21D "
					"  WHERE 1=1 "
					;
				if (mix_remain_no != "")
				{
					sqlstr_temp += " AND MIX_REMAIN_NO LIKE '%" + mix_remain_no + "%'";
				}
				if (heat_no != "")
				{
					sqlstr_temp += " AND HEAT_NO LIKE '%" + heat_no + "%'";
				}
				if (mix_remain_no != "")
				{
					sqlstr_temp += " AND MIX_REMAIN_NO = '" + mix_remain_no + "'";
				}
				if (prod_shift_no != "")
				{
					sqlstr_temp += " AND PROD_SHIFT_NO = '" + prod_shift_no + "'";
				}
				if (prod_shift_group != "")
				{
					sqlstr_temp += " AND PROD_SHIFT_GROUP = '" + prod_shift_group + "'";
				}
				if(steel_mix_remain_time_f != "")
				{
					sqlstr_temp += " AND STEEL_MIX_REMAIN_TIME >= '" + steel_mix_remain_time_f + "'";
				}
				if(steel_mix_remain_time_t != "")
				{
					sqlstr_temp += " AND STEEL_MIX_REMAIN_TIME <= '" + steel_mix_remain_time_t + "'";
				}

				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY STEEL_MIX_REMAIN_TIME DESC";
				sqlstr = sqlstr + sqlstr_temp;
				break;
		}

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32(); 
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],pageInfo.RecordFrom,pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL,"TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;	

	 }
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
