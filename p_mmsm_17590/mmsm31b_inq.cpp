/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 炼钢转炉作业其它实绩查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsm31b_inq)

int f_mmsm31b_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int TotalRecordCount = 0;

	CString prod_time_f = "";
	CString prod_time_t = "";
	CString station_no = "";
	CString heat_no = "";
	CString pono = "";
	CString cast_no = "";
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);

	try
	{
		try
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_F"))
			prod_time_f = bcls_rec->Tables[0].Rows[0]["PROD_TIME_F"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_T"))
			prod_time_t = bcls_rec->Tables[0].Rows[0]["PROD_TIME_T"].ToString().Trim();
		
		if (bcls_rec->Tables[0].Columns.Contains("STATION_NO"))
			station_no = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString().Trim();

		if (station_no == "")
		{
			strcpy(s.msg, "连铸机号" + station_no + "不能为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/* ***** 打印输入参数 ***** */
		//Log::Info("", __FUNCTION__, "prod_time_f  =[{0}]", prod_time_f);
		//Log::Info("", __FUNCTION__, "prod_time_t  =[{0}]", prod_time_t);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM31B "
				"  WHERE 1=1 "
				;
			sqlstr = " SELECT * "
				"   FROM TMMSM31B "
				"  WHERE 1=1 "
				;
			
			if (station_no != "")
			{
				sqlstr_temp += " AND STATION_NO = '" + station_no + "'";
			}
			if (prod_time_f != "")
			{
				sqlstr_temp += " AND LADLE_ARRIVE_TIME >= '" + prod_time_f + "'";
			}
			if (prod_time_t != "")
			{
				sqlstr_temp += " AND LADLE_ARRIVE_TIME <= '" + prod_time_t + "'";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY STATION_NO, LADLE_ARRIVE_TIME DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

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

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			
			sqlstr = " SELECT A.*,B. INTERMIX_TYPE, B.INTERMIX_WAY, B.INTERMIX_NOEXE_RESULT,B. INTERMIX_CC_WT "
				"  FROM TPSSM11 A,TPSSM10 B "
				"  WHERE  A.CC_MACH_NO =  '" + station_no + "'"
				"  AND  A.PONO NOT IN(SELECT PONO FROM TMMSM31B WHERE CC_MACH_NO = '" + station_no + "')" /*已新增到tmmsm31b表的记录过滤掉 */
				"  AND  A.PONO_STATUS > 80 "
				"  AND  A.PONO=B.PONO "
				"  ORDER  BY A.CAST_NO, A.CAST_DIV_NO"
				;

			break;
		}
		bcls_ret->Tables.Add();
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);

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
