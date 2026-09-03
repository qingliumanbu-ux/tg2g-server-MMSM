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
BM2F_ENTERACE(mmsm81_cf_inq)

int f_mmsm81_cf_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString cs_forname = "";

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm81("TMMSM81");

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
		tmmsm81.Reset();
		tmmsm81.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm81.Print();
		Log::Info("", __FUNCTION__, "FORM_EDIT_FLAG =[{0}]", tmmsm81["FORM_EDIT_FLAG"].ToString());
		Log::Info("", __FUNCTION__, "WEIGH_NO =[{0}]", tmmsm81["WEIGH_NO"].ToString());
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM81 T "
				"  WHERE 1=1 "
				;
			sqlstr =
				"  SELECT  NVL(T1.STATUS,NET_WT) STATUS, T.* FROM"
				" (SELECT MAT_CODE,MAT_NAME,WEIGH_NO,QUALITY_BATCH_NO,FORM_EDIT_FLAG,BUNKER_NO,MAT_RCV_TIME,NET_WT,STATUS,SHIP_NAME,RECEIVE_DATA_TIME,VEHICLE_NO,RAW_WEIGHT,"
				" RECEIVING_STATUS FROM TMMSM81  UNION"
				" SELECT MAT_CODE, MAT_NAME, WEIGH_NO, QUALITY_BATCH_NO, FORM_EDIT_FLAG, BUNKER_NO, MAT_RCV_TIME, NET_WT, STATUS, SHIP_NAME, RECEIVE_DATA_TIME, VEHICLE_NO,"
				" NET_WT RAW_WEIGHT, RECEIVING_STATUS FROM  TMMSM81_S)  T"
				" LEFT JOIN (SELECT WEIGH_NO,QUALITY_BATCH_NO,MAX(NET_WT)-SUM(STOCK_WT) STATUS FROM  TMMSM89"
				"  WHERE event_code	IN('CHARGE','EXTRA')    AND   QUALITY_BATCH_NO=@QUALITY_BATCH_NO "
				"  GROUP BY  WEIGH_NO,QUALITY_BATCH_NO) T1 ON T.WEIGH_NO = T1.WEIGH_NO and  T.QUALITY_BATCH_NO = T1.QUALITY_BATCH_NO WHERE 1 = 1"
				" AND T.BUNKER_NO not in(' ', '0') ";
			
			if (tmmsm81["QUALITY_BATCH_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T.QUALITY_BATCH_NO		like '%'|| @QUALITY_BATCH_NO||'%'";
			}
			if (tmmsm81["FORM_EDIT_FLAG"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND T.FORM_EDIT_FLAG	= @FORM_EDIT_FLAG";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY T.RECEIVE_DATA_TIME DESC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.Parameters.Set("QUALITY_BATCH_NO", tmmsm81["QUALITY_BATCH_NO"].ToString());
		cmd_inq.Parameters.Set("FORM_EDIT_FLAG", tmmsm81["FORM_EDIT_FLAG"].ToString());
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
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
