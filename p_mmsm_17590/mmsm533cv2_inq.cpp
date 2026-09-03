/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   mfj
Version:    1.0
Date:     2023-09-21 17:13:56
Description: 原辅料退货装车确认查询-子表查询MM
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/


/*************/
/*************/
/*****退货装货确认子表查询*******/
/*****应该车牌号为主********/
/*****查询多笔退货计划号和物料信息********/


/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm533cv2_inq)

int f_mmsm533cv2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString v_plate_number = "";//车牌号
	CString v_status = "";//状态    V为车辆信息   1为退货装货确认 



	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm84("TMMSM84");
	CModel tmmsm54("TMMSM54");

	CDbCommand cmd_inq(conn);

	try
	{


		if (bcls_rec->Tables[0].Columns.Contains("PLATE_NUMBER"))
			v_plate_number = bcls_rec->Tables[0].Rows[0]["PLATE_NUMBER"].ToString();

		Log::Info("", __FUNCTION__, "v_plate_number[{0}] ", v_plate_number);

		//Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm84["MAT_CODE"].ToString());

		//查询退货装货确认子表信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT * "
				"   FROM TMMSM84 "
				"  WHERE 1= 1  AND STATUS = '1' ";

			if (v_plate_number.Trim() != "")
			{
				sqlstr += " AND PLATE_NUMBER = '" + v_plate_number + "'";
			}

			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
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
