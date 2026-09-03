/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    1.0
Date:
Description: 石灰成品料仓查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(wmsmshlcs_inq)

int f_wmsmshlcs_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_grid_tab = "";	//Grid区分
	CString tableName = "";

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

			sqlstr = " SELECT\
				CASE t.bunker_no\
				WHEN 'BN-661_NORTH' THEN 'BN-661北'\
				WHEN 'BN-661_SOUTH' THEN 'BN-661南'\
				ELSE t.bunker_no\
				END AS bunker_no,\
				ROUND(NVL(l.stock_rate, 0) * 100 / t.full_capacity) AS stock_rate_ratio,\
				l.tag_in_time,\
				ROUND((1 - NVL(l.stock_rate, 0) / t.full_capacity) * 100) AS remaining_ratio\
				FROM(\
				SELECT 'BN-6612'    AS bunker_no, 1 AS sort_order, 9 AS full_capacity FROM dual\
				UNION ALL SELECT 'BN-6611', 2, 9 FROM dual\
				UNION ALL SELECT 'BN-6610', 3, 9 FROM dual\
				UNION ALL SELECT 'BN-669', 4, 9 FROM dual\
				UNION ALL SELECT 'BN-668', 5, 9 FROM dual\
				UNION ALL SELECT 'BN-667', 6, 9 FROM dual\
				UNION ALL SELECT 'BN-666', 7, 6 FROM dual\
				UNION ALL SELECT 'BN-665', 8, 11 FROM dual\
				UNION ALL SELECT 'BN-664', 9, 8 FROM dual\
				UNION ALL SELECT 'BN-663', 10, 7 FROM dual\
				UNION ALL SELECT 'BN-662', 11, 7 FROM dual\
				UNION ALL SELECT 'BN-661_SOUTH', 12, 12 FROM dual\
				UNION ALL SELECT 'BN-661_NORTH', 13, 12 FROM dual\
				) t\
				LEFT JOIN(\
				SELECT\
				SUBSTR(TAG_NAME, 14) AS bunker_no,\
				ROUND(TO_NUMBER(TAG_VALUE)) AS stock_rate,\
				tag_in_time\
				FROM(\
				SELECT\
				tag_name,\
				tag_value,\
				tag_in_time,\
				ROW_NUMBER() OVER(PARTITION BY SUBSTR(TAG_NAME, 14) ORDER BY tag_in_time DESC) AS rn\
				FROM TWMSMSHLC S\
				WHERE S.TAG_IN_TIME > TO_CHAR(SYSDATE - 3, 'YYYYMMDDHHMISS')\
				AND SUBSTR(TAG_NAME, 11, 2) != 'Bi'\
				)\
				WHERE rn = 1\
				) l ON t.bunker_no = l.bunker_no\
				ORDER BY t.sort_order ";
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
