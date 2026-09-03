
/// <summary>
/// 功能说明:根据时间,将炉次信息插入表中，以及消耗信息一起插入
/// </summary>


#include "stdafx.h"
#include "epex.h" 

BM2_FUNCTION_EXPORT
int f_mmsm_updcf(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0; 	
	CString sqlstr = "";
	CString sqlstr_where = "";
	CString heat_no = "";
	CString lock_flag = "";
	CString send_xh = "";
	
	CString stat_date = "";
	CString vtable = "";
	CDecimal all_wt = 0;
	CDecimal all_use_wt = 0;
	CString seq_id = "0";
	CDecimal seq_no = 0;
	CDecimal count_num = 0;
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString heatno_premelt = "";
	CString xh_flag = "1";
	CString pono = " ";
	int v_count = 0;

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_s(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_2(conn);
	CDbCommand cmd_inq_3(conn);
	
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsmgy06a("TMMSMGY06A");
	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsmhl("TMMSMHL");
	CModel tmmsm56a("TMMSM56A");
	CModel tmmsm56("TMMSM56");
	CModel tmmsmgy08("TMMSMGY08");

	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82320";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tmmsmgy08);

	try
	{
		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		Log::Info("", __FUNCTION__, "heat_no =[{0}]", heat_no);
		tmmsmgy05.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		tmmsmgy06.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables.Contains("xh"))   //
		{
			xh_flag = "1";
		}


		//判断之前会计期已经发送过了，则后续再发生确认取消再确认或是重新补产量再确认则不能再核算
		// 202603注掉
		/*sqlstr = " select stat_date from tmmsmgy05"
			" where 1=1"
			" and stat_date !=' '"
			" and stat_date < @stat_date_now"
			" and VALID_FLAG_1='1'"
			" and heat_no=@heat_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		if (dateNow.Substring(6, 2) != "01")
		{ 
			cmd_inq.Parameters.Set("stat_date_now", CDateTime::Now().ToString("yyyyMM"));
		}
		else
		{
			cmd_inq.Parameters.Set("stat_date_now", CDateTime::Now().AddMonths(-1).ToString("yyyyMM"));
		}
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			xh_flag = "0";
		}
		cmd_inq.Close(); 		
*/  
		if (xh_flag == "1")
		{
			//插入消耗值
			sqlstr = " delete from tmmsmgy08 t1"
				" where 1=1"
				" and heat_no=@heat_no"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
			//20251126
			sqlstr = " update tmmsmgy06 t1 set BACK_C1 =' ' where 1 = 1 and heat_no = @heat_no "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
			sqlstr = " update tmmsmgy06 t1 set BACK_C1 = (select t2.ratio_num from tmmsmgy09 t2 where t1.heat_no=t2.heat_no and t1.l2_proc_no=t2.l2_proc_no)"
				" where 1 = 1"
				" and exists(select 1 from tmmsmgy09 t2 where t1.heat_no = t2.heat_no and t1.l2_proc_no = t2.l2_proc_no)"
				" and heat_no = @heat_no "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			// 插入炉次信息
			//铁水信息，涉及到预熔液也是转炉或是AOD，则要将本身对应的铁水也加入消耗中
			sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
				" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,'TS0000',t1.START_TIME,case when  nvl((select DES_TREATMENT_NO from tmmsm21 t2 where t2.heat_no= t1.l2_proc_no and rownum=1),' ')=' ' then round(MOLTIRON_WT*RATIO_B* CASE WHEN BACK_C1 > 0 THEN (BACK_C1*0.01) ELSE 1 / HEAT_COUNT  END*1000,0) else round(MOLTIRON_WT*RATIO_B*RATIO_KR* CASE WHEN BACK_C1 > 0 THEN (BACK_C1*0.01) ELSE 1 / HEAT_COUNT  END*1000,0) end,HANDLE_DIV"
				" from ("
				" select sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,decode(HEAT_COUNT,0,1,HEAT_COUNT) HEAT_COUNT,TO_NUMBER(TRIM(decode(trim(BACK_C1),null,0,BACK_C1)))  as BACK_C1,HANDLE_DIV,START_TIME,RATIO_B,RATIO_KR"
				" from tmmsmgy06 t1 ,tmmsmw3 t3"
				" where 1=1"
				" and dev_code like 'B%'"
				" and heat_no=@heat_no"
				" ) t1 left join tmmsmgy05 t2 on t1.l2_proc_no=t2.heat_no"
				" where  nvl(MOLTIRON_WT,0)!=0"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
				" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,'TS0000',t1.START_TIME,case when  nvl((select DES_TREATMENT_NO from tmmsm21 t2 where t2.heat_no= t1.l2_proc_no and rownum=1),' ')=' ' then round(MOLTIRON_WT*RATIO_B* CASE WHEN BACK_C1 > 0 THEN (BACK_C1*0.01) ELSE 1 / HEAT_COUNT  END*1000,0) else round(MOLTIRON_WT*RATIO_B*RATIO_KR* CASE WHEN BACK_C1 > 0 THEN (BACK_C1*0.01) ELSE 1 / HEAT_COUNT  END*1000,0) end,HANDLE_DIV"
				" from ("
				" select sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,decode(HEAT_COUNT,0,1,HEAT_COUNT) HEAT_COUNT,TO_NUMBER(TRIM(decode(trim(BACK_C1),null,0,BACK_C1)))  as BACK_C1,HANDLE_DIV,START_TIME,RATIO_B,RATIO_KR"
				" from tmmsmgy06 t1 ,tmmsmw3 t3"
				" where 1=1"
				" and not exists(select 1 from tmmsmgy05 t4 WHERE t1.l2_proc_no = t4.heat_no)"
				" and dev_code like 'B%'"
				" and heat_no=@heat_no"
				" ) t1 left join tmmsm21 t2 on t1.l2_proc_no=t2.l2_proc_no"
				" where  nvl(MOLTIRON_WT,0)!=0"
				;
			//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
				" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,'TS0000',t1.START_TIME"
				",case when t2.HEATNO_PREMELT1 like 'D%' or t2.HEATNO_PREMELT1 like 'H%' then case when HEATNO_PREMELT1 in (select PROC_NO from tmmsm14 where DESMODE = '浅脱硅') then round(t2.WEIGHT_PREMELT1*RATIO_AS*1000,0) when  HEATNO_PREMELT1 in (select PROC_NO from tmmsm14 where DESMODE = '不处理') then  round(t2.WEIGHT_PREMELT1*RATIO_AN*1000,0) else  round(t2.WEIGHT_PREMELT1*RATIO_A*1000,0) end  else 0 end + case when t2.HEATNO_PREMELT2 like 'D%' or t2.HEATNO_PREMELT2 like 'H%' then case when HEATNO_PREMELT2 in (select PROC_NO from tmmsm14 where DESMODE = '浅脱硅') then round(t2.WEIGHT_PREMELT2*RATIO_AS*1000,0) when  HEATNO_PREMELT2 in (select PROC_NO from tmmsm14 where DESMODE = '不处理') then  round(t2.WEIGHT_PREMELT2*RATIO_AN*1000,0) else  round(t2.WEIGHT_PREMELT2*RATIO_A*1000,0) end  else 0 end + case when t2.HEATNO_PREMELT3 like 'D%' or t2.HEATNO_PREMELT3 like 'H%' then case when HEATNO_PREMELT3 in (select PROC_NO from tmmsm14 where DESMODE = '浅脱硅') then round(t2.WEIGHT_PREMELT3*RATIO_AS*1000,0) when  HEATNO_PREMELT3 in (select PROC_NO from tmmsm14 where DESMODE = '不处理') then  round(t2.WEIGHT_PREMELT3*RATIO_AN*1000,0) else  round(t2.WEIGHT_PREMELT3*RATIO_A*1000,0) end  else 0 end"
				",HANDLE_DIV"
				" from ("
				" select sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,START_TIME,HANDLE_DIV,RATIO_A,RATIO_AS,RATIO_AN"
				" from tmmsmgy06 t1 ,tmmsmw3 t3"
				" where 1=1"
				" and dev_code like 'A%'"
				" and heat_no=@heat_no"
				") t1 left join tmmsmgy05 t2 on t1.l2_proc_no=t2.heat_no"
				" where 1=1"
				" and case when t2.HEATNO_PREMELT1 like 'D%' or t2.HEATNO_PREMELT1 like 'H%' then case when HEATNO_PREMELT1 in (select PROC_NO from tmmsm14 where DESMODE = '浅脱硅') then round(t2.WEIGHT_PREMELT1*RATIO_AS*1000,0) when  HEATNO_PREMELT1 in (select PROC_NO from tmmsm14 where DESMODE = '不处理') then  round(t2.WEIGHT_PREMELT1*RATIO_AN*1000,0) else  round(t2.WEIGHT_PREMELT1*RATIO_A*1000,0) end  else 0 end + case when t2.HEATNO_PREMELT2 like 'D%' or t2.HEATNO_PREMELT2 like 'H%' then case when HEATNO_PREMELT2 in (select PROC_NO from tmmsm14 where DESMODE = '浅脱硅') then round(t2.WEIGHT_PREMELT2*RATIO_AS*1000,0) when  HEATNO_PREMELT2 in (select PROC_NO from tmmsm14 where DESMODE = '不处理') then  round(t2.WEIGHT_PREMELT2*RATIO_AN*1000,0) else  round(t2.WEIGHT_PREMELT2*RATIO_A*1000,0) end  else 0 end + case when t2.HEATNO_PREMELT3 like 'D%' or t2.HEATNO_PREMELT3 like 'H%' then case when HEATNO_PREMELT3 in (select PROC_NO from tmmsm14 where DESMODE = '浅脱硅') then round(t2.WEIGHT_PREMELT3*RATIO_AS*1000,0) when  HEATNO_PREMELT3 in (select PROC_NO from tmmsm14 where DESMODE = '不处理') then  round(t2.WEIGHT_PREMELT3*RATIO_AN*1000,0) else  round(t2.WEIGHT_PREMELT3*RATIO_A*1000,0) end  else 0 end !=0"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();



			sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,DEVO_WT,HANDLE_DIV,STK_NO,CHARGE_TYPE)"
				" select @rec_creator,@rec_create_time,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,round(sum(DEVO_WT* CASE WHEN BACK_C1 > 0 THEN (BACK_C1*0.01) ELSE 1 / HEAT_COUNT  END),0) DEVO_WT,NVL(trim(HANDLE_DIV),'I'),STK_NO,CHARGE_TYPE"
				" from ("
				" select t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.proc_no,t1.dev_code,decode(t1.HEAT_COUNT,0,1,t1.HEAT_COUNT) HEAT_COUNT,TO_NUMBER(TRIM(decode(trim(BACK_C1),null,0,BACK_C1)))  as BACK_C1,t2.mat_code,t2.ID_2A,t2.PROC_COUNT,t2.WEIGH_NO,t2.LOT_NO,t2.QUALITY_BATCH_NO,t2.DEVO_TIME,t2.DEVO_WT,t1.HANDLE_DIV,t2.STK_NO,t2.CHARGE_TYPE "
				" from tmmsmgy06 t1 left join tmmsm2a_yl t2 on t1.dev_code = t2.dev_code and t1.l2_proc_no=t2.l2_proc_no"
				" where 1=1"
				" and substr(t1.dev_code,1,1) not in ('F','R','S','V')"
				" and nvl(DEVO_WT,0)!=0"
				" and  t1.heat_no= @heat_no"
				")"
				" group by  sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,STK_NO,CHARGE_TYPE,NVL(trim(HANDLE_DIV),'I')"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " insert into tmmsmgy08(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,DEVO_WT,HANDLE_DIV,STK_NO,CHARGE_TYPE)"
				" select @rec_creator,@rec_create_time,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,round(sum(DEVO_WT* CASE WHEN BACK_C1 > 0 THEN (BACK_C1*0.01) ELSE 1 / HEAT_COUNT  END),0) DEVO_WT,NVL(trim(HANDLE_DIV),'I'),STK_NO,CHARGE_TYPE"
				" from ("
				" select t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.proc_no,t1.dev_code,decode(t1.HEAT_COUNT,0,1,t1.HEAT_COUNT) HEAT_COUNT,TO_NUMBER(TRIM(decode(trim(BACK_C1),null,0,BACK_C1)))  as BACK_C1,t2.mat_code,t2.ID_2A,t2.PROC_COUNT,t2.WEIGH_NO,t2.QUALITY_BATCH_NO,t2.LOT_NO,t2.DEVO_TIME,t2.DEVO_WT,t1.HANDLE_DIV,t2.STK_NO,t2.CHARGE_TYPE "
				" from tmmsmgy06 t1 left join tmmsm2a_yl t2 on substr(t1.dev_code,1,1) = substr(t2.dev_code,1,1) and t1.l2_proc_no=t2.l2_proc_no"
				" where 1=1"
				" and substr(t1.dev_code,1,1) in ('F','R','S','V')"
				" and nvl(DEVO_WT,0)!=0"
				" and  t1.heat_no= @heat_no"
				")"
				" group by  sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,STK_NO,CHARGE_TYPE,NVL(trim(HANDLE_DIV),'I')"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//插入回炉信息
			sqlstr = " insert into tmmsmgy08(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, heat_no, l2_proc_no, PROC_NO, dev_code, mat_code,mat_name, ID_2A, PROC_COUNT, WEIGH_NO, QUALITY_BATCH_NO, LOT_NO, DEVO_TIME, DEVO_WT, HANDLE_DIV, STK_NO, CHARGE_TYPE,HEAT_NO_OLD,RET_HEAT_NO)"
				" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,t1.mat_code,t2.mat_name,t1.ID_2A,t1.PROC_COUNT,t1.WEIGH_NO,t1.QUALITY_BATCH_NO,t1.LOT_NO,t1.DEVO_TIME,DEVO_WT ,'H',t1.STK_NO,t1.CHARGE_TYPE,HEAT_NO_OLD,ret_heat_no"
				" from tmmsmgy07a t1"
				" left join tmmsm50 t2 on t1.mat_code=t2.mat_code"
				" where  1=1"
				" and HANDLE_DIV = 'H'"
				" and heat_no = @heat_no"
				;
			Log::Trace("", "", "sqlstr={0}", sqlstr);
			cmd_inq_1.SetCommandText(sqlstr);
			cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			cmd_inq_1.Parameters.Set("heat_no", heat_no);
			cmd_inq_1.ExecuteNonQuery();
			cmd_inq_1.Close(); 

			//回炉撤销，需要将回炉的数据全部删除
			//sqlstr = " select count(1)"
			//	" from tpssm35"
			//	" where RET_HEAT_NO = @ret_heat_no"
			//	;
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("ret_heat_no", heat_no);
			//v_count = cmd_inq.ExecuteScalar().ToInt16();
			//cmd_inq.Close();
			//if (v_count > 0)
			//{
			//	sqlstr = " select RET_HEAT_NO,HEAT_NO,rate"
			//		" from tpssm35"
			//		" where RET_HEAT_NO = @ret_heat_no"
			//		;
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("ret_heat_no", heat_no);
			//	cmd_inq.ExecuteReader();
			//	while (cmd_inq.Read())
			//	{
			//		//插入回炉的信息
			//		sqlstr = " delete from tmmsmgy07"
			//			" where 1=1"
			//			" and HANDLE_DIV = 'H'"
			//			" and ret_heat_no = @ret_heat_no and HEAT_NO_OLD = @heat_no"
			//			;
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.ExecuteNonQuery();
			//		cmd_inq_1.Close();

			//		sqlstr = " insert into tmmsmgy07(REC_CREATOR, REC_CREATE_TIME, heat_no,l2_proc_no,dev_code,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT,DURATION_TIME ,RETURN_MLSL,HEAT_NO_OLD,RET_HEAT_NO,HANDLE_DIV)"
			//			" select @rec_creator,@rec_create_time,@ret_heat_no,l2_proc_no,dev_code,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT,round(DURATION_TIME*@rate,0) ,@rate,HEAT_NO,@ret_heat_no,'H'"
			//			" from tmmsmgy06 t1"
			//			" where  1=1"
			//			" and dev_code not like 'C%'"
			//			" and heat_no = @heat_no"
			//			;
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			//		cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			//		cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.Parameters.Set("rate", cmd_inq.GetDecimal(3));
			//		cmd_inq_1.ExecuteNonQuery();
			//		cmd_inq_1.Close();

			//		sqlstr = " insert into tmmsmgy07(REC_CREATOR, REC_CREATE_TIME, heat_no,l2_proc_no,dev_code,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT,DURATION_TIME ,RETURN_MLSL,HEAT_NO_OLD,RET_HEAT_NO,HANDLE_DIV)"
			//			" select @rec_creator,@rec_create_time,heat_no,l2_proc_no,dev_code,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT,0-round(DURATION_TIME*@rate,0) ,@rate,HEAT_NO,@ret_heat_no,'H'"
			//			" from tmmsmgy06 t1"
			//			" where  1=1"
			//			" and dev_code not like 'C%'"
			//			" and heat_no = @heat_no"
			//			;
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			//		cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			//		cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.Parameters.Set("rate", cmd_inq.GetDecimal(3));
			//		cmd_inq_1.ExecuteNonQuery();
			//		cmd_inq_1.Close();

			//		sqlstr = " delete from tmmsmgy08"
			//			" where 1=1"
			//			" and HANDLE_DIV = 'H'"
			//			" and ret_heat_no = @ret_heat_no and HEAT_NO_OLD = @heat_no"
			//			;
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.ExecuteNonQuery();
			//		cmd_inq_1.Close();

			//		//20241119 回炉的铁水使用浇次合并前的铁水量
			//		sqlstr = " insert into tmmsmgy08(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, heat_no, l2_proc_no, PROC_NO, dev_code, mat_code,mat_name, ID_2A, PROC_COUNT, WEIGH_NO, QUALITY_BATCH_NO, LOT_NO, DEVO_TIME, DEVO_WT, HANDLE_DIV, STK_NO, CHARGE_TYPE,HEAT_NO_OLD,RET_HEAT_NO)"
			//			" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,@ret_heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,t1.mat_code,t2.mat_name,t1.ID_2A,t1.PROC_COUNT,t1.WEIGH_NO,t1.QUALITY_BATCH_NO,t1.LOT_NO,t1.DEVO_TIME,round(DEVO_WT*@rate,DECIMAL_PLACE) ,'H',t1.STK_NO,t1.CHARGE_TYPE,HEAT_NO,@ret_heat_no"
			//			" from tmmsmgy08 t1"
			//			" left join tmmsm50 t2 on t1.mat_code=t2.mat_code"
			//			" where  1=1"
			//			" and t1.mat_code !='TS0000'"
			//			" and HANDLE_DIV != 'H'"
			//			" and HEAT_NO_OLD = ' '"
			//			" and heat_no = @heat_no"
			//			;
			//		Log::Trace("", "", "sqlstr={0}", sqlstr);
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			//		cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			//		cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.Parameters.Set("rate", cmd_inq.GetDecimal(3));
			//		cmd_inq_1.ExecuteNonQuery();
			//		cmd_inq_1.Close();

			//		sqlstr = " insert into tmmsmgy08(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, heat_no, l2_proc_no, PROC_NO, dev_code, mat_code,mat_name, ID_2A, PROC_COUNT, WEIGH_NO, QUALITY_BATCH_NO, LOT_NO, DEVO_TIME, DEVO_WT, HANDLE_DIV, STK_NO, CHARGE_TYPE,HEAT_NO_OLD,RET_HEAT_NO)"
			//			" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,@ret_heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,t1.mat_code,t2.mat_name,' ',0,' ',' ',' ',t1.DEVO_TIME,round(DEVO_WT*@rate,DECIMAL_PLACE) ,'H',' ',' ',HEAT_NO,@ret_heat_no"
			//			" from tmmsm2a_ts t1"
			//			" left join tmmsm50 t2 on t1.mat_code=t2.mat_code"
			//			" where  1=1"
			//			" and t1.mat_code ='TS0000'"
			//			" and HANDLE_DIV != 'H'" 						
			//			" and heat_no = @heat_no"
			//			;
			//		Log::Trace("", "", "sqlstr={0}", sqlstr);
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			//		cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			//		cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.Parameters.Set("rate", cmd_inq.GetDecimal(3));
			//		cmd_inq_1.ExecuteNonQuery();
			//		cmd_inq_1.Close();


			//		//判断是否是回炉的最大比例的最大炉号，进行尾插处理
			//		sqlstr = " select sum(rate),max(rate)"
			//			" from tpssm35"
			//			" where HEAT_NO = @heat_no"
			//			;
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.ExecuteReader();
			//		if (cmd_inq_1.Read())
			//		{
			//			if (cmd_inq_1.GetDecimal(1) == 1 && cmd_inq_1.GetDecimal(2) != 1)
			//			{
			//				//判断所有的回炉都已经核算，最后计算的进行尾差处理
			//				sqlstr = "  select count(1) "
			//					" from tpssm35"
			//					" where 1=1"
			//					" and RET_HEAT_NO not in (select HEAT_NO from tmmsmgy08 where HANDLE_DIV = 'H' and  HEAT_NO_OLD = @heat_no )"
			//					" and HEAT_NO = @heat_no"
			//					;
			//				//Log::Trace("", "", "sqlstr={0}", sqlstr);
			//				cmd_inq_2.SetCommandText(sqlstr);
			//				cmd_inq_2.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//				if (cmd_inq_2.ExecuteScalar() == 0)
			//				{
			//					//铁水涉及到按浇次分配，会变动，不好进行尾差处理。
			//					sqlstr = " select l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,STK_NO,CHARGE_TYPE,sum(devo_wt) "
			//						" from ("
			//						" select l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,STK_NO,CHARGE_TYPE,devo_wt"
			//						" from tmmsmgy08 "
			//						" where 1=1"
			//						//" and mat_code !='TS0000'"
			//						" and HANDLE_DIV != 'H' "
			//						" and heat_no = @heat_no"
			//						" union all"
			//						" select l2_proc_no,PROC_NO,dev_code,mat_code,ID_2A,PROC_COUNT,WEIGH_NO,QUALITY_BATCH_NO,LOT_NO,DEVO_TIME,STK_NO,CHARGE_TYPE,0-devo_wt devo_wt"
			//						" from tmmsmgy08 "
			//						" where 1=1"
			//						" and heat_no !=@heat_no"
			//						//" and mat_code !='TS0000'"
			//						" and HANDLE_DIV = 'H' "
			//						" and HEAT_NO_OLD = @heat_no"
			//						" )"
			//						" group by l2_proc_no,proc_no,dev_code,mat_code,id_2a,proc_count,weigh_no,quality_batch_no,lot_no,devo_time,stk_no,charge_type "
			//						" having sum(devo_wt)!=0"
			//						;
			//					//Log::Trace("", "", "sqlstr={0}", sqlstr);
			//					cmd_inq_3.SetCommandText(sqlstr);
			//					cmd_inq_3.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//					cmd_inq_3.ExecuteReader();
			//					while (cmd_inq_3.Read())
			//					{
			//						sqlstr = " update tmmsmgy08 set devo_wt = devo_wt+@dif_wt"
			//							" where 1=1"
			//							" and l2_proc_no = @l2_proc_no"
			//							" and proc_no = @proc_no"
			//							" and dev_code = @dev_code"
			//							" and mat_code = @mat_code"
			//							" and id_2a = @id_2a"
			//							" and proc_count = @proc_count"
			//							" and weigh_no = @weigh_no"
			//							" and quality_batch_no = @quality_batch_no"
			//							" and lot_no = @lot_no"
			//							" and devo_time = @devo_time"
			//							" and stk_no = @stk_no"
			//							" and charge_type = @charge_type"
			//							" and HANDLE_DIV = 'H'"
			//							" and ret_heat_no = @ret_heat_no"
			//							" and heat_no = @ret_heat_no and HEAT_NO_OLD = @heat_no"
			//							;
			//						Log::Trace("", "", "mat_code={0},heat_no = {1}", cmd_inq_3.GetString(4), cmd_inq.GetString(1));
			//						cmd_inq_s.SetCommandText(sqlstr);
			//						cmd_inq_s.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
			//						cmd_inq_s.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//						cmd_inq_s.Parameters.Set("l2_proc_no", cmd_inq_3.GetString(1));
			//						cmd_inq_s.Parameters.Set("proc_no", cmd_inq_3.GetString(2));
			//						cmd_inq_s.Parameters.Set("dev_code", cmd_inq_3.GetString(3));
			//						cmd_inq_s.Parameters.Set("mat_code", cmd_inq_3.GetString(4));
			//						cmd_inq_s.Parameters.Set("id_2a", cmd_inq_3.GetString(5));
			//						cmd_inq_s.Parameters.Set("proc_count", cmd_inq_3.GetString(6));
			//						cmd_inq_s.Parameters.Set("weigh_no", cmd_inq_3.GetString(7));
			//						cmd_inq_s.Parameters.Set("quality_batch_no", cmd_inq_3.GetString(8));
			//						cmd_inq_s.Parameters.Set("lot_no", cmd_inq_3.GetString(9));
			//						cmd_inq_s.Parameters.Set("devo_time", cmd_inq_3.GetString(10));
			//						cmd_inq_s.Parameters.Set("stk_no", cmd_inq_3.GetString(11));
			//						cmd_inq_s.Parameters.Set("charge_type", cmd_inq_3.GetString(12));
			//						cmd_inq_s.Parameters.Set("dif_wt", cmd_inq_3.GetDecimal(13));
			//						cmd_inq_s.ExecuteNonQuery();
			//						cmd_inq_s.Close();
			//					}
			//					cmd_inq_3.Close();

			//				}
			//				cmd_inq_2.Close();
			//			}
			//		}
			//		cmd_inq_1.Close();



			//		sqlstr = " insert into tmmsmgy08(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, heat_no, l2_proc_no, PROC_NO, dev_code, mat_code, mat_name,ID_2A, PROC_COUNT, WEIGH_NO, QUALITY_BATCH_NO, LOT_NO, DEVO_TIME, DEVO_WT, HANDLE_DIV, STK_NO, CHARGE_TYPE,HEAT_NO_OLD,RET_HEAT_NO)"
			//			" select @rec_creator,@rec_create_time,nvl(t2.sm_plan_nol2,' '),@heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,t1.mat_code,t1.mat_name,t1.ID_2A,t1.PROC_COUNT,t1.WEIGH_NO,t1.QUALITY_BATCH_NO,t1.LOT_NO,DEVO_TIME,0-DEVO_WT ,'H',t1.STK_NO,t1.CHARGE_TYPE,@heat_no,@ret_heat_no"
			//			" from tmmsmgy08 t1"
			//			" left join tmmsmgy05 t2 on t2.heat_no = @heat_no"
			//			" where  1=1"
			//			" and HANDLE_DIV = 'H'"
			//			" and t1.heat_no = @ret_heat_no"
			//			" and t1.ret_heat_no = @ret_heat_no and t1.HEAT_NO_OLD = @heat_no"
			//			;
			//		Log::Trace("", "", "sqlstr={0}", sqlstr);
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("rec_creator", s.userid);
			//		cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.Parameters.Set("rate", cmd_inq.GetDecimal(3));
			//		cmd_inq_1.ExecuteNonQuery();
			//		cmd_inq_1.Close();

			//		//更新pono,sm_plan_nol2
			//		sqlstr = " update tmmsmgy07 set (sm_plan_nol2,pono) = (select sm_plan_nol2,pono from tmmsmgy05 where heat_no=@ret_heat_no)"
			//			" where  1=1"
			//			" and exists ( select sm_plan_nol2,pono from tmmsmgy05 where heat_no=@ret_heat_no)"
			//			" and ret_heat_no = @ret_heat_no"
			//			;
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.ExecuteNonQuery();
			//		cmd_inq_1.Close();

			//		sqlstr = " update tmmsmgy08 set (sm_plan_nol2) = (select sm_plan_nol2 from tmmsmgy05 where heat_no=@ret_heat_no)"
			//			" where  1=1"
			//			" and exists ( select sm_plan_nol2,pono from tmmsmgy05 where heat_no=@ret_heat_no)"
			//			" and ret_heat_no = @ret_heat_no"
			//			;
			//		cmd_inq_1.SetCommandText(sqlstr);
			//		cmd_inq_1.Parameters.Set("ret_heat_no", cmd_inq.GetString(1));
			//		cmd_inq_1.Parameters.Set("heat_no", cmd_inq.GetString(2));
			//		cmd_inq_1.ExecuteNonQuery();
			//		cmd_inq_1.Close();

			//	}
			//	cmd_inq.Close();
			//}
			//else
			//{
			//	sqlstr = " delete from tmmsmgy07"
			//		" where 1=1"
			//		" and HANDLE_DIV = 'H'"
			//		" and ret_heat_no = @ret_heat_no"
			//		;
			//	cmd_inq_1.SetCommandText(sqlstr);
			//	cmd_inq_1.Parameters.Set("ret_heat_no", heat_no);
			//	cmd_inq_1.ExecuteNonQuery();
			//	cmd_inq_1.Close();

			//	sqlstr = " delete from tmmsmgy08"
			//		" where 1=1"
			//		" and HANDLE_DIV = 'H'"
			//		" and ret_heat_no = @ret_heat_no"
			//		;
			//	cmd_inq_1.SetCommandText(sqlstr);
			//	cmd_inq_1.Parameters.Set("ret_heat_no", heat_no);
			//	cmd_inq_1.ExecuteNonQuery();
			//	cmd_inq_1.Close();	
			//}

			//插入铁水信息
			sqlstr = " delete from tmmsm2a_ts"
				" where heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " insert into tmmsm2a_ts(REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV)"
				" select REC_CREATOR,REC_CREATE_TIME,sm_plan_nol2,heat_no,l2_proc_no,PROC_NO,dev_code,mat_code,DEVO_TIME,DEVO_WT,HANDLE_DIV"
				" from tmmsmgy08"
				" where mat_code = 'TS0000'"
				" and heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("rec_creator", s.userid);
			cmd_inq.Parameters.Set("rec_create_time", dateNow);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();


			sqlstr = " update tmmsmgy08 t1 set mat_name = (select mat_name from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
				" where 1=1"
				" and heat_no = @heat_no"
				" and exists (select 1 from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();





			//更新会计期
			sqlstr = "select max(recv_mat_time),sum(case when recv_mat_time=' ' then 1 else 0 end) "
				" from"
				" ( select recv_mat_time from tmmsm01 t2 where heat_no = @heat_no union all select recv_mat_time from hmmsm01 t2 where  heat_no = @heat_no)"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if (cmd_inq.GetDecimal(2) <= 0 && cmd_inq.GetString(1).Trim() != "")
				{
					stat_date = cmd_inq.GetString(1).SubstringNE(0, 6);
					//取会计期
					sqlstr = " update tmmsmgy05 t1 set STAT_DATE = @stat_date,recv_mat_time = @recv_mat_time "
						" where 1=1"
						" and heat_no = @heat_no"
						;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("stat_date", stat_date);
					cmd_inq_1.Parameters.Set("recv_mat_time", cmd_inq.GetString(1));
					cmd_inq_1.Parameters.Set("heat_no", heat_no);
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();
				}
				else
				{
					stat_date = " ";
					sqlstr = " update tmmsmgy05 t1 set STAT_DATE = ' ',recv_mat_time = ' ' "
						" where 1=1"
						" and heat_no = @heat_no"
						;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("stat_date", stat_date);
					cmd_inq_1.Parameters.Set("heat_no", heat_no);
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();

				}
			}
			cmd_inq.Close();

			//if (stat_date.Trim() != "")
			{
				sqlstr = " delete from tmmsm56b "
					" where 1=1"
					" and heat_no = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = " insert into tmmsm56b(rec_creator,rec_create_time,stat_date,sm_plan_nol2,sm_plan_no,heat_no,st_no,MAT_ACT_WT)"
					" select @rec_creator,@rec_create_time,@stat_date,t2.sm_plan_nol2,t2.sm_plan_nol2,t1.heat_no,t1.st_no,sum(t1.RECEIVE_WEIGHT) mat_act_wt"
					" from VMMSMCPCL_BB1 t1"
					" left join tmmsmgy05 t2 on t1.heat_no=t2.heat_no"
					" where t1.heat_no = @heat_no"
					" group by t2.sm_plan_nol2,t1.heat_no,t1.st_no"
					//" having sum(mat_act_wt)!=0"
					;
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.Parameters.Set("rec_creator", s.userid);
				cmd_inq.Parameters.Set("rec_create_time", dateNow);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();


				sqlstr = " update tmmsmgy06 t1 set STAT_DATE = @stat_date "
					" where 1=1"
					" and heat_no = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				//碳钢的铁水按浇次进行重新核算
				sqlstr = " update tmmsmgy05 t1 set (CAST_DIV_NO,TD_NO_1) = (select CAST_DIV_NO,TD_NO_1 from tmmsm31 t2 where t1.heat_no=t2.heat_no)"
					" where 1=1 "
					" and exists (select 1 from tmmsm31 t2 where t1.heat_no=t2.heat_no)"
					" and heat_no = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				if (stat_date.Trim() != "")
				{
					sqlstr = " select st_no,CAST_DIV_NO,TD_NO_1,CAST_DIV_NO_1 "
						" from tmmsmgy05"
						" where heat_no = @heat_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", heat_no);
					cmd_inq.ExecuteReader();
					while (cmd_inq.Read())
					{
						//铁水按产量进行分摊
						if ((cmd_inq.GetString(1).SubstringNE(0, 1) == "2" || cmd_inq.GetString(1).SubstringNE(0, 1) == "4") && cmd_inq.GetString(2).Trim() != "0")	//碳钢则按产量进行铁水的消耗
						{
							all_wt = 0;
							all_use_wt = 0;
							//先删除
							sqlstr = " delete tmmsmgy08"
								" where 1=1"
								" and mat_code = 'TS0000'"
								" and  heat_no in  (select heat_no from tmmsmgy05 t2 where cast_div_no_1 =@cast_div_no_1 and cast_div_no =@cast_div_no and td_no_1=@td_no_1  and stat_date=@stat_date)"
								;
							cmd_inq_1.SetCommandText(sqlstr);
							cmd_inq_1.Parameters.Set("cast_div_no", cmd_inq.GetString(2));
							cmd_inq_1.Parameters.Set("td_no_1", cmd_inq.GetString(3));
							cmd_inq_1.Parameters.Set("cast_div_no_1", cmd_inq.GetString(4));
							cmd_inq_1.Parameters.Set("stat_date", stat_date);
							cmd_inq_1.ExecuteNonQuery();
							cmd_inq_1.Close();

							sqlstr = " select sum(DEVO_WT) all_use"
								" from tmmsm2a_ts t1"
								" where 1=1"
								" and  heat_no in  (select heat_no from tmmsmgy05 t2 where cast_div_no_1 =@cast_div_no_1 and  cast_div_no =@cast_div_no and td_no_1=@td_no_1 and stat_date=@stat_date)"
								;
							cmd_inq_1.SetCommandText(sqlstr);
							cmd_inq_1.Parameters.Set("cast_div_no", cmd_inq.GetString(2));
							cmd_inq_1.Parameters.Set("td_no_1", cmd_inq.GetString(3));
							cmd_inq_1.Parameters.Set("cast_div_no_1", cmd_inq.GetString(4));
							cmd_inq_1.Parameters.Set("stat_date", stat_date);
							cmd_inq_1.ExecuteReader();
							if (cmd_inq_1.Read())
							{
								all_use_wt = cmd_inq_1.GetDecimal(1);
							}
							cmd_inq_1.Close();

							sqlstr = " select sum(mat_act_wt) "
								" from tmmsm56b t1"
								" where 1=1"
								" and  heat_no in  (select heat_no from tmmsmgy05 t2 where  cast_div_no_1 =@cast_div_no_1 and cast_div_no =@cast_div_no and td_no_1=@td_no_1 and stat_date=@stat_date)"
								;
							cmd_inq_1.SetCommandText(sqlstr);
							cmd_inq_1.Parameters.Set("cast_div_no", cmd_inq.GetString(2));
							cmd_inq_1.Parameters.Set("td_no_1", cmd_inq.GetString(3));
							cmd_inq_1.Parameters.Set("cast_div_no_1", cmd_inq.GetString(4));
							cmd_inq_1.Parameters.Set("stat_date", stat_date);
							cmd_inq_1.ExecuteReader();
							if (cmd_inq_1.Read())
							{
								all_wt = cmd_inq_1.GetDecimal(1);
							}
							cmd_inq_1.Close();

							if (all_wt != 0 && all_use_wt != 0)
							{
								sqlstr = " insert into tmmsmgy08(REC_CREATOR, REC_CREATE_TIME, sm_plan_nol2, heat_no, l2_proc_no, PROC_NO, dev_code, mat_code, DEVO_TIME, DEVO_WT, HANDLE_DIV,stat_date,MAT_NAME)"
									" select @rec_creator,@rec_create_time,t1.sm_plan_nol2,t1.heat_no,t1.l2_proc_no,t1.PROC_NO,t1.dev_code,'TS0000',DEVO_TIME,round(DEVO_WT/all_DEVO_WT*mat_act_wt*@all_use_wt/@all_wt,0),'I',@stat_date,'普通铁水'"
									" from tmmsm2a_ts t1"
									" left join "
									" ( select heat_no,sum(mat_act_wt) mat_act_wt from tmmsm56b where heat_no in  (select heat_no from tmmsmgy05 t2 where cast_div_no_1 =@cast_div_no_1 and  cast_div_no =@cast_div_no and td_no_1=@td_no_1 and stat_date=@stat_date) group by heat_no ) t2 on t1.heat_no=t2.heat_no"
									" left join "
									" ( select heat_no,sum(DEVO_WT) all_DEVO_WT from tmmsm2a_ts where heat_no in  (select heat_no from tmmsmgy05 t2 where cast_div_no_1 =@cast_div_no_1 and  cast_div_no =@cast_div_no and td_no_1=@td_no_1 and stat_date=@stat_date) group by heat_no ) t3 on t1.heat_no=t3.heat_no"
									" where 1=1"
									" and nvl(mat_act_wt,0)!=0 and nvl(all_DEVO_WT,0)!=0"
									" and  t1.heat_no in  (select heat_no from tmmsmgy05 t2 where cast_div_no_1 =@cast_div_no_1 and  cast_div_no =@cast_div_no and td_no_1=@td_no_1 and stat_date=@stat_date) "
									;
								//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("rec_creator", s.userid);
								cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
								cmd_inq_1.Parameters.Set("cast_div_no", cmd_inq.GetString(2));
								cmd_inq_1.Parameters.Set("td_no_1", cmd_inq.GetString(3));
								cmd_inq_1.Parameters.Set("cast_div_no_1", cmd_inq.GetString(4));
								cmd_inq_1.Parameters.Set("stat_date", stat_date);
								cmd_inq_1.Parameters.Set("all_wt", all_wt);
								cmd_inq_1.Parameters.Set("all_use_wt", all_use_wt);
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

								//尾差处理
								sqlstr = " update tmmsmgy08 set DEVO_WT = DEVO_WT+(select @all_use_wt-sum(DEVO_WT) from tmmsmgy08 where mat_code = 'TS0000' and  heat_no in (select heat_no from tmmsmgy05 t2 where cast_div_no_1 =@cast_div_no_1 and   cast_div_no =@cast_div_no and td_no_1=@td_no_1 and stat_date=@stat_date))"
									" where 1=1"
									" and DEVO_WT in (select max(DEVO_WT) from tmmsmgy08 where mat_code = 'TS0000'  and  heat_no in (select heat_no from tmmsmgy05 t2 where  cast_div_no_1 =@cast_div_no_1 and cast_div_no =@cast_div_no and td_no_1=@td_no_1 and stat_date=@stat_date))"
									" and mat_code = 'TS0000'"
									"  and  heat_no in (select heat_no from tmmsmgy05 t2 where cast_div_no_1 =@cast_div_no_1 and  cast_div_no =@cast_div_no and td_no_1=@td_no_1 and stat_date=@stat_date)"
									" and rownum =1"
									;
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("cast_div_no", cmd_inq.GetString(2));
								cmd_inq_1.Parameters.Set("td_no_1", cmd_inq.GetString(3));
								cmd_inq_1.Parameters.Set("cast_div_no_1", cmd_inq.GetString(4));
								cmd_inq_1.Parameters.Set("stat_date", stat_date);
								cmd_inq_1.Parameters.Set("all_use_wt", all_use_wt);
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

							}

							//需要针对该浇次号的所有消耗进行重新分配
							sqlstr = " select heat_no "
								" from tmmsmgy05 t1"
								" where 1=1"
								" and cast_div_no_1 =@cast_div_no_1 and   cast_div_no = @cast_div_no and td_no_1 = @td_no_1 and stat_date = @stat_date"
								;
							cmd_inq_3.SetCommandText(sqlstr);
							cmd_inq_3.Parameters.Set("cast_div_no", cmd_inq.GetString(2));
							cmd_inq_3.Parameters.Set("td_no_1", cmd_inq.GetString(3));
							cmd_inq_3.Parameters.Set("cast_div_no_1", cmd_inq.GetString(4));
							cmd_inq_3.Parameters.Set("stat_date", stat_date);
							cmd_inq_3.ExecuteReader();
							while (cmd_inq_3.Read())
							{
								sqlstr = " delete from tmmsm56"
									" where 1=1"
									" and HANDLE_DIV = 'I'"
									" and heat_no=@heat_no"
									;
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

								//判断是否两个钢种以上
								sqlstr = " select count(1) from tmmsm56b"
									" where 1=1"
									" and heat_no = @heat_no"
									;
								cmd_inq_s.SetCommandText(sqlstr);
								cmd_inq_s.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
								if (cmd_inq_s.ExecuteScalar() > 1)	 //如果两个钢种，则进行分摊，而且花纹钢只分配铁水和废钢，合金都归属于其他钢种
								{
									//分钢种原来的数据进行分摊 			
									sqlstr =
										" insert into tmmsm56(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,MAT_ACT_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
										" select @rec_creator,@rec_create_time,@stat_date,sm_plan_no,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME,WEIGH_NO,QUALITY_BATCH_NO,use_wt,mat_act_wt,use_wt,'I','I'||trim(to_char(rownum, '00000000')) "
										" from ("
										" select t1.sm_plan_no,t1.heat_no,t1.st_no,t3.dev_code,t3.l2_proc_no,t3.lot_no,t3.mat_code,t3.DEVO_TIME,t3.WEIGH_NO,t3.QUALITY_BATCH_NO,round(t3.use_wt/t2.all_wt*t1.mat_act_wt ,0) use_wt,nvl(t1.mat_act_wt,0) mat_act_wt"
										" from tmmsm56b t1"
										//炉总产量
										" left join (select heat_no,sum(mat_act_wt) all_wt from tmmsm56b where  heat_no=@heat_no group by heat_no) t2 on t1.heat_no=t2.heat_no"
										//机组消耗
										" left join ( select heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,mat_code,DEVO_TIME,sum(DEVO_WT) use_wt from tmmsmgy08 where mat_code in (select mat_code from tmmsm50 where MAT_TYPE in ('2','4')) and heat_no=@heat_no group by heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,mat_code ) t3 on t1.heat_no=t3.heat_no"
										" where  nvl(all_wt,0)!=0 and nvl(use_wt,0)!=0  and  t1.heat_no=@heat_no"
										")"
										;
									Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
									cmd_inq_1.SetCommandText(sqlstr);
									cmd_inq_1.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
									cmd_inq_1.Parameters.Set("rec_creator", s.userid);
									cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
									cmd_inq_1.ExecuteNonQuery();
									cmd_inq_1.Close();

									sqlstr =
										" insert into tmmsm56(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,MAT_ACT_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
										" select @rec_creator,@rec_create_time,@stat_date,sm_plan_no,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME,WEIGH_NO,QUALITY_BATCH_NO,use_wt,mat_act_wt,use_wt,'I','I'||trim(to_char(rownum, '00000000')) "
										" from ("
										" select t1.sm_plan_no,t1.heat_no,t1.st_no,t3.dev_code,t3.l2_proc_no,t3.lot_no,t3.mat_code,t3.DEVO_TIME,t3.WEIGH_NO,t3.QUALITY_BATCH_NO,round(t3.use_wt/t2.all_wt*t1.mat_act_wt ,0) use_wt,nvl(t1.mat_act_wt,0) mat_act_wt"
										" from tmmsm56b t1"
										//炉总产量
										" left join (select heat_no,sum(mat_act_wt) all_wt from tmmsm56b where st_no!='223030' and heat_no=@heat_no group by heat_no) t2 on t1.heat_no=t2.heat_no"
										//机组消耗
										" left join ( select heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,mat_code,DEVO_TIME,sum(DEVO_WT) use_wt from tmmsmgy08 where mat_code in (select mat_code from tmmsm50 where MAT_TYPE not in ('2','4')) and heat_no=@heat_no group by heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,mat_code ) t3 on t1.heat_no=t3.heat_no"
										" where  t1.st_no!='223030' and nvl(all_wt,0)!=0 and nvl(use_wt,0)!=0  and  t1.heat_no=@heat_no"
										")"
										;
									Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
									cmd_inq_1.SetCommandText(sqlstr);
									cmd_inq_1.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
									cmd_inq_1.Parameters.Set("rec_creator", s.userid);
									cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
									cmd_inq_1.ExecuteNonQuery();
									cmd_inq_1.Close();

									//尾插处理
									sqlstr = " select heat_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,sum(OUT_STOCK_WT) OUT_STOCK_WT"
										" from ("
										"select heat_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,0-OUT_STOCK_WT OUT_STOCK_WT"
										" from tmmsm56"
										" where 1=1"
										" and  heat_no=@heat_no"
										" and HANDLE_DIV = 'I'"
										" union all"
										" select heat_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME as OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,sum(DEVO_WT) OUT_STOCK_WT"
										" from tmmsmgy08"
										" where 1=1"
										" and  heat_no=@heat_no"
										" group by heat_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME ,WEIGH_NO,QUALITY_BATCH_NO"
										")"
										" group by heat_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO"
										" having sum(OUT_STOCK_WT)!=0"
										;
									//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
									cmd_inq_1.SetCommandText(sqlstr);
									cmd_inq_1.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
									cmd_inq_1.ExecuteReader();
									while (cmd_inq_1.Read())
									{
										tmmsm56.Reset();
										cmd_inq.Fetch(tmmsm56);
										sqlstr = " update tmmsm56 set OUT_STOCK_WT = OUT_STOCK_WT + @out_stock_wt,DEVO_WT = DEVO_WT+@out_stock_wt"
											" where 1=1"
											" and OUT_STOCK_WT in (select max(OUT_STOCK_WT) from tmmsm56 where  dev_code = @dev_code and mat_code=@mat_code and out_stock_time=@out_stock_time and weigh_no=@weigh_no and quality_batch_no = @quality_batch_no  and lot_no=@lot_no and l2_proc_no=@l2_proc_no and HANDLE_DIV = 'I' and heat_no=@heat_no)"
											" and dev_code = @dev_code and mat_code=@mat_code and out_stock_time=@out_stock_time and weigh_no=@weigh_no and quality_batch_no = @quality_batch_no  and lot_no=@lot_no and l2_proc_no=@l2_proc_no "
											" and HANDLE_DIV = 'I'"
											" and  heat_no=@heat_no"
											" and rownum=1"
											;
										//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
										cmd_inq_2.SetCommandText(sqlstr);
										cmd_inq_2.Parameters.Set("out_stock_wt", tmmsm56["OUT_STOCK_WT"].ToDecimal());
										cmd_inq_2.Parameters.Set("dev_code", tmmsm56["DEV_CODE"].ToString());
										cmd_inq_2.Parameters.Set("mat_code", tmmsm56["MAT_CODE"].ToString());
										cmd_inq_2.Parameters.Set("out_stock_time", tmmsm56["OUT_STOCK_TIME"].ToString());
										cmd_inq_2.Parameters.Set("weigh_no", tmmsm56["WEIGH_NO"].ToString());
										cmd_inq_2.Parameters.Set("lot_no", tmmsm56["LOT_NO"].ToString());
										cmd_inq_2.Parameters.Set("l2_proc_no", tmmsm56["L2_PROC_NO"].ToString());
										cmd_inq_2.Parameters.Set("quality_batch_no", tmmsm56["QUALITY_BATCH_NO"].ToString());
										cmd_inq_2.Parameters.Set("stat_date", stat_date);
										cmd_inq_2.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
										cmd_inq_2.ExecuteNonQuery();
										cmd_inq_2.Close();
									}
									cmd_inq_1.Close();
								}
								else
								{
									sqlstr =
										" insert into tmmsm56(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,MAT_ACT_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
										" select @rec_creator,@rec_create_time,@stat_date,sm_plan_no,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME,WEIGH_NO,QUALITY_BATCH_NO,use_wt,mat_act_wt,use_wt,'I','I'||trim(to_char(rownum, '00000000')) "
										" from ("
										" select t1.sm_plan_no,t1.heat_no,t1.st_no,t3.dev_code,t3.l2_proc_no,t3.lot_no,t3.mat_code,t3.DEVO_TIME,t3.WEIGH_NO,t3.QUALITY_BATCH_NO,round(t3.use_wt/t2.all_wt*t1.mat_act_wt ,0) use_wt,nvl(t1.mat_act_wt,0) mat_act_wt"
										" from tmmsm56b t1"
										//炉总产量
										" left join (select heat_no,sum(mat_act_wt) all_wt from tmmsm56b where  heat_no=@heat_no group by heat_no) t2 on t1.heat_no=t2.heat_no"
										//机组消耗
										" left join ( select heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,mat_code,DEVO_TIME,sum(DEVO_WT) use_wt from tmmsmgy08 where  heat_no=@heat_no group by heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,mat_code ) t3 on t1.heat_no=t3.heat_no"
										" where  nvl(all_wt,0)!=0 and nvl(use_wt,0)!=0  and  t1.heat_no=@heat_no"
										")"
										;
									Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
									cmd_inq_1.SetCommandText(sqlstr);
									cmd_inq_1.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
									cmd_inq_1.Parameters.Set("rec_creator", s.userid);
									cmd_inq_1.Parameters.Set("rec_create_time", dateNow);
									cmd_inq_1.ExecuteNonQuery();
									cmd_inq_1.Close();
								}
								cmd_inq_s.Close();

								//更新钢种大类 ,更新记账日期
								sqlstr = " update tmmsm56 t1 set (RECV_MAT_TIME,pono)=(select max(RECV_MAT_TIME),max(pono) from tmmsmgy05 t2 where  t2.heat_no=@heat_no )"
									" where 1=1"
									" and exists(select 1 from tmmsmgy05 t2 where  t2.heat_no=@heat_no )"
									" and HANDLE_DIV = 'I'"
									" and  heat_no=@heat_no"
									;
								//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

								sqlstr = "update tmmsm56 t1 set mat_name = (select mat_name from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
									" where 1=1"
									" and exists (select 1 from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
									" and  heat_no=@heat_no"
									;
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

								//更新批次号
								sqlstr = " update tmmsm56 t1 set LOT_NO = (select lot_no from vlotno t2 where t1.WEIGH_NO=t2.WEIGH_NO)"
									" where exists(select lot_no from vlotno t2 where t1.WEIGH_NO=t2.WEIGH_NO)"
									" and  heat_no=@heat_no"
									;
								cmd_inq_1.SetCommandText(sqlstr);
								cmd_inq_1.Parameters.Set("heat_no", cmd_inq_3.GetString(1));
								cmd_inq_1.ExecuteNonQuery();
								cmd_inq_1.Close();

							}
							cmd_inq_3.Close();

						}
					}
					cmd_inq.Close();
				}

				//分钢种原来的数据进行分摊
				sqlstr = " delete from tmmsmgy06a "
					" where 1=1"
					" and heat_no = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr =
					" insert into tmmsmgy06a(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,l2_proc_no,dev_code,DURATION_TIME,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT)"
					" select @rec_creator,@rec_create_time,@stat_date,sm_plan_no,heat_no,st_no,l2_proc_no,dev_code,use_wt,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT "
					" from ("
					" select t1.sm_plan_no,t1.heat_no,t1.st_no,t3.dev_code,t3.l2_proc_no,round(t3.use_wt/t2.all_wt*t1.mat_act_wt ,0) use_wt"
					" ,t3.PONO,t3.START_TIME,t3.END_TIME,t3.PROD_DATE,t3.PROD_SHIFT_NO,t3.PROD_SHIFT_GROUP,t3.HEAT_COUNT"
					" from tmmsm56b  t1"
					//炉总产量
					" left join (select heat_no,sum(mat_act_wt) all_wt from tmmsm56b where heat_no=@heat_no group by heat_no) t2 on t1.heat_no=t2.heat_no"
					//机组消耗
					" left join ( select heat_no,l2_proc_no,dev_code,PONO,START_TIME,END_TIME,PROD_DATE,PROD_SHIFT_NO,PROD_SHIFT_GROUP,HEAT_COUNT,DURATION_TIME use_wt from tmmsmgy06 where   heat_no=@heat_no"
					" union all "
					" select heat_no, l2_proc_no, dev_code, PONO, START_TIME, END_TIME, PROD_DATE, PROD_SHIFT_NO, PROD_SHIFT_GROUP, HEAT_COUNT, DURATION_TIME use_wt from tmmsmgy07 where   heat_no = @heat_no"
					" ) t3 on t1.heat_no=t3.heat_no"
					" where  nvl(all_wt,0)!=0 and nvl(use_wt,0)!=0  and  t1.heat_no=@heat_no"
					")"
					;
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.Parameters.Set("rec_creator", s.userid);
				cmd_inq.Parameters.Set("rec_create_time", dateNow);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();
				//尾插处理
				sqlstr = " select heat_no,dev_code,l2_proc_no,sum(DURATION_TIME) DURATION_TIME"
					" from ("
					"select heat_no,dev_code,l2_proc_no,0-DURATION_TIME DURATION_TIME"
					" from tmmsmgy06a"
					" where 1=1"
					" and  heat_no=@heat_no"
					" union all"
					" select heat_no,dev_code,l2_proc_no,sum(DURATION_TIME) DURATION_TIME"
					" from tmmsmgy06"
					" where 1=1"
					" and  heat_no=@heat_no"
					" group by heat_no,dev_code,l2_proc_no"
					" union all"
					" select heat_no,dev_code,l2_proc_no,sum(DURATION_TIME) DURATION_TIME"
					" from tmmsmgy07"
					" where 1=1"
					" and  heat_no=@heat_no"
					" group by heat_no,dev_code,l2_proc_no"
					")"
					" group by heat_no,dev_code,l2_proc_no"
					" having sum(DURATION_TIME)!=0"
					;
				//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsmgy06a);
					sqlstr = " update tmmsmgy06a set DURATION_TIME = DURATION_TIME + @dif_wt"
						" where 1=1"
						" and DURATION_TIME in (select max(DURATION_TIME) from tmmsmgy06a where  dev_code = @dev_code and l2_proc_no=@l2_proc_no  and heat_no=@heat_no)"
						" and dev_code = @dev_code and l2_proc_no=@l2_proc_no  "
						" and  heat_no=@heat_no"
						" and rownum=1"
						;
					//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("dev_code", tmmsmgy06a["DEV_CODE"].ToString());
					cmd_inq_1.Parameters.Set("l2_proc_no", tmmsmgy06a["L2_PROC_NO"].ToString());
					cmd_inq_1.Parameters.Set("dif_wt", cmd_inq.GetDecimal(4));
					cmd_inq_1.Parameters.Set("stat_date", stat_date);
					cmd_inq_1.Parameters.Set("heat_no", heat_no);
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();
				}
				cmd_inq.Close();


				//更新会计期
				sqlstr = " update tmmsmgy08 t1 set STAT_DATE = @stat_date"
					" where 1=1"
					" and heat_no = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = " delete from tmmsm56"
					" where 1=1"
					" and HANDLE_DIV = 'I'"
					" and heat_no=@heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				//判断是否两个钢种以上
				sqlstr = " select count(1) from tmmsm56b"
					" where 1=1"
					" and heat_no = @heat_no"
					;
				cmd_inq_s.SetCommandText(sqlstr);
				cmd_inq_s.Parameters.Set("heat_no", heat_no);
				if (cmd_inq_s.ExecuteScalar() > 1)	 //如果两个钢种，则进行分摊，而且花纹钢只分配铁水和废钢，合金都归属于其他钢种
				{
					//分钢种原来的数据进行分摊 			
					sqlstr =
						" insert into tmmsm56(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,MAT_ACT_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
						" select @rec_creator,@rec_create_time,@stat_date,sm_plan_no,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME,WEIGH_NO,QUALITY_BATCH_NO,use_wt,mat_act_wt,use_wt,'I','I'||trim(to_char(rownum, '00000000')) "
						" from ("
						" select t1.sm_plan_no,t1.heat_no,t1.st_no,t3.dev_code,t3.l2_proc_no,t3.lot_no,t3.mat_code,t3.DEVO_TIME,t3.WEIGH_NO,t3.QUALITY_BATCH_NO,round(t3.use_wt/t2.all_wt*t1.mat_act_wt ,0) use_wt,nvl(t1.mat_act_wt,0) mat_act_wt"
						" from tmmsm56b t1"
						//炉总产量
						" left join (select heat_no,sum(mat_act_wt) all_wt from tmmsm56b where  heat_no=@heat_no group by heat_no) t2 on t1.heat_no=t2.heat_no"
						//机组消耗
						" left join ( select heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,mat_code,DEVO_TIME,sum(DEVO_WT) use_wt from tmmsmgy08 where mat_code in (select mat_code from tmmsm50 where MAT_TYPE in ('2','4')) and heat_no=@heat_no group by heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,mat_code ) t3 on t1.heat_no=t3.heat_no"
						" where  nvl(all_wt,0)!=0 and nvl(use_wt,0)!=0  and  t1.heat_no=@heat_no"
						")"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", heat_no);
					cmd_inq.Parameters.Set("rec_creator", s.userid);
					cmd_inq.Parameters.Set("rec_create_time", dateNow);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					sqlstr =
						" insert into tmmsm56(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,MAT_ACT_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
						" select @rec_creator,@rec_create_time,@stat_date,sm_plan_no,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME,WEIGH_NO,QUALITY_BATCH_NO,use_wt,mat_act_wt,use_wt,'I','I'||trim(to_char(rownum, '00000000')) "
						" from ("
						" select t1.sm_plan_no,t1.heat_no,t1.st_no,t3.dev_code,t3.l2_proc_no,t3.lot_no,t3.mat_code,t3.DEVO_TIME,t3.WEIGH_NO,t3.QUALITY_BATCH_NO,round(t3.use_wt/t2.all_wt*t1.mat_act_wt ,0) use_wt,nvl(t1.mat_act_wt,0) mat_act_wt"
						" from tmmsm56b t1"
						//炉总产量
						" left join (select heat_no,sum(mat_act_wt) all_wt from tmmsm56b where st_no!='223030' and heat_no=@heat_no group by heat_no) t2 on t1.heat_no=t2.heat_no"
						//机组消耗
						" left join ( select heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,mat_code,DEVO_TIME,sum(DEVO_WT) use_wt from tmmsmgy08 where mat_code in (select mat_code from tmmsm50 where MAT_TYPE not in ('2','4')) and heat_no=@heat_no group by heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,mat_code ) t3 on t1.heat_no=t3.heat_no"
						" where  t1.st_no!='223030' and nvl(all_wt,0)!=0 and nvl(use_wt,0)!=0  and  t1.heat_no=@heat_no"
						")"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", heat_no);
					cmd_inq.Parameters.Set("rec_creator", s.userid);
					cmd_inq.Parameters.Set("rec_create_time", dateNow);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					//尾插处理
					sqlstr = " select heat_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,sum(OUT_STOCK_WT) OUT_STOCK_WT"
						" from ("
						"select heat_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,0-OUT_STOCK_WT OUT_STOCK_WT"
						" from tmmsm56"
						" where 1=1"
						" and  heat_no=@heat_no"
						" and HANDLE_DIV = 'I'"
						" union all"
						" select heat_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME as OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,sum(DEVO_WT) OUT_STOCK_WT"
						" from tmmsmgy08"
						" where 1=1"
						" and  heat_no=@heat_no"
						" group by heat_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME ,WEIGH_NO,QUALITY_BATCH_NO"
						")"
						" group by heat_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO"
						" having sum(OUT_STOCK_WT)!=0"
						;
					//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", heat_no);
					cmd_inq.ExecuteReader();
					while (cmd_inq.Read())
					{
						tmmsm56.Reset();
						cmd_inq.Fetch(tmmsm56);
						sqlstr = " update tmmsm56 set OUT_STOCK_WT = OUT_STOCK_WT + @out_stock_wt,DEVO_WT = DEVO_WT+@out_stock_wt"
							" where 1=1"
							" and OUT_STOCK_WT in (select max(OUT_STOCK_WT) from tmmsm56 where  dev_code = @dev_code and mat_code=@mat_code and out_stock_time=@out_stock_time and weigh_no=@weigh_no and quality_batch_no = @quality_batch_no  and lot_no=@lot_no and l2_proc_no=@l2_proc_no and HANDLE_DIV = 'I' and heat_no=@heat_no)"
							" and dev_code = @dev_code and mat_code=@mat_code and out_stock_time=@out_stock_time and weigh_no=@weigh_no and quality_batch_no = @quality_batch_no  and lot_no=@lot_no and l2_proc_no=@l2_proc_no "
							" and HANDLE_DIV = 'I'"
							" and  heat_no=@heat_no"
							" and rownum=1"
							;
						//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq_1.SetCommandText(sqlstr);
						cmd_inq_1.Parameters.Set("out_stock_wt", tmmsm56["OUT_STOCK_WT"].ToDecimal());
						cmd_inq_1.Parameters.Set("dev_code", tmmsm56["DEV_CODE"].ToString());
						cmd_inq_1.Parameters.Set("mat_code", tmmsm56["MAT_CODE"].ToString());
						cmd_inq_1.Parameters.Set("out_stock_time", tmmsm56["OUT_STOCK_TIME"].ToString());
						cmd_inq_1.Parameters.Set("weigh_no", tmmsm56["WEIGH_NO"].ToString());
						cmd_inq_1.Parameters.Set("lot_no", tmmsm56["LOT_NO"].ToString());
						cmd_inq_1.Parameters.Set("l2_proc_no", tmmsm56["L2_PROC_NO"].ToString());
						cmd_inq_1.Parameters.Set("quality_batch_no", tmmsm56["QUALITY_BATCH_NO"].ToString());
						cmd_inq_1.Parameters.Set("stat_date", stat_date);
						cmd_inq_1.Parameters.Set("heat_no", heat_no);
						cmd_inq_1.ExecuteNonQuery();
						cmd_inq_1.Close();
					}
					cmd_inq.Close();
				}
				else
				{
					sqlstr =
						" insert into tmmsm56(rec_creator,rec_create_time,stat_date,sm_plan_nol2,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,OUT_STOCK_TIME,WEIGH_NO,QUALITY_BATCH_NO,OUT_STOCK_WT,MAT_ACT_WT,DEVO_WT,HANDLE_DIV,OUT_STOCK_NO)"
						" select @rec_creator,@rec_create_time,@stat_date,sm_plan_no,heat_no,st_no,dev_code,l2_proc_no,lot_no,mat_code,DEVO_TIME,WEIGH_NO,QUALITY_BATCH_NO,use_wt,mat_act_wt,use_wt,'I','I'||trim(to_char(rownum, '00000000')) "
						" from ("
						" select t1.sm_plan_no,t1.heat_no,t1.st_no,t3.dev_code,t3.l2_proc_no,t3.lot_no,t3.mat_code,t3.DEVO_TIME,t3.WEIGH_NO,t3.QUALITY_BATCH_NO,case when nvl(t2.all_wt,0)=0 and t1.mat_act_wt=0 then t3.use_wt else round(t3.use_wt/t2.all_wt*t1.mat_act_wt ,0) end use_wt,nvl(t1.mat_act_wt,0) mat_act_wt"
						" from tmmsm56b t1"
						//炉总产量
						" left join (select heat_no,sum(mat_act_wt) all_wt from tmmsm56b where  heat_no=@heat_no group by heat_no) t2 on t1.heat_no=t2.heat_no"
						//机组消耗
						" left join ( select heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,mat_code,DEVO_TIME,sum(DEVO_WT) use_wt from tmmsmgy08 where  heat_no=@heat_no group by heat_no,dev_code,l2_proc_no,lot_no,WEIGH_NO,QUALITY_BATCH_NO,DEVO_TIME,mat_code ) t3 on t1.heat_no=t3.heat_no"
						" where  t1.heat_no=@heat_no"
						")"
						" where use_wt!=0"
						;
					Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", heat_no);
					cmd_inq.Parameters.Set("rec_creator", s.userid);
					cmd_inq.Parameters.Set("rec_create_time", dateNow);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}
				cmd_inq_s.Close();





				//更新钢种大类 ,更新记账日期
				sqlstr = " update tmmsm56 t1 set (RECV_MAT_TIME,pono)=(select max(RECV_MAT_TIME),max(pono) from tmmsmgy05 t2 where  t2.heat_no=@heat_no )"
					" where 1=1"
					" and exists(select 1 from tmmsmgy05 t2 where  t2.heat_no=@heat_no )"
					" and HANDLE_DIV = 'I'"
					" and  heat_no=@heat_no"
					;
				//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				sqlstr = "update tmmsm56 t1 set mat_name = (select mat_name from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
					" where 1=1"
					" and exists (select 1 from tmmsm50 t2 where t1.mat_code = t2.mat_code)"
					" and  heat_no=@heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stat_date", stat_date);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				//更新批次号
				sqlstr = " update tmmsm56 t1 set LOT_NO = (select lot_no from vlotno t2 where t1.WEIGH_NO=t2.WEIGH_NO)"
					" where exists(select lot_no from vlotno t2 where t1.WEIGH_NO=t2.WEIGH_NO)"
					" and  heat_no=@heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

			}
			//更新工艺路径20241128wcm
			sqlstr = " update tmmsmgy05 t1 set  F_ROUTE1=(select case when substr(t1.st_no, 1, 1) in('1', '4') then case when INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'B')>0 THEN 	CASE WHEN INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'F')>0  then 'IF+BOF' ELSE 'BOF'  END"
				" when  INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'D')>0 THEN   '三脱'"
				" when  INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'E')>0 THEN 	CASE WHEN INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'F')>0  then 'EAF+IF' ELSE 'EAF'   END "
				" when  (select count(1) from tpssm35 where RET_HEAT_NO=t1.heat_no)>0  THEN   '回炉钢' "
				" else  '高硅' end else ' ' end  as F_ROUTE1 from tmmsmgy05 t1 where t1.heat_no=@heat_no)"
				" where 1 = 1 "
				" and  t1.heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新钢水量
			sqlstr = " update tmmsmgy05 t1 set  OUT_STEEL_WT = nvl((select STEEL_NET_WEIGHT from VW_CCM_STEEL_WT t2 where t2.HEAT_NAME = t1.heat_no and rownum=1),0)"
				" where 1 = 1 "
				" and  heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " update tmmsmgy05 t1 set OUT_STEEL_WT = (select max(LADLE_ARRIVE_WT-LADLE_LEAVE_WT) from tmmsm31 t2 where t2.heat_no=@heat_no )"
				" where 1 = 1 "
				" and OUT_STEEL_WT<=0"
				" and exists(select 1 from tmmsm31 t2 where t2.heat_no =@heat_no) "
				" and  heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = " update tmmsmgy05 t1 set OUT_STEEL_WT = (select max(ACTRESULT) from tmmsm27 t2 where t1.heat_no=t2.heat_no )"
				" where 1 = 1 "
				" and OUT_STEEL_WT<=0"
				" and exists(select 1 from tmmsm27 t2 where t2.heat_no = @heat_no) "
				" and  heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
			
			//更新配料单号20250317
			sqlstr = " update tmmsmgy05 t1 set COMPOSE_LIST_NO2=(select COMPOSE_LIST_NO2 from tqmtscb11d_dr t2 where t1.F_ROUTE1 = t2.F_ROUTE1 and t2.GRADE_TYPE1 = (select grade_type3 from tqmtscb09_dr where STEEL_GRADE =t1.st_no) "
				" and DATE_TIME in(select max(DATE_TIME) from tqmtscb11d_dr t3 where t1.F_ROUTE1 = t3.F_ROUTE1 and t3.GRADE_TYPE1 = (select grade_type3 from tqmtscb09_dr where STEEL_GRADE = t1.st_no) and  date_time <= (select min(start_time) from tmmsmgy06 t4 where t4.handle_div = 'Y' and  t4.heat_no = t1.heat_no)) "
				" and rownum=1) "
				" where 1 = 1 "
					" and exists(select 1 from tqmtscb11d_dr t2 where t1.F_ROUTE1 = t2.F_ROUTE1 and t2.GRADE_TYPE1 = (select grade_type3 from tqmtscb09_dr where STEEL_GRADE = t1.st_no) "
					" and DATE_TIME in(select max(DATE_TIME) from tqmtscb11d_dr t3 where t1.F_ROUTE1 = t3.F_ROUTE1 and t3.GRADE_TYPE1 = (select grade_type3 from tqmtscb09_dr where STEEL_GRADE = t1.st_no) and  date_time <= (select min(start_time) from tmmsmgy06 t4 where t4.handle_div = 'Y' and  t4.heat_no = t1.heat_no)) "
					" ) "
					" and  update_time = ' ' and heat_no =@heat_no"
				;
			Log::Info("", __FUNCTION__, "sqlstr0317 =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			EIClass bcls_rec_xh;
			EIClass bcls_ret_xh;
			bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "STAT_DATE");
			bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			bcls_rec_xh.Tables[0].Rows.Add();
			bcls_rec_xh.Tables[0].Rows[0]["STAT_DATE"] = stat_date;
			bcls_rec_xh.Tables[0].Rows[0]["STAT_DATE"] = heat_no;
			doFlag = f_mmsm_updcf(&bcls_rec_xh, &bcls_ret_xh, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			sqlstr = " select B.MAT_TYPE,A.* from tmmsmgy08 A LEFT JOIN TMMSM50 B ON A.MAT_CODE=B.MAT_CODE  where HEAT_NO='" + heat_no + "' ";
			Log::Info("", __FUNCTION__, "sqlstr1300 =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsmgy08);
				tmmsmgy08["COMPANY_CODE"] = cmd_inq.GetString(1);
				tmmsmgy08.MergeTo(in_23m.Tables[1]);
			}
			cmd_inq.Close();

			if (in_23m.Tables[1].Rows.get_Count() > 0)
			{
				doFlag = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
			}
		}


	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


