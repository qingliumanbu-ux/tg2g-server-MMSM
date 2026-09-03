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
BM2F_ENTERACE(mmsm82k_inq)
int f_mmsm82k_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString stat_date = "";
	CString type_flag = "";
	CString heat_flag = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDbCommand cmd_inq(conn);
	int blkNum = 0;

	CModel tmmsm2a_send("TMMSM2A_SEND");  	

	try
	{ 	

		stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6); 	
		heat_flag = bcls_rec->Tables[0].Rows[0]["HEAT_FLAG"].ToString();
		tmmsm2a_send.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (heat_flag == "1")
		{
			sqlstr = "select heat_no, ST_NO,sum(MAT_ACT_WT) MAT_ACT_WT,sum(SH_WT) SH_WT,sum(HJ_WT) HJ_WT,sum(FL_WT) FL_WT,SUM(HJ_WT_FT) HJ_WT_FT,SUM(HJ_WT_TL) HJ_WT_TL"
				" ,decode(sum(MAT_ACT_WT),0,0,round(sum(HJ_WT)/sum(MAT_ACT_WT),3)) RATE  "
				" ,decode(sum(SH_WT),0,0,round(sum(HJ_WT)/sum(SH_WT),3)) RATE_SH  "
				" from ("
				"  select st_no,heat_no, sum(MAT_ACT_WT) MAT_ACT_WT,0 SH_WT,0 HJ_WT,0 FL_WT,0 HJ_WT_FT,0 HJ_WT_TL from VMMSMCPCL_BB1 t1 "
				" where  EXISTS (select 1 from tmmsmgy05 t2 where t2.heat_no = t1.heat_no and t2.STAT_DATE = @stat_date)"
				" group by st_no,heat_no"
				" union all"
				" select st_no,heat_no, 0 MAT_ACT_WT,sum(MAT_ACT_WT) SH_WT,0 HJ_WT,0 FL_WT,0 HJ_WT_FT,0 HJ_WT_TL from tmmsm56b t1 "
				" where  EXISTS (select 1 from tmmsmgy05 t2 where t2.heat_no = t1.heat_no and t2.STAT_DATE = @stat_date)"
				" group by st_no,heat_no"
				" union all"
				" select st_no,heat_no, 0 MAT_ACT_WT,0 SH_WT"
				" ,sum(case when mat_code in(select mat_code from tmmsm50 where mat_type in('1', '2', '4') and SEND_FLAG != '1') then DEVO_WT else 0 end) / 1000  HJ_WT"
				" ,sum(case when mat_code in(select mat_code from tmmsm50 where mat_type in('3') and SEND_FLAG != '1') then DEVO_WT else 0 end) / 1000  FL_WT"
				" ,sum(case when HANDLE_DIV='F' and mat_code in(select mat_code from tmmsm50 where mat_type in('1', '2', '4') and SEND_FLAG != '1') then DEVO_WT else 0 end) / 1000  HJ_WT_FT"
				" ,sum(case when  HANDLE_DIV='I' and mat_code in(select mat_code from tmmsm50 where mat_type in('1', '2', '4') and SEND_FLAG != '1') then DEVO_WT else 0 end) / 1000  HJ_WT_TL"
				" from TMMSM2A_SEND t1 "
				" where  1=1"
				;
			if (tmmsm2a_send["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr = sqlstr +
					" and MAT_CODE like @mat_code||'%' ";
			}
			sqlstr = sqlstr +
				" and SEND_FLAG = '1' and  RTN_FLAG != '1'"
				" and EXISTS (select 1 from tmmsmgy05 t2 where t2.heat_no = t1.heat_no and t2.STAT_DATE = @stat_date)"
				" group by st_no,heat_no"
				")"
				" where 1=1"
				;
			if (tmmsm2a_send["HEAT_NO"].ToString().Trim() != "")
				sqlstr = sqlstr + " and heat_no=@heat_no";		
			if (tmmsm2a_send["ST_NO"].ToString().Trim() != "")
				sqlstr = sqlstr + " and st_no=@st_no";
			sqlstr = sqlstr +  	" group by st_no,heat_no"					
				" order by ST_NO ,heat_no "
				;
			Log::Info("", __FUNCTION__, "sqlstr = [{0}]", sqlstr); 
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("st_no", tmmsm2a_send["ST_NO"].ToString());
			cmd_inq.Parameters.Set("heat_no", tmmsm2a_send["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("mat_code", tmmsm2a_send["MAT_CODE"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		else
		{  
			sqlstr = "select  ST_NO,sum(MAT_ACT_WT) MAT_ACT_WT,sum(SH_WT) SH_WT,sum(HJ_WT) HJ_WT,sum(FL_WT) FL_WT,SUM(HJ_WT_FT) HJ_WT_FT,SUM(HJ_WT_TL) HJ_WT_TL"
				" ,decode(sum(MAT_ACT_WT),0,0,round(sum(HJ_WT)/sum(MAT_ACT_WT),3)) RATE  "
				" ,decode(sum(SH_WT),0,0,round(sum(HJ_WT)/sum(SH_WT),3)) RATE_SH  "
				" from ("
				"  select st_no, sum(MAT_ACT_WT) MAT_ACT_WT,0 SH_WT,0 HJ_WT,0 FL_WT,0 HJ_WT_FT,0 HJ_WT_TL from VMMSMCPCL_BB1 t1 "
				" where  EXISTS (select 1 from tmmsmgy05 t2 where t2.heat_no = t1.heat_no and t2.STAT_DATE = @stat_date)"
				" group by st_no"
				" union all"
				" select st_no, 0 MAT_ACT_WT,sum(MAT_ACT_WT) SH_WT,0 HJ_WT,0 FL_WT,0 HJ_WT_FT,0 HJ_WT_TL from tmmsm56b t1 "
				" where  EXISTS (select 1 from tmmsmgy05 t2 where t2.heat_no = t1.heat_no and t2.STAT_DATE = @stat_date)"
				" group by st_no"
				" union all"
				" select st_no, 0 MAT_ACT_WT,0 SH_WT"
				" ,sum(case when mat_code in(select mat_code from tmmsm50 where mat_type in('1', '2', '4') and SEND_FLAG != '1') then DEVO_WT else 0 end) / 1000  HJ_WT"
				" ,sum(case when mat_code in(select mat_code from tmmsm50 where mat_type in('3') and SEND_FLAG != '1') then DEVO_WT else 0 end) / 1000  FL_WT"
				" ,sum(case when HANDLE_DIV='F' and mat_code in(select mat_code from tmmsm50 where mat_type in('1', '2', '4') and SEND_FLAG != '1') then DEVO_WT else 0 end) / 1000  HJ_WT_FT"
				" ,sum(case when  HANDLE_DIV='I' and mat_code in(select mat_code from tmmsm50 where mat_type in('1', '2', '4') and SEND_FLAG != '1') then DEVO_WT else 0 end) / 1000  HJ_WT_TL"	
				" from TMMSM2A_SEND t1 "
				" where  1=1"
				;
			if (tmmsm2a_send["MAT_CODE"].ToString().Trim() != "")
			{
				sqlstr = sqlstr +
					" and MAT_CODE like @mat_code||'%' ";
			}
			sqlstr = sqlstr +
				" and SEND_FLAG = '1' and  RTN_FLAG != '1'"
				" and EXISTS (select 1 from tmmsmgy05 t2 where t2.heat_no = t1.heat_no and t2.STAT_DATE = @stat_date)"
				" group by st_no"
				")"
				" where 1=1"
				;
			if (tmmsm2a_send["HEAT_NO"].ToString().Trim() != "")
				sqlstr = sqlstr + " and heat_no=@heat_no";
			if (tmmsm2a_send["ST_NO"].ToString().Trim() != "")
				sqlstr = sqlstr + " and st_no=@st_no";
			sqlstr = sqlstr + " group by st_no"
				" order by ST_NO  "
				;
			Log::Info("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("st_no", tmmsm2a_send["ST_NO"].ToString());
			cmd_inq.Parameters.Set("mat_code", tmmsm2a_send["MAT_CODE"].ToString());
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
