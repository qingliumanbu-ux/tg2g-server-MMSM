/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 检测异常信息
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82m_inq)
int f_mmsm_21c005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82m_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_where = "";
	CString stat_date = "";
	CString type_flag = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDbCommand cmd_inq(conn);

	CModel tmmsm2a_send("TMMSM2A_SEND");

	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	CDecimal cd_count = 0;
	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{  	

		stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6); 	
		type_flag = bcls_rec->Tables[0].Rows[0]["TYPE_FLAG"].ToString();
		tmmsm2a_send.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (type_flag.Trim() == "")
		{
			strcpy(s.msg, "错误类型不能为空！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (type_flag.Trim() == "01")
		{
			sqlstr =
				"select heat_no,  mat_code,mat_name,dev_code,lot_no,sum(devo_wt) devo_wt"
				" from TMMSM2A_SEND "
				" where 1 = 1 "
				" and RTN_FLAG = ' ' "
				" and stat_date = @stat_date "
				" group  heat_no,  mat_code,mat_name,dev_code,lot_no  "
				" having sum(devo_wt)<0	 "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		if (type_flag.Trim() == "04")	//无批次号信息
		{
			sqlstr_where = "";
			if (tmmsm2a_send["HEAT_NO"].ToString().Trim() != "")
				sqlstr_where = sqlstr_where + " and HEAT_NO = @heat_no";
			if (tmmsm2a_send["MAT_CODE"].ToString().Trim() != "")
				sqlstr_where = sqlstr_where + " and MAT_CODE like  @mat_code||'%'";

			sqlstr =
				"select heat_no,WEIGH_NO,mat_code,mat_name"
				" from tmmsm56 "
				" where 1 = 1 "
				" and lot_no=' '"
				+ sqlstr_where +
				" and mat_code in (select mat_code from tmmsm50 where QUALITY_FLAS='1')"
				" and stat_date = @stat_date "
				" group BY heat_no,WEIGH_NO,mat_code,mat_name  "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		if (type_flag.Trim() == "02")  //抛帐履历
		{ 
			sqlstr =
				"select *"
				" from TMMSM2A_SEND "
				" where 1 = 1 "
				;
			if (tmmsm2a_send["ST_NO"].ToString().Trim() != "")
				sqlstr = sqlstr + " and st_no = @st_no";
			if (tmmsm2a_send["HEAT_NO"].ToString().Trim() != "")
				sqlstr = sqlstr + " and HEAT_NO = @heat_no";
			if (tmmsm2a_send["MAT_CODE"].ToString().Trim() != "")
				sqlstr = sqlstr + " and MAT_CODE like  @mat_code||'%'";
			if (tmmsm2a_send["DEV_CODE"].ToString().Trim() != "")
				sqlstr = sqlstr + " and DEV_CODE = @dev_code";
			if (tmmsm2a_send["LOT_NO"].ToString().Trim() != "")
				sqlstr = sqlstr + " and LOT_NO = @lot_no";

			if (tmmsm2a_send["RTN_FLAG"].ToString() == "0")
			{
				sqlstr = sqlstr + " and RTN_FLAG = ' '";
			}
			if (tmmsm2a_send["RTN_FLAG"].ToString() == "1")
			{
				sqlstr = sqlstr + " and RTN_FLAG = '1'";
			}
			sqlstr = sqlstr + " and stat_date = @stat_date "
				" ORDER BY SEND_TIME,heat_no,dev_code,mat_code "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("st_no", tmmsm2a_send["ST_NO"].ToString());
			cmd_inq.Parameters.Set("heat_no", tmmsm2a_send["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("dev_code", tmmsm2a_send["DEV_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_code", tmmsm2a_send["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("lot_no", tmmsm2a_send["LOT_NO"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		if (type_flag.Trim() == "03")
		{
			sqlstr_where = "";
			if (tmmsm2a_send["ST_NO"].ToString().Trim() != "")
				sqlstr_where = sqlstr_where + " and st_no = @st_no";
			if (tmmsm2a_send["HEAT_NO"].ToString().Trim() != "")
				sqlstr_where = sqlstr_where + " and HEAT_NO = @heat_no";
			if (tmmsm2a_send["MAT_CODE"].ToString().Trim() != "")
				sqlstr_where = sqlstr_where + " and MAT_CODE like  @mat_code||'%'";
			if (tmmsm2a_send["DEV_CODE"].ToString().Trim() != "")
				sqlstr_where = sqlstr_where + " and DEV_CODE = @dev_code";
			if (tmmsm2a_send["LOT_NO"].ToString().Trim() != "")
				sqlstr_where = sqlstr_where + " and LOT_NO = @lot_no";

			sqlstr = " select stat_date,sm_plan_nol2,heat_no,pono,st_no,dev_code,mat_code,LOT_NO,prod_date,sum(devo_wt) devo_wt "
				" ,nvl((select mat_name from tmmsm50 t2 where t2.mat_code = t.mat_code),' ') mat_name"
						" from "
						"("
						" select stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,LOT_NO,substr(recv_mat_time,1,8) as prod_date,OUT_STOCK_WT as devo_wt"
						" from tmmsm56 t1"
						" where 1=1"
						"  and mat_code in (select  MAT_CODE FROM TMMSM50 WHERE QUALITY_FLAS = '1')"
						"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
						" and exists ( select 1 from tmmsmgy05 t2 where t1.heat_no=t2.heat_no  and t2.VALID_FLAG_2='1' and t2.STAT_DATE=@stat_date)"
						" union all"
						" select stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,' ' as LOT_NO,substr(recv_mat_time,1,8) as prod_date,OUT_STOCK_WT as devo_wt"
						" from tmmsm56 t1"
						" where 1=1"
						"  and mat_code in (select  MAT_CODE FROM TMMSM50 WHERE QUALITY_FLAS != '1')"
						"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
						" and exists ( select 1 from tmmsmgy05 t2 where t1.heat_no=t2.heat_no  and t2.VALID_FLAG_2='1' and t2.STAT_DATE=@stat_date)"
						" union all"
						" select stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,LOT_NO,prod_date,0-devo_wt as devo_wt"
						" from tmmsm2a_send t1"
						" where 1=1"
						" and HANDLE_DIV NOT IN ( 'F','H')"
						" and RTN_FLAG = ' ' "
						" and send_flag = '1'"
						" and exists ( select 1 from tmmsmgy05 t2 where t1.heat_no=t2.heat_no  and t2.VALID_FLAG_2='1' and t2.STAT_DATE=@stat_date)"
						") t"
						" where 1=1"
						+sqlstr_where+
						" group by stat_date,sm_plan_nol2,heat_no,pono,st_no,dev_code,mat_code,LOT_NO,prod_date"
						" having sum(devo_wt)!=0"
						" order by heat_no,st_no,dev_code,mat_code,LOT_NO,prod_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("st_no", tmmsm2a_send["ST_NO"].ToString());
			cmd_inq.Parameters.Set("heat_no", tmmsm2a_send["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("dev_code", tmmsm2a_send["DEV_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_code", tmmsm2a_send["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("lot_no", tmmsm2a_send["LOT_NO"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
