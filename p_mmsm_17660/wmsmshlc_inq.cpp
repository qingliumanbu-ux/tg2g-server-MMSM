/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    1.0
Date:
Description: 导入报表
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(wmsmshlc_inq)

int f_wmsmshlc_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_grid_tab = "";	//Grid区分
	CString tableName = "";


	CModel tmmsm60("TMMSM60");

	CDbCommand cmd_inq(conn);

	try
	{

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT T.*,100-STOCK_RATE as STOCK_RATE_1 FROM \
				(SELECT\
				TO_NUMBER(BUNKER_NO_NUM) || '号仓' AS BUNKER_NO,\
				STOCK_RATE,\
				tag_in_time\
				FROM(\
				SELECT\
				SUBSTR(TAG_NAME, 20) AS BUNKER_NO_NUM,\
				TO_NUMBER(TAG_VALUE) AS STOCK_RATE,\
				tag_in_time,\
				ROW_NUMBER() OVER(PARTITION BY SUBSTR(TAG_NAME, 20) ORDER BY tag_in_time DESC) AS rn\
				FROM(SELECT * FROM TWMSMSHLC S WHERE S.TAG_IN_TIME > TO_CHAR(SYSDATE - 6, 'YYYYMMDDHHMISS') and SUBSTR(TAG_NAME, 11,2)='Bi' order by TAG_IN_TIME) T\
				) t\
				WHERE rn = 1\
				ORDER BY TO_NUMBER(BUNKER_NO_NUM) ASC)T ";
			break;
		}
		Log::Info("", "", "111");
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交

	return doFlag;

}
