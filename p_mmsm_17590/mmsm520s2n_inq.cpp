/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 废钢自循环录入查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm520s2n_inq)

int f_mmsm520s2n_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString flag_ls = "0";
	


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm81_s("TMMSM81_S"); 
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
		tmmsm81_s.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		
			cs_receive_data_time_from = bcls_rec->Tables[0].Rows[0]["RECEIVE_DATA_TIME_FROM"].ToString();
			cs_receive_data_time_to = bcls_rec->Tables[0].Rows[0]["RECEIVE_DATA_TIME_TO"].ToString();
			flag_ls = bcls_rec->Tables[0].Rows[0]["FLAG_LS"].ToString();

			Log::Trace("", __FUNCTION__, "flag_ls = [{0}]", flag_ls);

			if (flag_ls == "1")
			{
				sqlstr_count = " SELECT COUNT(1) "
					"   FROM tmmsm68 "
					"  WHERE USE_LOGO IN ('0',' ') "
					;
				sqlstr = " select * from tmmsm68"
					" where USE_LOGO IN ('0',' ')"
					;
				sqlstr_temp = "";
				if (tmmsm81_s["WEIGH_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND WEIGH_NO		like '%'|| @weigh_no||'%'";
				}
				if (tmmsm81_s["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE		like '%'|| @mat_code||'%'";
				}
				if (tmmsm81_s["MAT_NAME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_NAME		like '%'|| @mat_name||'%'";
				}
				if (cs_receive_data_time_from != "")
				{
					sqlstr_temp += " AND PROD_TIME >= '" + cs_receive_data_time_from + "'";
				}
				if (cs_receive_data_time_to != "")
				{
					sqlstr_temp += " AND PROD_TIME <= '" + cs_receive_data_time_to + "'";
				}
				sqlstr = sqlstr + sqlstr_temp;
			}
			else
			{
				sqlstr_count = " SELECT COUNT(1) FROM "
					" ( SELECT RECEIVE_DATA_TIME,MAT_CODE,SHIP_NAME,WEIGH_NO,QUALITY_BATCH_NO,VEHICLE_NO,MAT_NAME,BUNKER_NO,UNLOAD_POINT_CODE,RECEIVING_STATUS FROM TMMSM81_S WHERE 1=1 and MARK_POS_CODE = '5' and FORM_EDIT_FLAG = '0' UNION"
					" SELECT RECEIVE_DATA_TIME,MAT_CODE,SHIP_NAME,WEIGH_NO,QUALITY_BATCH_NO,VEHICLE_NO,MAT_NAME,BUNKER_NO,UNLOAD_POINT_CODE,RECEIVING_STATUS FROM TMMSM81 WHERE SUBSTR(VOUCHER_ID,1,4)='21XH') "
					"  WHERE 1=1 "
					;
				sqlstr =" SELECT * FROM "
					" (SELECT RECEIVE_DATA_TIME,MAT_CODE,SHIP_NAME,WEIGH_NO,QUALITY_BATCH_NO,VEHICLE_NO,MAT_NAME,BUNKER_NO,UNLOAD_POINT_CODE,RECEIVING_STATUS FROM TMMSM81_S WHERE 1=1 and MARK_POS_CODE = '5' and FORM_EDIT_FLAG = '0' UNION"
					" SELECT RECEIVE_DATA_TIME,MAT_CODE,SHIP_NAME,WEIGH_NO,QUALITY_BATCH_NO,VEHICLE_NO,MAT_NAME,BUNKER_NO,UNLOAD_POINT_CODE,RECEIVING_STATUS FROM TMMSM81 WHERE SUBSTR(VOUCHER_ID,1,4)='21XH') WHERE 1=1 ";
				sqlstr_temp = "";
				if (tmmsm81_s["WEIGH_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND WEIGH_NO		like '%'|| @weigh_no||'%'";
				}
				if (tmmsm81_s["QUALITY_BATCH_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND QUALITY_BATCH_NO		like '%'|| @quality_batch_no||'%'";
				}
				if (tmmsm81_s["SHIP_NAME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND SHIP_NAME	like '%'|| @ship_name||'%'";
				}
				if (tmmsm81_s["VEHICLE_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND VEHICLE_NO	like '%'|| @vehicle_no||'%'";
				}
				if (tmmsm81_s["VOUCHER_ID"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND VOUCHER_ID		like '%'|| @voucher_id||'%'";
				} 			
				if (tmmsm81_s["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_CODE		like '%'|| @mat_code||'%'";
				}
				if (tmmsm81_s["MAT_NAME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND MAT_NAME		like '%'|| @mat_name||'%'";
				}
				if (tmmsm81_s["BUNKER_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND BUNKER_NO = @bunker_no ";
				}	
				if (tmmsm81_s["RECEIVING_STATUS"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND RECEIVING_STATUS = @receiving_status ";
				}
				if (cs_receive_data_time_from != "")
				{
					sqlstr_temp += " AND RECEIVE_DATA_TIME >= '" + cs_receive_data_time_from + "'";
				}
				if (cs_receive_data_time_to != "")
				{
					sqlstr_temp += " AND RECEIVE_DATA_TIME <= '" + cs_receive_data_time_to + "'";
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY RECEIVE_DATA_TIME DESC";
				sqlstr = sqlstr + sqlstr_temp;

			}			
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);

		cmd_inq.Parameters.Set("weigh_no", tmmsm81_s["WEIGH_NO"].ToString());
		cmd_inq.Parameters.Set("quality_batch_no", tmmsm81_s["QUALITY_BATCH_NO"].ToString());
		cmd_inq.Parameters.Set("ship_name", tmmsm81_s["SHIP_NAME"].ToString());
		cmd_inq.Parameters.Set("vehicle_no", tmmsm81_s["VEHICLE_NO"].ToString());
		cmd_inq.Parameters.Set("voucher_id", tmmsm81_s["VOUCHER_ID"].ToString());
		cmd_inq.Parameters.Set("mat_code", tmmsm81_s["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("mat_name", tmmsm81_s["MAT_NAME"].ToString());
		cmd_inq.Parameters.Set("bunker_no", tmmsm81_s["BUNKER_NO"].ToString());
		cmd_inq.Parameters.Set("receiving_status", tmmsm81_s["RECEIVING_STATUS"].ToString());
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
