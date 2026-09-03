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
BM2F_ENTERACE(mmsm81f3_zxhfg_inq)

int f_mmsm81f3_zxhfg_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString receive_data_time_s = "";
	CString receive_data_time_e = "";
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
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " SELECT A.MAT_CODE,A.MAT_NAME,'KG' MEASURE_UNIT,NVL(B.STOCK_WT,0) STOCK_WT FROM (SELECT * FROM TMMSM50  WHERE MAT_CODE IN ( SELECT CODE FROM TEP0002 t WHERE CODE_CLASS ='MMLC06') ) A LEFT JOIN	"
			"	(SELECT MAT_CODE, MAX(MAT_NAME) MAT_NAME, 'KG' MEASURE_UNIT, SUM(STOCK_WT) STOCK_WT FROM TMMSM85 WHERE MAT_CODE IN(SELECT CODE FROM TEP0002 t WHERE CODE_CLASS ='MMLC06') GROUP BY MAT_CODE) B ON A.MAT_CODE = B.MAT_CODE  order by A.MAT_CODE";
				;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		bcls_ret->Tables.Add();

		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", 1111);

		//tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		//if (bcls_rec->Tables[0].Columns.Contains("RECEIVE_DATA_TIME_S"))
		//{
		//	receive_data_time_s = bcls_rec->Tables[0].Rows[0]["RECEIVE_DATA_TIME_S"].ToString().Trim();
		//}
		//if (bcls_rec->Tables[0].Columns.Contains("RECEIVE_DATA_TIME_E"))
		//{
		//	 receive_data_time_e = bcls_rec->Tables[0].Rows[0]["RECEIVE_DATA_TIME_E"].ToString().Trim();
		//}
		//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", 22222);
		//
		//sqlstr = " select * from tmmsm81_s WHERE MARK_POS_CODE = '5' ";

		//if (tmmsm85["WEIGH_NO"].ToString().Trim()!= "" )
		//{
		//	sqlstr = sqlstr + " and WEIGH_NO like '%'|| @WEIGH_NO||'%'";
		//}
		//if (tmmsm85["MAT_CODE"].ToString().Trim() != "")
		//{
		//	sqlstr = sqlstr + " and MAT_CODE like '%'|| @MAT_CODE||'%'";
		//}
		//if (receive_data_time_s != "")
		//{
		//	sqlstr_temp += " AND RECEIVE_DATA_TIME >= '" + receive_data_time_s + "'";
		//}
		//if (receive_data_time_e != "")
		//{
		//	sqlstr_temp += " AND RECEIVE_DATA_TIME <= '" + receive_data_time_e + "'";
		//}
		//if (tmmsm85["SHIP_NAME"].ToString().Trim() != "")
		//{
		//	sqlstr = sqlstr + " and SHIP_NAME like '%'|| @SHIP_NAME||'%'";
		//}

		//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		////分页获取
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("WEIGH_NO", tmmsm85["WEIGH_NO"].ToString().Trim());
		//cmd_inq.Parameters.Set("MAT_CODE", tmmsm85["MAT_CODE"].ToString().Trim());
		//cmd_inq.Parameters.Set("SHIP_NAME", tmmsm85["SHIP_NAME"].ToString().Trim());
		//cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		//cmd_inq.Close();
		////返回分页总数量信息 
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
