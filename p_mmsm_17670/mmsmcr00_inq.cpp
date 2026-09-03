/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 铬镍收得率炉次明细表
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmcr00_inq)

int f_mmsmcr00_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sqlstr_heat = " ";
	CString check_flag = "0";  	
	CString v_table = "tmmsmzxhbb";
	CString do_flag = "00";

	CDecimal sum_cr = 0;
	CDecimal sum_ni = 0;
	CDecimal sum_mo = 0;
	
	CDbCommand cmd_inq(conn);
	CModel tmmsmzxhbb_lh("TMMSMZXHBB_LH");

	try
	{
		tmmsmzxhbb_lh.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Columns.Contains("DO_FLAG"))
			do_flag = bcls_rec->Tables[0].Rows[0]["DO_FLAG"].ToString();

		if (tmmsmzxhbb_lh["USER_ID"].ToString().Trim() == "" || tmmsmzxhbb_lh["TIME_STAMPS"].ToString().Trim() == "")
		{
			strcpy(s.msg, "请输入执行时间");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = " select *"
			" from tmmsmzxhbb_lh"
			" where 1=1"
			" and user_id = @user_id"
			" and time_stamps = @time_stamps"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
		cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{ 			
			tmmsmzxhbb_lh.Reset();
			cmd_inq.Fetch(tmmsmzxhbb_lh); 			
		}
		cmd_inq.Close();
		check_flag = tmmsmzxhbb_lh["CHECK_FLAG"].ToString();

		if (check_flag == "1")
		{
			v_table = "hmmsmzxhbb "; 			
		}
		else
		{
			v_table = "tmmsmzxhbb ";			
		}
		Log::Info("", __FUNCTION__, "do_flag   =[{0}]", do_flag);
		sqlstr_temp += " AND ((HEAT_NO>='" + tmmsmzxhbb_lh["AOD0_S"].ToString() + "' AND HEAT_NO<='" + tmmsmzxhbb_lh["AOD0_E"].ToString() + "')";
		sqlstr_temp += " OR (HEAT_NO>='" + tmmsmzxhbb_lh["AOD1_S"].ToString() + "' AND HEAT_NO<='" + tmmsmzxhbb_lh["AOD1_E"].ToString() + "')";
		sqlstr_temp += " OR (HEAT_NO>='" + tmmsmzxhbb_lh["AOD2_S"].ToString() + "' AND HEAT_NO<='" + tmmsmzxhbb_lh["AOD2_E"].ToString() + "')";
		sqlstr_temp += " OR (HEAT_NO>='" + tmmsmzxhbb_lh["AOD6_S"].ToString() + "' AND HEAT_NO<='" + tmmsmzxhbb_lh["AOD6_E"].ToString() + "')";
		sqlstr_temp += " OR (HEAT_NO>='" + tmmsmzxhbb_lh["BOF0_S"].ToString() + "' AND HEAT_NO<='" + tmmsmzxhbb_lh["BOF0_E"].ToString() + "')";
		sqlstr_temp += " OR (HEAT_NO>='" + tmmsmzxhbb_lh["BOF1_S"].ToString() + "' AND HEAT_NO<='" + tmmsmzxhbb_lh["BOF1_E"].ToString() + "')";
		sqlstr_temp += " OR (HEAT_NO>='" + tmmsmzxhbb_lh["BOF2_S"].ToString() + "' AND HEAT_NO<='" + tmmsmzxhbb_lh["BOF2_E"].ToString() + "')";
		sqlstr_temp += " OR (HEAT_NO>='" + tmmsmzxhbb_lh["BOF9_S"].ToString() + "' AND HEAT_NO<='" + tmmsmzxhbb_lh["BOF9_E"].ToString() + "')";
		if (tmmsmzxhbb_lh["HEAT_IN"].ToString().Trim() != "")
		{
			tmmsmzxhbb_lh["HEAT_IN"] = tmmsmzxhbb_lh["HEAT_IN"].ToString().Replace(",", "','");
			sqlstr_temp += " OR ( heat_no in ('" + tmmsmzxhbb_lh["HEAT_IN"].ToString() + "') )";
		}
		
		sqlstr_temp = sqlstr_temp + ")";
		if (tmmsmzxhbb_lh["HEAT_OUT"].ToString().Trim() != "")
		{
			tmmsmzxhbb_lh["HEAT_OUT"] = tmmsmzxhbb_lh["HEAT_OUT"].ToString().Replace(",", "','");
			sqlstr_temp += " and heat_no not in ('" + tmmsmzxhbb_lh["HEAT_OUT"].ToString() + "')";
		}
		Log::Info("", __FUNCTION__, "sqlstr_temp   =[{0}]", sqlstr_temp);
		if (do_flag == "00")  //铬镍炉次明细表
		{
			sqlstr = " select * "
				" from " + v_table +
				" where 1=1"
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		if (do_flag == "01")  //铬镍收得率汇分炉总报表
		{
			sqlstr = "select @time_stamps as date_1,@user_id as damin, tap_end_time	,origin_sys_code as dev_code,prod_out_wt as mat_wt"				
				"	,zxh.heat_no,zxh.st_no, zxh.st_no_desc st_no_ms, zxh.st_no_small_class1 st_no_lb, zxh.st_no_big_class st_no_dl "
				" ,nvl(mat_act_wt, 0) hgcl"
				" ,round(nvl(cr*mat_act_wt, 0), 3) at_cr"
				", round(nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else ni end)*mat_act_wt, 0), 3) at_ni"
				", round(nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  )  then 0 else mo end)*mat_act_wt, 0), 3) at_mo"
				" , round(zxh.ni_value, 3) davo_ni	,round(zxh.cr_value, 3) davo_cr,round(zxh.mo_value, 3) davo_mo "				
				" ,round(nvl(case when mat_act_wt*cr = 0 or zxh.cr_value = 0 then 0 else(mat_act_wt*cr)/(zxh.cr_value)   end, 0), 5) * 100 cr "
				" ,round(nvl(case when mat_act_wt*ni = 0 or zxh.ni_value = 0 or  zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  ) then 0 else (mat_act_wt*ni)/(zxh.ni_value)  end, 0), 5) * 100 ni "
				" ,round(nvl(case when mat_act_wt*mo = 0 or zxh.mo_value = 0 or  zxh.st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  ) then 0 else (mat_act_wt*mo)/(zxh.mo_value)  end, 0), 5) * 100 mo "
				" ,'1-成品成分'  CF_LY"
				"	from "
				"  (select heat_no, st_no,max(tap_end_time) tap_end_time,max(origin_sys_code) origin_sys_code,max(st_no_desc) st_no_desc,max(st_no_small_class1) st_no_small_class1,max(st_no_big_class) st_no_big_class, max(prod_out_wt) prod_out_wt"
				" , sum(ni_value*devo_wt) ni_value, sum(cr_value*devo_wt) cr_value, sum(mo_value*devo_wt) mo_value, sum(devo_wt) devo_wt "
				" from " + v_table +
				" where 1=1  "
				" and substr(st_no, 1, 1) in('1', '4') "
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by heat_no,st_no"
				"	) zxh"
				" left join ("
				" select heat_no,st_no,sum(mat_act_wt) mat_act_wt "
				" from vmmsmcpcl_bb1 "
				" where 1 = 1 "
				+ sqlstr_temp +
				" group by heat_no,st_no"
				") bb1 on zxh.heat_no = bb1.heat_no and zxh.st_no = bb1.st_no"
				" left join ("
				"	select  heat_no, max(nvl(case when elm_006 = -1 then 0 else elm_006 end,0)) cr, max(nvl(case when elm_007 = -1 then 0 else elm_007 end,0)) ni,max(nvl(case when elm_008 = -1 then 0 else elm_008 end,0)) mo "
				"   from tqmtsb0 where 1=1  "
				+ sqlstr_temp +
				" group by heat_no"
				" ) b0 	on zxh.heat_no = b0.heat_no   "
				" union all"
				" select @time_stamps as date_1,@user_id as damin, tap_end_time	,origin_sys_code as dev_code,prod_out_wt as mat_wt"
				"	,zxh.heat_no,zxh.st_no, zxh.st_no_desc st_no_ms, zxh.st_no_small_class1 st_no_lb, zxh.st_no_big_class st_no_dl "
				" ,nvl(mat_act_wt, 0) hgcl"
				" ,round(nvl(cr*mat_act_wt, 0), 3) at_cr"
				", round(nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else ni end)*mat_act_wt, 0), 3) at_ni"
				", round(nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  )  then 0 else mo end)*mat_act_wt, 0), 3) at_mo"
				" , round(zxh.ni_value, 3) davo_ni	,round(zxh.cr_value, 3) davo_cr,round(zxh.mo_value, 3) davo_mo "
				" ,round(nvl(case when mat_act_wt*cr = 0 or zxh.cr_value = 0 then 0 else(mat_act_wt*cr)/(zxh.cr_value)   end, 0), 5) * 100 cr "
				" ,round(nvl(case when mat_act_wt*ni = 0 or zxh.ni_value = 0 or  zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  ) then 0 else (mat_act_wt*ni)/(zxh.ni_value)  end, 0), 5) * 100 ni "
				" ,round(nvl(case when mat_act_wt*mo = 0 or zxh.mo_value = 0 or  zxh.st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  ) then 0 else (mat_act_wt*mo)/(zxh.mo_value)  end, 0), 5) * 100 mo "
				" ,'1-化验成分'  CF_LY"
				"	from "
				"  (select heat_no, st_no,max(tap_end_time) tap_end_time,max(origin_sys_code) origin_sys_code,max(st_no_desc) st_no_desc,max(st_no_small_class1) st_no_small_class1,max(st_no_big_class) st_no_big_class, max(prod_out_wt) prod_out_wt"
				" , sum(ni_value*devo_wt) ni_value, sum(cr_value*devo_wt) cr_value, sum(mo_value*devo_wt) mo_value, sum(devo_wt) devo_wt "
				" from " + v_table +
				" where 1=1  "
				" and substr(st_no, 1, 1) in('1', '4') "
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by heat_no,st_no"
				"	) zxh"
				" left join ("
				" select heat_no,st_no,sum(mat_act_wt) mat_act_wt "
				" from vmmsmcpcl_bb1 "
				" where 1 = 1 "
				+ sqlstr_temp +
				" group by heat_no,st_no"
				") bb1 on zxh.heat_no = bb1.heat_no and zxh.st_no = bb1.st_no"
				" left join ("
				" select heat_no, max(nvl(case when elm_006 = -1 then 0 else elm_006 end, 0)) cr, max(nvl(case when elm_007 = -1 then 0 else elm_007 end, 0)) ni, max(nvl(case when elm_008 = -1 then 0 else elm_008 end, 0)) mo  "
				"	  from tqmts24_init t1 where exists ( SELECT 1 FROM tqmtsb0  t2 WHERE  t2.ID_ELM=t1.ID_ELM and t2.ST_SAMPLE_NO = t1.ST_SAMPLE_NO)  " + sqlstr_temp + " and WHOLE_BACKLOG_CODE='C' group by heat_no"
				
				" ) b0 	on zxh.heat_no = b0.heat_no   "
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

	

		}
		if (do_flag == "02")  //铬镍收得率汇分钢种消耗总报表
		{
			sqlstr =
				//按钢种进行汇总
				" select @time_stamps AS JS_DATE,@user_id AS ADMIN,st_no,sum(mat_act_wt) HGCL,round(sum(NI_VALUE),3) TR_NI,round(sum(CR_VALUE),3) TR_CR,round(sum(MO_VALUE),3) TR_MO,sum(DEVO_WT) DEVO_WT"
				" ,round(sum(mat_act_wt*HY_CR),3) HY_CR"
				",round(sum(mat_act_wt*(case when st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else HY_NI end)),3) HY_NI"
				",round(sum(mat_act_wt*(case when st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  )  then 0 else HY_MO end)),3) HY_MO"
				",round(sum(mat_act_wt*Cr),3) CP_Cr"
				",round(sum(mat_act_wt*(case when st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else Ni end)),3) CP_Ni"
				",round(sum(mat_act_wt*(case when st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  )  then 0 else MO end)),3) CP_MO"
				" ,case when sum(CR_VALUE) = 0 then 0 else round(sum(mat_act_wt*HY_CR)*100/sum(cr_VALUE),3) end HY_CR_SDL"
				" ,case when st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  ) or sum(ni_value)=0 then 0 else round(sum(mat_act_wt*hy_ni)*100/sum(ni_value),3) end hy_ni_sdl"					
				" ,case when st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  ) or sum(mo_value)=0 then 0 else round(sum(mat_act_wt*hy_mo)*100/sum(mo_value),3) end hy_mo_sdl"
				" ,case when sum(cr_VALUE) = 0 then 0 else round(sum(mat_act_wt*CR)*100/sum(cr_VALUE),3) end CP_CR_SDL"
				" ,case when st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  ) or sum(ni_value)=0 then 0 else round(sum(mat_act_wt*NI)*100/sum(ni_value),3) end CP_NI_SDL"
				" ,case when st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  ) or sum(mo_value)=0 then 0 else round(sum(mat_act_wt*mo)*100/sum(mo_value),3) end CP_mo_SDL"
				",sum(PROD_OUT_WT) SCL"
				" from ("
				//取消耗的投入量				
				" (select BB.heat_no,BB.st_no,PROD_OUT_WT,NVL(mat_act_wt,0) mat_act_wt,NI_VALUE,CR_VALUE,MO_VALUE,DEVO_WT,NVL(HY_CR,0) HY_CR,NVL(HY_NI,0) HY_NI,NVL(HY_MO,0) HY_MO,nvl(Cr,0) Cr,nvl(Ni,0) Ni,nvl(MO,0) MO"
				" FROM "
				" (SELECT heat_no,st_no,MAX(PROD_OUT_WT) PROD_OUT_WT,SUM(NI_VALUE*DEVO_WT) NI_VALUE,SUM(CR_VALUE*DEVO_WT) CR_VALUE,SUM(MO_VALUE*DEVO_WT) MO_VALUE, SUM(DEVO_WT) DEVO_WT "
				" from "+v_table+
				" where 1=1  "
				" and substr(st_no, 1, 1) in('1', '4') "
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by heat_no,st_no"
				"	)  BB "
				//产量
				" left join ("
				" SELECT heat_no,st_no,sum(mat_act_wt) mat_act_wt "
				" FROM VMMSMCPCL_BB1 "
				" WHERE 1 = 1 "
				+ sqlstr_temp +
				" group by heat_no,st_no"
				" ) BB1 on BB.HEAT_NO=BB1.HEAT_NO and BB.ST_NO=BB1.ST_NO"
				//成分
				"	 LEFT JOIN ("
				"   SELECT HEAT_NO, max(NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0)) HY_CR, max(NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0)) HY_NI, max(NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0)) HY_MO"
				"	FROM TQMTS24_INIT t1"
				"   WHERE exists ( SELECT 1 FROM tqmtsb0  t2 WHERE  t2.ID_ELM=t1.ID_ELM and t2.ST_SAMPLE_NO = t1.ST_SAMPLE_NO) "
				+ sqlstr_temp +
				"  and WHOLE_BACKLOG_CODE='C' group by heat_no"
				" ) TS24 ON BB.HEAT_NO = TS24.HEAT_NO  "
				//成分
				"	LEFT JOIN  ("
				"	SELECT  HEAT_NO, max(NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END,0)) Cr, max(NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END,0)) Ni,max(NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END,0)) MO "
				"   FROM TQMTSB0 WHERE 1=1  "
				+ sqlstr_temp +
				" group by heat_no"
				"  ) B0 ON BB.HEAT_NO = B0.HEAT_NO  "
				" )"
				" ) group by st_no"
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());	 			
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		//03铬镍收得率汇总报表
		if (do_flag == "03")  
		{
			sqlstr = " SELECT  ROUND(SUM(NET_WT*NVL(CR,0))/1000,3) AS SUM_CR,ROUND(SUM(NET_WT*NVL(NI,0))/1000,3) AS SUM_NI,ROUND(SUM(NET_WT*NVL(MO,0))/1000,3) AS SUM_MO"
				" FROM"
				" (SELECT MAT_CODE,sum(NET_WT) NET_WT"
				" FROM TMMSM81_S"
				" WHERE MAT_CODE LIKE 'F06%'"				
				" AND REC_CREATE_TIME <= @end_time"
				" AND REC_CREATE_TIME >= @start_time"
				" group by MAT_CODE "
				" UNION"
				" SELECT MAT_CODE, SUM(STOCK_WT) "
				" FROM TMMSM89 "
				" WHERE EVENT_NAME = '自循环废钢冲销'"
				" AND REC_CREATE_TIME <= @end_time"
				" AND REC_CREATE_TIME >= @start_time"
				" group by MAT_CODE "
				" UNION"
				" SELECT MATERIAL_CODE as MAT_CODE,sum(MAT_WT) NET_WT"
				" FROM TWMSM61 "
				" WHERE SUBSTR(PLAN_NO,1,4)='21JF' "
				" and LOAD_END_TIME<= @end_time and LOAD_END_TIME>=@start_time"
				" group by MATERIAL_CODE) A LEFT JOIN  ZJ_SCRAP_ELEMENT  B ON A.MAT_CODE=B.MAT_ID" 				
				;
			cmd_inq.Parameters.Set("end_time", tmmsmzxhbb_lh["END_TIME"].ToString());
			cmd_inq.Parameters.Set("start_time", tmmsmzxhbb_lh["START_TIME"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				sum_cr = cmd_inq.GetDecimal(1);
				sum_ni = cmd_inq.GetDecimal(2);
				sum_mo = cmd_inq.GetDecimal(3);	 
			}
			cmd_inq.Close();

				sqlstr = "select @time_stamps AS time_stamps,@user_id AS user_id,round(sum(cr_value),3) DAVO_CR, round(sum(ni_value),3) DAVO_ni, round(sum(mo_value),3) DAVO_mo"
				" ,round(sum(nvl(mat_act_wt,0)*nvl(Cr,0)),3) AT_CR"
				",round(sum(nvl(mat_act_wt,0)*nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else ni end),0)),3) AT_ni"
				",round(sum(nvl(mat_act_wt,0)*nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  )  then 0 else mo end),0)),3) AT_mo"
				",CASE WHEN  sum(cr_value) =0  THEN 0 ELSE round((sum(nvl(mat_act_wt,0)*nvl(Cr,0))+@sum_cr)*100/sum(cr_value) , 3) END  cr  "
				",CASE WHEN  sum(ni_value) =0  THEN 0 ELSE round((sum(nvl(mat_act_wt,0)*nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else ni end),0))+@sum_ni)*100/sum(ni_value) , 3) END  ni  "
				",CASE WHEN  sum(mo_value) =0  THEN 0 ELSE round((sum(nvl(mat_act_wt,0)*nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else mo end),0))+@sum_mo)*100/sum(mo_value) ,3) END  mo  "
				",@sum_cr AS sum_cr,@sum_ni AS sum_ni,@sum_mo AS sum_mo,sum(nvl(mat_act_wt,0)) mat_act_wt"
				" ,'1-成品成分'  CF_LY"
				" from ("
				//消耗的
				" select  heat_no,st_no,sum(cr_value*devo_wt) cr_value,sum(ni_value*devo_wt) ni_value,sum(mo_value*devo_wt) mo_value"
				" from  " + v_table +
				"  where 1=1   "
				" and substr(st_no, 1, 1) in('1', '4')"
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by heat_no,st_no"
				" ) ZXH  "
				"	LEFT JOIN ("
				//合格量
				" select heat_no,st_no,sum(mat_act_wt) mat_act_wt from vmmsmcpcl_bb1 where 1=1   "
				+ sqlstr_temp +	
				" group by heat_no,st_no"
				" ) BB1 ON ZXH.HEAT_NO = BB1.HEAT_NO and ZXH.st_no = BB1.st_no"
				" LEFT JOIN "
				"	(SELECT HEAT_NO ,max(NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END,0)) Cr, max(NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END,0)) Ni, max(NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END,0)) MO  "
				"	  FROM TQMTSB0 WHERE 1=1  "
				+ sqlstr_temp +
				" group by heat_no"
				" ) B0 	ON BB1.HEAT_NO = B0.HEAT_NO "
				" union all"
				" select @time_stamps,@user_id,round(sum(cr_value),3) DAVO_CR, round(sum(ni_value),3) DAVO_ni, round(sum(mo_value),3) DAVO_mo"
				" ,round(sum(nvl(mat_act_wt,0)*nvl(Cr,0)),3) AT_CR"
				",round(sum(nvl(mat_act_wt,0)*nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else ni end),0)),3) AT_ni"
				",round(sum(nvl(mat_act_wt,0)*nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_mo = '1'  )  then 0 else mo end),0)),3) AT_mo"
				",CASE WHEN  sum(cr_value) =0  THEN 0 ELSE round((sum(nvl(mat_act_wt,0)*nvl(Cr,0))+@sum_cr)*100/sum(cr_value) , 3) END  cr  "
				",CASE WHEN  sum(ni_value) =0  THEN 0 ELSE round((sum(nvl(mat_act_wt,0)*nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else ni end),0))+@sum_ni)*100/sum(ni_value) , 3) END  ni  "
				",CASE WHEN  sum(mo_value) =0  THEN 0 ELSE round((sum(nvl(mat_act_wt,0)*nvl((case when zxh.st_no in (select st_no from tmmsmw8 where suggest_ni = '1'  )  then 0 else mo end),0))+@sum_mo)*100/sum(mo_value) ,3) END  mo  "
				",@sum_cr AS sum_cr,@sum_ni AS sum_ni,@sum_mo AS sum_mo,sum(nvl(mat_act_wt,0)) mat_act_wt"
				" ,'1-化验成分'  CF_LY"
				" from ("
				//消耗的
				" select  heat_no,st_no,sum(cr_value*devo_wt) cr_value,sum(Ni_value*devo_wt) Ni_value,sum(mo_value*devo_wt) mo_value"
				" from  " + v_table +
				"  where 1=1   "
				" and substr(st_no, 1, 1) in('1', '4')"
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by heat_no,st_no"
				" ) ZXH  "
				"	LEFT JOIN ("
				//合格量
				" select heat_no,st_no,sum(mat_act_wt) mat_act_wt from vmmsmcpcl_bb1 where 1=1   "
				+ sqlstr_temp +
				" group by heat_no,st_no"
				" ) BB1 ON ZXH.HEAT_NO = BB1.HEAT_NO and ZXH.st_no=BB1.st_no"
				" LEFT JOIN "
				"	(SELECT HEAT_NO ,max(NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END,0)) Cr, max(NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END,0)) Ni, max(NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END,0)) MO  "
				"	  FROM TQMTS24_INIT t1 WHERE exists ( SELECT 1 FROM tqmtsb0  t2 WHERE  t2.ID_ELM=t1.ID_ELM and t2.ST_SAMPLE_NO = t1.ST_SAMPLE_NO)" + sqlstr_temp + "   and WHOLE_BACKLOG_CODE='C' group by heat_no"
				" ) B0 	ON BB1.HEAT_NO = B0.HEAT_NO "
				;
				Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
				cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
				cmd_inq.Parameters.Set("sum_cr", sum_cr);
				cmd_inq.Parameters.Set("sum_ni", sum_ni);
				cmd_inq.Parameters.Set("sum_mo", sum_mo);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();

		}
		//04合格产量报表
		if (do_flag == "04") 
		{
			sqlstr = " select A.HEAT_NO,A.ST_NO,A.START_TIME,@time_stamps AS DATE_1,@user_id AS DAMIN "
				" ,A.SLAB_WT,B.SG_GRADE_1 AS ST_NO_DESC,B.ST_NO_BIG_CLASS  "
				" ,(CASE WHEN SUBSTR(A.ST_NO, 0, 2) = '1A' OR SUBSTR(A.ST_NO, 0, 2) = '1D' THEN '镍钢'  "
				" WHEN SUBSTR(A.ST_NO, 0, 2) = '1M' OR SUBSTR(A.ST_NO, 0, 2) = '1F' THEN '铬钢'  "
				" WHEN SUBSTR(A.ST_NO, 0, 1) = '1' THEN '不锈钢' else '碳钢' end  ) ST_NO_SMALL_CLASS  "
				" from "
				" (SELECT heat_no,st_no,max(REC_CREATE_TIME) START_TIME,sum(mat_act_wt) SLAB_WT "
				" FROM VMMSMCPCL_BB1 "
				" WHERE 1 = 1 "
				+ sqlstr_temp +
				" group by heat_no,st_no"
				") A left join TQMTS0X B on A.ST_NO = B.ST_NO"
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

		}
		 //05废钢报表
		if (do_flag == "05") 
		{

			sqlstr = " SELECT  ROUND(SUM(NET_WT*NVL(CR,0))/1000,3) AS SUM_CR,ROUND(SUM(NET_WT*NVL(NI,0))/1000,3) AS SUM_NI,ROUND(SUM(NET_WT*NVL(MO,0))/1000,3) AS SUM_MO"
				" FROM"
				" (SELECT MAT_CODE,sum(NET_WT) NET_WT"
				" FROM TMMSM81_S"
				" WHERE MAT_CODE LIKE 'F06%'"
				" AND MARK_POS_CODE = '5'"
				" AND REC_CREATE_TIME <= @end_time"
				" AND REC_CREATE_TIME >= @start_time"
				" group by MAT_CODE "
				" UNION"
				"      SELECT MAT_CODE, sum(NET_WT) NET_WT"
				"      FROM TMMSM81"
				"      WHERE  AUART='E'"
				"        AND RECEIVING_STATUS = '9' "
				"        AND REC_CREATE_TIME <= @end_time"
				"        AND REC_CREATE_TIME >= @start_time"
				"      group by MAT_CODE, BUNKER_NO "
				" UNION"
				" SELECT MAT_CODE,SUM(STOCK_WT) FROM TMMSM89 WHERE EVENT_NAME = '自循环废钢冲销' "
				" AND REC_CREATE_TIME <= @end_time"
				" AND REC_CREATE_TIME >= @start_time"
				" group by MAT_CODE "
				" UNION"
				" SELECT MATERIAL_CODE as MAT_CODE,sum(MAT_WT) NET_WT"
				" FROM TWMSM61 "
				" WHERE SUBSTR(PLAN_NO,1,4)='21JF' "
				" and LOAD_END_TIME<= @end_time and LOAD_END_TIME>=@start_time"
				" group by MATERIAL_CODE) A LEFT JOIN  ZJ_SCRAP_ELEMENT  B ON A.MAT_CODE=B.MAT_ID"
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.Parameters.Set("end_time", tmmsmzxhbb_lh["END_TIME"].ToString());
			cmd_inq.Parameters.Set("start_time", tmmsmzxhbb_lh["START_TIME"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				sum_cr = cmd_inq.GetDecimal(1);
				sum_ni = cmd_inq.GetDecimal(2);
				sum_mo = cmd_inq.GetDecimal(3);
			}
			cmd_inq.Close();


			sqlstr = " SELECT  @time_stamps AS DATE_1,@user_id AS DAMIN,@start_time as start_time,@end_time end_time ,@sum_cr sum_cr,@sum_ni sum_ni,@sum_mo sum_mo"
				",A.MAT_CODE,C.MAT_NAME ,max(NVL(CR,0)) CR	 ,max(NVL(NI,0)) NI,max(NVL(MO,0)) MO,SUM(NET_WT) DEVO_WT"
				",ROUND(SUM(NET_WT*NVL(CR,0))/1000,3) AS SUM_CR_1,ROUND(SUM(NET_WT*NVL(NI,0))/1000,3) AS SUM_NI_1,ROUND(SUM(NET_WT*NVL(MO,0))/1000,3) AS SUM_MO_1"
				" FROM"
				" (SELECT MAT_CODE,BUNKER_NO,sum(NET_WT) NET_WT"
				" FROM TMMSM81_S"
				" WHERE MAT_CODE LIKE 'F06%'"
				" AND MARK_POS_CODE = '5'"
				" AND REC_CREATE_TIME <= @end_time"
				" AND REC_CREATE_TIME >= @start_time"
				" group by MAT_CODE,BUNKER_NO "
				" UNION"
				"      SELECT MAT_CODE, BUNKER_NO, sum(NET_WT) NET_WT"
				"      FROM TMMSM81"
				"      WHERE  AUART='E'"
				"        AND RECEIVING_STATUS = '9' "
				"        AND REC_CREATE_TIME <= @end_time"
				"        AND REC_CREATE_TIME >= @start_time"
				"      group by MAT_CODE, BUNKER_NO "
				" UNION"
				" SELECT MAT_CODE,BUNKER_NO, SUM(STOCK_WT) FROM TMMSM89 WHERE EVENT_NAME = '自循环废钢冲销'  "
				" AND REC_CREATE_TIME <= @end_time"
				" AND REC_CREATE_TIME >= @start_time"
				" group by MAT_CODE,BUNKER_NO "
				" UNION"
				" SELECT MATERIAL_CODE as MAT_CODE,LOAD_CODE_FACTORY as BUNKER_NO,sum(MAT_WT) NET_WT"
				" FROM TWMSM61 "
				" WHERE SUBSTR(PLAN_NO,1,4)='21JF' "
				" and LOAD_END_TIME<= @end_time and LOAD_END_TIME>=@start_time"
				" group by MATERIAL_CODE,LOAD_CODE_FACTORY) A LEFT JOIN  ZJ_SCRAP_ELEMENT  B ON A.MAT_CODE=B.MAT_ID"
				" LEFT JOIN TMMSM50 C ON A.MAT_CODE = C.MAT_CODE"
				" group by A.MAT_CODE,C.MAT_NAME"
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("end_time", tmmsmzxhbb_lh["END_TIME"].ToString());
			cmd_inq.Parameters.Set("start_time", tmmsmzxhbb_lh["START_TIME"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("sum_cr", sum_cr);
			cmd_inq.Parameters.Set("sum_ni", sum_ni);
			cmd_inq.Parameters.Set("sum_mo", sum_mo);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

		}

		//07物料加权成分报表
		if (do_flag == "07")  
		{
			sqlstr = " SELECT @time_stamps AS DATE_1,@user_id AS DAMIN ,A.MAT_CODE,B.MAT_NAME, SUM(DEVO_WT) AS DEVO_WT ,SUM(DEVO_WT*NI_VALUE) AS SUM_NI_IN, SUM(DEVO_WT*CR_VALUE)  AS SUM_CR_IN, SUM(DEVO_WT*MO_VALUE)  AS SUM_MO_IN"
				" ,ROUND(DECODE(SUM(DEVO_WT),0,0,SUM(DEVO_WT*NI_VALUE)/SUM(DEVO_WT)),4) AS NI"
				", ROUND(DECODE(SUM(DEVO_WT),0,0,SUM(DEVO_WT*CR_VALUE)/SUM(DEVO_WT)),4) AS CR "
				", ROUND(DECODE(SUM(DEVO_WT),0,0,SUM(DEVO_WT*MO_VALUE)/SUM(DEVO_WT)),4) AS MO "
				" FROM " + v_table + " A"
				" LEFT JOIN TMMSM50 B ON A.MAT_CODE=B.MAT_CODE  "
				" where 1=1 "
				" and A.user_id = @user_id"
				" and A.time_stamps = @time_stamps"
				" group by A.MAT_CODE,B.MAT_NAME  "
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		//08消耗对比报表
		if (do_flag == "08")
		{
			sqlstr = " SELECT @time_stamps AS DATE_1,@user_id AS DAMIN,@start_time as start_time,@end_time end_time "
				",t1.HEAT_NO, DEVO_WT,DEVO_WT_MES ,DEVO_WT-DEVO_WT_MES CY"
				" from ("
				" select heat_no,sum(DEVO_WT) DEVO_WT"
				" from " + v_table +
				" where 1=1  "
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by heat_no"
				" ) t1 left join "
				" (select heat_no,sum(OUT_STOCK_WT/1000) DEVO_WT_MES"
				" from tmmsm56 "
				" where 1=1 "
				" and mat_code IN (SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG != '1') "
				+ sqlstr_temp +
				" group by heat_no"
				" ) t2 on t1.heat_no=t2.heat_no"
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("end_time", tmmsmzxhbb_lh["END_TIME"].ToString());
			cmd_inq.Parameters.Set("start_time", tmmsmzxhbb_lh["START_TIME"].ToString());
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		//10碳钢钢铁料钢种消耗报表
		if (do_flag == "10")  
		{
			sqlstr = " SELECT ZXH.ST_NO ST_NO_DESC,SUM(ZXH.PROD_OUT_WT) MAT_WT,SUM(MAT_ACT_WT) MAT_ACT_WT,SUM(DEVO_WT) DEVO_WT, "
				" ROUND(nvl(CASE WHEN SUM(DEVO_WT) = 0 OR SUM(ZXH.PROD_OUT_WT) = 0 THEN 0 ELSE SUM(DEVO_WT) / SUM(ZXH.PROD_OUT_WT) END, 0), 5) * 100 XHL "
				" from "
				" (SELECT HEAT_NO,ST_NO,MAX(PROD_OUT_WT) PROD_OUT_WT,SUM(DEVO_WT) DEVO_WT"
				" FROM TMMSMZXHBB ZXH   "
				" WHERE 1=1"
				" and mat_code in (select mat_code from tmmsm50 where mat_type in ('1','4'))"
				" and substr(st_no,1,1) in ('2','3','5')  "
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by HEAT_NO,ST_NO) zxh"
				" LEFT JOIN ("
				//合格量
				" select heat_no,st_no,sum(mat_act_wt) mat_act_wt from vmmsmcpcl_bb1 where 1=1   "
				" and substr(st_no,1,1) in ('2','3','5')  "
				+ sqlstr_temp +
				" group by heat_no,st_no"
				" ) BB1 ON BB1.HEAT_NO=ZXH.HEAT_NO AND  BB1.ST_NO = ZXH.ST_NO "
				" where substr(ZXH.st_no,1,1) in ('2','3','5') "
				" group by ZXH.ST_NO"
				; 
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close(); 
				
		}
		//11碳钢钢铁料炉次消耗报表
		if (do_flag == "11") 
		{
			sqlstr = " SELECT @time_stamps as date_1,@user_id as damin,ZXH.HEAT_NO,ZXH.ST_NO ST_NO_DESC,TAP_END_TIME,ORIGIN_SYS_CODE AS SCL_LY,SUM(ZXH.PROD_OUT_WT) MAT_WT,SUM(MAT_ACT_WT) MAT_ACT_WT,SUM(DEVO_WT) DEVO_WT, "
				" ROUND(nvl(CASE WHEN SUM(DEVO_WT) = 0 OR SUM(ZXH.PROD_OUT_WT) = 0 THEN 0 ELSE SUM(DEVO_WT) / SUM(ZXH.PROD_OUT_WT) END, 0), 5) * 100 XHL "
				" from "
				" (SELECT HEAT_NO,ST_NO,MAX(TAP_END_TIME) TAP_END_TIME,MAX(ORIGIN_SYS_CODE) ORIGIN_SYS_CODE,MAX(PROD_OUT_WT) PROD_OUT_WT,SUM(DEVO_WT) DEVO_WT"
				" FROM TMMSMZXHBB   "
				" WHERE 1=1"
				" and mat_code in (select mat_code from tmmsm50 where mat_type in ('1','4'))"
				" and substr(st_no,1,1) in ('2','3','5')  "
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by HEAT_NO,ST_NO) zxh"
				" LEFT JOIN ("
				//合格量
				" select heat_no,st_no,sum(mat_act_wt) mat_act_wt from vmmsmcpcl_bb1 where 1=1   "
				" and substr(st_no,1,1) in ('2','3','5')  "
				+ sqlstr_temp +
				" group by heat_no,st_no"
				" ) BB1 ON BB1.HEAT_NO=ZXH.HEAT_NO AND  BB1.ST_NO = ZXH.ST_NO "
				" group by ZXH.HEAT_NO,ZXH.ST_NO,TAP_END_TIME,ORIGIN_SYS_CODE"
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

		}

		//12转炉钢铁料炉次消耗报表
		if (do_flag == "12")  
		{
			sqlstr = "SELECT @time_stamps AS DATE_1,@user_id AS DAMIN ,HEAT_NO,ST_NO,MAX(TAP_END_TIME) TAP_END_TIME,MAX(ORIGIN_SYS_CODE) ORIGIN_SYS_CODE,MAX(PROD_OUT_WT) MAT_WT,SUM(DEVO_WT) DEVO_WT"
				" ,MAX(ROUTELIST) BACKLOG_CODE, MAX(ORIGIN_SYS_CODE) SCL_LY, MAX(DEV_CODE) DEV_CODE, MAX(SHIFT_GROUP) SHIFT_GROUP, ST_NO, MAX(ST_NO_DESC) ST_NO_DESC "
				" ,MAX(ST_NO_SMALL_CLASS1) ST_NO_LB,MAX(ST_NO_BIG_CLASS) ST_NO_DL   "
				" ,ROUND(nvl(CASE WHEN SUM(DEVO_WT) = 0 OR SUM(PROD_OUT_WT) = 0 THEN 0 ELSE SUM(DEVO_WT) / SUM(PROD_OUT_WT) END, 0), 5) * 100 XHL "
				" FROM TMMSMZXHBB ZXH   "
				" WHERE 1=1"
				" and mat_code in (select mat_code from tmmsm50 where mat_type in ('1','4'))"
				" and dev_code like 'B%'"
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by heat_no,st_no" 				
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

		}

		//13连铸收得率报表
		if (do_flag == "13")  
		{
			sqlstr = " SELECT @time_stamps AS DATE_1,@user_id AS DAMIN ,ZXH.HEAT_NO,ZXH.ST_NO,TAP_END_TIME,SCL_LY,ST_NO_DESC,ST_NO_LB,ST_NO_DL"
				",ZXH.PROD_OUT_WT AS OUT_STEEL_WT,MAT_ACT_WT,DEVO_WT "
				" ,CASE WHEN PROD_OUT_WT = 0 OR NVL(MAT_ACT_WT,0) = 0 THEN 0 ELSE ROUND(nvl(MAT_ACT_WT,0)*100/PROD_OUT_WT,3) END XHL "
				" from "
				" (SELECT HEAT_NO,ST_NO,MAX(TAP_END_TIME) TAP_END_TIME,MAX(ORIGIN_SYS_CODE) SCL_LY,MAX(PROD_OUT_WT) PROD_OUT_WT,SUM(DEVO_WT) DEVO_WT"
				", MAX(ZXH.ST_NO_DESC) ST_NO_DESC,MAX(ST_NO_SMALL_CLASS1) ST_NO_LB,MAX(ST_NO_BIG_CLASS) ST_NO_DL"
				" FROM TMMSMZXHBB ZXH   "
				" WHERE 1=1"
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by HEAT_NO,ST_NO) zxh"
				" LEFT JOIN ("
				//合格量
				" select heat_no,st_no,sum(mat_act_wt) mat_act_wt from vmmsmcpcl_bb1 where 1=1   "
				+ sqlstr_temp +
				" group by heat_no,st_no"
				" ) BB1 ON BB1.HEAT_NO=ZXH.HEAT_NO AND  BB1.ST_NO = ZXH.ST_NO "			
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		//14 AOD金属料消耗报表
		if (do_flag == "14") 
		{
			sqlstr = "SELECT @time_stamps AS DATE_1,@user_id AS DAMIN ,HEAT_NO,ST_NO,MAX(TAP_END_TIME) TAP_END_TIME,MAX(ORIGIN_SYS_CODE) ORIGIN_SYS_CODE,MAX(PROD_OUT_WT) MAT_WT,SUM(DEVO_WT) DEVO_WT"
				" ,MAX(ROUTELIST) BACKLOG_CODE, MAX(ORIGIN_SYS_CODE) SCL_LY, MAX(DEV_CODE) DEV_CODE, MAX(SHIFT_GROUP) SHIFT_GROUP, ST_NO, MAX(ST_NO_DESC) ST_NO_DESC "
				" ,MAX(ST_NO_SMALL_CLASS1) ST_NO_LB,MAX(ST_NO_BIG_CLASS) ST_NO_DL   "
				" ,ROUND(nvl(CASE WHEN SUM(DEVO_WT) = 0 OR SUM(PROD_OUT_WT) = 0 THEN 0 ELSE SUM(DEVO_WT) / SUM(PROD_OUT_WT) END, 0), 5) * 100 XHL "
				" FROM TMMSMZXHBB ZXH   "
				" WHERE 1=1"
				" and mat_code in (select mat_code from tmmsm50 where mat_type in ('1','2','4'))"
				" and dev_code like 'A%'"
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by heat_no,st_no"
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		//15 回收废钢报表
		if (do_flag == "15")  
		{
			sqlstr = " select @time_stamps as date_1,@user_id as damin,@start_time as start_time,@end_time end_time,FG_WT,HSFG_WT,mat_act_wt"
				" from "
				" (SELECT  sum(NET_WT) FG_WT"
				" FROM"
				" (SELECT sum(NET_WT) NET_WT"
				" FROM TMMSM81_S"
				" WHERE MAT_CODE LIKE 'F06%'"
				" AND REC_CREATE_TIME <= @end_time"
				" AND REC_CREATE_TIME >= @start_time" 				
				" UNION"
				" SELECT sum(MAT_WT) NET_WT"
				" FROM TWMSM61 "
				" WHERE SUBSTR(PLAN_NO,1,4)='21JF' "
				" and LOAD_END_TIME<= @end_time and LOAD_END_TIME>=@start_time"
				" )) A "
				",("
				" select sum(HSFG_WT) HSFG_WT"
				" from "
				" (select sum(mat_act_wt) HSFG_WT  from hmmsm01 t3 where t3.complex_decide_code = '9'  " + sqlstr_temp +
				" union all"
				" select sum(CUT_SCRAP_WT) HSFG_WT from tmmsmfp t3 where  1=1  " + sqlstr_temp +
				")) hsfg"
				" , ("
				//合格量
				" select sum(mat_act_wt) mat_act_wt from vmmsmcpcl_bb1 where 1=1   "				
				+ sqlstr_temp +
				" ) BB1  "
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("end_time", tmmsmzxhbb_lh["END_TIME"].ToString());
			cmd_inq.Parameters.Set("start_time", tmmsmzxhbb_lh["START_TIME"].ToString());
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		//16不锈钢钢种消耗报表
		if (do_flag == "16") 
		{
			sqlstr = " SELECT  @time_stamps as date_1,@user_id as damin,ZXH.ST_NO ST_NO_DESC,SUM(ZXH.PROD_OUT_WT) MAT_WT,SUM(MAT_ACT_WT) MAT_ACT_WT,SUM(DEVO_WT) DEVO_WT, "
				" ROUND(nvl(CASE WHEN SUM(DEVO_WT) = 0 OR SUM(ZXH.PROD_OUT_WT) = 0 THEN 0 ELSE SUM(DEVO_WT) / SUM(ZXH.PROD_OUT_WT) END, 0), 5) * 100 XHL "
				" from "
				" (SELECT HEAT_NO,ST_NO,MAX(PROD_OUT_WT) PROD_OUT_WT,SUM(DEVO_WT) DEVO_WT"
				" FROM TMMSMZXHBB ZXH   "
				" WHERE 1=1"
				//" and mat_code in (select mat_code from tmmsm50 where mat_type in ('1','4'))"
				" and substr(st_no,1,1) in ('1','4')  "
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" group by HEAT_NO,ST_NO) zxh"
				" LEFT JOIN ("
				//合格量
				" select heat_no,st_no,sum(mat_act_wt) mat_act_wt from vmmsmcpcl_bb1 where 1=1   "
				" and substr(st_no,1,1) in ('1','4')  "
				+ sqlstr_temp +
				" group by heat_no,st_no"
				" ) BB1 ON BB1.HEAT_NO=ZXH.HEAT_NO AND  BB1.ST_NO = ZXH.ST_NO "	 				
				" group by ZXH.ST_NO"
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

		}

		if (do_flag == "17")
		{
			sqlstr = " SELECT @time_stamps as DATE_1,@user_id as DAMIN,ZXH.TAP_END_TIME,ZXH.HEAT_NO,ZXH.PROC_NO,ZXH.PROD_OUT_WT MAT_WT,ZXH.ORIGIN_SYS_CODE SCL_LY, "
					"  NVL(MAT_ACT_WT, 0) HGCL, ZXH.ST_NO, ZXH.ST_NO_DESC ST_NO_MS, ZXH.ST_NO_SMALL_CLASS ST_NO_LB, ZXH.ST_NO_BIG_CLASS ST_NO_DL, "
					" ROUND(NVL(CR*MAT_ACT_WT, 0), 3) AT_CR, ROUND(NVL(Ni*MAT_ACT_WT, 0), 3) AT_NI, ROUND(ZXH.NI_VALUE, 3) DAVO_NI,  "
					"   ROUND(ZXH.CR_VALUE, 3) DAVO_CR,ZXH.DEV_CODE,  "
					"  CASE WHEN ZXH.ST_NO = (SELECT ST_NO FROM TMMSMW8 WHERE ST_NO =ZXH.ST_NO  ) THEN 0 ELSE ROUND(nvl(CASE WHEN MAT_ACT_WT*Ni = 0 OR ZXH.NI_VALUE = 0  "
					"  THEN 0 ELSE(ZXH.NI_VALUE) / (MAT_ACT_WT*Ni)  END, 0), 5) * 100 END  NI,  "
					"  ROUND(nvl(CASE WHEN MAT_ACT_WT*CR = 0 OR ZXH.CR_VALUE = 0 THEN 0 ELSE(ZXH.CR_VALUE) / (MAT_ACT_WT*CR) END, 0), 5) * 100 CR  "
					"  FROM  (  "
					"  SELECT  ZXH.HEAT_NO, ZXH.ST_NO,ZXH.DEV_CODE, MAX(TIME_1) TIME_1, MAX(USER_NAME) USER_NAME, MAX(TAP_END_TIME) TAP_END_TIME, MAX(PROC_NO)PROC_NO, MAX(PROD_OUT_WT) PROD_OUT_WT, "
					"  MAX(ORIGIN_SYS_CODE) ORIGIN_SYS_CODE, MAX(ST_NO_DESC) ST_NO_DESC, MAX(ST_NO_SMALL_CLASS1)ST_NO_SMALL_CLASS, MAX(ST_NO_BIG_CLASS) ST_NO_BIG_CLASS, SUM(NI_VALUE*DEVO_WT) NI_VALUE,  "
					"  SUM(CR_VALUE*DEVO_WT) CR_VALUE, SUM(DEVO_WT) DEVO_WT, NVL(MAX(B0.Ni), 0) Ni, NVL(MAX(B0.CR), 0) CR, MAX(MAT_ACT_WT) MAT_ACT_WT FROM   "
					"  TMMSMZXHBB ZXH    "
					"  LEFT JOIN VMMSMCPCL_BB1 BB1 ON ZXH.HEAT_NO = BB1.HEAT_NO AND ZXH.ST_NO = BB1.ST_NO   "
					"  LEFT JOIN (SELECT NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END,0) Cr,   "
					"  NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END,0) Ni,HEAT_NO  FROM TQMTSB0 ) B0 ON ZXH.HEAT_NO = B0.HEAT_NO   "
					"  WHERE 1=1 AND SUBSTR(ZXH.ST_NO,1,1) IN('1','4')   "
					"  and user_id = @user_id"
					"  and time_stamps = @time_stamps"
					"  GROUP BY ZXH.HEAT_NO,ZXH.ST_NO,ZXH.DEV_CODE) ZXH   "
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		//18碳钢钢铁料消耗汇总报表
		if (do_flag == "18")
		{
			sqlstr = " SELECT @time_stamps as date_1,@user_id as damin,@start_time as start_time,@end_time end_time,MAT_ACT_WT,DEVO_WT,ZF "
				" ,ROUND(nvl(CASE WHEN DEVO_WT = 0 OR MAT_ACT_WT = 0 THEN 0 ELSE DEVO_WT / MAT_ACT_WT END, 0), 5) * 100 XHL "
				" from "
				" (SELECT MAX(TAP_END_TIME) TAP_END_TIME,SUM(DEVO_WT) DEVO_WT"
				" FROM TMMSMZXHBB   "
				" WHERE 1=1"
				" and mat_code in (select mat_code from tmmsm50 where mat_type in ('1','4'))"
				" and substr(st_no,1,1) in ('2','3','5')  "
				" and user_id = @user_id"
				" and time_stamps = @time_stamps"
				" ) zxh"
				" , ("
				//合格量
				" select sum(mat_act_wt) mat_act_wt from vmmsmcpcl_bb1 where 1=1   "
				" and substr(st_no,1,1) in ('2','3','5')  "
				+ sqlstr_temp +					
				" ) BB1  "
				",("
				" select sum(ZF) ZF"
				" from "
				" (select sum(mat_act_wt) ZF  from hmmsm01 t3 where t3.complex_decide_code = '9'  and heat_no like 'A%' " + sqlstr_temp + 				
				" union all"
				" select sum(CUT_SCRAP_WT) ZF from tmmsmfp t3 where  1=1 and heat_no like 'A%' " + sqlstr_temp +
				")) fg"
				;
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("end_time", tmmsmzxhbb_lh["END_TIME"].ToString());
			cmd_inq.Parameters.Set("start_time", tmmsmzxhbb_lh["START_TIME"].ToString());
			cmd_inq.Parameters.Set("user_id", tmmsmzxhbb_lh["USER_ID"].ToString());
			cmd_inq.Parameters.Set("time_stamps", tmmsmzxhbb_lh["TIME_STAMPS"].ToString());
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
