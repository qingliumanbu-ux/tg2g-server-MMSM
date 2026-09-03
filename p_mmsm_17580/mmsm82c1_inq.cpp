/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 取炉次的开始信息
中频炉：tmmsm19
转炉:tmmsm21
电炉:tmmsm20
AOD:tmmsm27
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82c1_inq)
int f_mmsm82c1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int		TotalRecordCount = 0;
	CString flag = "";
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = " ";
	

	CDbCommand cmd_inq(conn);
	CModel tmmsm2a_yl("TMMSM2A_YL");

	CPageInfo pageInfo;

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["START_TIME_S"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["START_TIME_E"].ToString();

		tmmsm2a_yl.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Info("", __FUNCTION__, "PROC_NO =[{0}]", tmmsm2a_yl["L2_PROC_NO"].ToString());
		if (bcls_rec->Tables[0].Columns.Contains("UPD_HIS_RECORD"))
		{
			flag = bcls_rec->Tables[0].Rows[0]["UPD_HIS_RECORD"].ToString();
		}

		if (flag == "1")
		{
			//取履历投料信息;
			sqlstr = " select heat_no,proc_no,L2_PROC_NO,SM_PLAN_NOL2,PROC_COUNT,DEVO_TIME,MAT_CODE,MAT_NAME,DEVO_WT,DEV_CODE,STK_NO,WEIGH_NO, QUALITY_BATCH_NO,ID_2A,REMARK_2,SEQ_NO_2A,lot_no, EVENT_DESC "
				" from tmmsm2A_lv t1"
				" where 1=1"
				;
		}
		else
		{
			//取投料信息;
			sqlstr = " select heat_no,proc_no,L2_PROC_NO,SM_PLAN_NOL2,PROC_COUNT,DEVO_TIME,MAT_CODE,MAT_NAME,DEVO_WT,DEV_CODE,STK_NO,WEIGH_NO, QUALITY_BATCH_NO,ID_2A,REMARK_2,SEQ_NO_2A,lot_no, SUM(DEVO_WT) OVER (PARTITION BY DEV_CODE) DEV_CODE_SUM"
				" from tmmsm2A_yl t1"
				" where 1=1"
				;
		}
		
		sqlstr_count = " select count(1) from tmmsm2A_yl where 1=1";

		if (tmmsm2a_yl["L2_PROC_NO"].ToString().Trim() != "") 
		{
			sqlstr_temp += " AND L2_PROC_NO like '%'|| @L2_PROC_NO||'%'";
		}
		if (tmmsm2a_yl["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " and exists ( select 1 from tmmsmgy06 t2 where t2.l2_proc_no=t1.l2_proc_no and t2.heat_no = @HEAT_NO)";
		}
		if (tmmsm2a_yl["DEV_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND DEV_CODE like '%'|| @DEV_CODE||'%'";
		}
		if (end_time.Trim() != "")
		{
			sqlstr_temp += " AND DEVO_TIME <=@end_time";
		}
		if (begin_time.Trim() != "")
		{
			sqlstr_temp += " AND DEVO_TIME >=@begin_time";
		}
		
		sqlstr = sqlstr + sqlstr_temp + " order by  dev_code,DEVO_TIME,ID_2A,mat_code";
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("L2_PROC_NO", tmmsm2a_yl["L2_PROC_NO"].ToString());
		cmd_inq.Parameters.Set("HEAT_NO", tmmsm2a_yl["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("DEV_CODE", tmmsm2a_yl["DEV_CODE"].ToString()); 		
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
