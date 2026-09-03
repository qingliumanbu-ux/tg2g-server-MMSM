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
BM2F_ENTERACE(mmsm533bv_inq)

int f_mmsm533bv_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString v_weigh_no = "";//计量单号
	CString v_mat_code = "";//物料号
	CString v_start_time = "";//开始时刻
	CString v_end_time = "";//结束时刻
	CString v_sql_column = "";//字段名



	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm84("TMMSM84");
	CModel tmmsm54("TMMSM54");

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

		if (bcls_rec->Tables[0].Columns.Contains("WEIGH_NO"))
			v_weigh_no = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			v_mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();

		Log::Info("", __FUNCTION__, "[{0}]  [{1}]  [{2}]  [{3}]", v_weigh_no, v_mat_code, v_start_time, v_end_time);

		//Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm84["MAT_CODE"].ToString());

		v_sql_column = "MAT_CODE,MAT_NAME,MAT_TYPE,STOCK_PLACE_NO,STOCK_WT,RECEIVE_DATA_TIME, "
						" WEIGH_NO,SHIP_NAME,VEHICLE_NO,MEASURE_UNIT,RECEIVING_STATUS,TD_TYPE,SERIAL_NUMBER,FAC_CODE, "
						" DST_STOCK_CODE_GF,WORK_DATE,MATERIAL_BASKE_NO,ORDER_NO,WT_DATE_TIME,QUALITY_BATCH_NO,TRUST_ID, "
						" GROSS_WT,TARE_WT,NET_WT,GROSS_TIME,TARE_TIME,BUCKLE_WT,BUCKLE_REMARK,SECOND_NET_WT,PROJECT_NO, "
						" VOUCHER_ID,LOT_NO,MISSING_NO,BACK_CODE_1,BACK_CODE_2,BUSI_TYPE,PLAN_NO,BACK_CODE_5,BACK_CODE_6, "
						" RECV_DEPT_CODE,RECV_DEPT_NAME,LADE_CODE,LOAD_AREA_CODE,UNLOAD_POINT_CODE,UNLOAD_AREA_CODE ";

		//查询计量单
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT  " + v_sql_column + ""
				"   FROM TMMSM81 "
				"  WHERE 1=1 ";

			sqlstr_temp += " union all  SELECT " + v_sql_column + ""
				"   FROM TMMSM85 "
				"  WHERE 1=1 ";
			

			if (v_weigh_no.Trim() != "")
			{
				sqlstr += " AND WEIGH_NO = '" + v_weigh_no + "'";
				sqlstr_temp += " AND WEIGH_NO = '" + v_weigh_no + "'";
			}
			if (v_mat_code.Trim() != "")
			{
				sqlstr += " AND MAT_CODE = '" + v_mat_code + "'";
				sqlstr_temp += " AND MAT_CODE = '" + v_mat_code + "'";
			}
			if (v_start_time.Trim() != "")
			{
				sqlstr += " AND RECEIVE_DATA_TIME >= '" + v_start_time + "'";
				sqlstr_temp += " AND RECEIVE_DATA_TIME >= '" + v_start_time + "'";
			}
			if (v_end_time.Trim() != "")
			{
				sqlstr += " AND RECEIVE_DATA_TIME <= '" + v_end_time + "'";
				sqlstr_temp += " AND RECEIVE_DATA_TIME <= '" + v_end_time + "'";
			}


			
			//sqlstr = sqlstr + sqlstr_temp;
			
			break;
		}

		Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		bcls_ret->Tables.Add();

		//查询退货订单
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT * "
				"   FROM TMMSM54 "
				"  WHERE 1 = 1"; 

			/*if (v_weigh_no.Trim() != "")
			{
				sqlstr += " AND WEIGH_NO = '" + v_weigh_no + "'";
			}
			if (v_mat_code.Trim() != "")
			{
				sqlstr += " AND MAT_CODE = '" + v_mat_code + "'";
			}
			if (v_start_time.Trim() != "")
			{
				sqlstr += " AND PLAN_MAKE_TIME >= '" + v_start_time.Substring(0, 8) + "'";
			}
			if (v_end_time.Trim() != "")
			{
				sqlstr += " AND PLAN_MAKE_TIME <= '" + v_end_time.Substring(0, 8) + "'";
			}*/

			
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr222[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1], pageInfo.RecordFrom, pageInfo.PageSize);
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
