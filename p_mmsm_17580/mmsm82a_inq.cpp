/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 炉次信息
中频炉：tmmsm19
转炉:tmmsm21
电炉:tmmsm20
AOD:tmmsm27
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82a_inq)
int f_mmsm_21b006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82a_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString heat_no = "";
	CString heat_no_cx = "0";
	CString sh_flag = "0";
	CString cx_date = "";
	CString diffe_wt_flag = "0";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsm2a_send("TMMSM2A_SEND");

	try
	{ 	

		begin_time = bcls_rec->Tables[0].Rows[0]["START_TIME_S"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["START_TIME_E"].ToString();
		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		heat_no_cx = bcls_rec->Tables[0].Rows[0]["CX"].ToString();
		diffe_wt_flag = bcls_rec->Tables[0].Rows[0]["DIFFE_WT_FLAG"].ToString();
		sh_flag = bcls_rec->Tables[0].Rows[0]["SH_FLAG"].ToString();
		cx_date = bcls_rec->Tables[0].Rows[0]["CX_DATE"].ToString().SubstringNE(0,8);
		tmmsmgy05.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (tmmsmgy05["HEAT_NO"].ToString().Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and heat_no like trim(@heat_no)||'%'";
		} 	
		if (tmmsmgy05["ST_NO"].ToString().Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and st_no like trim(@st_no)||'%'";
		}
		if (tmmsmgy05["CC_NO"].ToString().Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and cc_no =@cc_no";
		}   		
		if (tmmsmgy05["VALID_FLAG_1"].ToString()== "1")
		{
			sqlstr_where = sqlstr_where + " and VALID_FLAG_1 ='1'";
		}
		if (tmmsmgy05["VALID_FLAG_1"].ToString() == "0")
		{
			sqlstr_where = sqlstr_where + " and VALID_FLAG_1 in ('0',' ')";
		} 		
		
		if (heat_no_cx == "1")
		{
			sqlstr = " select BOF0_S,BOF0_E,BOF1_S,BOF1_E,BOF2_S,BOF2_E,BOF9_S,BOF9_E,AOD0_S,AOD0_E,AOD1_S,AOD1_E,AOD2_S,AOD2_E,AOD6_S,AOD6_E"
				" from V_DA_HEAT_S_E1"
				" where 1=1"
				" and WEEK_DAY =@cx_date"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("cx_date", cx_date);
			cmd_inq_1.ExecuteReader();
			
			if (cmd_inq_1.Read())
			{ 
				sqlstr_where = sqlstr_where + " and (  ";
				if (cmd_inq_1.GetString(1).Trim() != ""&&cmd_inq_1.GetString(2).Trim() != "")
				{
					sqlstr_where = sqlstr_where + "  (heat_no <= '" + cmd_inq_1.GetString(2) + "' and heat_no >= '" + cmd_inq_1.GetString(1) + "')";
				}
				if (cmd_inq_1.GetString(3).Trim() != ""&&cmd_inq_1.GetString(4).Trim() != "")
				{
					sqlstr_where = sqlstr_where + " or (heat_no <= '" + cmd_inq_1.GetString(4) + "' and heat_no >= '" + cmd_inq_1.GetString(3) + "')";
				}
				if (cmd_inq_1.GetString(5).Trim() != ""&&cmd_inq_1.GetString(6).Trim() != "")
				{
					sqlstr_where = sqlstr_where + " or (heat_no <= '" + cmd_inq_1.GetString(6) + "' and heat_no >= '" + cmd_inq_1.GetString(5) + "')";
				}
				if (cmd_inq_1.GetString(7).Trim() != ""&&cmd_inq_1.GetString(8).Trim() != "")
				{
					sqlstr_where = sqlstr_where + " or (heat_no <= '" + cmd_inq_1.GetString(8) + "' and heat_no >= '" + cmd_inq_1.GetString(7) + "')";
				}
				if (cmd_inq_1.GetString(9).Trim() != ""&&cmd_inq_1.GetString(10).Trim() != "")
				{
					sqlstr_where = sqlstr_where + " or (heat_no <= '" + cmd_inq_1.GetString(10) + "' and heat_no >= '" + cmd_inq_1.GetString(9) + "')";
				}
				if (cmd_inq_1.GetString(11).Trim() != ""&&cmd_inq_1.GetString(12).Trim() != "")
				{
					sqlstr_where = sqlstr_where + " or (heat_no <= '" + cmd_inq_1.GetString(12) + "' and heat_no >= '" + cmd_inq_1.GetString(11) + "')";
				}
				if (cmd_inq_1.GetString(13).Trim() != ""&&cmd_inq_1.GetString(14).Trim() != "")
				{
					sqlstr_where = sqlstr_where + " or (heat_no <= '" + cmd_inq_1.GetString(14) + "' and heat_no >= '" + cmd_inq_1.GetString(13) + "')";
				}
				if (cmd_inq_1.GetString(15).Trim() != ""&&cmd_inq_1.GetString(16).Trim() != "")
				{
					sqlstr_where = sqlstr_where + " or (heat_no <= '" + cmd_inq_1.GetString(16) + "' and heat_no >= '" + cmd_inq_1.GetString(15) + "')";
				}
				sqlstr_where = sqlstr_where + ")";
			}
			cmd_inq_1.Close();
		
		}
		

			if (end_time.Trim() != "")
			{
				if (sh_flag == "1")
				{
					sqlstr_where = sqlstr_where + " and RECV_MAT_TIME <=@end_time";
				}
				else
				{
					sqlstr_where = sqlstr_where + " and CC_START_TIME <=@end_time";
				}

			}
			if (begin_time.Trim() != "")
			{
				if (sh_flag == "1")
				{
					sqlstr_where = sqlstr_where + " and RECV_MAT_TIME >=@begin_time";
				}
				else
				{
					sqlstr_where = sqlstr_where + " and CC_START_TIME >=@begin_time";
				}

			}
		

		sqlstr = " select t.*"
			" ,nvl((select count(1) from tmmsm01 t2 where t.heat_no = t2.heat_no),0) mat_count"
			" ,nvl((select count(1) from tmmsm01 t2 where  RECV_MAT_TIME=' ' and t.heat_no = t2.heat_no),0) recv_count"
			" ,nvl((select sum(case when mat_code in (select mat_code from tmmsm50 where mat_type in ('1','2','4') and SEND_FLAG != '1') then DEVO_WT else 0 end)/1000 from tmmsmgy08 t2 where t.heat_no=t2.heat_no),0) HJ_WT"
			" ,nvl((select sum(RECEIVE_WEIGHT) from (select sum(RECEIVE_WEIGHT) RECEIVE_WEIGHT from tmmsm01 t2 where  t.heat_no = t2.heat_no and t2.concess_con_flag!='1' union ALL select sum(RECEIVE_WEIGHT) RECEIVE_WEIGHT from hmmsm01 t2 where  mat_no not in (select IN_MAT_NO from tmmsm35 ) and t2.concess_con_flag!='1' and  t.heat_no = t2.heat_no)),0) mat_act_wt"
			" from tmmsmgy05 t"
			" where 1=1"
			;
		if (tmmsmgy05["VALID_FLAG_1"].ToString() == "3")	//差异
		{
			sqlstr = sqlstr + " and heat_no in (select heat_no from ( "
				" select heat_no,dev_code,ST_NO,MAT_CODE,LOT_NO,sum(DEVO_WT) DEVO_WT"
				" from ("
				" select heat_no,dev_code,ST_NO,MAT_CODE,LOT_NO,OUT_STOCK_WT DEVO_WT"
				" from tmmsm56"
				" where 1=1"
				"  and mat_code in (select  MAT_CODE FROM TMMSM50 WHERE QUALITY_FLAS = '1')"
				"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
				" and heat_no in (select heat_no from  tmmsmgy05 where VALID_FLAG_1 = '1'" + sqlstr_where + ")"
				" union all"
				" select heat_no,dev_code,ST_NO,MAT_CODE,' ' LOT_NO,OUT_STOCK_WT DEVO_WT"
				" from tmmsm56"
				" where 1=1"
				"  and mat_code in (select  MAT_CODE FROM TMMSM50 WHERE QUALITY_FLAS != '1')"
				"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
				" and heat_no in (select heat_no from  tmmsmgy05 where VALID_FLAG_1 = '1'" + sqlstr_where + ")"
				" union all"
				" select heat_no,dev_code,ST_NO,MAT_CODE,LOT_NO,0-DEVO_WT as DEVO_WT"
				" from tmmsm2a_send"
				" where 1=1"
				" and HANDLE_DIV!='F'"
				" and SEND_FLAG = '1' and  RTN_FLAG = ' '"
				" and heat_no in (select heat_no from  tmmsmgy05 where VALID_FLAG_1 = '1'" + sqlstr_where + ")"
				" )"
				" group by heat_no,dev_code,ST_NO,MAT_CODE,LOT_NO"
				" having sum(DEVO_WT)!=0"
				"))"
				;
		}
		if (diffe_wt_flag == "1")
		{
			sqlstr = sqlstr + " and heat_no in (select distinct t.HEAT_NO from VMMSMCPCL_BB t,(select s.heat_no,sum(s.mat_act_wt) mat_act_wt from (select distinct s0.st_no,s0.heat_no,s0.mat_act_wt from tmmsm2a_send s0) s group by s.heat_no) S1 where t.HEAT_NO=S1.heat_no and t.MAT_ACT_WT!=S1.mat_act_wt) ";
		}
		sqlstr = sqlstr +  sqlstr_where	 +
			" order by CC_START_TIME desc"
			;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("heat_no", tmmsmgy05["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("st_no", tmmsmgy05["ST_NO"].ToString());
			cmd_inq.Parameters.Set("cc_no", tmmsmgy05["CC_NO"].ToString());
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
