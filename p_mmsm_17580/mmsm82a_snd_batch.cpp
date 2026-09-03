/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2018-06-21
Description:批量执行抛帐
**************************************************/
//框架用头文件
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82a_snd_batch)
//-EP_SYSTEM_HEAD_END
int f_mmsm_21c005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_t82304_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送全程工艺路径
int f_t8f003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送能源消耗数据
int f_mmsm_t80r91_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送炉成本
int f_mmsm82a_snd_batch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	int i_idx = 0;
	CString sqlstr = "";
	CString stat_date = "";
	CString begin_time = "";
	CString end_time = "";
	CString end_day = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal i_count = 0;
	int j = 0;
	CString seq_id = "0";
	CString send_flag = "0";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm2a_send("TMMSM2A_SEND");
	CModel tmmsmgy06_sed("TMMSMGY06_SED");

	EIClass bcls_ret4;



	CString deal_flag = "";

	begin_time = CDateTime::Now().AddHours(-3).ToString("yyyyMMddHHmmss");
	end_time = CDateTime::Now().AddHours(-2).ToString("yyyyMMddHHmmss");
	end_day = CDateTime::Now().AddDays(-32).ToString("yyyyMMddHHmmss");
	Log::Info("", __FUNCTION__, "begin_time =[{0}],end_time=[{1}]", begin_time, end_time);

	try
	{
		//当天收货的数据重新核算一把
		sqlstr = " select distinct heat_no from ("
			" select heat_no from tmmsm01 where recv_mat_time<=@end_time and recv_mat_time>=@begin_time  "
			" union "
			" select heat_no from hmmsm01 where recv_mat_time<=@end_time and recv_mat_time>=@begin_time  "
		    " union "
			" select heat_no from tmmsmgy05 where CC_START_TIME>=@end_day and recv_mat_time = ' '"
			") t"
			" where not exists (select 1 from tmmsm01 t2 where recv_mat_time=' '  and  CONCESS_CON_FLAG!='1' and t2.heat_no=t.heat_no)"
			;			
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("end_day", end_day);
		cmd_inq.ExecuteQuery(bcls_ret4.Tables[0]);
		cmd_inq.Close();

		EIClass bcls_ret1;
		EIClass bcls_rec1;
		bcls_rec1.Tables[0].Columns.Add(tmmsmgy05);
		bcls_rec1.Tables[0].Rows.Clear();
		bcls_rec1.Tables[0].Rows.Add();
		for (int i = 0; i < bcls_ret4.Tables[0].Rows.get_Count(); i++)
		{
			bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = bcls_ret4.Tables[0].Rows[i]["HEAT_NO"].ToString();
			doFlag = f_mmsm_gyupd(&bcls_rec1, &bcls_ret1, conn);
			if (doFlag < 0)
			{
				//throw CApplicationException(-1, s.msg, log.Location);
			}
			tpcommit(0);
			tpbegin(0, 0);
		}		

		//判断钢种是否一致，如果不一致则重新计算
		sqlstr = " select distinct heat_no from ("
			" select heat_no,st_no,sum(mat_act_wt)"
			" from ("
			" select heat_no,st_no,sum(RECEIVE_WEIGHT) mat_act_wt from VMMSMCPCL_BB1 where  heat_no in (select heat_no from tmmsmgy05 where STAT_DATE = @stat_date ) group by heat_no,st_no "			
			" union all"
			" select heat_no,st_no,0-mat_act_wt from tmmsm56b where STAT_DATE = @stat_date  "
			" )"
			" group by heat_no,st_no having sum(mat_act_wt)!=0"
			" )"
			;
		bcls_ret4.Tables[0].Rows.Clear();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", begin_time.SubstringNE(0,6));
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteQuery(bcls_ret4.Tables[0]);
		cmd_inq.Close();

		for (int i = 0; i < bcls_ret4.Tables[0].Rows.get_Count(); i++)
		{
			bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = bcls_ret4.Tables[0].Rows[i]["HEAT_NO"].ToString();
			doFlag = f_mmsm_gyupd(&bcls_rec1, &bcls_ret1, conn);
			if (doFlag < 0)
			{
				//throw CApplicationException(-1, s.msg, log.Location);
			}
			tpcommit(0);
			tpbegin(0, 0);
		}

		//更新质量的不一致
		sqlstr = " update tmmsmgy05 set SEND_T823 = '2'  "
			" where heat_no in ( select heat_no from ("
			" select HEAT_NO,st_no,mat_code,sum(DEVO_WT) DEVO_WT from ("
			"SELECT HEAT_NO,st_no,mat_code,sum(OUT_STOCK_WT) DEVO_WT "
			"  FROM tmmsm56 t1"
			" where 1=1"
			" and exists (select 1 from tmmsmgy05 t2 where t2.STAT_DATE = @stat_date and t1.heat_no=t2.heat_no)"
			"  group by HEAT_NO,st_no,mat_code "
			"  union all "
			"  select  HEAT_NO,st_no,mat_code, 0-sum(DEVO_WT) DEVO_WT "
			"  from HMMSM2A  t1"
			"  where 1=1"
			"  and (HEAT_NO,SUBSTR(REC_CREATE_TIME, 0, 10)) in "
			"  (select HEAT_NO,MAX(SUBSTR(REC_CREATE_TIME, 0, 10)) REC_CREATE_TIME "
			"  from HMMSM2A t3"
			"  where REC_CREATE_TIME != ' ' "
			"  and exists (select 1 from tmmsmgy05 t2 where t2.STAT_DATE = @stat_date and t3.heat_no=t2.heat_no)"
			"  group by HEAT_NO )  "
			"  and exists (select 1 from tmmsmgy05 t2 where t2.STAT_DATE = @stat_date and t1.heat_no=t2.heat_no)"
			"  group by HEAT_NO, st_no, mat_code"
			" )"
			"  group by HEAT_NO, st_no, mat_code"
			" having sum(DEVO_WT)!=0"
			" )"
			" )"
			" and SEND_T823 = '1'"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", begin_time.SubstringNE(0, 6));
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();		


		////抛送实际路径
		//sqlstr = " select heat_no from tmmsm31 where START_TIME>'202408'"				
		//	;
		//EIClass bcls_ret2;
		//EIClass bcls_rec2;
		//bcls_rec2.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		//bcls_rec2.Tables[0].Rows.Add();
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("begin_time", begin_time);
		//cmd_inq.Parameters.Set("end_time", end_time);
		//cmd_inq.ExecuteReader();
		//while (cmd_inq.Read())
		//{
		//	bcls_rec2.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
		//	doFlag = f_t82304_snd(&bcls_rec2, &bcls_ret2, conn);
		//	if (doFlag < 0)
		//	{
		//		Log::Trace("", __FUNCTION__, "-------调用f_t82304_snd失败-------");
		//		//throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//}
		//cmd_inq.Close();
		
		//抛送能源消耗
		sqlstr = " select heat_no from tmmsmgy05"
			" where 1=1"
			" and RECV_MAT_TIME<=@end_time"
			" and RECV_MAT_TIME>=@begin_time"
			" order by RECV_MAT_TIME"
			;
		EIClass bcls_ret3;
		EIClass bcls_rec3;
		bcls_rec3.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_rec3.Tables[0].Rows.Add();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteQuery(bcls_rec3.Tables[0]);
		doFlag = f_t8f003_snd(&bcls_rec3, &bcls_ret3, conn);
		if (doFlag < 0)
		{
			Log::Trace("", __FUNCTION__, "-------调用f_t8f003_snd失败-------");
			//throw CApplicationException(-1, s.msg, log.Location);
		}
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
