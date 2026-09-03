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

int f_mmsm_21c005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_210049_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn); //给L4的过钢量
int f_t82304_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送全程工艺路径
// service入口
BM2F_ENTERACE(mmsm82h_snd)
int f_mmsm82h_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	int i_idx = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";		 
	CString recv_mat_time = "";		
	CString heat_no = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal i_count = 0;
	int j = 0;
	CString seq_id = "0";
	CString send_flag = "0";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CModel tmmsmgy07("TMMSMGY07");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm2a_send("TMMSM2A_SEND");
	CModel tmmsmgy06_sed("TMMSMGY06_SED");
	

	i_idx = bcls_rec->Tables[0].Rows.get_Count();
	CString deal_flag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();

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
	blkNum = bcls_rec2.Tables.IndexOf("MMLCSND");
	if (blkNum < 0)
	{
		bcls_rec2.Tables.Add("MMLCSND");
	}
	bcls_rec2.Tables["MMLCSND"].Columns.Add(tmmsm2a_send);
	bcls_rec2.Tables["MMLCSND"].Rows.Clear();
	if (!bcls_rec2.Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
	{
		bcls_rec2.Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
	}


	try
	{
		//压力测试

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmgy07.Reset();
			tmmsmgy07.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			deal_flag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();

			sqlstr = " select SEND_FLAG,RTN_FLAG"
				" from tmmsmgy07"
				" where 1=1"
				" and heat_no_old = @heat_no_old"
				" and heat_no = @heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if (cmd_inq.GetString(1) == "1"&&deal_flag == "D")
				{
					send_flag = "1";
				}
				if (cmd_inq.GetString(1) != "1" && deal_flag != "D")
				{
					send_flag = "1";
				}
			}
			cmd_inq.Close();

			Log::Trace("", __FUNCTION__, "-------send_flag=[{0}],deal_flag =[{1}]", send_flag, deal_flag);

			if (send_flag == "1")
			{ 
				if (deal_flag == "D")
				{
					//发送过钢量
					sqlstr = " select * from tmmsmgy06_sed"
						" where 1=1"
						" and TC_SEND_FLAG = '1' and  RTN_FLAG = ' '"
						" and HEAT_NO_OLD = @heat_no_old"
						" and heat_no = @heat_no"
						;
					bcls_rec1.Tables["MMSMSND"].Rows.Clear();
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO_OLD"].ToString());
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

						sqlstr = " update tmmsmgy06_sed set RTN_FLAG = '1',RETURN_TIME=@datetime"
							" where 1=1"
							" and TC_SEND_FLAG = '1'"
							" and RTN_FLAG = ' '"
							" and HEAT_NO_OLD = @heat_no_old"
							" and heat_no = @heat_no"
							;
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("heat_no", tmmsmgy07["HEAT_NO"].ToString());
						cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO_OLD"].ToString());
						cmd_inq.Parameters.Set("datetime", datetime);
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();

					}

					//给铁区发送
					sqlstr = " select * from TMMSM2A_SEND t1"
						" where 1=1"
						" and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')" //排除不发送数据					
						" and exists (select 1 from tmmsm50 t2 where t2.SYSTEM_ID_MAT = 'B' and t1.mat_code = t2.mat_code )"
						" and HANDLE_DIV = 'H'"
						" and  SEND_FLAG = '1'  and  RTN_FLAG = ' ' "
						" and HEAT_NO_OLD = @heat_no_old"
						" and heat_no = @heat_no"
						" order by DEVO_WT"
						;
					bcls_rec2.Tables["MMLCSND"].Rows.Clear();
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO_OLD"].ToString());
					cmd_inq.ExecuteQuery(bcls_rec2.Tables["MMLCSND"]);
					cmd_inq.Close();

					if (bcls_rec2.Tables["MMLCSND"].Rows.get_Count() > 0)
					{
						if (!bcls_rec2.Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
						{
							bcls_rec2.Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
						}
						bcls_rec2.Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = deal_flag;
						doFlag = f_mmsm_21b006_snd(&bcls_rec2, &bcls_ret2, conn);
						if (doFlag < 0)
						{
							Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21b006_snd失败-------");
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}

					//给资源发送
					sqlstr = " select * from TMMSM2A_SEND t1"
						" where 1=1"
						" and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')" //排除不发送数据 				
						" and exists (select 1 from tmmsm50 t2 where t2.SYSTEM_ID_MAT = 'C' and t1.mat_code = t2.mat_code )"
						" and HANDLE_DIV = 'H'"
						" and  SEND_FLAG = '1' and RTN_FLAG = ' ' "
						" and HEAT_NO_OLD = @heat_no_old"
						" and heat_no = @heat_no"
						" order by DEVO_WT"
						;
					bcls_rec2.Tables["MMLCSND"].Rows.Clear();
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO_OLD"].ToString());
					cmd_inq.ExecuteQuery(bcls_rec2.Tables["MMLCSND"]);
					cmd_inq.Close();

					if (bcls_rec2.Tables["MMLCSND"].Rows.get_Count() > 0)
					{
						if (!bcls_rec2.Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
						{
							bcls_rec2.Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
						}
						bcls_rec2.Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = deal_flag;
						doFlag = f_mmsm_21c005_snd(&bcls_rec2, &bcls_ret2, conn);
						if (doFlag < 0)
						{
							Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c005_snd失败-------");
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}

					sqlstr = " update tmmsm2a_send set TC_SEND_FLAG='0',RTN_FLAG = '1',RETURN_TIME=@datetime"
						" where 1=1"
						" and HANDLE_DIV = 'H'"
						" and SEND_FLAG = '1'"
						" and RTN_FLAG = ' '"
						" and HEAT_NO_OLD = @heat_no_old"
						" and heat_no = @heat_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO_OLD"].ToString());
					cmd_inq.Parameters.Set("datetime", datetime);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();


					sqlstr = " update tmmsmgy07 set SEND_FLAG = '0',RTN_FLAG = '1',RETURN_TIME=@datetime"
						" where 1=1"
						" and heat_no_old = @heat_no_old"
						" and heat_no = @heat_no"
						;
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("datetime", datetime);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();


				}
				else
				{

					//发送过钢量
					sqlstr = " select  SM_PLAN_NOL2,HEAT_NO,L2_PROC_NO,PROD_DATE,DEV_CODE,START_TIME,END_TIME,DURATION_TIME,STAT_DATE,PROD_SHIFT_GROUP,PROD_SHIFT_NO,st_no,HEAT_NO_OLD,'H' AS HANDLE_DIV"
						" from tmmsmgy07 t1"
						" where 1=1"
						" and substr(dev_code,1,1) not in ('C','H','M','S','D')"
						" and exists (select 1 from tmmsmgy05 where VALID_FLAG_1='1' and heat_no = @heat_no )"
						" and heat_no_old = @heat_no_old"
						" and heat_no = @heat_no"
						" union all"
						" select  SM_PLAN_NOL2,HEAT_NO_OLD as HEAT_NO,L2_PROC_NO,PROD_DATE,DEV_CODE,START_TIME,END_TIME,0 - DURATION_TIME AS DURATION_TIME,STAT_DATE,PROD_SHIFT_GROUP,PROD_SHIFT_NO,st_no,HEAT_NO_OLD,'H' AS HANDLE_DIV from tmmsmgy07"
						" where 1=1"
						" and exists (select 1 from tmmsmgy05 where VALID_FLAG_1='1' and heat_no = @heat_no_old )"
						" and substr(dev_code,1,1) not in ('C','H','M','S','D')"
						" and heat_no_old = @heat_no_old"
						" and heat_no = @heat_no"
						;
					//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					bcls_rec1.Tables["MMSMSND"].Rows.Clear();
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
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
							" and heat_no_old = @heat_no_old"
							" and (heat_no =  @heat_no OR  heat_no =  @heat_no_old)"
							;
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
						cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
						cmd_inq.Parameters.Set("datetime", datetime);
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();						
					}

					//往抛帐表插数据
					sqlstr = " delete from tmmsm2a_send"
						" where 1=1"
						" and HANDLE_DIV = 'H'"
						" and SEND_FLAG in ( ' ','0')"
						" and heat_no_old = @heat_no_old"
						" and (heat_no = @heat_no or heat_no = @heat_no_old)"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					sqlstr = " select max(SEQ_NO_2A) "
						" from tmmsm2a_send"
						" where 1=1"
						" and HANDLE_DIV = 'H'"
						" and stat_date in (select substr(RECV_MAT_TIME,1,6) from tmmsmgy05 where  heat_no=@heat_no)"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
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

					sqlstr = " insert into tmmsm2a_send(rec_creator,rec_create_time,stat_date,L2_PROC_NO,heat_no,HEAT_NO_OLD,st_no,dev_code,mat_code,LOT_NO,DEVO_TIME,prod_date,devo_wt,SEQ_NO_2A,mat_name,SYSTEM_ID_MAT,HANDLE_DIV)"
						" select @rec_creator,@rec_create_time,t1.stat_date,t1.L2_PROC_NO,t1.heat_no,t1.HEAT_NO_OLD,t1.st_no,t1.dev_code,t1.mat_code,t1.LOT_NO,@rec_create_time,substr(t1.RECV_MAT_TIME,1,8) prod_date,devo_wt ,'H'||stat_date||trim(to_char(rownum+@seq_id, '00000000')),t2.mat_name,t2.SYSTEM_ID_MAT,'H' "
						" from ("
						" select   RECV_MAT_TIME,substr(RECV_MAT_TIME,1,6) STAT_DATE,HEAT_NO,L2_PROC_NO,HEAT_NO_OLD,LOT_NO,DEV_CODE,ST_NO,MAT_CODE,SUM(DEVO_WT) DEVO_WT"
						" from TMMSMGY07A "
						" where 1=1"
						"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
						" and exists (select 1 from tmmsmgy05 where VALID_FLAG_1='1' and heat_no = @heat_no )"
						" and heat_no_old = @heat_no_old"
						" and heat_no = @heat_no"
						" group by  RECV_MAT_TIME,substr(RECV_MAT_TIME,1,6),HEAT_NO,L2_PROC_NO,HEAT_NO_OLD,LOT_NO,DEV_CODE,ST_NO,MAT_CODE"
						" union all"
						" select   RECV_MAT_TIME,substr(RECV_MAT_TIME,1,6) STAT_DATE,HEAT_NO_OLD as HEAT_NO,L2_PROC_NO,HEAT_NO_OLD,LOT_NO,DEV_CODE,ST_NO,MAT_CODE,SUM(0-DEVO_WT) DEVO_WT"
						" from TMMSMGY07A "
						" where 1=1"
						"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
						" and exists (select 1 from tmmsmgy05 where VALID_FLAG_1='1' and heat_no = @heat_no_old )"
						" and heat_no_old = @heat_no_old"
						" and heat_no = @heat_no"
						" group by  RECV_MAT_TIME,substr(RECV_MAT_TIME,1,6),L2_PROC_NO,HEAT_NO_OLD,LOT_NO,DEV_CODE,ST_NO,MAT_CODE"
						") t1 left join tmmsm50 t2 on t1.mat_code = t2.mat_code"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("rec_creator", s.userid);
					cmd_inq.Parameters.Set("rec_create_time", datetime);
					cmd_inq.Parameters.Set("seq_id", atol(seq_id));
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					//给铁区发送
					sqlstr = " select * from TMMSM2A_SEND t1"
						" where 1=1"
						" and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')" //排除不发送数据					
						" and exists (select 1 from tmmsm50 t2 where t2.SYSTEM_ID_MAT = 'B' and t1.mat_code = t2.mat_code )"
						" and  SEND_FLAG in ( ' ','0')"
						" and HANDLE_DIV = 'H'"
						" and heat_no_old = @heat_no_old"
						" and (heat_no = @heat_no or heat_no = @heat_no_old)"
						" order by DEVO_WT"
						;
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					bcls_rec2.Tables["MMLCSND"].Rows.Clear();
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.ExecuteQuery(bcls_rec2.Tables["MMLCSND"]);
					cmd_inq.Close();
					if (bcls_rec2.Tables["MMLCSND"].Rows.get_Count() > 0)
					{
						if (!bcls_rec2.Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
						{
							bcls_rec2.Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
						}
						bcls_rec2.Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = deal_flag;
						doFlag = f_mmsm_21b006_snd(&bcls_rec2, &bcls_ret2, conn);
						if (doFlag < 0)
						{
							Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21b006_snd失败-------");
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}

					//给资源发送
					sqlstr = " select * from TMMSM2A_SEND t1"
						" where 1=1"
						" and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')" //排除不发送数据 				
						" and exists (select 1 from tmmsm50 t2 where t2.SYSTEM_ID_MAT = 'C' and t1.mat_code = t2.mat_code )"
						" and HANDLE_DIV = 'H'"
						" and  SEND_FLAG in ( ' ','0')"
						" and heat_no_old = @heat_no_old"
						" and (heat_no = @heat_no or heat_no = @heat_no_old)"
						" order by DEVO_WT"
						;
					bcls_rec2.Tables["MMLCSND"].Rows.Clear();
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.ExecuteQuery(bcls_rec2.Tables["MMLCSND"]);
					cmd_inq.Close();
					if (bcls_rec2.Tables["MMLCSND"].Rows.get_Count() > 0)
					{
						if (!bcls_rec2.Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
						{
							bcls_rec2.Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
						}
						bcls_rec2.Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = deal_flag;
						doFlag = f_mmsm_21c005_snd(&bcls_rec2, &bcls_ret2, conn);
						if (doFlag < 0)
						{
							Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c005_snd失败-------");
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}

					sqlstr = " update tmmsm2a_send set SEND_FLAG = '1',SEND_TIME=@datetime"
						" where 1=1"
						" and HANDLE_DIV = 'H'"
						" and SEND_FLAG in ( ' ','0')"
						" and heat_no_old = @heat_no_old"
						" and (heat_no = @heat_no or heat_no = @heat_no_old)"
						;
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("datetime", datetime);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();


					sqlstr = " update tmmsmgy07 set SEND_FLAG = '1',SEND_TIME=@datetime"
						" where 1=1"
						" and heat_no_old = @heat_no_old"
						" and (heat_no = @heat_no or heat_no = @heat_no_old)"
						;
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tmmsmgy07["RET_HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("heat_no_old", tmmsmgy07["HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("datetime", datetime);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}

			}
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
