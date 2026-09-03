/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   wencm
Version:    1.0
Date:     2024-01-26 17:13:56
Description: 料仓分区域信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm85fq_bunker_inq)

int f_mmsm85fq_bunker_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
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

		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		bcls_ret->Tables.Add();
		//--------------------------------
		//获取传入参数
		tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		Log::Info("", __FUNCTION__, "BUNKER_TYPE =[{0}]", tmmsm85["BUNKER_TYPE"].ToString());
		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm85["BUNKER_NO"].ToString());
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区01
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'CL01' order by  BUNKER_NO "
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr0 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区02
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1 AND BUNKER_TYPE = 'CL02'  order by  BUNKER_NO"
				;

			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr1 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区03
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'CL03'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区04
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'CL04'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[3]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区05
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'CL05'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[4]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区06
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'CL06'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[5]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区07
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'CL07'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[6]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区08
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'CL08'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[7]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区09
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'CL09'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[8]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区10
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'CL10'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[9]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区COOLN-121-130
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'COOLN'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[10]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区COOLS-131-140
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'COOLS'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[11]);
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			//废钢料场降温料区COOLS-131-140
			sqlstr = "  SELECT   BUNKER_NO ,BUNKER_NAME ,BUNKER_TYPE , MAT_CODE , MAT_NAME, MAT_TYPE, STOCK_WT,RATE  FROM  TMMSM60    "
				"  WHERE 1=1  AND BUNKER_TYPE = 'COOLW'  order by  BUNKER_NO"
				;
			break;
		}


		//分页获取
		Log::Info("", __FUNCTION__, "sqlstr2 =[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[12]);
		cmd_inq.Close();
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
