
/// <summary>
/// 功能说明:新增工艺路径和物料消耗信息
/// </summary>
#include "stdafx.h"

// Service 入口
BM2F_ENTERACE(mmsm82c_save)
int f_mmsm_yry(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_t82304_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送全程工艺路径
int f_210010_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82c_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int affectRows = 0;
	CString sqlstr = " ";
	CString old_l2_proc_no = "";
	CString heat_no = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsm2a_lv("TMMSM2A_LV");
	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsmgy08("TMMSMGY08");
	CModel tmmsm27("TMMSM27");
	CModel tmmsm21("TMMSM21");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	try
	{
		EIClass bcls_rec_xh;
		EIClass bcls_ret_xh;
		bcls_rec_xh.Tables[0].set_TableName("xh");
		bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_rec_xh.Tables[0].Rows.Add();
		
		if (bcls_rec->Tables.Contains("gy"))   //工艺路径新增
		{
			for (int i = 0; i < bcls_rec->Tables["del"].Rows.get_Count(); i++)
			{
				tmmsmgy06.MergeFrom(bcls_rec->Tables["del"].Rows[i]);
				heat_no = tmmsmgy06["HEAT_NO"].ToString();
				tmmsmgy06.Delete("HEAT_NO,L2_PROC_NO,PROC_NO,DEV_CODE");  			
			}	
			for (int i = 0; i < bcls_rec->Tables["add"].Rows.get_Count(); i++)
			{
				tmmsmgy06.MergeFrom(bcls_rec->Tables["add"].Rows[i]);
				tmmsmgy06["REC_CREATOR"] = s.userid;
				tmmsmgy06["REC_CREATE_TIME"] = dateNow;
				if (tmmsmgy06["L2_PROC_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "处理号不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//根据处理号去获取开始时间结束时间和过钢量
				sqlstr = " select  dev_code,START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,ST_NO "
					" from tmmsm24"
					" where l2_proc_no =@l2_proc_no"
					" union "
					" select  dev_code,START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,ST_NO "
					" from tmmsm23"
					" where l2_proc_no =@l2_proc_no"
					" union "
					" select  dev_code,START_TIME, END_TIME, case when END_TIME !=' ' and start_time !=' ' then ROUND((to_date(END_TIME, 'yyyy-mm-dd hh24-mi-ss')- to_date(START_TIME,'yyyy-mm-dd hh24-mi-ss')) * 24 *60,0) else 0 end DURATION_TIME,ST_NO "
					" from tmmsm25"
					" where l2_proc_no =@l2_proc_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("l2_proc_no", tmmsmgy06["L2_PROC_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsmgy06["DEV_CODE"] = cmd_inq.GetString(1);
					tmmsmgy06["START_TIME"] = cmd_inq.GetString(2);
					tmmsmgy06["END_TIME"] = cmd_inq.GetString(3);
					tmmsmgy06["DURATION_TIME"] = cmd_inq.GetString(4);
					tmmsmgy06["ST_NO"] = cmd_inq.GetString(5);
				}
				cmd_inq.Close(); 

				if (tmmsmgy06["HEAT_NO"].ToString().Trim() == "")
				{
					tmmsmgy06["HEAT_NO"] = bcls_rec->Tables["gy"].Rows[0]["HEAT_NO"].ToString();
				}
				if (tmmsmgy06["SM_PLAN_NOL2"].ToString().Trim() == "")
				{
					tmmsmgy06["SM_PLAN_NOL2"] = bcls_rec->Tables["gy"].Rows[0]["SM_PLAN_NOL2"].ToString();
				}
				if (tmmsmgy06["ST_NO"].ToString().Trim() == "")
				{
					tmmsmgy06["ST_NO"] = bcls_rec->Tables["gy"].Rows[0]["ST_NO"].ToString();
				}
				heat_no = tmmsmgy06["HEAT_NO"].ToString();
				tmmsmgy06["HANDLE_DIV"] = "U";
				tmmsmgy06.TrimOrBlank();
				tmmsmgy06.Insert();

				//更新时间的合理性
				sqlstr = " update tmmsmgy06 t1 set DURATION_TIME = (select STD_TIME from tmmsmw2 t3 where 1=1 and t3.DEV_CODE = t1.dev_code)"
					" where 1=1"
					" and exists (select 1 from tmmsmw2 t3 where 1=1 and t3.HEAT_DURATION<t1.DURATION_TIME  and t3.DEV_CODE = t1.dev_code )"
					" and heat_no=@heat_no"
					" and l2_proc_no=@l2_proc_no"
					" and AFFIRM_FLAG !='1'"
					;
				//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", tmmsmgy06["HEAT_NO"].ToString());
				cmd_inq.Parameters.Set("l2_proc_no", tmmsmgy06["L2_PROC_NO"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

			}
			for (int i = 0; i < bcls_rec->Tables["upd"].Rows.get_Count(); i++)
			{
				tmmsmgy06.MergeFrom(bcls_rec->Tables["upd"].Rows[i]);
				tmmsmgy06["REC_REVISOR"] = s.userid;
				tmmsmgy06["REC_REVISE_TIME"] = dateNow;
				heat_no = tmmsmgy06["HEAT_NO"].ToString();
				tmmsmgy06.Update("REC_REVISOR,REC_REVISE_TIME,DURATION_TIME", "SM_PLAN_NOL2,HEAT_NO,L2_PROC_NO,PROC_NO,DEV_CODE");
				
			}	 
			
			if (heat_no != "")
			{
				bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = heat_no;
				doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				doFlag = f_t82304_snd(&bcls_rec_xh, &bcls_ret_xh, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t82304_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		if (bcls_rec->Tables.Contains("xh"))   //消耗维护
		{
			for (int i = 0; i < bcls_rec->Tables["del"].Rows.get_Count(); i++)
			{
				tmmsm2a_yl.MergeFrom(bcls_rec->Tables["del"].Rows[i]);
				heat_no = tmmsm2a_yl["HEAT_NO"].ToString();
				sqlstr = " select * from tmmsm2a_yl"
					" where 1=1"
					" and SEQ_NO_2A=@seq_no_2a"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("seq_no_2a", tmmsm2a_yl["SEQ_NO_2A"].ToString());
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					tmmsm2a_lv.Reset();
					cmd_inq.Fetch(tmmsm2a_lv);
					tmmsm2a_lv["REC_CREATOR"] = s.userid;
					tmmsm2a_lv["REC_CREATE_TIME"] = dateNow;
					tmmsm2a_lv["EVENT_DESC"] = "删除";
					tmmsm2a_lv.TrimOrBlank();
					tmmsm2a_lv.Insert();
				}
				cmd_inq.Close(); 

				tmmsm2a_yl.Delete("SEQ_NO_2A");
			}
			for (int i = 0; i < bcls_rec->Tables["add"].Rows.get_Count(); i++)
			{
				tmmsm2a_yl.MergeFrom(bcls_rec->Tables["add"].Rows[i]);
				heat_no = tmmsm2a_yl["HEAT_NO"].ToString();
				tmmsm2a_yl["REC_CREATOR"] = s.userid;
				tmmsm2a_yl["REC_CREATE_TIME"] = dateNow;
				tmmsm2a_yl["ID_2A"] = " ";
				tmmsm2a_yl["PROC_COUNT"] = 1;

				sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm2a_yl["SEQ_NO_2A"] = dateNow + cmd_inq.GetString(1).Trim();
				}
				cmd_inq.Close();
				
				tmmsm2a_yl["ADJUST_FLAG"] = "U";
				tmmsm2a_yl["REMARK_2"] = " ";
				tmmsm2a_yl["DEVO_TIME"] = dateNow;
				sqlstr = " select mat_name from tmmsm50 where mat_code = @mat_code";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_code", tmmsm2a_yl["MAT_CODE"].ToString());
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					tmmsm2a_yl["MAT_NAME"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();

				if (tmmsm2a_yl["HEAT_NO"].ToString().Trim() == "")
				{
					//tmmsm2a_yl["HEAT_NO"] = bcls_rec->Tables["xh"].Rows[0]["HEAT_NO"].ToString();
				}
				if (tmmsm2a_yl["SM_PLAN_NOL2"].ToString().Trim() == "")
				{
					//tmmsm2a_yl["SM_PLAN_NOL2"] = bcls_rec->Tables["xh"].Rows[0]["SM_PLAN_NOL2"].ToString();
				} 			
				tmmsm2a_yl.TrimOrBlank();
				tmmsm2a_yl.Insert();

				tmmsm2a_lv.CopyFrom(tmmsm2a_yl);
				tmmsm2a_lv["EVENT_DESC"] = "新增";
			}
			for (int i = 0; i < bcls_rec->Tables["upd"].Rows.get_Count(); i++)
			{
				tmmsm2a_yl.MergeFrom(bcls_rec->Tables["upd"].Rows[i]);
				heat_no = tmmsm2a_yl["HEAT_NO"].ToString();
				tmmsm2a_yl["REC_REVISOR"] = s.userid;
				tmmsm2a_yl["REC_REVISE_TIME"] = dateNow;
				tmmsm2a_yl["ADJUST_FLAG"] = "U";
				sqlstr = " select * from tmmsm2a_yl"
					" where 1=1"
					" and SEQ_NO_2A=@seq_no_2a"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("seq_no_2a", tmmsm2a_yl["SEQ_NO_2A"].ToString());
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					tmmsm2a_lv.Reset();
					cmd_inq.Fetch(tmmsm2a_lv);
					tmmsm2a_lv["REC_CREATOR"] = s.userid;
					tmmsm2a_lv["REC_CREATE_TIME"] = dateNow;
					tmmsm2a_lv["EVENT_DESC"] = "修改";
					tmmsm2a_lv.TrimOrBlank();
					tmmsm2a_lv.Insert();
					old_l2_proc_no = tmmsm2a_lv["L2_PROC_NO"].ToString();
				}
				cmd_inq.Close();

				tmmsm2a_yl.Update("REC_REVISOR,REC_REVISE_TIME,DEVO_WT,ADJUST_FLAG,WEIGH_NO,QUALITY_BATCH_NO,HEAT_NO,L2_PROC_NO,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO", "SEQ_NO_2A");
				//更改处理号 重新计算之前炉号
				if (old_l2_proc_no != tmmsm2a_yl["L2_PROC_NO"].ToString())
				{
					sqlstr = " select distinct  heat_no from tmmsmgy06"
						" where l2_proc_no = @l2_proc_no"
						;
					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.Parameters.Set("l2_proc_no", old_l2_proc_no);
					cmd_sql.ExecuteReader();
					while (cmd_sql.Read())
					{
						bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = cmd_sql.GetString(1);
						doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					cmd_sql.Close();
				}
			}
			
			if (heat_no != "")
			{
				bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = heat_no;
				doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//如果有回炉到别的炉号，对应的炉号信息也需要更新
				sqlstr = " select ret_heat_no "
					" from tpssm35"
					" where heat_no=@heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
					doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				cmd_inq.Close();
			}

		}

		if (bcls_rec->Tables.Contains("yry"))   //预熔液修改
		{
			EIClass bcls_rec_yry;
			bcls_rec_yry.Tables[0].Columns.Add(DT_STRING, "HEATNO_PREMELT");
			bcls_rec_yry.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			int j = 0;
			for (int i = 0; i < bcls_rec->Tables["yry"].Rows.get_Count(); i++)
			{ 
				bcls_rec_yry.Tables[0].Rows.Clear();
				tmmsmgy05.MergeFrom(bcls_rec->Tables["yry"].Rows[i]);
				tmmsmgy05["REC_REVISOR"] = s.userid;
				tmmsmgy05["REC_REVISE_TIME"] = dateNow;
				//根据判断来确定预熔液是否有变化，如果有则进行预熔液的重新分配
				sqlstr = " select HEATNO_PREMELT1,HEATNO_PREMELT2,HEATNO_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,MOLTIRON_WT"
					" from tmmsmgy05"
					" where 1=1"
					" and SM_PLAN_NOL2 = @sm_plan_nol2"
					" and heat_no=@heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("sm_plan_nol2", tmmsmgy05["SM_PLAN_NOL2"].ToString());
				cmd_inq.Parameters.Set("heat_no", tmmsmgy05["HEAT_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					if (cmd_inq.GetString(1).Trim() != ""&&cmd_inq.GetString(1) != tmmsmgy05["HEATNO_PREMELT1"].ToString() && cmd_inq.GetString(1) != tmmsmgy05["HEATNO_PREMELT2"].ToString() && cmd_inq.GetString(1) != tmmsmgy05["HEATNO_PREMELT3"].ToString() && cmd_inq.GetString(1) != tmmsmgy05["HEATNO_PREMELT4"].ToString() && cmd_inq.GetString(1) != tmmsmgy05["HEATNO_PREMELT5"].ToString() && cmd_inq.GetString(1) != tmmsmgy05["HEATNO_PREMELT6"].ToString())
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = cmd_inq.GetString(1);
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (cmd_inq.GetString(2).Trim() != ""&&cmd_inq.GetString(2) != tmmsmgy05["HEATNO_PREMELT1"].ToString() && cmd_inq.GetString(2) != tmmsmgy05["HEATNO_PREMELT2"].ToString() && cmd_inq.GetString(2) != tmmsmgy05["HEATNO_PREMELT3"].ToString() && cmd_inq.GetString(2) != tmmsmgy05["HEATNO_PREMELT4"].ToString() && cmd_inq.GetString(2) != tmmsmgy05["HEATNO_PREMELT5"].ToString() && cmd_inq.GetString(2) != tmmsmgy05["HEATNO_PREMELT6"].ToString())
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = cmd_inq.GetString(2);
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (cmd_inq.GetString(3).Trim() != ""&&cmd_inq.GetString(3) != tmmsmgy05["HEATNO_PREMELT1"].ToString() && cmd_inq.GetString(3) != tmmsmgy05["HEATNO_PREMELT2"].ToString() && cmd_inq.GetString(3) != tmmsmgy05["HEATNO_PREMELT3"].ToString() && cmd_inq.GetString(3) != tmmsmgy05["HEATNO_PREMELT4"].ToString() && cmd_inq.GetString(3) != tmmsmgy05["HEATNO_PREMELT5"].ToString() && cmd_inq.GetString(3) != tmmsmgy05["HEATNO_PREMELT6"].ToString())
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = cmd_inq.GetString(3);
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (cmd_inq.GetString(4).Trim() != ""&&cmd_inq.GetString(4) != tmmsmgy05["HEATNO_PREMELT1"].ToString() && cmd_inq.GetString(4) != tmmsmgy05["HEATNO_PREMELT2"].ToString() && cmd_inq.GetString(4) != tmmsmgy05["HEATNO_PREMELT3"].ToString() && cmd_inq.GetString(4) != tmmsmgy05["HEATNO_PREMELT4"].ToString() && cmd_inq.GetString(4) != tmmsmgy05["HEATNO_PREMELT5"].ToString() && cmd_inq.GetString(4) != tmmsmgy05["HEATNO_PREMELT6"].ToString())
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = cmd_inq.GetString(4);
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (cmd_inq.GetString(5).Trim() != ""&&cmd_inq.GetString(5) != tmmsmgy05["HEATNO_PREMELT1"].ToString() && cmd_inq.GetString(5) != tmmsmgy05["HEATNO_PREMELT2"].ToString() && cmd_inq.GetString(5) != tmmsmgy05["HEATNO_PREMELT3"].ToString() && cmd_inq.GetString(5) != tmmsmgy05["HEATNO_PREMELT4"].ToString() && cmd_inq.GetString(5) != tmmsmgy05["HEATNO_PREMELT5"].ToString() && cmd_inq.GetString(5) != tmmsmgy05["HEATNO_PREMELT6"].ToString())
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = cmd_inq.GetString(5);
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (cmd_inq.GetString(6).Trim() != ""&&cmd_inq.GetString(6) != tmmsmgy05["HEATNO_PREMELT1"].ToString() && cmd_inq.GetString(6) != tmmsmgy05["HEATNO_PREMELT2"].ToString() && cmd_inq.GetString(6) != tmmsmgy05["HEATNO_PREMELT3"].ToString() && cmd_inq.GetString(6) != tmmsmgy05["HEATNO_PREMELT4"].ToString() && cmd_inq.GetString(6) != tmmsmgy05["HEATNO_PREMELT5"].ToString() && cmd_inq.GetString(6) != tmmsmgy05["HEATNO_PREMELT6"].ToString())
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = cmd_inq.GetString(6);
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (tmmsmgy05["HEATNO_PREMELT1"].ToString().Trim() != ""&& tmmsmgy05["HEATNO_PREMELT1"].ToString() != cmd_inq.GetString(1) && tmmsmgy05["HEATNO_PREMELT1"].ToString() != cmd_inq.GetString(2) && tmmsmgy05["HEATNO_PREMELT1"].ToString() != cmd_inq.GetString(3) && tmmsmgy05["HEATNO_PREMELT1"].ToString() != cmd_inq.GetString(4) && tmmsmgy05["HEATNO_PREMELT1"].ToString() != cmd_inq.GetString(5) && tmmsmgy05["HEATNO_PREMELT1"].ToString() != cmd_inq.GetString(6))
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = tmmsmgy05["HEATNO_PREMELT1"].ToString();
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (tmmsmgy05["HEATNO_PREMELT2"].ToString().Trim() != ""&& tmmsmgy05["HEATNO_PREMELT2"].ToString() != cmd_inq.GetString(1) && tmmsmgy05["HEATNO_PREMELT2"].ToString() != cmd_inq.GetString(2) && tmmsmgy05["HEATNO_PREMELT2"].ToString() != cmd_inq.GetString(3) && tmmsmgy05["HEATNO_PREMELT2"].ToString() != cmd_inq.GetString(4) && tmmsmgy05["HEATNO_PREMELT2"].ToString() != cmd_inq.GetString(5) && tmmsmgy05["HEATNO_PREMELT2"].ToString() != cmd_inq.GetString(6))
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = tmmsmgy05["HEATNO_PREMELT2"].ToString();
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (tmmsmgy05["HEATNO_PREMELT3"].ToString().Trim() != ""&& tmmsmgy05["HEATNO_PREMELT3"].ToString() != cmd_inq.GetString(1) && tmmsmgy05["HEATNO_PREMELT3"].ToString() != cmd_inq.GetString(2) && tmmsmgy05["HEATNO_PREMELT3"].ToString() != cmd_inq.GetString(3) && tmmsmgy05["HEATNO_PREMELT3"].ToString() != cmd_inq.GetString(4) && tmmsmgy05["HEATNO_PREMELT3"].ToString() != cmd_inq.GetString(5) && tmmsmgy05["HEATNO_PREMELT3"].ToString() != cmd_inq.GetString(6))
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = tmmsmgy05["HEATNO_PREMELT3"].ToString();
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (tmmsmgy05["HEATNO_PREMELT4"].ToString().Trim() != ""&& tmmsmgy05["HEATNO_PREMELT4"].ToString() != cmd_inq.GetString(1) && tmmsmgy05["HEATNO_PREMELT4"].ToString() != cmd_inq.GetString(2) && tmmsmgy05["HEATNO_PREMELT4"].ToString() != cmd_inq.GetString(3) && tmmsmgy05["HEATNO_PREMELT4"].ToString() != cmd_inq.GetString(4) && tmmsmgy05["HEATNO_PREMELT4"].ToString() != cmd_inq.GetString(5) && tmmsmgy05["HEATNO_PREMELT4"].ToString() != cmd_inq.GetString(6))
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = tmmsmgy05["HEATNO_PREMELT4"].ToString();
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (tmmsmgy05["HEATNO_PREMELT5"].ToString().Trim() != ""&& tmmsmgy05["HEATNO_PREMELT5"].ToString() != cmd_inq.GetString(1) && tmmsmgy05["HEATNO_PREMELT5"].ToString() != cmd_inq.GetString(2) && tmmsmgy05["HEATNO_PREMELT5"].ToString() != cmd_inq.GetString(3) && tmmsmgy05["HEATNO_PREMELT5"].ToString() != cmd_inq.GetString(4) && tmmsmgy05["HEATNO_PREMELT5"].ToString() != cmd_inq.GetString(5) && tmmsmgy05["HEATNO_PREMELT5"].ToString() != cmd_inq.GetString(6))
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = tmmsmgy05["HEATNO_PREMELT5"].ToString();
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}
					if (tmmsmgy05["HEATNO_PREMELT6"].ToString().Trim() != ""&& tmmsmgy05["HEATNO_PREMELT6"].ToString() != cmd_inq.GetString(1) && tmmsmgy05["HEATNO_PREMELT6"].ToString() != cmd_inq.GetString(2) && tmmsmgy05["HEATNO_PREMELT6"].ToString() != cmd_inq.GetString(3) && tmmsmgy05["HEATNO_PREMELT6"].ToString() != cmd_inq.GetString(4) && tmmsmgy05["HEATNO_PREMELT6"].ToString() != cmd_inq.GetString(5) && tmmsmgy05["HEATNO_PREMELT6"].ToString() != cmd_inq.GetString(6))
					{
						bcls_rec_yry.Tables[0].Rows.Add();
						bcls_rec_yry.Tables[0].Rows[j]["HEATNO_PREMELT"] = tmmsmgy05["HEATNO_PREMELT6"].ToString();
						bcls_rec_yry.Tables[0].Rows[j]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						j++;
					}

					//铁水看是否有变化
					if (cmd_inq.GetDecimal(7) != tmmsmgy05["MOLTIRON_WT"].ToDecimal())
					{
						if (bcls_rec_yry.Tables[0].Rows.get_Count() > 0)
						{
							bcls_rec_yry.Tables[0].Rows[0]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						}
						else
						{
							bcls_rec_yry.Tables[0].Rows.Add();
							bcls_rec_yry.Tables[0].Rows[0]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
						}
					}
				}
				cmd_inq.Close(); 

				tmmsmgy05.Update("REC_REVISOR,REC_REVISE_TIME,HEATNO_PREMELT1,WEIGHT_PREMELT1,HEATNO_PREMELT2,WEIGHT_PREMELT2,HEATNO_PREMELT3,WEIGHT_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6,MOLTIRON_WT", "SM_PLAN_NOL2,HEAT_NO");

				//同时更新AOD表
				tmmsm27.MergeFrom(bcls_rec->Tables["yry"].Rows[i]);
				tmmsm27["REC_REVISOR"] = s.userid;
				tmmsm27["REC_REVISE_TIME"] = dateNow;
				tmmsm27.Update("REC_REVISOR,REC_REVISE_TIME,HEATNO_PREMELT1,WEIGHT_PREMELT1,HEATNO_PREMELT2,WEIGHT_PREMELT2,HEATNO_PREMELT3,WEIGHT_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6", "SM_PLAN_NOL2,HEAT_NO");

				//同时更新AOD表
				tmmsm21.MergeFrom(bcls_rec->Tables["yry"].Rows[i]);
				tmmsm21["REC_REVISOR"] = s.userid;
				tmmsm21["REC_REVISE_TIME"] = dateNow;
				tmmsm21.Update("REC_REVISOR,REC_REVISE_TIME,HEATNO_PREMELT1,WEIGHT_PREMELT1,HEATNO_PREMELT2,WEIGHT_PREMELT2,HEATNO_PREMELT3,WEIGHT_PREMELT3,HEATNO_PREMELT4,HEATNO_PREMELT5,HEATNO_PREMELT6", "SM_PLAN_NOL2,HEAT_NO");

				if (bcls_rec_yry.Tables[0].Rows.get_Count() > 0)
				{
					doFlag = f_mmsm_yry(&bcls_rec_yry, &bcls_ret_xh, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					for (int i = 0; i < bcls_rec_yry.Tables[0].Rows.get_Count(); i++)
					{
						sqlstr = " select heat_no from tmmsmgy06"
							" where 1=1"
							" and HANDLE_DIV = 'Y'"
							" and heat_no !=@heat_no"
							" and l2_proc_no = @heatno_premelt"
							;
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("heatno_premelt", bcls_rec_yry.Tables[0].Rows[i]["HEATNO_PREMELT"].ToString());
						cmd_inq.Parameters.Set("heat_no", bcls_rec_yry.Tables[0].Rows[i]["HEAT_NO"].ToString());
						cmd_inq.ExecuteReader();
						while (cmd_inq.Read())
						{
							bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
							doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}

							bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
							doFlag = f_210010_snd(&bcls_rec_xh, &bcls_ret_xh, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
						cmd_inq.Close();
					}
				}


				bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = tmmsmgy05["HEAT_NO"].ToString();
				doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}				
				doFlag = f_210010_snd(&bcls_rec_xh, &bcls_ret_xh, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//如果有回炉到别的炉号，对应的炉号信息也需要更新
				sqlstr = " select ret_heat_no "
					" from tpssm35"
					" where heat_no=@heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", tmmsmgy05["HEAT_NO"].ToString());
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
					doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					doFlag = f_210010_snd(&bcls_rec_xh, &bcls_ret_xh, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				cmd_inq.Close(); 

				//将计划的钢包对应的预熔液进行更新
				if (tmmsmgy05["HEATNO_PREMELT1"].ToString().Trim() != "")
				{ 				
				sqlstr = " update tpssm12zt set PROC_NO = ' '"
					" where PROC_NO=@proc_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT1"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();
				sqlstr = " update tpssm12zt set PROC_NO = ' '"
					" where PROC_NO2=@proc_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT1"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();
				sqlstr = " update tpssm12zt set PROC_NO = ' '"
					" where PROC_NO3=@proc_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT1"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();
				sqlstr = " update tpssm12zt set PROC_NO = ' '"
					" where PROC_NO4=@proc_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT1"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();  
				}	 
			
				if (tmmsmgy05["HEATNO_PREMELT2"].ToString().Trim() != "")
				{
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT2"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO2=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT2"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO3=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT2"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO4=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT2"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}

				if (tmmsmgy05["HEATNO_PREMELT3"].ToString().Trim() != "")
				{
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT3"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO2=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT3"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO3=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT3"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO4=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT3"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}
				if (tmmsmgy05["HEATNO_PREMELT4"].ToString().Trim() != "")
				{
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT4"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO2=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT4"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO3=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT4"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO4=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT4"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}

				if (tmmsmgy05["HEATNO_PREMELT5"].ToString().Trim() != "")
				{
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT5"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO2=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT5"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO3=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT5"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO4=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT5"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}
				if (tmmsmgy05["HEATNO_PREMELT6"].ToString().Trim() != "")
				{
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT6"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO2=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT6"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO3=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT6"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					sqlstr = " update tpssm12zt set PROC_NO = ' '"
						" where PROC_NO4=@proc_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("proc_no", tmmsmgy05["HEATNO_PREMELT6"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}

			}
			
			
			
		}
		

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


