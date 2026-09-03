/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    1.0
Date:
Description: 石灰卸车调度查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(wmsmshxcs_inq)

int f_wmsmshxcs_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

			/*sqlstr = " SELECT * FROM (\
				SELECT REC_CREATE_TIME, REC_CREATOR, CARRIER_DES, CAR_NAME, REC_ID, ROWNO,\
				ROW_NUMBER() OVER(PARTITION BY CARRIER_DES, CAR_NAME ORDER BY ROWNO) as rn\
				FROM twmsmshxc\
				WHERE rec_creator = '2' AND rec_create_time = (SELECT max(t.rec_create_time) FROM twmsmshxc t\
				WHERE t.rec_creator = '2')) RankedData\
				WHERE rn = 1 ORDER BY REC_ID,ROWNO";*/
			sqlstr = "select * from TWMSMSHXC_CC t where T.REC_ID = '二级混灰' \
					 				AND t.REC_CREATE_TIME = (SELECT MAX(REC_CREATE_TIME) FROM TWMSMSHXC_CC where REC_ID = '二级混灰')";
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
