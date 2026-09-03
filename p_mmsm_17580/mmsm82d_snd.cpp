/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 分摊量发送
中频炉：tmmsm19
转炉:tmmsm21
电炉:tmmsm20
AOD:tmmsm27
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

int f_mmsm_21c005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm82d_snd)
int f_mmsm82d_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	int i_idx = 0;
	CString sqlstr = "";
	CString stat_date = "";

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
	

	
	CString deal_flag =  "";
	
	

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
		deal_flag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();
		stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString();

		Log::Trace("", __FUNCTION__, " 日期= [{0}]", datetime.Substring(6, 2));

		//判断会计期后面不能发送
		if (datetime.Substring(0, 6) > stat_date)
		{
			if (CDateTime::Now().AddMonths(-1).ToString("yyyyMM") > stat_date || datetime.Substring(6, 2) != "01")
			{
				sprintf(s.msg, "该会计期:" + stat_date + "不在操作时间内");
				//strcpy(s.sysmsg,s.msg);
				throw CApplicationException(-1, s.msg, log.Location);

			}
		}

		
		
		if (deal_flag == "D")
		{
			
				//给铁区发送
				/*sqlstr = " select * from TMMSM2A_SEND t1"
					" where 1=1"
					" and SYSTEM_ID_MAT = 'B'"
					" and HANDLE_DIV = 'F'"
					" and  SEND_FLAG = '1'  and  RTN_FLAG = ' ' "
					" and stat_date = @stat_date"
					" order by DEVO_WT"
					;
				bcls_rec->Tables["MMLCSND"].Rows.Clear();
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.ExecuteQuery(bcls_rec->Tables["MMLCSND"]);
				cmd_inq.Close();

				if (bcls_rec->Tables["MMLCSND"].Rows.get_Count() > 0)
				{
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
					" and SYSTEM_ID_MAT = 'C'"
					" and HANDLE_DIV = 'F'"
					" and  SEND_FLAG = '1' and RTN_FLAG = ' ' "
					" and  mat_code in (select mat_code from tmmsm50 where QUALITY_FLAS ='1')"
					" and prod_date LIKE '202410%' "
					" and lot_no =' '"
					" order by mat_code,DEVO_WT"
					;
				bcls_rec->Tables["MMLCSND"].Rows.Clear();
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
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

				sqlstr = " update tmmsm2a_send set RTN_FLAG = '1',RETURN_TIME=@datetime"
					" where 1=1"
					" and HANDLE_DIV = 'F'"						
					" and RTN_FLAG != '1'"
					" and SEND_FLAG = '1'"
					" and SYSTEM_ID_MAT = 'C'"
					" and HANDLE_DIV = 'F'"
					" and  SEND_FLAG = '1' and RTN_FLAG = ' ' "
					" and  mat_code in (select mat_code from tmmsm50 where QUALITY_FLAS ='1')"
					" and prod_date LIKE '202410%' "
					" and lot_no =' '"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.Parameters.Set("datetime", datetime);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close(); 
				*/
				

		}
		else
		{

			//分摊完成将结果插入到表
			sqlstr = " delete from tmmsm2a_send"
				" where 1=1"
				" and HANDLE_DIV = 'F'"
				" and send_flag in (' ','0')"
				" and stat_date = @stat_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " select max(SEQ_NO_2A) "
				" from tmmsm2a_send"
				" where 1=1"
				" and HANDLE_DIV = 'F'"
				" and stat_date =@stat_date"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
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

			sqlstr = " insert into tmmsm2a_send(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,DEVO_TIME,prod_date,devo_wt,SEQ_NO_2A,HANDLE_DIV,mat_name,SYSTEM_ID_MAT)"
				" select @rec_creator,@rec_create_time,t1.stat_date,t1.sm_plan_nol2,t1.heat_no,t1.st_no,t1.dev_code,t1.mat_code,t1.lot_no,@rec_create_time,t1.prod_date,t1.devo_wt ,'F'||@stat_date||trim(to_char(rownum+@seq_id, '00000000')) ,'F',t2.mat_name,t2.SYSTEM_ID_MAT"
				" from ("
				" select stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,prod_date,lot_no,sum(devo_wt) devo_wt "
				" from ("
				" select stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,WEIGH_NO,QUALITY_BATCH_NO,lot_no,substr(recv_mat_time,1,8) as prod_date,OUT_STOCK_WT as devo_wt"
				" from tmmsm56ft"
				" where 1=1 "
				" and SEND_FLAG !='1'"
				" and HANDLE_DIV = 'F'"
				" and stat_date =@stat_date"
				/*" union all"
				" select stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,WEIGH_NO,QUALITY_BATCH_NO,prod_date,0-devo_wt as devo_wt"
				" from tmmsm2a_send"
				" where 1=1"
				" and send_flag = '1' and  RTN_FLAG = ' '"
				" and HANDLE_DIV = 'F'"
				" and stat_date =@stat_date"*/
				") group by stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,prod_date,lot_no"
				" having sum(devo_wt)!=0"
				") t1 left join tmmsm50 t2 on t1.mat_code=t2.mat_code"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", datetime);
			cmd_inq.Parameters.Set("seq_id", atol(seq_id));
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close(); 


				//给铁区发送
				sqlstr = " select * from TMMSM2A_SEND t1"
					" where 1=1"
					" and SYSTEM_ID_MAT = 'B'"
					" and  SEND_FLAG in ( ' ','0')"
					" and HANDLE_DIV = 'F'"
					" and stat_date = @stat_date"
					" order by MAT_CODE,HEAT_NO,DEVO_WT"
					;
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				bcls_rec->Tables["MMLCSND"].Rows.Clear();
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.ExecuteQuery(bcls_rec->Tables["MMLCSND"]);
				cmd_inq.Close();
				if (bcls_rec->Tables["MMLCSND"].Rows.get_Count() > 0)
				{
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
					" and SYSTEM_ID_MAT = 'C'"
					" and HANDLE_DIV = 'F'"
					" and  SEND_FLAG in ( ' ','0')"
					" and stat_date = @stat_date"
					" order by MAT_CODE,HEAT_NO,DEVO_WT"
					;
				bcls_rec->Tables["MMLCSND"].Rows.Clear();
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
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
					" and HANDLE_DIV = 'F'"
					" and SEND_FLAG in ( ' ','0')"
					" and stat_date = @stat_date"
					;
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.Parameters.Set("datetime", datetime);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close(); 

				sqlstr = " update tmmsm56a set SEND_FLAG = '1',SEND_TIME=@datetime"
					" where 1=1"
					" and SEND_FLAG !='1'"
					" and stat_date = @stat_date"
					;
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.Parameters.Set("datetime", datetime);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = " update tmmsm56ft set SEND_FLAG = '1',SEND_TIME=@datetime"
					" where 1=1"
					" and SEND_FLAG !='1'"
					" and stat_date = @stat_date"
					;
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.Parameters.Set("datetime", datetime);
				cmd_inq.ExecuteNonQuery();
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
