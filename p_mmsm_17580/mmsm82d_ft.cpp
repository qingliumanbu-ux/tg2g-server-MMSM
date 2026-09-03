
/// <summary>
/// 功能说明:针对消耗信息进行分摊操作
/*  分摊方式FT_FLAG 5-产量;7-按周处理号；4-处理大于125量的（未开发）;3-按指定机组分摊;6-指定所有消耗
*/
/// </summary>


#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsm82d_ft)
int f_mmsm82d_ft(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int affectRows = 0;
	int	 union_flag = 0;
	int	 empty_flag = 0;
	CString curr_heat_no = " ";
	CString sqlstr = " ";
	CString sqlstr_temp = "";
	CString sqlstr_where = " ";
	CString mat_code = " ";
	CString stat_date = "";
	CString vtable = "";	
	CDecimal all_wt = 0;
	CDecimal dif_wt = 0;
	CDecimal dif_wt_i = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString sg_sign = "";
	CString dev_remark_1 = "";
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsm56a("TMMSM56A");
	CModel tmmsm56a2("TMMSM56A2");
	CModel tmmsm56ft("TMMSM56FT");

	CString seq_id = "0";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_2(conn);
	try
	{


		stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6);
		Log::Info("", __FUNCTION__, "stat_date =[{0}]", stat_date);	  		

		sqlstr = " delete from tmmsm56ft"
			" where 1=1"
			" and SEND_FLAG!='1'"
			" and HANDLE_DIV = 'F'"
			" and stat_date=@stat_date"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//插入消耗信息
		sqlstr = " select *  from  tmmsm56a"
			" where 1=1"
			" and send_flag !='1'"
			" and stat_date=@stat_date"
			" order by SEQ_ID"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq_2.SetCommandText(sqlstr);
		cmd_inq_2.Parameters.Set("stat_date", stat_date);
		cmd_inq_2.ExecuteReader();
		while (cmd_inq_2.Read())
		{
			tmmsm56a.Reset();
			cmd_inq_2.Fetch(tmmsm56a);

			Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm56a["MAT_CODE"].ToString());
			tmmsm56a["OUT_STOCK_WT"] = tmmsm56a["OUT_STOCK_WT"].ToDecimal() * 1000;
			if (tmmsm56a["RATE"].ToDecimal() == 0)
			{
				tmmsm56a["RATE"] = 100;
			}
			
			if (tmmsm56a["FT_FLAG"].ToString() == "3")
			{
				sg_sign = "";
				dev_remark_1 = "";
				sqlstr_where = "";
				if (tmmsm56a["SG_SIGN"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = " and st_no in ('" + tmmsm56a["SG_SIGN"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["SG_SIGN_OUT"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = sg_sign + " and st_no not in ('" + tmmsm56a["SG_SIGN_OUT"].ToString().Replace(",", "','") + "')";
				} 				
				//Log::Info("", __FUNCTION__, "sg_sign =[{0}],dev_remark_1 = [{1}]", sg_sign, dev_remark_1);
				sqlstr_where = sqlstr_where + sg_sign + dev_remark_1;
				if (tmmsm56a["HEAT_NO"].ToString().Trim() != "")  //进行拼接
				{
					sqlstr_where = sqlstr_where + " and heat_no in ('" + tmmsm56a["HEAT_NO"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["STEEL_TYPE"].ToString().Trim() != "")  //进行拼接
				{
					if (tmmsm56a["STEEL_TYPE"].ToString() == "C")  //碳钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('2','3','5') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "S")	//不锈钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('1','4') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Cr")	 //铬钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1F','1M','4F','4M') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Ni")	 //镍钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1A','1D','4A','4D') ";
					}
				}

				if (tmmsm56a["TYPE_CODE"].ToString().Trim() == "Y")	// 	使用推荐的物料编码对应的大类作为基数来分摊
				{
					all_wt = 0;
					sqlstr = " select sum(DEVO_WT) use_wt from tmmsm2a_send"
						" where 1=1"
						+ sqlstr_where +
						" and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
						" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
					}
					else
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
					}
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						all_wt = cmd_inq.GetDecimal(1);
					}
					cmd_inq.Close();
					if (all_wt > 0)	 //如果有值则按原消耗量比例来，否则按产量来
					{
						sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,sm_plan_nol2,heat_no,st_no,@dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select sm_plan_nol2, heat_no, st_no, lot_no, @mat_code as mat_code,round(@all_use_wt*DEVO_WT/all_wt,0) use_wt,DEVO_WT"
							" from "
							" ("
							"  select sm_plan_nol2, heat_no, st_no, lot_no,devo_wt,sum(devo_wt) over() all_wt "
							"  from "
							" ("
							"  select sm_plan_nol2, heat_no, st_no, lot_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt "
							"  from "
							"  (select sm_plan_nol2, heat_no, st_no, lot_no,sum(DEVO_WT) DEVO_WT"
							"  from tmmsm2a_send	t1 "
							"  where 1=1"
							+ sqlstr_where +
							"  and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							"  and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
							"  and stat_date = @stat_date"
							"  group by sm_plan_nol2, heat_no, st_no, lot_no)"
							" )"
							" where  all_wt_x<=all_wt*@xs/100"
							" )"
							" )"
							" where use_wt!=0"
							;
						Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("rec_creator", s.userid);
						cmd_inq.Parameters.Set("rec_create_time", dateNow);
						cmd_inq.Parameters.Set("all_use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
						cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
						cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
						cmd_inq.Parameters.Set("dev_code", tmmsm56a["DEV_REMARK_1"].ToString().SubstringNE(0,2));
						if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
						}
						else
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
						}
						cmd_inq.Parameters.Set("all_wt", all_wt);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
						cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
						cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}
				}
				else
				{
					all_wt = 0;
					sqlstr = " select sum(DEVO_WT) use_wt from tmmsm2a_send"
						" where 1=1"
						+ sqlstr_where +
						" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						" and mat_code =@mat_code_t"
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
					}
					else
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
					}
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						all_wt = cmd_inq.GetDecimal(1);
					}
					cmd_inq.Close();
					if (all_wt > 0)	 //如果有值则按原消耗量比例来
					{
						sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,sm_plan_nol2,heat_no,st_no,@dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,WEIGH_NO,QUALITY_BATCH_NO,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select sm_plan_nol2, heat_no, st_no,  lot_no, @mat_code as mat_code,  WEIGH_NO, QUALITY_BATCH_NO,round((@all_use_wt*devo_wt/all_wt),0) use_wt,devo_wt"
							" from  ("
							" select sm_plan_nol2, heat_no, st_no,  lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over() all_wt"
							" from ("
							" select sm_plan_nol2, heat_no, st_no,  lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt"
							" from "
							" (select sm_plan_nol2, heat_no, st_no,  lot_no, weigh_no, quality_batch_no,sum(DEVO_WT) devo_wt"
							" from tmmsm2a_send	t1 "
							" where 1=1"
							+ sqlstr_where +
							" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							" and mat_code =@mat_code_t"
							" and stat_date = @stat_date"
							" group by sm_plan_nol2, heat_no, st_no, lot_no,WEIGH_NO, QUALITY_BATCH_NO)"
							" )"
							" where all_wt_x<=@xs*all_wt/100"
							" )"
							" )"
							" where use_wt!=0"
							;
						Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("rec_creator", s.userid);
						cmd_inq.Parameters.Set("rec_create_time", dateNow);
						cmd_inq.Parameters.Set("all_use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
						cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
						cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
						cmd_inq.Parameters.Set("dev_code", tmmsm56a["DEV_REMARK_1"].ToString().SubstringNE(0, 2));
						if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
						}
						else
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
						}
						cmd_inq.Parameters.Set("all_wt", all_wt);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
						cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
						cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}

				}

				for (int i = 0; i < 1000; i++)
				{
					dif_wt = 0;
					//尾插处理
					sqlstr = " select sum(OUT_STOCK_WT) OUT_STOCK_WT"
						" from tmmsm56ft"
						" where 1=1"
						" and OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
						" and HANDLE_DIV = 'F'"
						" and mat_code =@mat_code"
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						Log::Info("", __FUNCTION__, "总消耗 =[{0}],分摊的量 = [{1}]", tmmsm56a["OUT_STOCK_WT"].ToDecimal(), cmd_inq.GetDecimal(1));
						if (cmd_inq.GetDecimal(1) != tmmsm56a["OUT_STOCK_WT"].ToDecimal())
						{
							dif_wt = tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1);
							if (dif_wt < 1)
							{
								dif_wt_i = -1;
							}
							else
							{
								dif_wt_i = 1;
							}

							sqlstr = " select OUT_STOCK_NO,DEVO_WT "
								" from tmmsm56ft"
								" where OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
								" and HANDLE_DIV = 'F'"
								" and mat_code =@mat_code"
								" and stat_date = @stat_date"
								" order by DEVO_WT desc,heat_no"
								;
							cmd_inq_s.SetCommandText(sqlstr);
							cmd_inq_s.Parameters.Set("stat_date", stat_date);
							cmd_inq_s.Parameters.Set("out_stock_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1));
							cmd_inq_s.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
							cmd_inq_s.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
							cmd_inq_s.ExecuteReader();
							while (cmd_inq_s.Read())
							{
								sqlstr = " update tmmsm56ft set OUT_STOCK_WT = OUT_STOCK_WT + @out_stock_wt"
									" where 1=1"
									" and OUT_STOCK_NO =@out_stock_no"
									" and HANDLE_DIV = 'F'"
									" and mat_code =@mat_code"
									" and stat_date = @stat_date"
									" and rownum=1"
									;
								Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("out_stock_no", cmd_inq_s.GetString(1));
								cmd_inq_1.Parameters.Set("stat_date", stat_date);
								if (dif_wt<1 && dif_wt>-1)
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt);
									dif_wt = 0;
								}
								else
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt_i);
								}
								cmd_inq_1.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
								cmd_inq_1.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

								dif_wt = dif_wt - dif_wt_i;
								if (dif_wt == 0)
								{
									break;
								}
							}
							cmd_inq_s.Close();

						}
					}
					cmd_inq.Close();

					if (dif_wt == 0)
					{
						break;
					}
				}
			}
			if (tmmsm56a["FT_FLAG"].ToString() == "4")
			{  				
			}
			else if (tmmsm56a["FT_FLAG"].ToString() == "5")	 //根据产量来分摊
			{  			

				sg_sign = "";
				dev_remark_1 = "";
				sqlstr_where = "";
				if (tmmsm56a["SG_SIGN"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = " and st_no in ('" + tmmsm56a["SG_SIGN"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["SG_SIGN_OUT"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = sg_sign + " and st_no not in ('" + tmmsm56a["SG_SIGN_OUT"].ToString().Replace(",", "','") + "')";
				}
				
				//机组用来判断消耗
				//if (tmmsm56a["DEV_REMARK_1"].ToString().Trim() != "")  //进行拼接
				//{

				//	dev_remark_1 = " and heat_no in ( select heat_no from tmmsmgy06 where dev_code in ('" + tmmsm56a["DEV_REMARK_1"].ToString().Replace(",", "','") + "') and stat_date=@stat_date)";
				//}
				sqlstr_where = sqlstr_where + sg_sign + dev_remark_1;

				if (tmmsm56a["HEAT_NO"].ToString().Trim() != "")  //进行拼接
				{
					sqlstr_where = sqlstr_where + " and heat_no in ('" + tmmsm56a["HEAT_NO"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["STEEL_TYPE"].ToString().Trim() != "")  //进行拼接
				{
					if (tmmsm56a["STEEL_TYPE"].ToString() == "C")  //碳钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('2','3','5') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "S")	//不锈钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('1','4') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Cr")	 //铬钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1F','1M','4F','4M') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Ni")	 //镍钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1A','1D','4A','4D') ";
					}
				}


				all_wt = 0;
				sqlstr = " select sum(MAT_ACT_WT) use_wt from tmmsm56b"
					" where 1=1"
					+ sqlstr_where +
					" and stat_date = @stat_date"
					;
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					all_wt = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();
				if (all_wt > 0)
				{
					sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
						" select @rec_creator,@rec_create_time,@stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,@out_stock_time,@out_stock_time,use_wt,mat_act_wt,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
						" from ("
						" select sm_plan_nol2, heat_no, st_no, nvl(@dev_code,substr(heat_no,1,2)) as dev_code, @mat_code as mat_code,@lot_no as lot_no, mat_act_wt,round(@all_use_wt*mat_act_wt/all_wt,0) use_wt"
						" from ("
						"  select sm_plan_nol2, heat_no, st_no,mat_act_wt,sum(mat_act_wt) over() all_wt"
						"  from ("
						"      select sm_plan_nol2, heat_no, st_no,mat_act_wt,sum(mat_act_wt) over(order by mat_act_wt desc) all_wt_x,sum(mat_act_wt) over() all_wt"
						"      from tmmsm56B	t1 "
						"      where 1=1"
						+ sqlstr_where +
						"      and stat_date = @stat_date"
						"      )"
						" where  all_wt_x<=all_wt*@xs/100"
						" )"
						" )"
						" where use_wt!=0"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.Parameters.Set("rec_creator", s.userid);
					cmd_inq.Parameters.Set("rec_create_time", dateNow);
					cmd_inq.Parameters.Set("all_use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
					cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					cmd_inq.Parameters.Set("all_wt", all_wt);
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
					cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
					cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
					cmd_inq.Parameters.Set("dev_code", tmmsm56a["DEV_REMARK_1"].ToString().SubstringNE(0, 2).Trim());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}
				else
				{
					sprintf(s.msg, "没有符合条件的分摊方式，物料为：" + tmmsm56a["MAT_CODE"].ToString());
					//strcpy(s.sysmsg,s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}


				//尾插处理
				sqlstr = " select sum(OUT_STOCK_WT) OUT_STOCK_WT"
					" from tmmsm56ft"
					" where 1=1"
					" and OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
					" and HANDLE_DIV = 'F'"
					" and mat_code =@mat_code"
					" and stat_date = @stat_date"
					;
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
				cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					if (cmd_inq.GetDecimal(1) != tmmsm56a["OUT_STOCK_WT"].ToDecimal())
					{
						dif_wt = tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1);
						if (dif_wt<1)
						{
							dif_wt_i = -1;
						}
						else
						{
							dif_wt_i = 1;
						}

						sqlstr = " select OUT_STOCK_NO,DEVO_WT "
							" from tmmsm56ft"
							" where OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
							" and HANDLE_DIV = 'F'"
							" and mat_code =@mat_code"
							" and stat_date = @stat_date"
							" order by DEVO_WT desc,heat_no"
							;
						cmd_inq_s.SetCommandText(sqlstr);
						cmd_inq_s.Parameters.Set("stat_date", stat_date);
						cmd_inq_s.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
						cmd_inq_s.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
						cmd_inq_s.ExecuteReader();
						while (cmd_inq_s.Read())
						{
							sqlstr = " update tmmsm56ft set OUT_STOCK_WT = OUT_STOCK_WT + @out_stock_wt"
								" where 1=1"
								" and OUT_STOCK_NO =@out_stock_no"
								" and HANDLE_DIV = 'F'"
								" and mat_code =@mat_code"
								" and stat_date = @stat_date"
								" and rownum=1"
								;
							Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
							cmd_inq_1.SetCommandText(sqlstr);
							cmd_inq_1.Parameters.Set("out_stock_no", cmd_inq_s.GetString(1));
							cmd_inq_1.Parameters.Set("stat_date", stat_date);
							if (dif_wt<1 && dif_wt>-1)
							{
								cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt);
								dif_wt = 0;
							}
							else
							{
								cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt_i);
							}
							cmd_inq_1.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
							cmd_inq_1.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
							cmd_inq_1.ExecuteNonQuery();
							cmd_inq_1.Close();

							dif_wt = dif_wt - dif_wt_i;
							if (dif_wt == 0)
							{
								break;
							}
						}
						cmd_inq_s.Close();
					}
				}
				cmd_inq.Close();
			}
			else if (tmmsm56a["FT_FLAG"].ToString() == "6")	 //根据数据精准直接插入
			{
			
					sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
						" values( @rec_creator,@rec_create_time,@stat_date,' ',@heat_no,@sg_sign,@dev_remark_1,@mat_code,@lot_no,@out_stock_time,@out_stock_time,@use_wt,0,'F','F'||trim(to_char(@seq_id, '0000'))|| '00000001' )" 						
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.Parameters.Set("rec_creator", s.userid);
					cmd_inq.Parameters.Set("rec_create_time", dateNow);
					cmd_inq.Parameters.Set("use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
					cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
					cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
					cmd_inq.Parameters.Set("dev_remark_1", tmmsm56a["DEV_REMARK_1"].ToString().SubstringNE(0, 2).Trim());
					cmd_inq.Parameters.Set("sg_sign", tmmsm56a["SG_SIGN"].ToString());
					cmd_inq.Parameters.Set("heat_no", tmmsm56a["HEAT_NO"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();  			
							
			}
			else if (tmmsm56a["FT_FLAG"].ToString() == "7")
			{
				
				//查周炉号
				sqlstr_where = "";
				sqlstr = " select * from tmmsm56a2 "
				" where 1=1"
				" and  proc_count=@proc_count "
				" and  stat_date=@stat_date "
				;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", tmmsm56a["STAT_DATE"].ToString());
				cmd_inq.Parameters.Set("proc_count", tmmsm56a["PROC_COUNT"].ToDecimal());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm56a2.Reset();
					cmd_inq.Fetch(tmmsm56a2);

					//AOD
					sqlstr_where = "";
					if (tmmsm56a2["AOD0_S"].ToString().Trim() != ""&& tmmsm56a2["AOD0_E"].ToString().Trim() != "")
					{
						sqlstr_where += " AND ((HEAT_NO>='" + tmmsm56a2["AOD0_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD0_E"].ToString() + "')";
					}

					if (sqlstr_where.Trim() == "")
					{
						if (tmmsm56a2["AOD1_S"].ToString().Trim() != ""&& tmmsm56a2["AOD1_E"].ToString().Trim() != "")
						{
							sqlstr_where += " and ((HEAT_NO>='" + tmmsm56a2["AOD1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD1_E"].ToString() + "')";
						}
					}
					else
					{
						if (tmmsm56a2["AOD1_S"].ToString().Trim() != ""&& tmmsm56a2["AOD1_E"].ToString().Trim() != "")
						{
							sqlstr_where += " OR (HEAT_NO>='" + tmmsm56a2["AOD1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD1_E"].ToString() + "')";
						}
					}


					if (sqlstr_where.Trim() == "")
					{
						if (tmmsm56a2["AOD2_S"].ToString().Trim() != ""&& tmmsm56a2["AOD2_E"].ToString().Trim() != "")
						{
							sqlstr_where += " and ((HEAT_NO>='" + tmmsm56a2["AOD2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD2_E"].ToString() + "')";
						}
					}
					else
					{
						if (tmmsm56a2["AOD2_S"].ToString().Trim() != ""&& tmmsm56a2["AOD2_E"].ToString().Trim() != "")
						{
							sqlstr_where += " OR (HEAT_NO>='" + tmmsm56a2["AOD2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD2_E"].ToString() + "')";
						}
					}

					if (sqlstr_where.Trim() == "")
					{
						if (tmmsm56a2["AOD6_S"].ToString().Trim() != ""&& tmmsm56a2["AOD6_E"].ToString().Trim() != "")
						{
							sqlstr_where += " and ((HEAT_NO>='" + tmmsm56a2["AOD6_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD6_E"].ToString() + "')";
						}
					}
					else
					{
						if (tmmsm56a2["AOD6_S"].ToString().Trim() != ""&& tmmsm56a2["AOD6_E"].ToString().Trim() != "")
						{
							sqlstr_where += " OR (HEAT_NO>='" + tmmsm56a2["AOD6_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["AOD6_E"].ToString() + "')";
						}
					}
					if (sqlstr_where.Trim() == "")
					{
						if (tmmsm56a2["BOF0_S"].ToString().Trim() != ""&& tmmsm56a2["BOF0_E"].ToString().Trim() != "")
						{
							sqlstr_where += " and ((HEAT_NO>='" + tmmsm56a2["BOF0_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF0_E"].ToString() + "')";
						}
					}
					else
					{
						if (tmmsm56a2["BOF0_S"].ToString().Trim() != ""&& tmmsm56a2["BOF0_E"].ToString().Trim() != "")
						{
							sqlstr_where += " OR (HEAT_NO>='" + tmmsm56a2["BOF0_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF0_E"].ToString() + "')";
						}
					}
					if (sqlstr_where.Trim() == "")
					{
						if (tmmsm56a2["BOF1_S"].ToString().Trim() != ""&& tmmsm56a2["BOF1_E"].ToString().Trim() != "")
						{
							sqlstr_where += " and ((HEAT_NO>='" + tmmsm56a2["BOF1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF1_E"].ToString() + "')";
						}
					}
					else
					{
						if (tmmsm56a2["BOF1_S"].ToString().Trim() != ""&& tmmsm56a2["BOF1_E"].ToString().Trim() != "")
						{
							sqlstr_where += " OR (HEAT_NO>='" + tmmsm56a2["BOF1_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF1_E"].ToString() + "')";
						}
					}
					if (sqlstr_where.Trim() == "")
					{
						if (tmmsm56a2["BOF2_S"].ToString().Trim() != ""&& tmmsm56a2["BOF2_E"].ToString().Trim() != "")
						{
							sqlstr_where += " and ((HEAT_NO>='" + tmmsm56a2["BOF2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF2_E"].ToString() + "')";
						}
					}
					else
					{
						if (tmmsm56a2["BOF2_S"].ToString().Trim() != ""&& tmmsm56a2["BOF2_E"].ToString().Trim() != "")
						{
							sqlstr_where += " OR (HEAT_NO>='" + tmmsm56a2["BOF2_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF2_E"].ToString() + "')";
						}
					}

					if (sqlstr_where.Trim() == "")
					{
						if (tmmsm56a2["BOF9_S"].ToString().Trim() != ""&& tmmsm56a2["BOF9_E"].ToString().Trim() != "")
						{
							sqlstr_where += " and ((HEAT_NO>='" + tmmsm56a2["BOF9_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF9_E"].ToString() + "')";
						}
					}
					else
					{
						if (tmmsm56a2["BOF9_S"].ToString().Trim() != ""&& tmmsm56a2["BOF9_E"].ToString().Trim() != "")
						{
							sqlstr_where += " OR (HEAT_NO>='" + tmmsm56a2["BOF9_S"].ToString() + "' AND HEAT_NO<='" + tmmsm56a2["BOF9_E"].ToString() + "')";
						}
					}

					if (sqlstr_where.Trim() != "")
					{
						sqlstr_where = sqlstr_where + ")";
					}
				}
				cmd_inq.Close();

				sg_sign = "";
				dev_remark_1 = "";
				
				if (tmmsm56a["SG_SIGN"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = " and st_no in ('" + tmmsm56a["SG_SIGN"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["SG_SIGN_OUT"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = sg_sign + " and st_no not in ('" + tmmsm56a["SG_SIGN_OUT"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["DEV_REMARK_1"].ToString().Trim() != "")  //进行拼接
				{
					dev_remark_1 = " and dev_code in ('" + tmmsm56a["DEV_REMARK_1"].ToString().Replace(",", "','") + "')";
				}
				//Log::Info("", __FUNCTION__, "sg_sign =[{0}],dev_remark_1 = [{1}]", sg_sign, dev_remark_1);
				sqlstr_where = sqlstr_where + sg_sign + dev_remark_1;
				if (tmmsm56a["HEAT_NO"].ToString().Trim() != "")  //进行拼接
				{
					sqlstr_where = sqlstr_where + " and heat_no in ('" + tmmsm56a["HEAT_NO"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["STEEL_TYPE"].ToString().Trim() != "")  //进行拼接
				{
					if (tmmsm56a["STEEL_TYPE"].ToString() == "C")  //碳钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('2','3','5') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "S")	//不锈钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('1','4') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Cr")	 //铬钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1F','1M','4F','4M') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Ni")	 //镍钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1A','1D','4A','4D') ";
					}
				}

				if (tmmsm56a["TYPE_CODE"].ToString().Trim() == "Y")	// 	使用推荐的物料编码对应的大类作为基数来分摊
				{
					all_wt = 0;
					sqlstr = " select sum(DEVO_WT) use_wt from tmmsm2a_send"
						" where 1=1"
						+ sqlstr_where +
						" and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
						" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
					}
					else
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
					}
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						all_wt = cmd_inq.GetDecimal(1);
					}
					cmd_inq.Close();
					if (all_wt > 0)	 //如果有值则按原消耗量比例来，否则按产量来
					{
						sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select sm_plan_nol2, heat_no, st_no, dev_code,lot_no, @mat_code as mat_code,round(@all_use_wt*DEVO_WT/all_wt,0) use_wt,DEVO_WT"
							" from "
							" ("
							"  select sm_plan_nol2, heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over() all_wt "
							"  from "
							" ("
							"  select sm_plan_nol2, heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt "
							"  from "
							"  (select sm_plan_nol2, heat_no, st_no, dev_code,lot_no,sum(DEVO_WT) DEVO_WT"
							"  from tmmsm2a_send	t1 "
							"  where 1=1"
							+ sqlstr_where +
							"  and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							"  and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
							"  and stat_date = @stat_date"
							"  group by sm_plan_nol2, heat_no, st_no, dev_code,lot_no)"
							" )"
							" where  all_wt_x<=all_wt*@xs/100"
							" )"
							" )"
							" where use_wt!=0"
							;
						Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("rec_creator", s.userid);
						cmd_inq.Parameters.Set("rec_create_time", dateNow);
						cmd_inq.Parameters.Set("all_use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
						cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
						cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
						if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
						}
						else
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
						}
						cmd_inq.Parameters.Set("all_wt", all_wt);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
						cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
						cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}
				}
				else
				{
					all_wt = 0;
					sqlstr = " select sum(DEVO_WT) use_wt from tmmsm2a_send"
						" where 1=1"
						+ sqlstr_where +
						" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						" and mat_code =@mat_code_t"
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
					}
					else
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
					}
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						all_wt = cmd_inq.GetDecimal(1);
					}
					cmd_inq.Close();
					if (all_wt > 0)	 //如果有值则按原消耗量比例来
					{
						sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,WEIGH_NO,QUALITY_BATCH_NO,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, @mat_code as mat_code,  WEIGH_NO, QUALITY_BATCH_NO,round((@all_use_wt*devo_wt/all_wt),0) use_wt,devo_wt"
							" from  ("
							" select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over() all_wt"
							" from ("
							" select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt"
							" from "
							" (select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,sum(DEVO_WT) devo_wt"
							" from tmmsm2a_send	t1 "
							" where 1=1"
							+ sqlstr_where +
							" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							" and mat_code =@mat_code_t"
							" and stat_date = @stat_date"
							" group by sm_plan_nol2, heat_no, st_no, dev_code,lot_no,WEIGH_NO, QUALITY_BATCH_NO)"
							" )"
							" where all_wt_x<=@xs*all_wt/100"
							" )"
							" )"
							" where use_wt!=0"
							;
						Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("rec_creator", s.userid);
						cmd_inq.Parameters.Set("rec_create_time", dateNow);
						cmd_inq.Parameters.Set("all_use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
						cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
						cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
						if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
						}
						else
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
						}
						cmd_inq.Parameters.Set("all_wt", all_wt);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
						cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
						cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}

				}

				for (int i = 0; i < 1000; i++)
				{
					dif_wt = 0;
					//尾插处理
					sqlstr = " select sum(OUT_STOCK_WT) OUT_STOCK_WT"
						" from tmmsm56ft"
						" where 1=1"
						" and OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
						" and HANDLE_DIV = 'F'"
						" and mat_code =@mat_code"
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						Log::Info("", __FUNCTION__, "总消耗 =[{0}],分摊的量 = [{1}]", tmmsm56a["OUT_STOCK_WT"].ToDecimal(), cmd_inq.GetDecimal(1));
						if (cmd_inq.GetDecimal(1) != tmmsm56a["OUT_STOCK_WT"].ToDecimal())
						{
							dif_wt = tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1);
							if (dif_wt < 1)
							{
								dif_wt_i = -1;
							}
							else
							{
								dif_wt_i = 1;
							}

							sqlstr = " select OUT_STOCK_NO,DEVO_WT "
								" from tmmsm56ft"
								" where OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
								" and HANDLE_DIV = 'F'"
								" and mat_code =@mat_code"
								" and stat_date = @stat_date"
								" order by DEVO_WT desc,heat_no"
								;
							cmd_inq_s.SetCommandText(sqlstr);
							cmd_inq_s.Parameters.Set("stat_date", stat_date);
							cmd_inq_s.Parameters.Set("out_stock_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1));
							cmd_inq_s.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
							cmd_inq_s.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
							cmd_inq_s.ExecuteReader();
							while (cmd_inq_s.Read())
							{
								sqlstr = " update tmmsm56ft set OUT_STOCK_WT = OUT_STOCK_WT + @out_stock_wt"
									" where 1=1"
									" and OUT_STOCK_NO =@out_stock_no"
									" and HANDLE_DIV = 'F'"
									" and mat_code =@mat_code"
									" and stat_date = @stat_date"
									" and rownum=1"
									;
								Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("out_stock_no", cmd_inq_s.GetString(1));
								cmd_inq_1.Parameters.Set("stat_date", stat_date);
								if (dif_wt<1 && dif_wt>-1)
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt);
									dif_wt = 0;
								}
								else
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt_i);
								}
								cmd_inq_1.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
								cmd_inq_1.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

								dif_wt = dif_wt - dif_wt_i;
								if (dif_wt == 0)
								{
									break;
								}
							}
							cmd_inq_s.Close();

						}
					}
					cmd_inq.Close();

					if (dif_wt == 0)
					{
						break;
					}
				}
	

			}
			//20250430加分摊类型8
			else if (tmmsm56a["FT_FLAG"].ToString() == "8")
			{
				sg_sign = "";
				dev_remark_1 = "";
				sqlstr_where = "";
				if (tmmsm56a["SG_SIGN"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = " and st_no in ('" + tmmsm56a["SG_SIGN"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["SG_SIGN_OUT"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = sg_sign + " and st_no not in ('" + tmmsm56a["SG_SIGN_OUT"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["DEV_REMARK_1"].ToString().Trim() != "")  //进行拼接
				{
					dev_remark_1 = " and dev_code in ('" + tmmsm56a["DEV_REMARK_1"].ToString().Replace(",", "','") + "')";
				}
				//Log::Info("", __FUNCTION__, "sg_sign =[{0}],dev_remark_1 = [{1}]", sg_sign, dev_remark_1);
				sqlstr_where = sqlstr_where + sg_sign + dev_remark_1;
				if (tmmsm56a["HEAT_NO"].ToString().Trim() != "")  //进行拼接
				{
					sqlstr_where = sqlstr_where + " and heat_no in ('" + tmmsm56a["HEAT_NO"].ToString().Replace(",", "','") + "')";
				}
				//20250415bywcm
				if (tmmsm56a["LOT_NO"].ToString().Trim() != "")  //进行拼接
				{
					sqlstr_where = sqlstr_where + " and LOT_NO in ('" + tmmsm56a["LOT_NO"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["STEEL_TYPE"].ToString().Trim() != "")  //进行拼接
				{
					if (tmmsm56a["STEEL_TYPE"].ToString() == "C")  //碳钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('2','3','5') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "S")	//不锈钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('1','4') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Cr")	 //铬钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1F','1M','4F','4M') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Ni")	 //镍钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1A','1D','4A','4D') ";
					}
				}

				if (tmmsm56a["TYPE_CODE"].ToString().Trim() == "Y")	// 	使用推荐的物料编码对应的大类作为基数来分摊
				{
					all_wt = 0;
					sqlstr = " select sum(DEVO_WT) use_wt from tmmsm2a_send"
						" where 1=1"
						+ sqlstr_where +
						" and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
						" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
					}
					else
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
					}
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						all_wt = cmd_inq.GetDecimal(1);
					}
					cmd_inq.Close();
					if (all_wt > 0)	 //如果有值则按原消耗量比例来，否则按产量来
					{
						/*sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
						" select @rec_creator,@rec_create_time,@stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
						" from ("
						" select sm_plan_nol2, heat_no, st_no, dev_code,lot_no, @mat_code as mat_code,round(@all_use_wt*DEVO_WT/all_wt,0) use_wt,DEVO_WT"
						" from "
						" ("
						"  select sm_plan_nol2, heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over() all_wt "
						"  from "
						" ("
						"  select sm_plan_nol2, heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt "
						"  from "
						"  (select sm_plan_nol2, heat_no, st_no, dev_code,lot_no,sum(DEVO_WT) DEVO_WT"
						"  from tmmsm2a_send	t1 "
						"  where 1=1"
						+ sqlstr_where +
						"  and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						"  and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
						"  and stat_date = @stat_date"
						"  group by sm_plan_nol2, heat_no, st_no, dev_code,lot_no)"
						" )"
						" where  all_wt_x<=all_wt*@xs/100"
						" )"
						" )"
						" where use_wt!=0"
						;*/
						sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select  heat_no, st_no, dev_code,lot_no, @mat_code as mat_code,round(@all_use_wt*DEVO_WT/all_wt,0) use_wt,DEVO_WT"
							" from "
							" ("
							"  select  heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over() all_wt "
							"  from "
							" ("
							"  select  heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt "
							"  from "
							"  (select heat_no, st_no, dev_code,lot_no,sum(DEVO_WT) DEVO_WT"
							"  from tmmsm2a_send	t1 "
							"  where 1=1"
							+ sqlstr_where +
							"  and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							"  and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
							"  and stat_date = @stat_date"
							"  group by  heat_no, st_no, dev_code,lot_no)"
							" )"
							" where  all_wt_x<=all_wt*@xs/100"
							" )"
							" )"
							" where use_wt!=0"
							;
						Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("rec_creator", s.userid);
						cmd_inq.Parameters.Set("rec_create_time", dateNow);
						cmd_inq.Parameters.Set("all_use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
						cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
						cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
						if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
						}
						else
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
						}
						cmd_inq.Parameters.Set("all_wt", all_wt);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
						cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
						cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}
				}
				else
				{
					all_wt = 0;
					sqlstr = " select sum(DEVO_WT) use_wt from tmmsm2a_send"
						" where 1=1"
						+ sqlstr_where +
						" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						" and mat_code =@mat_code_t"
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
					}
					else
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
					}
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						all_wt = cmd_inq.GetDecimal(1);
					}
					cmd_inq.Close();
					if (all_wt > 0)	 //如果有值则按原消耗量比例来
					{
						/*sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
						" select @rec_creator,@rec_create_time,@stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,WEIGH_NO,QUALITY_BATCH_NO,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
						" from ("
						" select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, @mat_code as mat_code,  WEIGH_NO, QUALITY_BATCH_NO,round((@all_use_wt*devo_wt/all_wt),0) use_wt,devo_wt"
						" from  ("
						" select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over() all_wt"
						" from ("
						" select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt"
						" from "
						" (select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,sum(DEVO_WT) devo_wt"
						" from tmmsm2a_send	t1 "
						" where 1=1"
						+ sqlstr_where +
						" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						" and mat_code =@mat_code_t"
						" and stat_date = @stat_date"
						" group by sm_plan_nol2, heat_no, st_no, dev_code,lot_no,WEIGH_NO, QUALITY_BATCH_NO)"
						" )"
						" where all_wt_x<=@xs*all_wt/100"
						" )"
						" )"
						" where use_wt!=0"

						;*/
						sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,WEIGH_NO,QUALITY_BATCH_NO,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select  heat_no, st_no, dev_code, lot_no, @mat_code as mat_code,  WEIGH_NO, QUALITY_BATCH_NO,round((@all_use_wt*devo_wt/all_wt),0) use_wt,devo_wt"
							" from  ("
							" select  heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over() all_wt"
							" from ("
							" select heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt"
							" from "
							" (select heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,sum(DEVO_WT) devo_wt"
							" from tmmsm2a_send	t1 "
							" where 1=1"
							+ sqlstr_where +
							" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							" and mat_code =@mat_code_t"
							" and stat_date = @stat_date"
							" group by heat_no, st_no, dev_code,lot_no,WEIGH_NO, QUALITY_BATCH_NO)"
							" )"
							" where all_wt_x<=@xs*all_wt/100"
							" )"
							" )"
							" where use_wt!=0";
						Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("rec_creator", s.userid);
						cmd_inq.Parameters.Set("rec_create_time", dateNow);
						cmd_inq.Parameters.Set("all_use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
						cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
						cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
						if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
						}
						else
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
						}
						cmd_inq.Parameters.Set("all_wt", all_wt);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
						cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
						cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}

				}

				for (int i = 0; i < 1000; i++)
				{
					dif_wt = 0;
					//尾插处理
					sqlstr = " select sum(OUT_STOCK_WT) OUT_STOCK_WT"
						" from tmmsm56ft"
						" where 1=1"
						" and OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
						" and HANDLE_DIV = 'F'"
						" and mat_code =@mat_code"
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						Log::Info("", __FUNCTION__, "总消耗 =[{0}],分摊的量 = [{1}]", tmmsm56a["OUT_STOCK_WT"].ToDecimal(), cmd_inq.GetDecimal(1));
						if (cmd_inq.GetDecimal(1) != tmmsm56a["OUT_STOCK_WT"].ToDecimal())
						{
							dif_wt = tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1);
							if (dif_wt < 1)
							{
								dif_wt_i = -1;
							}
							else
							{
								dif_wt_i = 1;
							}

							sqlstr = " select OUT_STOCK_NO,DEVO_WT "
								" from tmmsm56ft"
								" where OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
								" and HANDLE_DIV = 'F'"
								" and mat_code =@mat_code"
								" and stat_date = @stat_date"
								" order by DEVO_WT desc,heat_no"
								;
							cmd_inq_s.SetCommandText(sqlstr);
							cmd_inq_s.Parameters.Set("stat_date", stat_date);
							cmd_inq_s.Parameters.Set("out_stock_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1));
							cmd_inq_s.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
							cmd_inq_s.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
							cmd_inq_s.ExecuteReader();
							while (cmd_inq_s.Read())
							{
								sqlstr = " update tmmsm56ft set OUT_STOCK_WT = OUT_STOCK_WT + @out_stock_wt"
									" where 1=1"
									" and OUT_STOCK_NO =@out_stock_no"
									" and HANDLE_DIV = 'F'"
									" and mat_code =@mat_code"
									" and stat_date = @stat_date"
									" and rownum=1"
									;
								Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("out_stock_no", cmd_inq_s.GetString(1));
								cmd_inq_1.Parameters.Set("stat_date", stat_date);
								if (dif_wt<1 && dif_wt>-1)
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt);
									dif_wt = 0;
								}
								else
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt_i);
								}
								cmd_inq_1.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
								cmd_inq_1.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

								dif_wt = dif_wt - dif_wt_i;
								if (dif_wt == 0)
								{
									break;
								}
							}
							cmd_inq_s.Close();

						}
					}
					cmd_inq.Close();

					if (dif_wt == 0)
					{
						break;
					}
				}
			}
			else if (tmmsm56a["FT_FLAG"].ToString() == "9")
			{
				Log::Info("", __FUNCTION__, "执行分摊方式9：按原消耗量比例INSERT，纯插入多退少补");

				sg_sign = "";
				sqlstr_where = "";

				// 拼接条件：炉号、钢种、物料编码
				if (tmmsm56a["SG_SIGN"].ToString().Trim() != "")
				{
					sg_sign = " and st_no in ('" + tmmsm56a["SG_SIGN"].ToString().Replace(",", "','") + "')";
				}

				sqlstr_where = sqlstr_where + sg_sign;

				// 炉号 HEAT_NO
				if (tmmsm56a["HEAT_NO"].ToString().Trim() != "")
				{
					sqlstr_where = sqlstr_where + " and heat_no in ('" + tmmsm56a["HEAT_NO"].ToString().Replace(",", "','") + "')";
				}

				// ===================== 取总基数 DEVO_WT =====================
				CDecimal all_wt = 0;
				CDecimal target_wt = tmmsm56a["OUT_STOCK_WT"].ToDecimal(); // 目标总重量

				sqlstr = " select sum(DEVO_WT) use_wt from tmmsm2a_send "
					" where 1=1 "
					+ sqlstr_where +
					" and SEND_FLAG = '1' and RTN_FLAG !='1' "
					" and mat_code = @mat_code_t "
					" and stat_date = @stat_date ";

				Log::Info("", __FUNCTION__, "查询总基数 sqlstr=[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);


				cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());

				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					all_wt = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				// ===================== 按比例 INSERT 主数据 =====================
				if (all_wt!=target_wt)
				{
					sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
						" select @rec_creator,@rec_create_time,@stat_date,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
						" from ("
						" select  heat_no, st_no, dev_code,lot_no, @mat_code as mat_code,round(@all_use_wt*DEVO_WT/all_wt,0) use_wt,DEVO_WT"
						" from "
						" ("
						"  select  heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over() all_wt "
						"  from "
						" ("
						"  select  heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt "
						"  from "
						"  (select heat_no, st_no, dev_code,lot_no,sum(DEVO_WT) DEVO_WT"
						"  from tmmsm2a_send	t1 "
						"  where 1=1"
						+ sqlstr_where +
						"  and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						"  and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
						"  and stat_date = @stat_date"
						"  group by  heat_no, st_no, dev_code,lot_no)"
						" )"
						" where  all_wt_x<=all_wt*@xs/100"
						" )"
						" )"
						" where use_wt!=0"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.Parameters.Set("rec_creator", s.userid);
					cmd_inq.Parameters.Set("rec_create_time", dateNow);
					cmd_inq.Parameters.Set("all_use_wt", target_wt - all_wt);
					cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
					}
					else
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
					}
					cmd_inq.Parameters.Set("all_wt", all_wt);
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
					cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
					cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					
				
					Log::Info("", __FUNCTION__, "INSERT主数据 sqlstr=[{0}]", sqlstr);
					
				}
				//尾插处理
				for (int i = 0; i < 1000; i++)
				{
					dif_wt = 0;
					
					sqlstr = " select sum(OUT_STOCK_WT) OUT_STOCK_WT"
						" from tmmsm56ft"
						" where 1=1"
						" and OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
						" and HANDLE_DIV = 'F'"
						" and mat_code =@mat_code"
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						Log::Info("", __FUNCTION__, "总消耗 =[{0}],分摊的量 = [{1}]", tmmsm56a["OUT_STOCK_WT"].ToDecimal(), cmd_inq.GetDecimal(1));
						if (cmd_inq.GetDecimal(1) != tmmsm56a["OUT_STOCK_WT"].ToDecimal())
						{
							dif_wt = tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1);
							if (dif_wt < 1)
							{
								dif_wt_i = -1;
							}
							else
							{
								dif_wt_i = 1;
							}

							sqlstr = " select OUT_STOCK_NO,DEVO_WT "
								" from tmmsm56ft"
								" where OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
								" and HANDLE_DIV = 'F'"
								" and mat_code =@mat_code"
								" and stat_date = @stat_date"
								" order by DEVO_WT desc,heat_no"
								;
							cmd_inq_s.SetCommandText(sqlstr);
							cmd_inq_s.Parameters.Set("stat_date", stat_date);
							cmd_inq_s.Parameters.Set("out_stock_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1));
							cmd_inq_s.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
							cmd_inq_s.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
							cmd_inq_s.ExecuteReader();
							while (cmd_inq_s.Read())
							{
								sqlstr = " update tmmsm56ft set OUT_STOCK_WT = OUT_STOCK_WT + @out_stock_wt"
									" where 1=1"
									" and OUT_STOCK_NO =@out_stock_no"
									" and HANDLE_DIV = 'F'"
									" and mat_code =@mat_code"
									" and stat_date = @stat_date"
									" and rownum=1"
									;
								Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("out_stock_no", cmd_inq_s.GetString(1));
								cmd_inq_1.Parameters.Set("stat_date", stat_date);
								if (dif_wt<1 && dif_wt>-1)
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt);
									dif_wt = 0;
								}
								else
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt_i);
								}
								cmd_inq_1.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
								cmd_inq_1.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

								dif_wt = dif_wt - dif_wt_i;
								if (dif_wt == 0)
								{
									break;
								}
							}
							cmd_inq_s.Close();

						}
					}
					cmd_inq.Close();

					if (dif_wt == 0)
					{
						break;
					}
				}
				Log::Info("", __FUNCTION__, "===== 分摊方式9 完成：目标重量={0} =====", target_wt);
			}
			//分摊类型1
			else
			{
				sg_sign = "";
				dev_remark_1 = "";
				sqlstr_where = "";
				if (tmmsm56a["SG_SIGN"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = " and st_no in ('" + tmmsm56a["SG_SIGN"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["SG_SIGN_OUT"].ToString().Trim() != "")  //进行拼接
				{
					sg_sign = sg_sign + " and st_no not in ('" + tmmsm56a["SG_SIGN_OUT"].ToString().Replace(",", "','") + "')";
				}
				if (tmmsm56a["DEV_REMARK_1"].ToString().Trim() != "")  //进行拼接
				{
					dev_remark_1 = " and dev_code in ('" + tmmsm56a["DEV_REMARK_1"].ToString().Replace(",", "','") + "')";
				}
				//Log::Info("", __FUNCTION__, "sg_sign =[{0}],dev_remark_1 = [{1}]", sg_sign, dev_remark_1);
				sqlstr_where = sqlstr_where + sg_sign + dev_remark_1;
				if (tmmsm56a["HEAT_NO"].ToString().Trim() != "")  //进行拼接
				{
					sqlstr_where = sqlstr_where + " and heat_no in ('" + tmmsm56a["HEAT_NO"].ToString().Replace(",", "','") + "')";
				}
				
				if (tmmsm56a["STEEL_TYPE"].ToString().Trim() != "")  //进行拼接
				{
					if (tmmsm56a["STEEL_TYPE"].ToString() == "C")  //碳钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('2','3','5') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "S")	//不锈钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,1) in ('1','4') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Cr")	 //铬钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1F','1M','4F','4M') ";
					}
					if (tmmsm56a["STEEL_TYPE"].ToString() == "Ni")	 //镍钢
					{
						sqlstr_where = sqlstr_where + " and substr(st_no,1,2) in ('1A','1D','4A','4D') ";
					}
				}

				if (tmmsm56a["TYPE_CODE"].ToString().Trim() == "Y")	// 	使用推荐的物料编码对应的大类作为基数来分摊
				{
					all_wt = 0;
					sqlstr = " select sum(DEVO_WT) use_wt from tmmsm2a_send"
						" where 1=1"
						+ sqlstr_where +
						" and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
						" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
					}
					else
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
					}
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						all_wt = cmd_inq.GetDecimal(1);
					}
					cmd_inq.Close();
					if (all_wt > 0)	 //如果有值则按原消耗量比例来，否则按产量来
					{
						/*sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select sm_plan_nol2, heat_no, st_no, dev_code,lot_no, @mat_code as mat_code,round(@all_use_wt*DEVO_WT/all_wt,0) use_wt,DEVO_WT"
							" from "
							" ("
							"  select sm_plan_nol2, heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over() all_wt "
							"  from "
							" ("
							"  select sm_plan_nol2, heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt "
							"  from "
							"  (select sm_plan_nol2, heat_no, st_no, dev_code,lot_no,sum(DEVO_WT) DEVO_WT"
							"  from tmmsm2a_send	t1 "
							"  where 1=1"
							+ sqlstr_where +
							"  and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							"  and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
							"  and stat_date = @stat_date"
							"  group by sm_plan_nol2, heat_no, st_no, dev_code,lot_no)"							
							" )"
							" where  all_wt_x<=all_wt*@xs/100"
							" )"
							" )"
							" where use_wt!=0"
							;*/
						sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select  heat_no, st_no, dev_code,lot_no, @mat_code as mat_code,round(@all_use_wt*DEVO_WT/all_wt,0) use_wt,DEVO_WT"
							" from "
							" ("
							"  select  heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over() all_wt "
							"  from "
							" ("
							"  select  heat_no, st_no, dev_code,lot_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt "
							"  from "
							"  (select heat_no, st_no, dev_code,lot_no,sum(DEVO_WT) DEVO_WT"
							"  from tmmsm2a_send	t1 "
							"  where 1=1"
							+ sqlstr_where +
							"  and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							"  and mat_code in (select mat_code from tmmsmw4 where MAT_CLASS in (select MAT_CLASS from tmmsmw4 where mat_code =@mat_code_t))"
							"  and stat_date = @stat_date"
							"  group by  heat_no, st_no, dev_code,lot_no)"
							" )"
							" where  all_wt_x<=all_wt*@xs/100"
							" )"
							" )"
							" where use_wt!=0"
							;
						Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("rec_creator", s.userid);
						cmd_inq.Parameters.Set("rec_create_time", dateNow);
						cmd_inq.Parameters.Set("all_use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
						cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
						cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
						if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
						}
						else
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
						}
						cmd_inq.Parameters.Set("all_wt", all_wt);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
						cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
						cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}
				}
				else
				{
					all_wt = 0;
					sqlstr = " select sum(DEVO_WT) use_wt from tmmsm2a_send"
						" where 1=1"
						+ sqlstr_where +
						" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
						" and mat_code =@mat_code_t"
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
					}
					else
					{
						cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
					}
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						all_wt = cmd_inq.GetDecimal(1);
					}
					cmd_inq.Close();
					if (all_wt > 0)	 //如果有值则按原消耗量比例来
					{
						/*sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,WEIGH_NO,QUALITY_BATCH_NO,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, @mat_code as mat_code,  WEIGH_NO, QUALITY_BATCH_NO,round((@all_use_wt*devo_wt/all_wt),0) use_wt,devo_wt"
							" from  ("
							" select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over() all_wt"
							" from ("
							" select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt"
							" from "
							" (select sm_plan_nol2, heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,sum(DEVO_WT) devo_wt"
							" from tmmsm2a_send	t1 "
							" where 1=1"
							+ sqlstr_where +
							" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							" and mat_code =@mat_code_t"
							" and stat_date = @stat_date"
							" group by sm_plan_nol2, heat_no, st_no, dev_code,lot_no,WEIGH_NO, QUALITY_BATCH_NO)"
							" )"
							" where all_wt_x<=@xs*all_wt/100"
							" )"
							" )"
							" where use_wt!=0"

							;*/
						sqlstr = " insert into tmmsm56ft(rec_creator,rec_create_time,stat_date,heat_no,st_no,dev_code,mat_code,lot_no,OUT_STOCK_TIME,recv_mat_time,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
							" select @rec_creator,@rec_create_time,@stat_date,heat_no,st_no,dev_code,mat_code,decode(@lot_no,' ',lot_no,@lot_no),@out_stock_time,@out_stock_time,WEIGH_NO,QUALITY_BATCH_NO,use_wt,DEVO_WT,'F','F'||trim(to_char(@seq_id, '0000'))||trim(to_char(rownum, '00000000')) "
							" from ("
							" select  heat_no, st_no, dev_code, lot_no, @mat_code as mat_code,  WEIGH_NO, QUALITY_BATCH_NO,round((@all_use_wt*devo_wt/all_wt),0) use_wt,devo_wt"
							" from  ("
							" select  heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over() all_wt"
							" from ("
							" select heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,devo_wt,sum(devo_wt) over(order by devo_wt desc) all_wt_x,sum(devo_wt) over() all_wt"
							" from "
							" (select heat_no, st_no, dev_code, lot_no, weigh_no, quality_batch_no,sum(DEVO_WT) devo_wt"
							" from tmmsm2a_send	t1 "
							" where 1=1"
							+ sqlstr_where +
							" and  SEND_FLAG = '1' and RTN_FLAG !='1' "
							" and mat_code =@mat_code_t"
							" and stat_date = @stat_date"
							" group by heat_no, st_no, dev_code,lot_no,WEIGH_NO, QUALITY_BATCH_NO)"
							" )"
							" where all_wt_x<=@xs*all_wt/100"
							" )"
							" )"
							" where use_wt!=0";
						Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("rec_creator", s.userid);
						cmd_inq.Parameters.Set("rec_create_time", dateNow);
						cmd_inq.Parameters.Set("all_use_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal());
						cmd_inq.Parameters.Set("out_stock_time", tmmsm56a["OUT_STOCK_TIME"].ToString());
						cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
						if (tmmsm56a["MAT_CODE_T"].ToString().Trim() != "")	 //使用推荐的物料编码对应的大类
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE_T"].ToString());
						}
						else
						{
							cmd_inq.Parameters.Set("mat_code_t", tmmsm56a["MAT_CODE"].ToString());
						}
						cmd_inq.Parameters.Set("all_wt", all_wt);
						cmd_inq.Parameters.Set("stat_date", stat_date);
						cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
						cmd_inq.Parameters.Set("lot_no", tmmsm56a["LOT_NO"].ToString());
						cmd_inq.Parameters.Set("xs", tmmsm56a["RATE"].ToDecimal());
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}

				}

				for (int i = 0; i < 1000; i++)
				{
					dif_wt = 0;
					//尾插处理
					sqlstr = " select sum(OUT_STOCK_WT) OUT_STOCK_WT"
						" from tmmsm56ft"
						" where 1=1"
						" and OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
						" and HANDLE_DIV = 'F'"
						" and mat_code =@mat_code"
						" and stat_date = @stat_date"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
					cmd_inq.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
					cmd_inq.Parameters.Set("stat_date", stat_date);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						Log::Info("", __FUNCTION__, "总消耗 =[{0}],分摊的量 = [{1}]", tmmsm56a["OUT_STOCK_WT"].ToDecimal(), cmd_inq.GetDecimal(1));
						if (cmd_inq.GetDecimal(1) != tmmsm56a["OUT_STOCK_WT"].ToDecimal())
						{
							dif_wt = tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1);
							if (dif_wt < 1)
							{
								dif_wt_i = -1;
							}
							else
							{
								dif_wt_i = 1;
							}

							sqlstr = " select OUT_STOCK_NO,DEVO_WT "
								" from tmmsm56ft"
								" where OUT_STOCK_NO like 'F'||trim(to_char(@seq_id, '0000'))||'%'"
								" and HANDLE_DIV = 'F'"
								" and mat_code =@mat_code"
								" and stat_date = @stat_date"
								" order by DEVO_WT desc,heat_no"
								;
							cmd_inq_s.SetCommandText(sqlstr);
							cmd_inq_s.Parameters.Set("stat_date", stat_date);
							cmd_inq_s.Parameters.Set("out_stock_wt", tmmsm56a["OUT_STOCK_WT"].ToDecimal() - cmd_inq.GetDecimal(1));
							cmd_inq_s.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
							cmd_inq_s.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
							cmd_inq_s.ExecuteReader();
							while (cmd_inq_s.Read())
							{
								sqlstr = " update tmmsm56ft set OUT_STOCK_WT = OUT_STOCK_WT + @out_stock_wt"
									" where 1=1"
									" and OUT_STOCK_NO =@out_stock_no"
									" and HANDLE_DIV = 'F'"
									" and mat_code =@mat_code"
									" and stat_date = @stat_date"
									" and rownum=1"
									;
								Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("out_stock_no", cmd_inq_s.GetString(1));
								cmd_inq_1.Parameters.Set("stat_date", stat_date);
								if (dif_wt<1 && dif_wt>-1)
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt);
									dif_wt = 0;
								}
								else
								{
									cmd_inq_1.Parameters.Set("out_stock_wt", dif_wt_i);
								}
								cmd_inq_1.Parameters.Set("mat_code", tmmsm56a["MAT_CODE"].ToString());
								cmd_inq_1.Parameters.Set("seq_id", tmmsm56a["SEQ_ID"].ToDecimal());
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

								dif_wt = dif_wt - dif_wt_i;
								if (dif_wt == 0)
								{
									break;
								}
							}
							cmd_inq_s.Close();

						}
					}
					cmd_inq.Close();

					if (dif_wt == 0)
					{
						break;
					}
				}
			}
			//

		}
		cmd_inq_2.Close();

		sqlstr = "update tmmsm56ft t1 set mat_name = (select mat_name from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
			" where 1=1"
			" and exists (select 1 from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
			" and SEND_FLAG!='1'"
			" and stat_date =@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

	}
	catch (CDbException& ex)  //捕获数据库操作异常 
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.sysmsg, (const char*)ex.GetMsg(), sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


