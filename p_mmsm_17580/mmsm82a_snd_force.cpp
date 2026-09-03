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
int f_t82306_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn); //给智慧质量发送消耗
int f_t82304_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送全程工艺路径
// service入口
BM2F_ENTERACE(mmsm82a_snd_force)
int f_mmsm82a_snd_force(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString send_flag_u = "0";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq_s(conn);
	CDbCommand cmd_2a(conn);

	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm2a_send("TMMSM2A_SEND");
	CModel tmmsmgy06_sed("TMMSMGY06_SED");
	CModel tmmsm2a("TMMSM2A");


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

	blkNum = bcls_rec->Tables.IndexOf("T823");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("T823");
	}
	bcls_rec->Tables["T823"].Columns.Add(tmmsm2a);
	if (!bcls_rec->Tables["T823"].Columns.Contains("DEAL_FLAG"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "DEAL_FLAG");
	}

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();
			deal_flag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();

			Log::Trace("", __FUNCTION__, "heat_no = [{0}],deal_flag=[{1}]", heat_no, deal_flag);



			send_flag = "1";

			sqlstr = " select stat_date"
				" from tmmsmgy05"
				" where heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				//判断会计期后面不能发送 
				if (cmd_inq.GetString(1).Trim() != "")
				{
					if (datetime.Substring(0, 6) > cmd_inq.GetString(1))
					{
						if (CDateTime::Now().AddMonths(-1).ToString("yyyyMM") > cmd_inq.GetString(1) || datetime.Substring(6, 2) != "01")
						{
							//判断如果是本月发送的
							//Log::Trace("", __FUNCTION__, "send_flag1111 = [{0}]", send_flag);
							send_flag = "0";
						}
					}
				}
				else
				{
					sqlstr = " select prod_date from TMMSM2A_SEND "
						" where 1=1"
						" and HANDLE_DIV!='F'"
						" and SEND_FLAG = '1' and  RTN_FLAG = ' '"
						" and heat_no = @heat_no"
						;
					cmd_inq_s.SetCommandText(sqlstr);
					cmd_inq_s.Parameters.Set("heat_no", heat_no);
					cmd_inq_s.ExecuteReader();
					if (cmd_inq_s.Read())
					{
						//Log::Trace("", __FUNCTION__, "上月 = [{0}],会计期 = [{1}]", CDateTime::Now().AddMonths(-1).ToString("yyyyMM"), cmd_inq_s.GetString(1).Substring(0, 6));
						if ((CDateTime::Now().AddMonths(-1).ToString("yyyyMM") > cmd_inq_s.GetString(1).Substring(0, 6)) || (CDateTime::Now().AddMonths(-1).ToString("yyyyMM") == cmd_inq_s.GetString(1).Substring(0, 6) && datetime.Substring(6, 2) != "01"))
						{
							//Log::Trace("", __FUNCTION__, "send_flag = [{0}],上月 = [{1}],会计期 = [{2}]", send_flag, CDateTime::Now().AddMonths(-1).ToString("yyyyMM"), cmd_inq_s.GetString(1).Substring(0, 6));
							//判断如果是本月发送的
							send_flag = "0";
						}
					}
					cmd_inq_s.Close();
				}
			}
			cmd_inq.Close();



			Log::Trace("", __FUNCTION__, "send_flag = [{0}]", send_flag);

			if (deal_flag == "D")
			{
				sqlstr = " select VALID_FLAG_1 from tmmsmgy05 where  heat_no=@heat_no "
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					if (cmd_inq.GetString(1) != "1")
					{
						send_flag = "0";
					}
				}
				cmd_inq.Close();

				if (send_flag == "1")
				{
					i_count = i_count + 1;

					//发送过钢量
					sqlstr = " select * from tmmsmgy06_sed"
						" where 1=1"
						" and TC_SEND_FLAG = '1' and  RTN_FLAG = ' '"
						" and heat_no = @heat_no"
						;
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

						sqlstr = " update tmmsmgy06_sed set RTN_FLAG = '1',RETURN_TIME=@datetime"
							" where 1=1"
							" and TC_SEND_FLAG = '1'"
							" and RTN_FLAG = ' '"
							" and heat_no = @heat_no"
							;
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("heat_no", heat_no);
						cmd_inq.Parameters.Set("datetime", datetime);
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();

					}

					//给铁区发送
					sqlstr = " select * from TMMSM2A_SEND t1"
						" where 1=1"
						" and SYSTEM_ID_MAT = 'B'" //冲的时候按抛送的全冲						
						" and HANDLE_DIV != 'F'"
						" and  SEND_FLAG = '1'  and  RTN_FLAG != '1' "
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
						" and SYSTEM_ID_MAT = 'C'" //冲的时候按抛送的全冲	
						" and HANDLE_DIV != 'F'"
						" and  SEND_FLAG = '1' and RTN_FLAG != '1' "
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

					sqlstr = " update tmmsm2a_send set RTN_FLAG = '1',RETURN_TIME=@datetime"
						" where 1=1"
						" and HANDLE_DIV != 'F'"
						" and SEND_FLAG = '1'"
						" and RTN_FLAG != '1'"
						" and heat_no = @heat_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", heat_no);
					cmd_inq.Parameters.Set("datetime", datetime);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					tmmsmgy05["HEAT_NO"] = heat_no;
					tmmsmgy05["VALID_FLAG_2"] = "0";
					tmmsmgy05["VALID_FLAG_1"] = "0";
					tmmsmgy05["VALID_FLAG_3"] = "0";
					tmmsmgy05.Update("VALID_FLAG_1,VALID_FLAG_2,VALID_FLAG_3", "HEAT_NO");
				}

			}
			if (deal_flag == "I")
			{

				//判断是否全部收货完成
				sqlstr = " select AFFIRM_FLAG from tmmsmgy06 where AFFIRM_FLAG !='1' and  heat_no=@heat_no "
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					send_flag = "0";
					strcpy(s.msg, "工艺路径未确认，不能发送!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd_inq.Close();

				//20250623Update强制上传
				sqlstr = " update tmmsm56 t1 set t1.recv_mat_time=(select max(recv_mat_time) from "
					"(select recv_mat_time from tmmsm01 where recv_mat_time != ' ' and heat_no =@heat_no "
				"union "
					"select recv_mat_time  from hmmsm01 where recv_mat_time != ' ' and heat_no =@heat_no)) where t1.heat_no =@heat_no and t1.recv_mat_time = ' '"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				//20250730update收货时刻
				sqlstr = " update tmmsmgy05 t1 set t1.recv_mat_time=(select max(recv_mat_time) from "
					"(select recv_mat_time from tmmsm01 where recv_mat_time != ' ' and heat_no =@heat_no "
					"union "
					"select recv_mat_time  from hmmsm01 where recv_mat_time != ' ' and heat_no =@heat_no)) where t1.heat_no =@heat_no and t1.recv_mat_time = ' '"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = " select VALID_FLAG_1,RECV_MAT_TIME from tmmsmgy05 where  heat_no=@heat_no "
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					if (cmd_inq.GetString(1) == "1")
					{
						send_flag = "0";
						/*strcpy(s.msg, "炉号[" + heat_no + "]已经上传，不能重复上传！");
						throw CApplicationException(-1, s.msg, log.Location);*/
					}
					recv_mat_time = cmd_inq.GetString(2);
				}
				cmd_inq.Close();

				//20250801更新stat_date
				sqlstr = " update tmmsm56 t1 set stat_date=@recv_mat_time where  t1.heat_no=@heat_no and t1.stat_date= ' '";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.Parameters.Set("recv_mat_time", recv_mat_time.SubstringNE(0, 6));
				cmd_inq.ExecuteNonQuery();
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



				if (send_flag == "1")
				{
					i_count = i_count + 1;
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

					//给智慧质量发送消耗
					/*sqlstr = " select dev_code,HEAT_NO,SM_PLAN_NOL2,MAT_CODE,MAT_NAME,ST_NO,sum(DEVO_WT) DEVO_WT"
					" from TMMSM56 "
					" WHERE 1=1"
					" and HEAT_NO=@heat_no"
					" group by DEV_CODE, HEAT_NO, SM_PLAN_NOL2, MAT_CODE, MAT_NAME, ST_NO "
					" having sum(DEVO_WT)!=0"
					;
					cmd_2a.SetCommandText(sqlstr);
					cmd_2a.Parameters.Set("heat_no", heat_no);
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
					tmmsm2a.Reset();
					cmd_2a.Fetch(tmmsm2a);
					bcls_rec->Tables["T823"].Rows.Clear();
					tmmsm2a.MergeTo(bcls_rec->Tables["T823"], false);
					bcls_rec->Tables["T823"].Rows[0]["DEAL_FLAG"] = "I";
					doFlag = f_t82306_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
					throw CApplicationException(-1, s.msg, log.Location);
					}
					}
					cmd_2a.Close();*/

					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					tmmsmgy05["HEAT_NO"] = heat_no;
					tmmsmgy05["VALID_FLAG_2"] = "1";
					tmmsmgy05["VALID_FLAG_1"] = "1";
					tmmsmgy05["VALID_FLAG_3"] = "1";
					tmmsmgy05.Update("VALID_FLAG_1,VALID_FLAG_2,VALID_FLAG_3", "HEAT_NO");

				}
			}

			if (deal_flag == "U")
			{
				sqlstr = " select VALID_FLAG_1,recv_mat_time from tmmsmgy05 where  heat_no=@heat_no "
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					if (cmd_inq.GetString(1) != "1")
					{
						send_flag = "0";
					}
					recv_mat_time = cmd_inq.GetString(2);

					if (recv_mat_time.Trim() == "")	 //跨月撤销或是修改的
					{
						sqlstr = " select prod_date from TMMSM2A_SEND "
							" where 1=1"
							" and HANDLE_DIV!='F'"
							" and SEND_FLAG = '1' and  RTN_FLAG = ' '"
							" and heat_no = @heat_no"
							;
						cmd_inq_s.SetCommandText(sqlstr);
						cmd_inq_s.Parameters.Set("heat_no", heat_no);
						cmd_inq_s.ExecuteReader();
						if (cmd_inq_s.Read())
						{
							if (CDateTime::Now().AddMonths(-1).ToString("yyyyMM") > cmd_inq_s.GetString(1).Substring(0, 6) || datetime.Substring(5, 2) != "01")
							{
								//判断如果是本月发送的
								send_flag = "0";
							}
						}
						cmd_inq_s.Close();
					}
				}
				cmd_inq.Close();

				if (send_flag == "1")
				{
					////发送过钢量,判断是否有差异，如果有则发送，没有则不发
					sqlstr = " select heat_no,dev_code,sum(DURATION_TIME) DURATION_TIME"
						" from ("
						" select heat_no,dev_code,st_no,DURATION_TIME"
						" from tmmsmgy06A"
						" where 1=1"
						" and substr(dev_code,1,1) not in ('C','H','M','S','D')"
						" and heat_no = @heat_no"
						" union all"
						" select heat_no,dev_code,st_no,0-DURATION_TIME as DURATION_TIME"
						" from tmmsmgy06_sed"
						" where 1=1"
						" and TC_SEND_FLAG = '1' and  RTN_FLAG = ' '"
						" and heat_no = @heat_no"
						" )"
						" group by heat_no,dev_code,st_no"
						" having sum(DURATION_TIME)!=0"
						;
					cmd_inq_s.SetCommandText(sqlstr);
					cmd_inq_s.Parameters.Set("heat_no", heat_no);
					cmd_inq_s.ExecuteReader();
					if (cmd_inq_s.Read())
					{
						sqlstr = " select * from tmmsmgy06_sed"
							" where 1=1"
							" and TC_SEND_FLAG = '1' and  RTN_FLAG = ' '"
							" and heat_no = @heat_no"
							;
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
							bcls_rec1.Tables["MMSMSND"].Rows[0]["DEAL_FLAG"] = "D";
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
								" and heat_no = @heat_no"
								;
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Set("heat_no", heat_no);
							cmd_inq.Parameters.Set("datetime", datetime);
							cmd_inq.ExecuteNonQuery();
							cmd_inq.Close();
						}

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
							bcls_rec1.Tables["MMSMSND"].Rows[0]["DEAL_FLAG"] = "I";
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


					}
					cmd_inq_s.Close();

					sqlstr = " select heat_no,dev_code,ST_NO,MAT_CODE,LOT_NO,sum(DEVO_WT) DEVO_WT"
						" from ("
						" select heat_no,dev_code,ST_NO,MAT_CODE,LOT_NO,OUT_STOCK_WT DEVO_WT"
						" from tmmsm56"
						" where 1=1"
						"  and mat_code in (select  MAT_CODE FROM TMMSM50 WHERE QUALITY_FLAS = '1')"
						"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
						" and heat_no = @heat_no"
						" union all"
						" select heat_no,dev_code,ST_NO,MAT_CODE,' ' LOT_NO,OUT_STOCK_WT DEVO_WT"
						" from tmmsm56"
						" where 1=1"
						"  and mat_code in (select  MAT_CODE FROM TMMSM50 WHERE QUALITY_FLAS != '1')"
						"  and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
						" and heat_no = @heat_no"
						" union all"
						" select heat_no,dev_code,ST_NO,MAT_CODE,LOT_NO,0-DEVO_WT as DEVO_WT"
						" from tmmsm2a_send"
						" where 1=1"
						" and HANDLE_DIV!='F'"
						" and SEND_FLAG = '1' and  RTN_FLAG = ' '"
						" and heat_no = @heat_no"
						" )"
						" group by heat_no,dev_code,ST_NO,MAT_CODE,LOT_NO"
						" having sum(DEVO_WT)!=0"
						;
					cmd_inq_s.SetCommandText(sqlstr);
					cmd_inq_s.Parameters.Set("heat_no", heat_no);
					cmd_inq_s.ExecuteReader();
					if (cmd_inq_s.Read())
					{
						//给铁区发送
						sqlstr = " select * from TMMSM2A_SEND t1"
							" where 1=1"
							" and SYSTEM_ID_MAT = 'B'"
							" and HANDLE_DIV != 'F'"
							" and  SEND_FLAG = '1'  and  RTN_FLAG != '1' "
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
							bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "D";
							doFlag = f_mmsm_21b006_snd(bcls_rec, bcls_ret, conn);
							if (doFlag < 0)
							{
								Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21b006_snd失败-------");
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}

						//冲的时候按抛送的时候发的信息来冲
						sqlstr = " select * from TMMSM2A_SEND t1"
							" where 1=1"
							" and SYSTEM_ID_MAT = 'C'"
							" and HANDLE_DIV != 'F'"
							" and  SEND_FLAG = '1' and RTN_FLAG != '1' "
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
							bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "D";
							doFlag = f_mmsm_21c005_snd(bcls_rec, bcls_ret, conn);
							if (doFlag < 0)
							{
								Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c005_snd失败-------");
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}

						sqlstr = " update tmmsm2a_send set RTN_FLAG = '1',RETURN_TIME=@datetime"
							" where 1=1"
							" and HANDLE_DIV != 'F'"
							" and SEND_FLAG = '1'"
							" and RTN_FLAG = ' '"
							" and heat_no = @heat_no"
							;
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("heat_no", heat_no);
						cmd_inq.Parameters.Set("datetime", datetime);
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();

						//判断是否批次为空的，有则不发送
						send_flag_u = "1";
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
								send_flag_u = "0";
							}
						}
						cmd_inq.Close();

						if (send_flag_u == "1")
						{


							//分摊完成将结果插入到表
							sqlstr = " delete from tmmsm2a_send"
								" where 1=1"
								" and send_flag in (' ','0')"
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
								" select stat_date,sm_plan_nol2,heat_no,L2_PROC_NO,pono,st_no,dev_code,mat_code,' ' LOT_NO,substr(recv_mat_time,1,8) as prod_date,OUT_STOCK_WT as devo_wt"
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
								" and RTN_FLAG = ' ' "
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
								" and SYSTEM_ID_MAT = 'B'"
								" and  SEND_FLAG in ( ' ','0')"
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
								bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "I";
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
								" and HANDLE_DIV != 'F'"
								" and  SEND_FLAG in ( ' ','0')"
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
								bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "I";
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
								" and SEND_FLAG in ( ' ','0')"
								" and heat_no = @heat_no"
								;
							Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Set("heat_no", heat_no);
							cmd_inq.Parameters.Set("datetime", datetime);
							cmd_inq.ExecuteNonQuery();
							cmd_inq.Close();
						}

					}
					cmd_inq_s.Close();



				}

			}

		}

		strcpy(s.msg, "成功处理[" + i_count.ToString() + "条记录！");

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
