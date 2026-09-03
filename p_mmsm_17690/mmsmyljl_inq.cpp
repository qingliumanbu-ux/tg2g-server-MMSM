/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料进料汇总查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmyljl_inq)

int f_mmsmyljl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int count = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString begin_time = "";
	CString end_time = "";

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm89("TMMSM89");

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
		tmmsm89.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
		Log::Info("", __FUNCTION__, "BEGIN_TIME =[{0}]", begin_time);
		Log::Info("", __FUNCTION__, "END_TIME =[{0}]", end_time);
		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM tmmsm89 "
				"  WHERE 1=1 "
				;
			sqlstr = " select a.mat_code,a.MAT_NAME,sum(A.STOCK_WT) STOCK_WT FROM TMMSM89 A  "
				"   where 1 = 1 and  A.EVENT_NAME='高位料仓上料'  AND A.MAT_CODE <>' ' "
				
				;

			if (begin_time.Trim() != "")
			{
				sqlstr_temp += " AND EVENT_TIME>=@begin_time";
			}
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " AND EVENT_TIME<=@end_time";
			}
			
			
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " GROUP BY a.mat_code,a.MAT_NAME ORDER BY mat_code  ";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		
		
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取

		Log::Trace(" ", __FUNCTION__, "sqlstr={[0]}", sqlstr);
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