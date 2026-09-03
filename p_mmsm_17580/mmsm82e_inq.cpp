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
BM2F_ENTERACE(mmsm82e_inq) 
int f_mmsm82e_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString heat_no = "";
	CString heatno_premelt = "";  	
	CString sqlstr_where = "";
	CString sqlstr_count = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal max_wt = 0;
	CDecimal min_wt = 0;
	
	CModel tmmsmgy05("TMMSMGY05");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_s(conn);
	CModel tmmsm56("TMMSM56");

	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	CDecimal cd_count = 0;
	//系统的分页类信息。
	CPageInfo pageInfo;

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
		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM") && bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
				current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
			}
		}
		else {
			record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		}


		begin_time = bcls_rec->Tables[0].Rows[0]["START_TIME_S"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["START_TIME_E"].ToString();
		max_wt = bcls_rec->Tables[0].Rows[0]["MAX_WT"].ToDecimal();
		min_wt = bcls_rec->Tables[0].Rows[0]["MIN_WT"].ToDecimal();
		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		heatno_premelt = bcls_rec->Tables[0].Rows[0]["HEATNO_PREMELT"].ToString();
		tmmsmgy05.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if ((begin_time.Trim() == "" || end_time.Trim() == "") && heat_no.Trim() == "")
		{
			sprintf(s.msg, "必须传入时间或是炉号。");
			//strcpy(s.sysmsg,s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		
		if (tmmsmgy05["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and t.heat_no like trim(@heat_no)||'%'";
		}
		if (end_time.Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and t.start_time <=@end_time";
		}
		if (begin_time.Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and t.start_time >=@begin_time";
		} 

		sqlstr_count = " select count(1) from tmmsmgy05 t where 1=1";
		sqlstr_count = sqlstr_count + sqlstr_where;
		Log::Info("", __FUNCTION__, "sqlstr_count =[{0}]", sqlstr_count);
		cmd_inq.SetCommandText(sqlstr_count);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("heat_no", tmmsmgy05["HEAT_NO"].ToString());			
		cd_count = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}

		sqlstr = " SELECT SM_PLAN_NOL2,HEAT_NO,ST_NO,DEV_CODE,START_TIME,END_TIME,MOLTIRON_WT,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3,HJ_WT,FL_WT,ALL_WT,prod_wt,HJ_WT_S,FL_WT_S,ALL_WT_S,VALID_FLAG_2,STAT_DATE,RECV_MAT_TIME,CAST_DIV_NO,TD_NO_1 "
			" from ("
			" select SM_PLAN_NOL2,HEAT_NO,ST_NO,DEV_CODE,START_TIME,END_TIME,MOLTIRON_WT,HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,WEIGHT_PREMELT1,WEIGHT_PREMELT2,WEIGHT_PREMELT3,VALID_FLAG_2,STAT_DATE,RECV_MAT_TIME,CAST_DIV_NO,TD_NO_1 "
			" ,nvl((select sum(case when mat_code in (select mat_code from tmmsm50 where mat_type in ('1','2','4') and SEND_FLAG != '1') then DEVO_WT else 0 end)/1000 from tmmsmgy08 t2 where t.heat_no=t2.heat_no),0) HJ_WT"
			" ,nvl((select sum(case when mat_code in (select mat_code from tmmsm50 where mat_type in ('3')  and SEND_FLAG != '1') then DEVO_WT else 0 end)/1000 from tmmsmgy08 t2 where t.heat_no=t2.heat_no),0) FL_WT"
			",nvl((select sum(case when mat_code in (select mat_code from tmmsm50 where  SEND_FLAG != '1') then DEVO_WT else 0 end)/1000 from tmmsmgy08 t2 where t.heat_no=t2.heat_no),0) ALL_WT"
			" ,nvl((select sum(case when mat_code in (select mat_code from tmmsm50 where mat_type in ('1','2','4') and SEND_FLAG != '1') then DEVO_WT else 0 end)/1000 from tmmsm2a_send t2 where t2.RTN_FLAG!='1' and t2.send_flag = '1' and HANDLE_DIV = 'I' and t2.heat_no=t.heat_no),0) HJ_WT_S"
			" ,nvl((select sum(case when mat_code in (select mat_code from tmmsm50 where mat_type in ('3')  and SEND_FLAG != '1') then DEVO_WT else 0 end)/1000 from tmmsm2a_send t2 where t2.RTN_FLAG!='1' and t2.send_flag = '1' and HANDLE_DIV = 'I' and t2.heat_no=t.heat_no),0) FL_WT_S"
			",nvl((select sum(case when mat_code in (select mat_code from tmmsm50 where  SEND_FLAG != '1') then DEVO_WT else 0 end)/1000 from tmmsm2a_send t2 where t2.RTN_FLAG!='1' and t2.send_flag = '1' and HANDLE_DIV = 'I' and  t2.heat_no=t.heat_no),0) ALL_WT_S"
			",nvl((select sum(mat_act_wt) from ( select mat_act_wt from tmmsm01 t2 where t.heat_no=t2.heat_no union all select mat_act_wt from hmmsm01 t2 where  mat_no not in (select IN_MAT_NO from tmmsm35 ) and t.heat_no=t2.heat_no)),0) prod_wt"
			" from tmmsmgy05 t"
			" where 1=1"
			;
		sqlstr = sqlstr + sqlstr_where +" ) where 1=1";
		if (min_wt != 0)
		{
			sqlstr = sqlstr + " and ALL_WT<=@min_wt";
		}
		if (max_wt != 0)
		{
			sqlstr = sqlstr + " and ALL_WT>=@max_wt";
		}
		if (heatno_premelt.Trim() != "")
		{
			sqlstr = sqlstr + " and (HEATNO_PREMELT1 like @heatno_premelt||'%' or HEATNO_PREMELT2 like @heatno_premelt||'%' or HEATNO_PREMELT3 like @heatno_premelt||'%' or HEATNO_PREMELT4  like @heatno_premelt||'%')";
		}

		if (min_wt != 0)
		{
			sqlstr = sqlstr + " ORDER BY  ALL_WT ";
		}
		else
		{
			sqlstr = sqlstr + " ORDER BY  ALL_WT desc";
		}
		Log::Info("", __FUNCTION__, "max_wt =[{0}]", max_wt);
		Log::Info("", __FUNCTION__, "min_wt =[{0}]", min_wt);
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("min_wt", min_wt);
		cmd_inq.Parameters.Set("max_wt", max_wt);
		cmd_inq.Parameters.Set("heatno_premelt", heatno_premelt);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();


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
