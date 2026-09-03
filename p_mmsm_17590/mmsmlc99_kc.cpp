/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 根据计量单号取计量信息、库存信息、料仓信息
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
// service入口
BM2F_ENTERACE(mmsmlc99_kc)

int f_mmsmlc99_kc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString weigh_no = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm81al("TMMSM81AL");

	CDbCommand cmd_inq(conn);

	try
	{  	
		weigh_no = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString();

		Log::Trace("", __FUNCTION__, "weigh_no=[{0}]", weigh_no); 

		//计量信息
		sqlstr = " select * from ("
			" select FACTORY_DIV, MAT_CODE, MAT_NAME, MAT_TYPE, STOCK_PLACE_NO, STOCK_WT, RECEIVE_DATA_TIME, WEIGH_NO, SHIP_NAME, VEHICLE_NO, MEASURE_UNIT, RECEIVING_STATUS, TD_TYPE, SERIAL_NUMBER, FAC_CODE, DST_STOCK_CODE_GF, WORK_DATE, MATERIAL_BASKE_NO, ORDER_NO, WT_DATE_TIME, QUALITY_BATCH_NO, TRUST_ID, GROSS_WT, TARE_WT, NET_WT,RAW_WEIGHT, GROSS_TIME, TARE_TIME, BUCKLE_WT, BUCKLE_REMARK, SECOND_NET_WT, PROJECT_NO, VOUCHER_ID, MISSING_NO, BACK_CODE_1, BACK_CODE_2, BUSI_TYPE, PLAN_NO, BACK_CODE_5, BACK_CODE_6, RECV_DEPT_CODE, RECV_DEPT_NAME, LADE_CODE, LOAD_AREA_CODE, UNLOAD_POINT_CODE, UNLOAD_AREA_CODE, STATUS, MATERIAL_NAME, SG_SIGN, STOCK_ROOM, INSPECT_PLAN, COMBINE_YN, UPLOAD_101, UPLOAD_301, SRC_STOCK_CODE, SRC_LOC_CODE, DST_LOC_CODE, DST_STOCK_CODE, SETTLEMENT_WT, PURCH_NO, BUNKER_NO, AUART, LOT_NO, FORM_EDIT_FLAG, SEND_FLAG, DATI_MSG_SENT, BUNKER_TYPE, VOUCHER_CODE, MAT_RCV_TIME, TRNP_MODE_CODE, DEDUCT_WGT, BUCKLE_2WT, UPLOAD_TIME, MAT_CODE_LOT_NO from tmmsm81"
			" where weigh_no = @weigh_no"
			" union all"
			" select FACTORY_DIV, MAT_CODE, MAT_NAME, MAT_TYPE, STOCK_PLACE_NO, STOCK_WT, RECEIVE_DATA_TIME, WEIGH_NO, SHIP_NAME, VEHICLE_NO, MEASURE_UNIT, RECEIVING_STATUS, TD_TYPE, SERIAL_NUMBER, FAC_CODE, DST_STOCK_CODE_GF, WORK_DATE, MATERIAL_BASKE_NO, ORDER_NO, WT_DATE_TIME, QUALITY_BATCH_NO, TRUST_ID, GROSS_WT, TARE_WT, NET_WT,RAW_WEIGHT, GROSS_TIME, TARE_TIME, BUCKLE_WT, BUCKLE_REMARK, SECOND_NET_WT, PROJECT_NO, VOUCHER_ID, MISSING_NO, BACK_CODE_1, BACK_CODE_2, BUSI_TYPE, PLAN_NO, BACK_CODE_5, BACK_CODE_6, RECV_DEPT_CODE, RECV_DEPT_NAME, LADE_CODE, LOAD_AREA_CODE, UNLOAD_POINT_CODE, UNLOAD_AREA_CODE, STATUS, MATERIAL_NAME, SG_SIGN, STOCK_ROOM, INSPECT_PLAN, COMBINE_YN, UPLOAD_101, UPLOAD_301, SRC_STOCK_CODE, SRC_LOC_CODE, DST_LOC_CODE, DST_STOCK_CODE, SETTLEMENT_WT, PURCH_NO, BUNKER_NO, AUART, LOT_NO, FORM_EDIT_FLAG, SEND_FLAG, DATI_MSG_SENT, BUNKER_TYPE, VOUCHER_CODE, MAT_RCV_TIME, TRNP_MODE_CODE, DEDUCT_WGT, BUCKLE_2WT, UPLOAD_TIME, MAT_CODE_LOT_NO from tmmsm81_s"
			" where weigh_no = @weigh_no"
			")"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("weigh_no", weigh_no);		
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		//库存信息
		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "NET_WT");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "STOCK_WT");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "STOCK_WT_H");
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "STOCK_WT_L");
		bcls_ret->Tables[1].Rows.Add();
		if (bcls_ret->Tables[0].Rows.get_Count() > 0)
		{ 
			bcls_ret->Tables[1].Rows[0]["NET_WT"] = bcls_ret->Tables[0].Rows[0]["NET_WT"].ToDecimal();
		}
		sqlstr = " select sum(STOCK_WT)"
			" ,sum(case when bunker_no in (select bunker_no from tmmsm60 where FLAG1 = 'H') then STOCK_WT else 0 end)"
			" ,sum(case when bunker_no in (select bunker_no from tmmsm60 where FLAG1 = 'U') then STOCK_WT else 0 end)"
			" from tmmsm85"
			" where  weigh_no = @weigh_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("weigh_no", weigh_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			Log::Trace("", __FUNCTION__, "STOCK_WT=[{0}],STOCK_WT_H=[{1}],STOCK_WT_L=[{2}]", cmd_inq.GetDecimal(1), cmd_inq.GetDecimal(2), cmd_inq.GetDecimal(3));
			bcls_ret->Tables[1].Rows[0]["STOCK_WT"] = cmd_inq.GetDecimal(1);
			bcls_ret->Tables[1].Rows[0]["STOCK_WT_H"] = cmd_inq.GetDecimal(2);
			bcls_ret->Tables[1].Rows[0]["STOCK_WT_L"] = cmd_inq.GetDecimal(3);
		}
		cmd_inq.Close();

		//料仓计量信息
		sqlstr = " select *"
			" from tmmsm85"
			" where  weigh_no = @weigh_no"
			;
		bcls_ret->Tables.Add();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("weigh_no", weigh_no);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
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
