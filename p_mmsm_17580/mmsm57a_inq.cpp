/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 北区废钢库存

**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include <regex>
//#include "h_common_aid.h"

// service入口
BM2F_ENTERACE(mmsm57a_inq);
int f_mmsm57a_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	bool sqlflag = false;
	CString sqlstr = "";
	CString sql_ft = "";
	CString sql_sj = "";
	CString stat_date = "";
	CString sqlstr_where = "";
	CString heat_no = "";
	CString v_table = "tmmsm57a";
	CString cx_date = "";
	CString out_stock_time = "";
	
	CModel tmmsm57a("TMMSM57A");
	CModel tmmsm57c("TMMSM57C");
	CModel tmmsm56a2("TMMSM56A2");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	try
	{ 	

		
		v_table = bcls_rec->Tables[0].Rows[0]["TABLE_FLAG"].ToString().ToLower();
		stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0,8);	

		tmmsm57a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Info("", __FUNCTION__, "stat_date =[{0}],v_table=[{1}]", stat_date, v_table);

		if (v_table == "tmmsm57b")
		{
			if (stat_date.Trim() == "")
			{
				sprintf(s.msg, "必须传入的时间不能为空。");
				//strcpy(s.sysmsg,s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			} 			

			sqlstr = 
				" select stat_date,seq_id,mat_code,mat_name,STOCK_WT_QC,IN_STOCK_WT1,STOCK_WT_QC1,IN_STOCK_WT,IN_STOCK_WT2,OUT_STOCK_WT1"
				",OUT_STOCK_WT2,OUT_STOCK_WT3,STOCK_WT1,STOCK_WT2, DIF_WT1, DIF_WT2, DIF_WT3, DIF_WT4,FT_WT"
				", FT_FLAG, RATE1, RATE2, FT_WT1, FT_WT2, OUT_STOCK_WT, RATE3, REMARK, REC_TIME,STATUS,STOCK_ADJ_WT "
				",STOCK_WT,MES_FT,MES_WT,FT_WT_HH"				
				",case when MES_WT = 0 and FT_WT_HH<0 then '1'"
				" when(STOCK_WT_QC + IN_STOCK_WT1 + IN_STOCK_WT - abs(FT_WT_HH))<0 then '2'"
				" when FT_WT_HH + MES_WT<0 then '3'"
				" when MES_WT = 0 and FT_WT_HH>0 then '4'"
				" else '0' end  as FT_TYPE"	  //分摊类型
				",FT_WT_HH - MES_FT as dif_wt_sj"	  //报表调整量与实际调整量差  
				",STOCK_WT - FT_WT_HH as dif_wt_kc"//报表调整量与库存差,
				",STOCK_WT - (FT_WT_HH - MES_FT) as dif_wt_ft"//报表调整加已调整量与库存的差异, 
				" from ("
				" select t1.stat_date,t1.seq_id,t1.mat_code,t1.mat_name,t1.STOCK_WT_QC,t1.IN_STOCK_WT1,t1.STOCK_WT_QC1,t1.IN_STOCK_WT,t1.IN_STOCK_WT2,t1.OUT_STOCK_WT1"
				",t1.OUT_STOCK_WT2,t1.OUT_STOCK_WT3,t1.STOCK_WT1,t1.STOCK_WT2, t1.DIF_WT1, t1.DIF_WT2, t1.DIF_WT3, t1.DIF_WT4,FT_WT"
				", t1.FT_FLAG, t1.RATE1, t1.RATE2, t1.FT_WT1, t1.FT_WT2, t1.OUT_STOCK_WT, t1.RATE3, t1.REMARK, t1.REC_TIME,STATUS,STOCK_ADJ_WT "
				",nvl((select sum(STOCK_WT) from (select STOCK_WT  from tmmsm57c t2 where stat_date in ( select max(stat_date) from tmmsm57c where stat_date like  @stat_date_s||'%'  and STOCK_CODE='6241' )  and STOCK_CODE='6241'   and  t1.mat_code = t2.mat_code  union all select sum(STOCK_WT) from tmmsm57d t3 where stat_date in ( select max(stat_date) from tmmsm57d where stat_date like  @stat_date_s||'%')   and  t3.mat_code = t1.mat_code) ),0) as STOCK_WT" //取铁区资源的库存
			",nvl((select sum(DEVO_WT) from  tmmsm2A_send t2 where  HANDLE_DIV = 'F' and RTN_FLAG!='1' and send_flag = '1' and  stat_date = @stat_date_s  and  t1.mat_code = t2.mat_code )/1000,0) as MES_FT" //MES分摊量
			",nvl((select sum(DEVO_WT) from  tmmsm2A_send t2 where   RTN_FLAG!='1' and send_flag = '1' and  stat_date = @stat_date_s  and  t1.mat_code = t2.mat_code )/1000,0) as MES_WT" //MES消耗量
			",decode(t1.STATUS, '1', DIF_WT4+STOCK_ADJ_WT, '2', STOCK_ADJ_WT, FT_WT+STOCK_ADJ_WT)  as FT_WT_HH"	  //最终调整量(会后):1=本月差异 2=会后调整量 3=最终调整量
				" from tmmsm57b  t1"
				" where 1=1"
				;
			if (stat_date.Trim() != "")
				sqlstr = sqlstr + " and t1.stat_date = @stat_date";
			if (tmmsm57a["MAT_CODE"].ToString().Trim() != "")
				sqlstr = sqlstr + " and t1.mat_code like @mat_code||'%'";
			if (tmmsm57a["MAT_NAME"].ToString().Trim() != "")
				sqlstr = sqlstr + " and t1.mat_name like '%'||@mat_name||'%'";
			sqlstr = sqlstr + ") order by mat_code ";

			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("stat_date_s", stat_date.Substring(0,6));
			cmd_inq.Parameters.Set("mat_code", tmmsm57a["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_name", tmmsm57a["MAT_NAME"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

			if (CDateTime::Now().ToString("yyyyMM") == stat_date.Substring(0, 6))
			{
				out_stock_time = CDateTime::Now().ToString("yyyyMMdd");
			}
			else
			{
				sqlstr = "select to_number(to_char(last_day(to_date(@stat_date,'yyyyMMdd')),'yyyyMMdd')) "
					" from dual"
					;
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("stat_date", stat_date);
				cmd_inq_1.ExecuteReader();
				if (cmd_inq_1.Read())
				{
					out_stock_time = cmd_inq_1.GetString(1);
				}
				cmd_inq_1.Close();
			}

			sqlstr =
				" select stat_date,seq_id,mat_code,mat_name"
				",STATUS,STOCK_ADJ_WT "
				",STOCK_WT,MES_FT,MES_WT,FT_WT_HH"
				",case when MES_WT = 0 and FT_WT_HH<0 then '1'"
				" when(STOCK_WT_QC + IN_STOCK_WT1 + IN_STOCK_WT - abs(FT_WT_HH))<0 then '2'"
				" when FT_WT_HH + MES_WT<0 then '3'"
				" when MES_WT = 0 and FT_WT_HH>0 then '4'"
				" else '0' end  as FT_TYPE"	  //分摊类型
				",FT_WT_HH - MES_FT as OUT_STOCK_WT"	  //报表调整量与实际调整量差  				
				",STOCK_WT - (FT_WT_HH - MES_FT) as dif_wt_ft"//报表调整加已调整量与库存的差异, 
				",'N' AS TYPE_CODE"
				",'6240' AS FACTORY_CODE"
				",'1' AS FT_FLAG"
				",@out_stock_time AS OUT_STOCK_TIME"
				",STOCK_WT-(FT_WT_HH - MES_FT) AS dif_zy_kc"
				",decode(MES_WT,0,0,round((FT_WT_HH - MES_FT)*100/MES_WT,2)) as kc_mes"
				" from ("
				" select t1.stat_date,t1.seq_id,t1.mat_code,t1.mat_name,t1.STOCK_WT_QC,t1.IN_STOCK_WT1,t1.STOCK_WT_QC1,t1.IN_STOCK_WT,t1.IN_STOCK_WT2,t1.OUT_STOCK_WT1"
				",t1.OUT_STOCK_WT2,t1.OUT_STOCK_WT3,t1.STOCK_WT1,t1.STOCK_WT2, t1.DIF_WT1, t1.DIF_WT2, t1.DIF_WT3, t1.DIF_WT4"
				", t1.FT_FLAG, t1.RATE1, t1.RATE2, t1.FT_WT1, t1.FT_WT2, t1.OUT_STOCK_WT, t1.RATE3, t1.REMARK, t1.REC_TIME,STATUS,STOCK_ADJ_WT "
				",nvl((select sum(STOCK_WT) from (select STOCK_WT  from tmmsm57c t2 where stat_date in ( select max(stat_date) from tmmsm57c where stat_date like  @stat_date_s||'%'  and STOCK_CODE='6241' )  and STOCK_CODE='6241'   and  t1.mat_code = t2.mat_code  union all select sum(STOCK_WT) from tmmsm57d t3 where stat_date in ( select max(stat_date) from tmmsm57d where stat_date like  @stat_date_s||'%')   and  t3.mat_code = t1.mat_code) ),0) as STOCK_WT" //取铁区资源的库存
				",nvl((select sum(DEVO_WT) from  tmmsm2A_send t2 where  HANDLE_DIV = 'F' and RTN_FLAG!='1' and send_flag = '1' and  stat_date = @stat_date_s  and  t1.mat_code = t2.mat_code )/1000,0) as MES_FT" //MES分摊量
				",nvl((select sum(DEVO_WT) from  tmmsm2A_send t2 where   RTN_FLAG!='1' and send_flag = '1' and  stat_date = @stat_date_s  and  t1.mat_code = t2.mat_code )/1000,0) as MES_WT" //MES消耗量
				",decode(t1.STATUS, '1', DIF_WT4+STOCK_ADJ_WT, '2', STOCK_ADJ_WT, FT_WT+STOCK_ADJ_WT)  as FT_WT_HH"	  //最终调整量(会后):1=本月差异 2=会后调整量 3=最终调整量
				" from tmmsm57b  t1"
				" where 1=1"
				;
			if (stat_date.Trim() != "")
				sqlstr = sqlstr + " and t1.stat_date = @stat_date";
			if (tmmsm57a["MAT_CODE"].ToString().Trim() != "")
				sqlstr = sqlstr + " and t1.mat_code like @mat_code||'%'";
			if (tmmsm57a["MAT_NAME"].ToString().Trim() != "")
				sqlstr = sqlstr + " and t1.mat_name like '%'||@mat_name||'%'";
			sqlstr = sqlstr + ") order by mat_code ";

			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);

			
			
			bcls_ret->Tables.Add();
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("stat_date_s", stat_date.Substring(0, 6));
			cmd_inq.Parameters.Set("mat_code", tmmsm57a["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_name", tmmsm57a["MAT_NAME"].ToString());
			cmd_inq.Parameters.Set("out_stock_time", out_stock_time);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			cmd_inq.Close();

		}
		else if (v_table == "tmmsm57a")
		{
			stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6);
			cx_date = bcls_rec->Tables[0].Rows[0]["WEEK_DAY"].ToString().SubstringNE(0, 8);
			tmmsm56a2.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			if (stat_date.Trim() == "")
			{
				sprintf(s.msg, "必须传入的时间不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			if (bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim() != "")
			{
				sql_ft = " and send_time <= '" + bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString() + "'";

			}
			if (bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim() != "")
			{
				sql_ft = sql_ft + " and send_time >= '" + bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString() + "'";
			}

			if (sql_ft.Trim() == "")
			{
				sql_ft = " and stat_date=@stat_date";
			}

			//如果炉号为空则取当前会计期的数据，否则取炉号的值
			if (tmmsm56a2["AOD0_S"].ToString().Trim() != "" || tmmsm56a2["AOD0_E"].ToString().Trim() != "" || tmmsm56a2["AOD1_S"].ToString().Trim() != "" || tmmsm56a2["AOD1_E"].ToString().Trim() != "" || tmmsm56a2["AOD2_S"].ToString().Trim() != "" || tmmsm56a2["AOD2_E"].ToString().Trim() != "" || tmmsm56a2["AOD6_S"].ToString().Trim() != "" || tmmsm56a2["AOD6_E"].ToString().Trim() != ""
				|| tmmsm56a2["BOF0_S"].ToString().Trim() != "" || tmmsm56a2["BOF0_E"].ToString().Trim() != "" || tmmsm56a2["BOF1_S"].ToString().Trim() != "" || tmmsm56a2["BOF1_E"].ToString().Trim() != "" || tmmsm56a2["BOF2_S"].ToString().Trim() != "" || tmmsm56a2["BOF2_E"].ToString().Trim() != "" || tmmsm56a2["BOF9_S"].ToString().Trim() != "" || tmmsm56a2["BOF9_E"].ToString().Trim() != "")
			{
				if (tmmsm56a2["AOD0_S"].ToString().Trim() == "")
				{
					tmmsm56a2["AOD0_S"] = "A0000001";
				}
				if (tmmsm56a2["AOD0_E"].ToString().Trim() == "")
				{
					tmmsm56a2["AOD0_E"] = "A0999999";
				}
				if (tmmsm56a2["AOD1_S"].ToString().Trim() == "")
				{
					tmmsm56a2["AOD1_S"] = "A1000001";
				}
				if (tmmsm56a2["AOD1_E"].ToString().Trim() == "")
				{
					tmmsm56a2["AOD1_S"] = "A1999999";
				}
				if (tmmsm56a2["AOD2_S"].ToString().Trim() == "")
				{
					tmmsm56a2["AOD2_S"] = "A2000001";
				}
				if (tmmsm56a2["AOD2_E"].ToString().Trim() == "")
				{
					tmmsm56a2["AOD2_E"] = "A2999999";
				}
				if (tmmsm56a2["AOD6_S"].ToString().Trim() == "")
				{
					tmmsm56a2["AOD6_S"] = "A6000001";
				}
				if (tmmsm56a2["AOD6_E"].ToString().Trim() == "")
				{
					tmmsm56a2["AOD6_E"] = "A6999999";
				}
				if (tmmsm56a2["BOF0_S"].ToString().Trim() == "")
				{
					tmmsm56a2["BOF0_S"] = "B0000001";
				}
				if (tmmsm56a2["BOF0_E"].ToString().Trim() == "")
				{
					tmmsm56a2["BOF0_E"] = "B0999999";
				}
				if (tmmsm56a2["BOF1_S"].ToString().Trim() == "")
				{
					tmmsm56a2["BOF1_S"] = "B1000001";
				}
				if (tmmsm56a2["BOF1_E"].ToString().Trim() == "")
				{
					tmmsm56a2["BOF1_S"] = "B1999999";
				}
				if (tmmsm56a2["BOF2_S"].ToString().Trim() == "")
				{
					tmmsm56a2["BOF2_S"] = "B2000001";
				}
				if (tmmsm56a2["BOF2_E"].ToString().Trim() == "")
				{
					tmmsm56a2["BOF2_E"] = "B2999999";
				}
				if (tmmsm56a2["BOF9_S"].ToString().Trim() == "")
				{
					tmmsm56a2["BOF9_S"] = "B9000001";
				}
				if (tmmsm56a2["BOF9_E"].ToString().Trim() == "")
				{
					tmmsm56a2["BOF9_E"] = "B9999999";
				}

				sql_sj += " AND ((HEAT_NO>='" + tmmsm56a2["AOD0_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD0_E"].ToString() + "')";
				sql_sj += " OR (HEAT_NO>='" + tmmsm56a2["AOD1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD1_E"].ToString() + "')";
				sql_sj += " OR (HEAT_NO>='" + tmmsm56a2["AOD2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD2_E"].ToString() + "')";
				sql_sj += " OR (HEAT_NO>='" + tmmsm56a2["AOD6_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD6_E"].ToString() + "')";
				sql_sj += " OR (HEAT_NO>='" + tmmsm56a2["BOF0_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF0_E"].ToString() + "')";
				sql_sj += " OR (HEAT_NO>='" + tmmsm56a2["BOF1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF1_E"].ToString() + "')";
				sql_sj += " OR (HEAT_NO>='" + tmmsm56a2["BOF2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF2_E"].ToString() + "')";
				sql_sj += " OR (HEAT_NO>='" + tmmsm56a2["BOF9_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF9_E"].ToString() + "')";
				sql_sj = sql_sj + ")";
				sqlflag = true;
			}
			else
			{
				sql_sj = " and stat_date=@stat_date";
			}
			if (bcls_rec->Tables[0].Rows[0]["HEAT_IN"].ToString().Trim() != "")
			{
				bcls_rec->Tables[0].Rows[0]["HEAT_IN"] = bcls_rec->Tables[0].Rows[0]["HEAT_IN"].ToString().Replace(",", "',' ");
				if (sqlflag)
				{
					Log::Info("", __FUNCTION__, "sql_sj =[{0}]", sql_sj);
					sql_sj = sql_sj.SubstringNE(0, sql_sj.GetLength() - 1);
					Log::Info("", __FUNCTION__, "sql_sj =[{0}]", sql_sj);
					sql_sj += " OR (heat_no in ('" + bcls_rec->Tables[0].Rows[0]["HEAT_IN"].ToString() + "') AND HANDLE_DIV != 'F'))";
				}
				
			}
			if (bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString().Trim() != "")
			{ 
			Log::Info("", __FUNCTION__, "HEAT_OUT =[{0}],HEAT_OUT后=[{1}]", bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString(), bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString().Replace(",", "', '"));

				bcls_rec->Tables[0].Rows[0]["HEAT_OUT"] = bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString().Replace(",", "',' "); 				
				sql_sj += " and heat_no not in ('" + bcls_rec->Tables[0].Rows[0]["HEAT_OUT"].ToString() + "')";

			}

			sqlstr = " SELECT EAF1_E,EAF2_E,IF1_E,IF2_E,IF3_E,IF4_E,IF5_E,IF6_E,IF7_E,IF8_E "
				" from  V_DA_HEAT_S_E_COMPUTE"				
				" where WEEK_DAY = @cx_date"
				;
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("cx_date", cx_date);
			cmd_inq_1.ExecuteReader();
			if (cmd_inq_1.Read())
			{
				sqlstr = " SELECT EAF1_E,EAF2_E,IF1_E,IF2_E,IF3_E,IF4_E,IF5_E,IF6_E,IF7_E,IF8_E "
					" from  V_DA_HEAT_S_E1"
					" where WEEK_DAY = @cx_date"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("cx_date", cx_date);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					sqlstr_where = " and (";
					if (cmd_inq_1.GetString(1).Trim() != ""&&cmd_inq.GetString(1).Trim() != "")
					{
						sqlstr_where = sqlstr_where + "  (l2_proc_no>'" + cmd_inq_1.GetString(1) + "' and l2_proc_no<='" + cmd_inq.GetString(1) + "')";
					}
					if (cmd_inq_1.GetString(2).Trim() != ""&&cmd_inq.GetString(2).Trim() != "")
					{
						if (sqlstr_where == " and (")
						{
							sqlstr_where = sqlstr_where + "  (l2_proc_no>'" + cmd_inq_1.GetString(2) + "' and l2_proc_no<='" + cmd_inq.GetString(2) + "')";
						}
						else
						{
							sqlstr_where = sqlstr_where + " or (l2_proc_no>'" + cmd_inq_1.GetString(2) + "' and l2_proc_no<='" + cmd_inq.GetString(2) + "')";
						}
						
					}
					if (cmd_inq_1.GetString(3).Trim() != ""&&cmd_inq.GetString(3).Trim() != "")
					{
						if (sqlstr_where == " and (")
						{
							sqlstr_where = sqlstr_where + "  (l2_proc_no>'" + cmd_inq_1.GetString(3) + "' and l2_proc_no<='" + cmd_inq.GetString(3) + "')";
						}
						else
						{
							sqlstr_where = sqlstr_where + " or (l2_proc_no>'" + cmd_inq_1.GetString(3) + "' and l2_proc_no<='" + cmd_inq.GetString(3) + "')";
						}
					}
					if (cmd_inq_1.GetString(4).Trim() != ""&&cmd_inq.GetString(4).Trim() != "")
					{
						if (sqlstr_where == " and (")
						{
							sqlstr_where = sqlstr_where + "  (l2_proc_no>'" + cmd_inq_1.GetString(4) + "' and l2_proc_no<='" + cmd_inq.GetString(4) + "')";
						}
						else
						{
							sqlstr_where = sqlstr_where + " or (l2_proc_no>'" + cmd_inq_1.GetString(4) + "' and l2_proc_no<='" + cmd_inq.GetString(4) + "')";
						}
					}
					if (cmd_inq_1.GetString(5).Trim() != ""&&cmd_inq.GetString(5).Trim() != "")
					{
						if (sqlstr_where == " and (")
						{
							sqlstr_where = sqlstr_where + "  (l2_proc_no>'" + cmd_inq_1.GetString(5) + "' and l2_proc_no<='" + cmd_inq.GetString(5) + "')";
						}
						else
						{
							sqlstr_where = sqlstr_where + " or (l2_proc_no>'" + cmd_inq_1.GetString(5) + "' and l2_proc_no<='" + cmd_inq.GetString(5) + "')";
						}
					}
					if (cmd_inq_1.GetString(6).Trim() != ""&&cmd_inq.GetString(6).Trim() != "")
					{
						if (sqlstr_where == " and (")
						{
							sqlstr_where = sqlstr_where + "  (l2_proc_no>'" + cmd_inq_1.GetString(6) + "' and l2_proc_no<='" + cmd_inq.GetString(6) + "')";

						}
						else
						{
							sqlstr_where = sqlstr_where + " or (l2_proc_no>'" + cmd_inq_1.GetString(6) + "' and l2_proc_no<='" + cmd_inq.GetString(6) + "')";
						}
					}
					if (cmd_inq_1.GetString(7).Trim() != ""&&cmd_inq.GetString(7).Trim() != "")
					{
						if (sqlstr_where == " and (")
						{
							sqlstr_where = sqlstr_where + " or (l2_proc_no>'" + cmd_inq_1.GetString(7) + "' and l2_proc_no<='" + cmd_inq.GetString(7) + "')";

						}
						else
						{
							sqlstr_where = sqlstr_where + " or (l2_proc_no>'" + cmd_inq_1.GetString(7) + "' and l2_proc_no<='" + cmd_inq.GetString(7) + "')";
						}
					}
					if (cmd_inq_1.GetString(8).Trim() != ""&&cmd_inq.GetString(8).Trim() != "")
					{
						if (sqlstr_where == " and (")
						{
							sqlstr_where = sqlstr_where + "  (l2_proc_no>'" + cmd_inq_1.GetString(8) + "' and l2_proc_no<='" + cmd_inq.GetString(8) + "')";

						}
						else
						{
							sqlstr_where = sqlstr_where + " or (l2_proc_no>'" + cmd_inq_1.GetString(8) + "' and l2_proc_no<='" + cmd_inq.GetString(8) + "')";
						}
							
					}
				}
				cmd_inq.Close();

				sqlstr_where = sqlstr_where + " )";
			}
			cmd_inq_1.Close();

			Log::Info("", __FUNCTION__, "sqlstr_where =[{0}]", sqlstr_where);
			if (sqlstr_where == " and ( )" || sqlstr_where == "")
			{
				sqlstr_where = " and 1=0";
			} 


			if (CDateTime::Now().ToString("yyyyMM") == stat_date.Substring(0, 6))
			{
				out_stock_time = CDateTime::Now().ToString("yyyyMMdd");
			}
			else
			{
				sqlstr = "select to_number(to_char(last_day(to_date(@stat_date,'yyyyMM')),'yyyyMMdd')) "
					" from dual"
					;
				cmd_inq_1.SetCommandText(sqlstr);
				cmd_inq_1.Parameters.Set("stat_date", stat_date);
				cmd_inq_1.ExecuteReader();
				if (cmd_inq_1.Read())
				{
					out_stock_time = cmd_inq_1.GetString(1);
				}
				cmd_inq_1.Close();
			}

			sqlstr = " SELECT STAT_DATE,MAT_CODE,MAT_NAME,STOCK_WT_QC,STOCK_WT_QC1,STOCK_WT_QC2,IN_STOCK_WT1,IN_STOCK_WT2,IN_STOCK_WT,OUT_STOCK_WT,OUT_STOCK_WT1,OUT_STOCK_WT2,OUT_STOCK_WT3,STOCK_WT,STOCK_WT1,STOCK_WT2,REMARK,SEQ_ID,FT_FLAG,FT_WT"
				",mes_wt/1000 AS mes_wt"
				",NOUP_WT/1000 AS NOUP_WT"
				",OUT_STOCK_WT1-mes_wt/1000-NOUP_WT/1000 as dif_mes"
				",STOCK_WT_ZY "
				",STOCK_WT_ZY - (OUT_STOCK_WT1-mes_wt/1000-NOUP_WT/1000) as dif_zy"				
				" FROM ("
				" select STAT_DATE,t1.MAT_CODE,MAT_NAME,STOCK_WT_QC,STOCK_WT_QC1,STOCK_WT_QC2,IN_STOCK_WT1,IN_STOCK_WT2,IN_STOCK_WT,OUT_STOCK_WT,OUT_STOCK_WT1,OUT_STOCK_WT2,OUT_STOCK_WT3,STOCK_WT,STOCK_WT1,STOCK_WT2,REMARK,SEQ_ID,FT_FLAG,FT_WT"
				" ,nvl(mes_wt, 0) mes_wt"
				" ,nvl((select sum(devo_wt) from tmmsmgy08 t2 where 1=1 and t1.mat_code = t2.mat_code " + sqlstr_where + "),0) NOUP_WT"
				",nvl((select sum(STOCK_WT) from (select STOCK_WT  from tmmsm57c t2 where stat_date in ( select max(stat_date) from tmmsm57c where stat_date like  @stat_date_s||'%'  and STOCK_CODE='6241' )  and STOCK_CODE='6241'   and  t1.mat_code = t2.mat_code  union all select sum(STOCK_WT) from tmmsm57d t3 where stat_date in ( select max(stat_date) from tmmsm57d where stat_date like  @stat_date_s||'%')   and  t3.mat_code = t1.mat_code) ),0) as STOCK_WT_ZY" //取铁区资源的库存 				
				" from tmmsm57a t1"	
				" left join ("
				" select mat_code,sum(devo_wt) mes_wt from (select mat_code,sum(devo_wt) devo_wt from  tmmsm2a_send where 1 = 1 and  send_flag = '1' and  RTN_FLAG != '1' AND HANDLE_DIV != 'F'" + sql_sj + " group by mat_code  union all select mat_code,sum(devo_wt) devo_wt from tmmsm2a_send where 1 = 1 and  send_flag = '1' and  RTN_FLAG != '1' AND HANDLE_DIV = 'F'" + sql_ft + "  group by mat_code ) group by mat_code"
				" ) t2 on t1.mat_code = t2.mat_code"
				" where 1=1"
				;
			if (tmmsm57a["MAT_CODE"].ToString().Trim() != "")
				sqlstr = sqlstr + " and t1.mat_code like @mat_code||'%'";
			if (tmmsm57a["MAT_NAME"].ToString().Trim() != "")
				sqlstr = sqlstr + " and t1.mat_name like '%'||@mat_name||'%'";
			sqlstr = sqlstr + " and stat_date like @stat_date||'%' ) order by mat_code";

			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("mat_code", tmmsm57a["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_name", tmmsm57a["MAT_NAME"].ToString());
			cmd_inq.Parameters.Set("stat_date_s", stat_date.Substring(0, 6));
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

		
			sqlstr = " SELECT STAT_DATE,MAT_CODE,MAT_NAME"
				",mes_wt/1000 AS mes_wt"
				",OUT_STOCK_WT1-mes_wt/1000-NOUP_WT/1000 as OUT_STOCK_WT"
				",OUT_STOCK_WT1"
				",STOCK_WT_ZY  as STOCK_WT"
				",'N' AS TYPE_CODE"
				",'6240' AS FACTORY_CODE"
				",'1' AS FT_FLAG"
				",@out_stock_time AS OUT_STOCK_TIME"   
				",STOCK_WT_ZY-(OUT_STOCK_WT1-mes_wt/1000-NOUP_WT/1000) AS dif_zy_kc"
				",decode(MES_WT,0,0,round((OUT_STOCK_WT1-mes_wt/1000-NOUP_WT/1000)*100/(mes_wt/1000),2)) as kc_mes"
				" FROM ("
				" select STAT_DATE,t1.MAT_CODE,MAT_NAME,STOCK_WT_QC,STOCK_WT_QC1,STOCK_WT_QC2,IN_STOCK_WT1,IN_STOCK_WT2,IN_STOCK_WT,OUT_STOCK_WT,OUT_STOCK_WT1,OUT_STOCK_WT2,OUT_STOCK_WT3,STOCK_WT,STOCK_WT1,STOCK_WT2,REMARK,SEQ_ID,FT_FLAG,FT_WT"
				" ,nvl(mes_wt,0) mes_wt"
				" ,nvl((select sum(devo_wt) from tmmsmgy08 t2 where 1=1 and t1.mat_code = t2.mat_code " + sqlstr_where + "),0) NOUP_WT"
				",nvl((select sum(STOCK_WT) from (select STOCK_WT  from tmmsm57c t2 where stat_date in ( select max(stat_date) from tmmsm57c where stat_date like  @stat_date_s||'%'  and STOCK_CODE='6241' )  and STOCK_CODE='6241'   and  t1.mat_code = t2.mat_code  union all select sum(STOCK_WT) from tmmsm57d t3 where stat_date in ( select max(stat_date) from tmmsm57d where stat_date like  @stat_date_s||'%')   and  t3.mat_code = t1.mat_code) ),0) as STOCK_WT_ZY" //取铁区资源的库存
				" from tmmsm57a t1"
				" left join ("
				" select mat_code,sum(devo_wt) mes_wt from (select mat_code,sum(devo_wt) devo_wt from  tmmsm2a_send where 1 = 1 and  send_flag = '1' and  RTN_FLAG != '1' AND HANDLE_DIV != 'F'" + sql_sj + " group by mat_code  union all select mat_code,sum(devo_wt) devo_wt from tmmsm2a_send where 1 = 1 and  send_flag = '1' and  RTN_FLAG != '1' AND HANDLE_DIV = 'F'" + sql_ft + "  group by mat_code ) group by mat_code"
				" ) t2 on t1.mat_code = t2.mat_code"
				" where 1=1"
				;
			if (tmmsm57a["MAT_CODE"].ToString().Trim() != "")
				sqlstr = sqlstr + " and t1.mat_code like @mat_code||'%'";
			if (tmmsm57a["MAT_NAME"].ToString().Trim() != "")
				sqlstr = sqlstr + " and t1.mat_name like '%'||@mat_name||'%'";
			sqlstr = sqlstr + " and stat_date like @stat_date||'%' ) order by mat_code ";

			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			

			bcls_ret->Tables.Add();
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("mat_code", tmmsm57a["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_name", tmmsm57a["MAT_NAME"].ToString());
			cmd_inq.Parameters.Set("stat_date_s", stat_date.Substring(0, 6));
			cmd_inq.Parameters.Set("out_stock_time", out_stock_time);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			cmd_inq.Close();

			
		}
		else
		{
			stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 8);
			tmmsm57c.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			sqlstr = " select * "
				" from " + v_table + " t1"
				" where 1=1"
				;
			if (stat_date.Trim() != "")
				sqlstr = sqlstr + " and stat_date like @stat_date||'%'";
			if (tmmsm57c["MAT_CODE"].ToString().Trim() != "")
				sqlstr = sqlstr + " and mat_code like @mat_code||'%'";
			if (tmmsm57c["MAT_NAME"].ToString().Trim() != "")
				sqlstr = sqlstr + " and mat_name like '%'||@mat_name||'%'";
			if (tmmsm57c["STOCK_CODE"].ToString().Trim() != "")
				sqlstr = sqlstr + " and STOCK_CODE =@stock_code";

			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stat_date", stat_date);
			cmd_inq.Parameters.Set("mat_code", tmmsm57c["MAT_CODE"].ToString());
			cmd_inq.Parameters.Set("mat_name", tmmsm57c["MAT_NAME"].ToString());
			cmd_inq.Parameters.Set("stock_code", tmmsm57c["STOCK_CODE"].ToString());
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
