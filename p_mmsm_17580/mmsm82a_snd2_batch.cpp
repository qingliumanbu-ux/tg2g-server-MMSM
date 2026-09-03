/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2018-06-21
Description:自动抛实绩消耗
**************************************************/
//框架用头文件
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82a_snd2_batch)
//-EP_SYSTEM_HEAD_END
int f_mmsm_21c005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_210049_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn); //给L4的过钢量
int f_t82304_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送全程工艺路径
int f_mmsm82a_snd2_batch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal i_count = 0;
	int j = 0;
	CString seq_id = "0";
	CString deal_flag = "I";
	CString send_flag = "0";
	CString confrm_auto_flag = "0";
	CString upload_auto_flag = "0";
	CString recv_mat_time = " ";

	CString heat_no = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq_s(conn);
	CDbCommand cmd_2a(conn);

	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm2a_send("TMMSM2A_SEND");
	CModel tmmsmgy06_sed("TMMSMGY06_SED");



	//当前时间前4个小时
	begin_time = CDateTime::Now().AddDays(-31).ToString("yyyyMMddHHmmss");  
	end_time = CDateTime::Now().AddHours(-4).ToString("yyyyMMddHHmmss");

	Log::Info("", __FUNCTION__, "begin_time =[{0}],end_time=[{1}]", begin_time, end_time);


	EIClass bcls_ret1;
	EIClass bcls_rec1;
	blkNum = bcls_rec1.Tables.IndexOf("MMSMSND");
	if (blkNum < 0)
	{
		bcls_rec1.Tables.Add("MMSMSND");
	}
	bcls_rec1.Tables["MMSMSND"].Columns.Add(tmmsmgy06);
	bcls_rec1.Tables["MMSMSND"].Rows.Clear();
	if (!bcls_rec1.Tables["MMSMSND"].Columns.Contains("DEAL_FLAG"))
	{
		bcls_rec1.Tables["MMSMSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
	}

	EIClass bcls_ret2;
	EIClass bcls_rec2;
	bcls_rec2.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec2.Tables[0].Rows.Add();

	//发送消耗-资源/铁区
	blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MMLCSND");
	}
	bcls_rec->Tables["MMLCSND"].Columns.Add(tmmsm2a_send);
	bcls_rec->Tables["MMLCSND"].Rows.Clear();

	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
	}


	try
	{
		//自动开关 0-关 1-开
		sqlstr = " SELECT CODE1 FROM TMMSMZD WHERE CODE ='MMLC01'";
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.ExecuteReader();
		if (cmd_inq1.Read())
		{
			confrm_auto_flag = cmd_inq1.GetString(1);
		}
		cmd_inq1.Close();
		sqlstr = " SELECT CODE1 FROM TMMSMZD WHERE CODE ='MMLC02'";
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.ExecuteReader();
		if (cmd_inq1.Read())
		{
			upload_auto_flag = cmd_inq1.GetString(1);
		}
		cmd_inq1.Close();

		if (confrm_auto_flag == "1")
		{
			//20241211 设置工艺自动确认
			sqlstr = " update tmmsmgy06 t1 set AFFIRM_FLAG ='1',AFFIRM_TIME = @datetime"
				" where 1=1"
				" and exists ( select 1 from tmmsmgy05 t2 where 1=1 and t2.heat_no=t1.heat_no"
				" and t2.stat_date !=' ' "
				" and t2.AFFIRM_FLAG !='1'"
				" and t2.CC_START_TIME<=@end_time"
				" and t2.CC_START_TIME>=@begin_time"
				")"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("datetime", datetime);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " update tmmsmgy05 set LOCK_FLAG = 'Y',AFFIRM_FLAG ='1',AFFIRM_TIME = @datetime"
				" where 1=1"
				" and stat_date !=' ' "
				" and AFFIRM_FLAG !='1'"
				" and CC_START_TIME<=@end_time"
				" and CC_START_TIME>=@begin_time"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("datetime", datetime);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			tpcommit(0);
			tpbegin(0, 0);
		}
		
		EIClass bcls_ret1;
		//根据钢包到达时间的前几个小时
		sqlstr = " select heat_no,recv_mat_time from tmmsmgy05"
			" where 1=1" 			
			" and stat_date !=' ' "
			" and VALID_FLAG_1 != '1'"
			" and AFFIRM_FLAG ='1'"				
			" and CC_START_TIME<=@end_time"
			" and CC_START_TIME>=@begin_time"
			" order by CC_START_TIME"
			;  		
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.Parameters.Set("begin_time", begin_time);
		cmd_inq1.Parameters.Set("end_time", end_time);
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.ExecuteQuery(bcls_ret1.Tables[0]);
		cmd_inq1.Close();
		for (int i = 0; i < bcls_ret1.Tables[0].Rows.get_Count(); i++)
		{
			heat_no = bcls_ret1.Tables[0].Rows[i]["HEAT_NO"].ToString();
			recv_mat_time = bcls_ret1.Tables[0].Rows[i]["RECV_MAT_TIME"].ToString();
			deal_flag = "I";
			send_flag = "1"; 

			if (datetime.Substring(0, 6) > recv_mat_time.Substring(0, 6))
			{
				if (CDateTime::Now().AddMonths(-1).ToString("yyyyMM") > recv_mat_time.SubstringNE(0,6) || datetime.Substring(6, 2) != "01")
				{
					//判断如果是本月发送的
					send_flag = "0";
				}
			}

			//判断是否是否全部收货完成
			sqlstr = 
				" select mat_no from tmmsm01 where recv_mat_time=' ' and  heat_no=@heat_no "
				" union "
				" select mat_no from hmmsm01 where recv_mat_time = ' ' and  heat_no = @heat_no "
			
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				send_flag = "0";
			}
			cmd_inq.Close();

			//判断如果需要的批次号没有全部批次号，则不能发送
			sqlstr = " select count(1)"
				" from tmmsm56"
				" where  1=1"
				" and lot_no=' '"
				" and mat_code in (select mat_code from tmmsm50 where QUALITY_FLAS='1')"
				" and heat_no = @heat_no	"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if (cmd_inq.GetDecimal(1) >0)
				{
					send_flag = "0";
				}
			}
			cmd_inq.Close();

			if (send_flag == "1"&& upload_auto_flag == "1")
			{
				Log::Info("", __FUNCTION__, "heat_no =[{0}],recv_mat_time=[{1}]", heat_no, recv_mat_time);

				//发送过钢量
				sqlstr = " select * from tmmsmgy06A"
					" where 1=1"
					" and substr(dev_code,1,1) not in ('C','H','M','S','D')"
					" and heat_no = @heat_no"
					;
				//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				bcls_rec1.Tables["MMSMSND"].Rows.Clear();
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteQuery(bcls_rec1.Tables["MMSMSND"]);
				cmd_inq.Close();
				if (bcls_rec1.Tables["MMSMSND"].Rows.get_Count() > 0)
				{
					if (!bcls_rec1.Tables["MMSMSND"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec1.Tables["MMSMSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}
					bcls_rec1.Tables["MMSMSND"].Rows[0]["DEAL_FLAG"] = deal_flag;
					doFlag = f_mmsm_210049_snd(&bcls_rec1, &bcls_ret1, conn);
					if (doFlag < 0)
					{
						Log::Trace("", __FUNCTION__, "-------调用f_mmsm_210049_snd失败-------");
						throw CApplicationException(-1, s.msg, log.Location);
					}

					sqlstr = " update tmmsmgy06_sed set TC_SEND_FLAG = '1',DATI_MSG_SENT=@datetime"
						" where 1=1"
						" and TC_SEND_FLAG = ' '"
						" and heat_no = @heat_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", heat_no);
					cmd_inq.Parameters.Set("datetime", datetime);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}


				//分摊完成将结果插入到表
				sqlstr = " delete from tmmsm2a_send"
					" where 1=1"
					" and send_flag !='1'"
					" and heat_no=@heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = " select max(SEQ_NO_2A) "
					" from tmmsm2a_send"
					" where 1=1"
					" and HANDLE_DIV = 'I'"
					" and stat_date=@stat_date"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", recv_mat_time.SubstringNE(0, 6));
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					seq_id = cmd_inq.GetString(1).SubstringNE(8, 8);
				}
				cmd_inq.Close();

				if (seq_id.Trim() == "")
				{
					seq_id = "0";
				}

				Log::Info("", __FUNCTION__, "seq_id =[{0}]", seq_id);

				//更新批次号
				sqlstr = " update tmmsm56 t1 set LOT_NO = (select lot_no from vlotno t2 where t1.WEIGH_NO=t2.WEIGH_NO)"
					" where exists(select lot_no from vlotno t2 where t1.WEIGH_NO=t2.WEIGH_NO)"
					" and  heat_no=@heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = " insert into tmmsm2a_send(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,LOT_NO,DEVO_TIME,prod_date,devo_wt,SEQ_NO_2A,mat_name,SYSTEM_ID_MAT,HANDLE_DIV)"
					" select @rec_creator,@rec_create_time,t1.stat_date,t1.sm_plan_nol2,t1.heat_no,t1.L2_PROC_NO,t1.pono,t1.st_no,t1.dev_code,t1.mat_code,t1.LOT_NO,@rec_create_time,t1.prod_date,devo_wt ,'I'||@stat_date||trim(to_char(rownum+@seq_id, '00000000')),t2.mat_name,t2.SYSTEM_ID_MAT,'I' "
					" from ("
					" select stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,LOT_NO,prod_date,sum(devo_wt) devo_wt "
					" from "
					"("
					" select stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,LOT_NO,substr(recv_mat_time,1,8) as prod_date,OUT_STOCK_WT as devo_wt"
					" from tmmsm56"
					" where 1=1"
					"  and mat_code in (select  MAT_CODE FROM TMMSM50 WHERE QUALITY_FLAS = '1')"
					"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
					" and heat_no=@heat_no"
					" union all"
					" select stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,' ' as LOT_NO,substr(recv_mat_time,1,8) as prod_date,OUT_STOCK_WT as devo_wt"
					" from tmmsm56"
					" where 1=1"
					"  and mat_code in (select  MAT_CODE FROM TMMSM50 WHERE QUALITY_FLAS != '1')"
					"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
					" and heat_no=@heat_no"
					" union all"
					" select stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,LOT_NO,prod_date,0-devo_wt as devo_wt"
					" from tmmsm2a_send"
					" where 1=1"
					" and HANDLE_DIV NOT IN ( 'F','H')"
					" and RTN_FLAG != '1' "
					" and send_flag = '1'"
					" and heat_no=@heat_no"
					")"
					" group by stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,LOT_NO,prod_date"
					" having sum(devo_wt)!=0"
					") t1 left join tmmsm50 t2 on t1.mat_code= t2.mat_code"
					;
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", recv_mat_time.SubstringNE(0, 6));
				cmd_inq.Parameters.Set("rec_creator", s.userid);
				cmd_inq.Parameters.Set("rec_create_time", datetime);
				cmd_inq.Parameters.Set("seq_id", atol(seq_id));
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				//更新产量
				sqlstr = " update TMMSM2A_SEND t1 set mat_act_wt = (select sum(mat_act_wt) from tmmsm56b t2 where t1.st_no=t2.st_no and  t2.heat_no=@heat_no)"
					" where exists(select 1 from tmmsm56b t2 where t1.st_no=t2.st_no and  t2.heat_no=@heat_no)"
					" and  SEND_FLAG !='1'"
					" and HANDLE_DIV != 'F'"
					" and  heat_no=@heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();



				//给铁区发送
				sqlstr = " select * from TMMSM2A_SEND t1"
					" where 1=1"
					" and SYSTEM_ID_MAT = 'B'" //发送铁区	
					" and  SEND_FLAG !='1'"
					" and HANDLE_DIV != 'F'"
					" and heat_no = @heat_no"
					" order by heat_no,ST_NO,dev_code,MAT_CODE,DEVO_WT"
					;
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				bcls_rec->Tables["MMLCSND"].Rows.Clear();
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteQuery(bcls_rec->Tables["MMLCSND"]);
				cmd_inq.Close();
				if (bcls_rec->Tables["MMLCSND"].Rows.get_Count() > 0)
				{
					//Log::Trace("", __FUNCTION__, "   11111111111111]");
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}
					bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = deal_flag;
					doFlag = f_mmsm_21b006_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21b006_snd失败-------");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				//给资源发送
				sqlstr = " select * from TMMSM2A_SEND t1"
					" where 1=1"
					" and SYSTEM_ID_MAT = 'C'" //排除不发送数据	
					" and  SEND_FLAG !='1'"
					" and HANDLE_DIV != 'F'"
					" and heat_no = @heat_no"
					" order by heat_no,ST_NO,dev_code,MAT_CODE,DEVO_WT"
					;
				bcls_rec->Tables["MMLCSND"].Rows.Clear();
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteQuery(bcls_rec->Tables["MMLCSND"]);
				cmd_inq.Close();
				if (bcls_rec->Tables["MMLCSND"].Rows.get_Count() > 0)
				{
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}
					bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = deal_flag;
					doFlag = f_mmsm_21c005_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c005_snd失败-------");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				sqlstr = " update tmmsm2a_send set SEND_FLAG = '1',SEND_TIME=@datetime"
					" where 1=1"
					" and PROD_DATE!=' '"
					" and HANDLE_DIV != 'F'"
					" and SEND_FLAG !='1'"
					" and heat_no = @heat_no"
					;
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.Parameters.Set("datetime", datetime);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();


				//给智慧质量发送全程工艺路径
				bcls_rec2.Tables[0].Rows[0]["HEAT_NO"] = heat_no;
				doFlag = f_t82304_snd(&bcls_rec2, &bcls_ret2, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t82304_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				tmmsmgy05["HEAT_NO"] = heat_no;
				tmmsmgy05["VALID_FLAG_2"] = "1";
				tmmsmgy05["VALID_FLAG_1"] = "1";
				tmmsmgy05["VALID_FLAG_3"] = "1";
				tmmsmgy05.Update("VALID_FLAG_1,VALID_FLAG_2,VALID_FLAG_3", "HEAT_NO");
			}

			tpcommit(0);
			tpbegin(0, 0);
		}

		cmd_inq1.Close();

	


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
