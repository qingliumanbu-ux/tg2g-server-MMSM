/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 炉号查询
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

// service入口
BM2F_ENTERACE(mmsm21a_phn)

int f_mmsm21a_phn(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";

	CString factory_div = "";
	CString area_id = "";
	CString station_id = "";
	CString station_no = "";
	CString heat_no = "";
	CString table_name = "";

	CDbCommand cmd_inq(conn);

	try
	{
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("AREA_ID"))
			area_id = bcls_rec->Tables[0].Rows[0]["AREA_ID"].ToString().Trim();
		if(bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_NO"))
			station_no = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
			table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "factory_div  =[{0}]", factory_div);
		Log::Info("", __FUNCTION__, "area_id  =[{0}]", area_id);
		Log::Info("", __FUNCTION__, "station_id  =[{0}]", station_id);
		Log::Info("", __FUNCTION__, "station_no  =[{0}]", station_no);
		Log::Info("", __FUNCTION__, "table_name  =[{0}]", table_name);

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * FROM("
							" SELECT"
							" T1.SM_PLAN_NO, T1.HEAT_NO, T1.DEV_CODE, T1.FACTORY_DIV, T1.AREA_ID, T1.PROC_NO,"
							" T2.PONO, T2.ST_NO, T2.CAST_NO, T2.CAST_DIV_NO,"
							" T3.STATION_NO, T3.STATION_ID"
							" FROM TPSSM12 T1"
							" LEFT JOIN TPSSM11 T2 ON T1.SM_PLAN_NO = T2.SM_PLAN_NO AND T1.FACTORY_DIV = T2.FACTORY_DIV"
							" LEFT JOIN TPSSMD1 T3 ON T1.DEV_CODE = T3.DEV_CODE AND T1.AREA_ID = T3.AREA_ID AND T1.FACTORY_DIV = T3.FACTORY_DIV"
							" WHERE T1.HEAT_NO <> ' ' AND T1.PROC_NO <> ' '"
						" ) T1 WHERE 1 = 1";

				if ("" != table_name){
					if ("B" == station_id || "A" == station_id){
						sqlstr += " AND NOT EXISTS(SELECT 1 FROM " + table_name + " T2 WHERE T1.HEAT_NO = T2.HEAT_NO)";
					}
					else
					{
						sqlstr += " AND NOT EXISTS(SELECT 1 FROM " + table_name + " T2 WHERE T1.HEAT_NO = T2.HEAT_NO AND T1.PROC_NO = T2.PROC_NO)";
					}
				}
				if ("A" == station_id)
				{
					//梅钢吹氩站无单独计划（TPSSM12），需用转炉计划
					station_id = "B";
					area_id = "3";
				}
				if (factory_div != "")
				{
					sqlstr += " AND FACTORY_DIV = '" + factory_div + "'";
				}
				if (area_id != "")
				{
					sqlstr += " AND AREA_ID = " + area_id;
				}
				if (heat_no != "")
				{
					sqlstr += " AND HEAT_NO LIKE '%" + heat_no + "%'";
				}
				if (station_no != "")
				{
					sqlstr += " AND STATION_NO = '" + station_no + "'";
				}
				if (station_id != "")
				{
					sqlstr += " AND STATION_ID = '" + station_id + "'";
				}

				sqlstr += " ORDER BY AREA_ID, STATION_ID, STATION_NO, HEAT_NO DESC, PROC_NO DESC";
				break;
		}
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
	 }
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
