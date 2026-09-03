/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2023-09-22
Description: 退位料仓
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsm861v_pro)

int f_mmsm861v_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString bunker_no = "";
	int TotalRecordCount = 0;
	CString  now = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CModel tmmsm85("TMMSM85");

	try
	{
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			bunker_no = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString().Trim();
			Log::Info("", __FUNCTION__, "bunker_no =[{0}]", bunker_no);
			sqlstr = " select WEIGH_NO, MAT_CODE, MAT_NAME, TIME_INSTOCK, STOCK_WT  "
				" from tmmsm85 where BUNKER_TYPE='0' and BUNKER_NO='" + bunker_no + "'  ";
			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			tmmsm85["WEIGH_NO"] = cmd_inq.GetString(1);
			tmmsm85["REC_REVISE_TIME"] = now;
			tmmsm85["REC_REVISOR"] = s.userid;
			tmmsm85["BUNKER_TYPE"] = "1";
			tmmsm85.Update("BUNKER_TYPE,REC_REVISOR,REC_REVISE_TIME", "WEIGH_NO");
		}
		Log::Info("", __FUNCTION__, "sql =[{0}]", sqlstr);
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
