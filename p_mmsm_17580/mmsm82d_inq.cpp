/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 总的消耗信息
中频炉：tmmsm19
转炉:tmmsm21
电炉:tmmsm20
AOD:tmmsm27
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include <regex>

// service入口
BM2F_ENTERACE(mmsm82d_inq)
int f_mmsm82d_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString stat_date = "";
	CString sqlstr_where = "";
	CString heat_no = "";
	CModel tmmsm56a("TMMSM56A");

	CDbCommand cmd_inq(conn);

	try
	{ 	

		stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6); 	

		tmmsm56a.MergeFrom(bcls_rec->Tables[0].Rows[0]); 

		if (tmmsm56a["SEND_FLAG"].ToString().Trim() != "1")
		{  
			sqlstr = " select t1.*"
				" ,t1.OUT_STOCK_WT-NVL(t2.OUT_STOCK_WT/1000,0) as dif_wt"
				" from tmmsm56a t1"
				" left join (select mat_code,substr(OUT_STOCK_NO,2,4) SEQ_ID ,sum(OUT_STOCK_WT) OUT_STOCK_WT from tmmsm56ft where  HANDLE_DIV = 'F' and stat_date = @stat_date group by mat_code,substr(OUT_STOCK_NO,2,4)) t2 on  trim(to_char(t1.SEQ_ID, '0000')) = t2.SEQ_ID and t1.mat_code = t2.mat_code"
				" where 1=1"
				" and send_flag != '1'"
				" and stat_date = @stat_date"
				;
			if (tmmsm56a["MAT_CODE"].ToString().Trim() != "") sqlstr = sqlstr + " and t1.mat_code = @mat_code";
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

			bcls_ret->Tables.Add();
			sqlstr = " select MAT_CODE,mat_name,OUT_STOCK_WT,DEVO_WT,DEV_CODE,HEAT_NO,L2_PROC_NO,ST_NO,LOT_NO,decode(DEVO_WT,0,0,ROUND(OUT_STOCK_WT*100/DEVO_WT,3)) AS RATE"
				" from tmmsm56ft t1"
				" where 1=1"
				" and send_flag != '1'"
				" and HANDLE_DIV = 'F'"
				" and stat_date = @stat_date"
				;
			if (tmmsm56a["MAT_CODE"].ToString().Trim() != "") sqlstr = sqlstr + " and t1.mat_code = @mat_code";
			sqlstr = sqlstr + " order by decode(DEVO_WT,0,0,ROUND(OUT_STOCK_WT/DEVO_WT,3)),mat_code,heat_no,DEVO_WT"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			cmd_inq.Close();
		}
		else
		{
			sqlstr = " select t1.*"	 				
				" from tmmsm56a t1"
				" where 1=1"
				" and send_flag = '1'"
				" and stat_date = @stat_date"
				;
			if (tmmsm56a["MAT_CODE"].ToString().Trim() != "") sqlstr = sqlstr + " and t1.mat_code = @mat_code";
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

			bcls_ret->Tables.Add();
			sqlstr = " select MAT_CODE,mat_name,OUT_STOCK_WT,DEVO_WT,DEV_CODE,HEAT_NO,L2_PROC_NO,ST_NO,LOT_NO,decode(DEVO_WT,0,0,ROUND(OUT_STOCK_WT*100/DEVO_WT,3)) AS RATE"
				" from tmmsm56ft t1"
				" where 1=1"
				" and send_flag = '1'"
				" and HANDLE_DIV = 'F'"
				" and stat_date = @stat_date"
				;
			if (tmmsm56a["MAT_CODE"].ToString().Trim() != "") sqlstr = sqlstr + " and t1.mat_code = @mat_code";
			sqlstr = sqlstr + " order by decode(DEVO_WT,0,0,ROUND(OUT_STOCK_WT/DEVO_WT,3)),mat_code,heat_no,DEVO_WT"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			cmd_inq.Close();
		}

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
