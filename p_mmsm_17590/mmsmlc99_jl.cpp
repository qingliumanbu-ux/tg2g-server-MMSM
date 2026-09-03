/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 根据ID及工位、炉号取消耗信息
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmlc99_jl)

int f_mmsmlc99_jl(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString begin_time = "";
	CString end_time = "";

	CModel tmmsm89("TMMSM89");
	CModel tmmsm2a_yl("TMMSM2A_YL");

	CDbCommand cmd_inq(conn);

	try
	{  	
		tmmsm89.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Info("", __FUNCTION__, "bunker_no =[{0}]", tmmsm89["BUNKER_NO_ORIGINAL"].ToString());
		Log::Info("", __FUNCTION__, "id_2a =[{0}]", tmmsm89["ID_2A"].ToString());
		Log::Info("", __FUNCTION__, "heat_no =[{0}]", tmmsm89["HEAT_NO"].ToString());
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT * "
				"   FROM tmmsm2a_yl "
				"  WHERE 1=1 "
				" AND STK_NO	=@bunker_no"
				" AND id_2a	=@id_2a"
				 " AND heat_no	=@heat_no"
				 ;
			break;
		}
		cmd_inq.Parameters.Set("bunker_no", tmmsm89["BUNKER_NO_ORIGINAL"].ToString());
		cmd_inq.Parameters.Set("id_2a", tmmsm89["ID_2A"].ToString());
		cmd_inq.Parameters.Set("heat_no", tmmsm89["HEAT_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr);	
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


	return doFlag;

}
