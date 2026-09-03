/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 总消耗查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_getprice(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_zxh01(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString backlog_code = "";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString start_time = " ";
	CString end_time_1 = " ";
	CString end_time = " ";
	CModel tmmsmzxhbb("TMMSMZXHBB");
	CDecimal cd_count = 0;
	CDecimal devo_wt = 0;
	CDecimal devo_wt1 = 0;
	CString biaoji = "0";
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	int i_idx = 0;
	int i_count = 0;

	CString sqlstr = "";

	CString stat_date = " ";

	CString origin_sys_code = "";
	CString heat_no = "";
	CDecimal prod_out_wt = 0;

	CDecimal ni_wt = 0; // 成分*同炉同物料汇总重量Ni
	CDecimal cr_wt = 0; // 成分*同炉同物料汇总重量Cr
	CDecimal ni_wt1 = 0; // ni平均数据
	CDecimal cr_wt1 = 0; // Cr平均数据
	CDecimal ni_xs = 0; // Ni系数
	CDecimal cr_xs = 0; // Cr系数
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CDbCommand cmd_inq3(conn);
	CModel tmmsmzxh01("TMMSMZXH01");
	CString datetime = CDateTime::Now().AddHours(-3).ToString("yyyyMMddHHmmss");
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass EItables;

	try
	{
		stat_date = datetime.SubstringNE(0, 6);
		if (bcls_rec->Tables[0].Columns.Contains("STAT_DATE"))	
		{
			stat_date = bcls_rec->Tables[0].Rows[0]["STAT_DATE"].ToString().SubstringNE(0, 6);
		}
		Log::Trace("", __FUNCTION__, "stat_date = [{0}]", stat_date);


		//插入产量
		sqlstr = " delete from TQMTSCB02_MX t1"
			" where 1=1"
			" and  DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//碳钢和不锈钢回收分开
		sqlstr = " insert into tqmtscb02_mx(REC_CREATOR,REC_CREATE_TIME,DATE_C,AOD_BOF_E_DTIME,CAST_SEQ,TD_NO_1,CAST_DIV_NO_1,CC_NO,HEATNR,TS_SHIFTNO,F_ROUTE1,GRADE_ID,steel_wt,ORIGIN_SYS_CODE,MAT_ACT_WT,RECEIVE_WEIGHT,XF_WT,CUT_SCRAP_WT)"
			" select @rec_creator, @rec_create_time,stat_date"
			//", nvl((select substr(TO_CHAR(TO_DATE(end_time,'YYYY-MM-DD HH24:MI:SS')-12/24,'YYYYMMDDHH24MISS'),1,8) from tmmsm27 where heat_no like 'A%' and heat_no = t1.heat_no union select substr(TO_CHAR(TO_DATE(end_time,'YYYY-MM-DD HH24:MI:SS')-12/24,'YYYYMMDDHH24MISS'),1,8) from tmmsm21 where heat_no like 'B%' and heat_no = t1.heat_no), ' ')"
			", nvl((select substr(end_time,1,8) from tmmsm27 where heat_no like 'A%' and heat_no = t1.heat_no union select substr(end_time,1,8) from tmmsm21 where heat_no like 'B%' and heat_no = t1.heat_no), ' ')"
			",t1.CAST_DIV_NO,t1.TD_NO_1,t1.CAST_DIV_NO_1,t1.CC_NO,t1.HEAT_NO"
			" ,nvl((select PROD_SHIFT_GROUP from tmmsm27 where heat_no like 'A%' and heat_no=t1.heat_no union select PROD_SHIFT_GROUP from tmmsm21 where heat_no like 'B%' and heat_no=t1.heat_no),' ')"
			",case when substr(t1.st_no,1,1) in ('1','4') then case when INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'B')>0 THEN 	CASE WHEN INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'F')>0  then 'IF+BOF' ELSE 'BOF'  END"
			" when  INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'D')>0 THEN   '三脱'"
			" when  INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'E')>0 THEN 	CASE WHEN INSTR(t1.heatno_premelt1 || t1.heatno_premelt2 || t1.heatno_premelt3, 'F')>0  then 'EAF+IF' ELSE 'EAF'   END "
			" when  (select count(1) from tpssm35 where RET_HEAT_NO=t1.heat_no)>0  THEN   '回炉钢' "
			" else  '高硅' end else ' ' end  as F_ROUTE1"
			",t1.st_no"
			",nvl(t2.LADLE_ARRIVE_WT-t2.LADLE_LEAVE_WT,0),'CCM'"
			",nvl((select sum(mat_act_wt) from VMMSMCPCL_BB1 t3 where t3.heat_no=t1.heat_no ),0)"
			",nvl(( select sum(RECEIVE_WEIGHT) from VMMSMCPCL_BB1 t3 where t3.heat_no=t1.heat_no ),0)"
			",nvl((select sum(mat_act_wt) from hmmsm01 t3 where t3.complex_decide_code = '9' and t3.heat_no=t1.heat_no ),0)"
			",nvl((select sum(CUT_SCRAP_WT) from tmmsmfp t3 where  t3.heat_no=t1.heat_no ),0)"
			" from tmmsmgy05 t1"
			" left join tmmsm31 t2 on t1.heat_no=t2.heat_no"
			" where 1=1"
			" and nvl(t2.LADLE_ARRIVE_TIME,' ')!=' '"
			" and  t1.stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 

		//更细产量和最大合格量的钢种
		//更新合格量	,炉次钢种 	
		sqlstr = " update tqmtscb02_mx t1 set GRADE_ID = (select st_no from (select st_no,sum(mat_act_wt) mat_act_wt from  VMMSMCPCL_BB1 t2 where t1.HEATNR=t2.heat_no  group by st_no order by  mat_act_wt desc) where 1=1 and rownum=1)"
			" where 1=1"
			" and exists (select 1 from VMMSMCPCL_BB1 t2 where t1.HEATNR=t2.heat_no)"
			" and  DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新大类
		sqlstr = " update tqmtscb02_mx t1 set GRADE_TYPE1 = (select  nvl(CODE_DESC_1_CONTENT,' ') from TQMTS0X t2  left join TEP0002 t3 on  t2.LABEL9 = t3.code  and CODE_CLASS='QMIP' where t1.GRADE_ID=t2.st_no )"
			" where 1=1"
			" and exists (select 1 from TQMTS0X t2 where t1.GRADE_ID=t2.st_no)"
			" and  DATE_C in (@stat_date)"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新大类
		sqlstr = " update tqmtscb02_mx t1 set GRADE_TYPE1 = (select GRADE_TYPE3 from tqmtscb09_dr t2 where t1.GRADE_ID=t2.STEEL_GRADE )"
			" where 1=1"
			" and exists (select 1 from tqmtscb09_dr t2 where t1.GRADE_ID=t2.STEEL_GRADE)"
			" and  DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新钢水量
		//更新钢水量	1、一级 2、连铸 3、AOD  4、连铸前最后一个精炼
		sqlstr = " update tqmtscb02_mx t1 set steel_wt = (select STEEL_NET_WEIGHT from VW_CCM_STEEL_WT t2 where t1.HEATNR=t2.HEAT_NAME and rownum=1) ,ORIGIN_SYS_CODE='L1'"
			" where 1=1"
			" and exists (select 1 from VW_CCM_STEEL_WT t2 where t1.HEATNR=t2.HEAT_NAME)"
			" and  DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();  		

		sqlstr = " update tqmtscb02_mx t1 set steel_wt = (select ACTRESULT from tmmsm27 t2 where t1.HEATNR=t2.l2_proc_no) ,ORIGIN_SYS_CODE='AOD'"
			" where 1=1"
			" and steel_wt <= 0 "
			" and HEATNR like 'A%'"
			" and exists (select 1 from tmmsm27 t2 where t1.HEATNR=t2.l2_proc_no)"
			" and  DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		//20250528addbegin
		sqlstr = " update tqmtscb02_mx t1 set lts_steel_wt = (SELECT STEEL_WT FROM TMMSM26 t2 where t1.HEATNR=t2.l2_proc_no  and rownum=1) "
			" where 1=1"
			" and exists (select 1 from TMMSM26 t2 where t1.HEATNR=t2.l2_proc_no)"
			" and  DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		//20250525addend
		//如果是转炉而且没有钢水的按消耗量按连铸前一个个去找
		sqlstr = " select distinct HEATNR"
			" from tqmtscb02_mx"
			" where 1=1"
			" and steel_wt <= 0 "
			" and  DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			heat_no = cmd_inq.GetString(1);
			prod_out_wt = 0;
			origin_sys_code = " ";
			sqlstr = "select dev_code,l2_proc_no"
				" from tmmsmgy06"
				" where 1=1"
				" and dev_code not like 'C%'"
				" and heat_no = @heat_no"
				" order by START_TIME desc"
				;
			cmd_inq1.SetCommandText(sqlstr);
			cmd_inq1.Parameters.Set("heat_no", heat_no);
			cmd_inq1.ExecuteReader();
			while (cmd_inq1.Read())
			{

				if (cmd_inq1.GetString(1).SubstringNE(0, 1) == "F") // LF
				{
					sqlstr = " SELECT MOLTIRON_WT FROM TMMSM24 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "LF";
					}
					cmd_inq2.Close();
				}
				else if (cmd_inq1.GetString(1).SubstringNE(0, 1) == "S") // LTS
				{
					sqlstr = " SELECT STEEL_WT FROM TMMSM26 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "LTS";
					}
					cmd_inq2.Close();

				}
				else if (cmd_inq1.GetString(1).SubstringNE(0, 1) == "V") // VOD
				{
					sqlstr = " SELECT ACTRESULT FROM TMMSM25 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "VOD";
					}
					cmd_inq2.Close();

				}
				else if (cmd_inq1.GetString(1).SubstringNE(0, 1) == "R") // RH
				{
					sqlstr = " SELECT ACTRESULT FROM TMMSM23 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "RH";
					}
					cmd_inq2.Close();
				}
				else // 都没有取转炉重量
				{
					sqlstr = " SELECT ACTRESULT FROM TMMSM21 WHERE l2_proc_no = @heat_no";
					cmd_inq2.Parameters.Set("heat_no", heat_no);
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						prod_out_wt = cmd_inq2.GetDecimal(1);
						origin_sys_code = "BOF";
					}
					cmd_inq2.Close();
				}

				if (prod_out_wt > 0)
				{
					break;
				}
			}
			cmd_inq1.Close();

			//更新值
			if (prod_out_wt > 0)
			{
				sqlstr = " update tqmtscb02_mx t1 set steel_wt =@prod_out_wt ,ORIGIN_SYS_CODE=@origin_sys_code"
					" where 1=1"
					" and HEATNR = @heat_no"
					" and  DATE_C = @stat_date"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("heat_no", heat_no);
				cmd_inq1.Parameters.Set("stat_date", stat_date);
				cmd_inq1.Parameters.Set("prod_out_wt", prod_out_wt);
				cmd_inq1.Parameters.Set("origin_sys_code", origin_sys_code);
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();
			}
		}
		cmd_inq.Close();


		//更新总的预熔液
		sqlstr = " update tqmtscb02_mx t1 set ACTRESULT = (select EAF_WEIGHT/1000 from tmmsm27 t2 where t1.HEATNR=t2.heat_no)"
			" where exists(select 1 from tmmsm27 t2 where t1.HEATNR=t2.heat_no)"
			" and  t1.DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新总的预熔液
		sqlstr = " update tqmtscb02_mx t1 set ACTRESULT = (select ACTRESULT from tmmsm21 t2 where t1.HEATNR=t2.l2_proc_no)"
			" where exists(select 1 from tmmsm21 t2 where t1.HEATNR=t2.l2_proc_no)"
			" and  t1.DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//折算产量 1、更新折算系数
		sqlstr = " select CAST_SEQ,TD_NO_1,CAST_DIV_NO_1,sum(steel_wt),sum(mat_act_wt),sum(xf_wt+CUT_SCRAP_WT)"
			" from tqmtscb02_mx"
			" where 1=1"
			" and DATE_C = @stat_date"
			" group by CAST_SEQ,TD_NO_1,CAST_DIV_NO_1"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			if (cmd_inq.GetDecimal(4) != 0)
			{
				sqlstr = " update tqmtscb02_mx set CONVERSION_OK_WT =round(steel_wt*@mat_wt/@steel_wt,3) ,CONVERSION_ALLOY_WT = round(steel_wt*@cut_scrap_wt/@steel_wt,3)"
					" where 1=1"
					" and cast_seq = @cast_seq"
					" and td_no_1 = @td_no_1"
					" and cast_div_no_1 = @cast_div_no_1"
					" and DATE_C = @stat_date"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("stat_date", stat_date);
				cmd_inq1.Parameters.Set("cast_seq", cmd_inq.GetString(1));
				cmd_inq1.Parameters.Set("td_no_1", cmd_inq.GetString(2));
				cmd_inq1.Parameters.Set("cast_div_no_1", cmd_inq.GetString(3));
				cmd_inq1.Parameters.Set("steel_wt", cmd_inq.GetDecimal(4));
				cmd_inq1.Parameters.Set("mat_wt", cmd_inq.GetDecimal(5));
				cmd_inq1.Parameters.Set("cut_scrap_wt", cmd_inq.GetDecimal(6));
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();
			}
		}
		cmd_inq.Close();


		//按月份进行删除
		sqlstr = " delete from  tmmsmzxh01 where  stat_date = @stat_date ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//插入消耗
		sqlstr = " insert into tmmsmzxh01 (REC_CREATOR,REC_CREATE_TIME,STAT_DATE,HEAT_NO,L2_PROC_NO,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO,WEIGH_NO,QUALITY_BATCH_NO,ST_NO,DEVO_WT,TYPE_DESC,SG_SMALL_CLASS_DESC)"
			" select @rec_creator,@rec_create_time,STAT_DATE,HEAT_NO,L2_PROC_NO,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO,WEIGH_NO,QUALITY_BATCH_NO,ST_NO,sum(OUT_STOCK_WT/1000),'计量单成分'"
			",CASE WHEN SUBSTR(ST_NO, 0, 2) in('1A', '1D') THEN '镍钢' "
			"	WHEN SUBSTR(ST_NO, 0, 2) in ( '1M' ,'1F') THEN '铬钢' "
			"	WHEN SUBSTR(ST_NO, 0, 1) = '1' and SUBSTR(ST_NO, 0, 2) not in ( '1A' ,'1D','1M' ,'1F') THEN '不锈钢'"
			" else '碳钢' end"
			" from tmmsm56 t1" 			
			" where   1=1" 			
			" and mat_code IN (SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG != '1') "
			" and stat_date = @stat_date "
			" group by STAT_DATE,HEAT_NO,L2_PROC_NO,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO,WEIGH_NO,QUALITY_BATCH_NO,ST_NO"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 
		

		//插入分摊20250120取消原注掉in（）
		sqlstr = " insert into tmmsmzxh01 (REC_CREATOR,REC_CREATE_TIME,STAT_DATE,HEAT_NO,L2_PROC_NO,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO,WEIGH_NO,QUALITY_BATCH_NO,ST_NO,DEVO_WT,TYPE_DESC,SG_SMALL_CLASS_DESC,HANDLE_DIV)"
			" select @rec_creator,@rec_create_time,STAT_DATE,HEAT_NO,L2_PROC_NO,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO,WEIGH_NO,QUALITY_BATCH_NO,ST_NO,sum(DEVO_WT/1000),'计量单成分'"
			",CASE WHEN SUBSTR(ST_NO, 0, 2) in('1A', '1D') THEN '镍钢' "
			"	WHEN SUBSTR(ST_NO, 0, 2) in ( '1M' ,'1F') THEN '铬钢' "
			"	WHEN SUBSTR(ST_NO, 0, 1) = '1' and SUBSTR(ST_NO, 0, 2) not in ( '1A' ,'1D','1M' ,'1F') THEN '不锈钢'"
			" else '碳钢' end"
			",'F'"
			" from tmmsm2a_send t1"
			" where   1=1"
			" and mat_code IN (SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG != '1') "
			" and RTN_FLAG != '1' and SEND_FLAG = '1'"
			" and HANDLE_DIV = 'F'"
			" and stat_date = @stat_date "
			" group by STAT_DATE,HEAT_NO,L2_PROC_NO,DEV_CODE,MAT_CODE,MAT_NAME,LOT_NO,WEIGH_NO,QUALITY_BATCH_NO,ST_NO"
			" having sum(DEVO_WT/1000)!=0"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		//更新合格量	,炉次钢种
		sqlstr = " update tmmsmzxh01 t1 set (mat_act_wt,st_no_plan,PROD_OUT_WT,ORIGIN_SYS_CODE) = (select max(mat_act_wt),max(GRADE_ID),max(steel_wt),max(ORIGIN_SYS_CODE) from tqmtscb02_mx t2 where t1.heat_no=t2.HEATNR)"
			" where 1=1"
			" and exists (select 1 from tqmtscb02_mx t2 where t1.heat_no=t2.HEATNR)"
			" and  stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		//更新连铸炉次班组
		sqlstr = " update tmmsmzxh01 t1 set (TAP_END_TIME,SHIFT_GROUP,SHIFT_NO) = (select LADLE_ARRIVE_TIME,PROD_SHIFT_GROUP,PROD_SHIFT_NO from tmmsm31 t2 where t1.heat_no=t2.heat_no)"
			" where 1=1"
			" and exists (select 1 from tmmsm31 t2 where t1.heat_no=t2.heat_no)"
			" and  stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 

		//更新工艺路径
		sqlstr = " update tmmsmzxh01 t1 set PROCESS_ROUTE = (SELECT  LISTAGG(DISTINCT DEV_CODE,'/') WITHIN GROUP (ORDER BY START_TIME) FROM TMMSMGY06  t2 where t2.HANDLE_DIV=' ' and t1.heat_no=t2.heat_no)"
			" where 1=1"
			" and exists (select 1 from TMMSMGY06 t2 where t1.heat_no=t2.heat_no)"
			" and  stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新原料类型
		sqlstr = " update tmmsmzxh01 t1 set TYPE_CODE1 = "
			" (SELECT MAT_TYPE_DESC  FROM tqmtscb08_dr t2  where t1.MAT_CODE=t2.MAT_CODE_DR) "
			" where 1=1"
			" and exists (select 1 from tqmtscb08_dr t2 where t1.MAT_CODE=t2.MAT_CODE_DR)"
			" and  stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		
		//更新钢种
		sqlstr = " update tmmsmzxh01 t1 set  (ST_NO_DESC,ST_NO_BIG_CLASS) = (select SG_GRADE_1,ST_NO_BIG_CLASS from TQMTS0X t2 where t1.ST_NO=t2.ST_NO)"
			" where 1=1"
			" and exists (select 1 from TQMTS0X t2 where t1.ST_NO=t2.ST_NO)"
			" and  stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();   

		//更新成分 优先级：
		//熔清成分 TMMSMWQ
		//固定表成分1		
		//复验补录 		
	    //计量单成分
		//该物料最近成分 

		//固定表（ZJ_MAT_ELEMENT类别：1.按表内成分填写，2用成分乘以表中系数，3只用表内Cr成分，4只用表中Ni成分）

		//熔清
		//sqlstr = " update tmmsmzxh01 t1 set  (QUALITY_BATCH_NO,CR_VALUE,NI_VALUE,MO_VALUE,C_VALUE,SI_VALUE,S_VALUE,MN_VALUE) = (select LOT_NO,round(nvl(CR_VALUE,0)*nvl((select INCLUDE_CR from TQMTSCB11_DR where mat_code = t1.MAT_CODE),0),4),round(nvl(NI_VALUE,0)*nvl((select INCLUDE_NI from TQMTSCB11_DR where mat_code = t1.MAT_CODE),0),4),round(nvl(MO_VALUE,0)*nvl((select INCLUDE_mo from TQMTSCB11_DR where mat_code = t1.MAT_CODE),0),4),nvl(C_VALUE,0),nvl(SI_VALUE,0),nvl(S_VALUE,0),nvl(MN_VALUE,0) from TMMSMWQ t2 where t1.LOT_NO=t2.LOT_NO) ,TYPE_DESC='熔清成分'"
			//" where 1=1"
			//" and TYPE_DESC ='计量单成分'"
			//" and exists (select 1 from TMMSMWQ t2 where t1.LOT_NO=t2.LOT_NO)"
			//" and  stat_date = @stat_date"
			//;
		//熔清20251031w
		sqlstr = " update tmmsmzxh01 t1 set  (QUALITY_BATCH_NO,CR_VALUE,NI_VALUE,MO_VALUE,C_VALUE,SI_VALUE,S_VALUE,MN_VALUE) =(select LOT_NO,round(nvl(CR_VALUE,0)*nvl((select INCLUDE_CR from TQMTSCB11_DR where mat_code =t1.MAT_CODE),0),4),round(nvl(NI_VALUE,0)*nvl((select INCLUDE_NI from TQMTSCB11_DR where mat_code = t1.MAT_CODE),0),4),round(nvl(MO_VALUE,0)*nvl((select INCLUDE_mo from TQMTSCB11_DR where mat_code = t1.MAT_CODE),0),4),C_VALUE,SI_VALUE,S_VALUE,MN_VALUE from (SELECT LOT_NO,nvl(CR_VALUE, 0) as CR_VALUE,nvl(NI_VALUE, 0) as NI_VALUE,nvl(MO_VALUE, 0) as MO_VALUE,nvl(C_VALUE,0) as C_VALUE,nvl(SI_VALUE,0) as SI_VALUE,nvl(S_VALUE,0) AS S_VALUE,nvl(MN_VALUE,0) AS MN_VALUE,row_number() over (partition by LOT_NO order by REC_CREATE_TIME desc) as rn FROM TMMSMWQ t2 WHERE t1.LOT_NO = t2.LOT_NO) WHERE rn = 1) ,TYPE_DESC='熔清成分'"
			" where 1=1"
			" and TYPE_DESC ='计量单成分'"
			" and exists (select 1 from TMMSMWQ t2 where t1.LOT_NO=t2.LOT_NO)"
			" and  stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsmzxh01 t1 set (QUALITY_BATCH_NO,CR_VALUE,NI_VALUE,MO_VALUE) = (select 'ZJ_MAT_ELEMENT',CR,NI,MO from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID  and t2.TYPE='1') ,TYPE_DESC='固定类型1'"
			" where 1=1"
			" and TYPE_DESC ='计量单成分'"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='1')"
			" and  stat_date = @stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		

		//更新复验	
		sqlstr = " update tmmsmzxh01 t1 set QUALITY_BATCH_NO= (SELECT  max(QUALITY_BATCH_NO) FROM TMMSM81AH t2 WHERE t1.MAT_CODE=t2.MAT_CODE and  t1.LOT_NO=t2.LOT_NO AND REMARK_1 = 'F')"
		" ,TYPE_DESC='复验成分'"
		" where 1=1"
		" and TYPE_DESC ='计量单成分'"
		" and exists (select 1 from  TMMSM81AH t2 where  t1.MAT_CODE=t2.MAT_CODE and  t1.LOT_NO=t2.LOT_NO AND REMARK_1 = 'F')"
		" and  stat_date = @stat_date"
		;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 

		sqlstr = " update tmmsmzxh01 t1 set QUALITY_BATCH_NO = (SELECT MAX(QUALITY_BATCH_NO) FROM tmmsm81ah"
            " WHERE REC_CREATE_TIME in (select max(REC_CREATE_TIME) from tmmsm81ah t2 where  t2.MAT_CODE=t1.MAT_CODE )) ,TYPE_DESC='最近质检批'"
            " where 1=1"
            " and QUALITY_BATCH_NO =' ' "
            " and exists (select 1 from tmmsm81ah t2 where t1.MAT_CODE=t2.MAT_CODE) "
			" and  stat_date = @stat_date"
         	;
			Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		//更新物料最近成分
		sqlstr = " update tmmsmzxh01 t1 set (QUALITY_BATCH_NO,CR_VALUE,NI_VALUE,MO_VALUE,C_VALUE,SI_VALUE,S_VALUE,MN_VALUE)= (SELECT QUALITY_BATCH_NO ,max(nvl(Cr,0)),max(nvl(Ni,0)), max(nvl(Mo,0)), max(nvl(C,0)), max(nvl(Si,0)), max(nvl(S,0)), max(nvl(Mn,0))  FROM ("
			" SELECT  * FROM ( SELECT QUALITY_BATCH_NO ,ELM_VALUE,ELM_NAME FROM  TMMSM81AL )"
			" PIVOT ( SUM(ELM_VALUE) FOR ELM_NAME IN ( 'Cr' AS Cr ,'Ni' AS Ni, 'Mo' AS Mo, 'C' AS C , 'S' AS S, 'Si' AS Si, 'Mn' AS Mn))) t2  where t1.QUALITY_BATCH_NO=t2.QUALITY_BATCH_NO GROUP BY  QUALITY_BATCH_NO)"
			" where 1=1"
			" and TYPE_DESC not in ('固定类型1','熔清成分') "
			" and exists (select 1 from  TMMSM81AL t2 where t1.QUALITY_BATCH_NO=t2.QUALITY_BATCH_NO)"
			" and  stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 

		sqlstr = " update tmmsmzxh01 t1 set (CR_VALUE) = (select CR from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID) ,TYPE_DESC='固定类型3'"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='3')"
			" and  stat_date = @stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsmzxh01 t1 set (NI_VALUE) = (select NI from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID) ,TYPE_DESC='固定类型4'"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='4')"
			" and  stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " update tmmsmzxh01 t1 set (MO_VALUE) = (select MO from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID) ,TYPE_DESC='固定类型5'"
			" where 1=1"
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='5')"
			" and  stat_date = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		sqlstr = " update tmmsmzxh01 t1 set (CR_VALUE,NI_VALUE) = (select CR*t1.CR_VALUE,NI*t1.NI_VALUE from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID) ,TYPE_DESC='固定类型2'"
			" where 1=1"  			
			" and exists (select 1 from ZJ_MAT_ELEMENT t2 where t1.MAT_CODE=t2.MAT_ID and t2.TYPE='2')"
			" and  stat_date = @stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		//更新物料价格
		EIClass bcls_ret3;
		EIClass bcls_rec3;
		bcls_rec3.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");
		bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "CR_VALUE");
		bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "NI_VALUE");
		bcls_rec3.Tables[0].Columns.Add(DT_DECIMAL, "MO_VALUE");
		bcls_rec3.Tables[0].Rows.Add();

		sqlstr = " select mat_code,CR_VALUE,NI_VALUE,MO_VALUE"
			" from tmmsmzxh01"
			" where 1=1"
			" and  stat_date = @stat_date"
			" group by mat_code,CR_VALUE,NI_VALUE,MO_VALUE"
			;

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			bcls_rec3.Tables[0].Rows[0]["MAT_CODE"] = cmd_inq.GetString(1);
			bcls_rec3.Tables[0].Rows[0]["CR_VALUE"] = cmd_inq.GetDecimal(2);
			bcls_rec3.Tables[0].Rows[0]["NI_VALUE"] = cmd_inq.GetDecimal(3);
			bcls_rec3.Tables[0].Rows[0]["MO_VALUE"] = cmd_inq.GetDecimal(4);
			doFlag = f_mmsm_getprice(&bcls_rec3, &bcls_ret3, conn);	
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_getprice失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				sqlstr = " update tmmsmzxh01 set  unit_price = @unit_price"
					" ,cost_hj = Round(devo_wt* @unit_price,2)"
					",unit_price_cr = @unit_price_cr"
					",unit_price_ni = @unit_price_ni"
					" where 1=1"
					" and CR_VALUE =@cr_value"
					" and NI_VALUE =@ni_value"
					" and MO_VALUE =@mo_value"
					" and mat_code = @mat_code"
					" and  stat_date = @stat_date"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("stat_date", stat_date);
				cmd_inq1.Parameters.Set("cr_value", cmd_inq.GetDecimal(2));
				cmd_inq1.Parameters.Set("ni_value", cmd_inq.GetDecimal(3));
				cmd_inq1.Parameters.Set("mo_value", cmd_inq.GetDecimal(4));
				cmd_inq1.Parameters.Set("mat_code", cmd_inq.GetString(1));
				cmd_inq1.Parameters.Set("unit_price", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE"].ToDecimal());
				cmd_inq1.Parameters.Set("unit_price_cr", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_CR"].ToDecimal());
				cmd_inq1.Parameters.Set("unit_price_ni", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE_NI"].ToDecimal());
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();
			}
		}
		cmd_inq.Close();

		sqlstr = " delete from tqmtscb04_fx"
			" where DATE_C =@stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//插入标准的单价值
		sqlstr = " insert into tqmtscb04_fx(REC_CREATOR,REC_CREATE_TIME,DATE_C,GRADE_TYPE1,PROCESS_ROUTE,ZB_DESC,WT_UNIT,SEQ_NO,INCLUDE_CR,INCLUDE_NI,INCLUDE_MO,MAT_CODE_DR,MAT_NAME_DR)"
			" select @rec_creator, @rec_create_time,@stat_date,t1.GRADE_TYPE1,t1.F_ROUTE1,t2.ZB_DESC,t2.WT_UNIT,t2.SEQ_NO,INCLUDE_CR,INCLUDE_NI,INCLUDE_MO,MAT_CODE_DR,MAT_NAME_DR"
			" from (select GRADE_TYPE1,F_ROUTE1 from tqmtscb02_mx where 1=1 and  DATE_C = @stat_date group by GRADE_TYPE1,F_ROUTE1) t1"
			" left join tqmtscb03_fx t2 on t1.GRADE_TYPE1=t2.GRADE_TYPE1 and t1.F_ROUTE1=t2.PROCESS_ROUTE and t2.MAT_CODE_DR!=' '"
			" where nvl(t2.GRADE_TYPE1,' ')!=' '"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新标准价格
		sqlstr = " select MAT_CODE_DR,INCLUDE_CR,INCLUDE_NI,INCLUDE_MO"
			" from tqmtscb04_fx"
			" where 1=1"
			" and MAT_CODE_DR!=' '"
			" and  DATE_C = @stat_date"
			" group by MAT_CODE_DR,INCLUDE_CR,INCLUDE_NI,INCLUDE_MO"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			bcls_rec3.Tables[0].Rows[0]["MAT_CODE"] = cmd_inq.GetString(1);
			bcls_rec3.Tables[0].Rows[0]["CR_VALUE"] = cmd_inq.GetDecimal(2);
			bcls_rec3.Tables[0].Rows[0]["NI_VALUE"] = cmd_inq.GetDecimal(3);
			bcls_rec3.Tables[0].Rows[0]["MO_VALUE"] = cmd_inq.GetDecimal(4);
			doFlag = f_mmsm_getprice(&bcls_rec3, &bcls_ret3, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_getprice失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				sqlstr = " update tqmtscb04_fx set  UNIT_PRICE = @unit_price"
					" ,COST_UNIT = Round(WT_UNIT* @unit_price/1000,2)"
					" where 1=1"
					" and INCLUDE_CR =@cr_value"
					" and INCLUDE_NI =@ni_value"
					" and INCLUDE_MO =@mo_value"
					" and MAT_CODE_DR = @mat_code"
					" and  DATE_C = @stat_date"
					;
				cmd_inq1.SetCommandText(sqlstr);
				cmd_inq1.Parameters.Set("stat_date", stat_date);
				cmd_inq1.Parameters.Set("cr_value", cmd_inq.GetDecimal(2));
				cmd_inq1.Parameters.Set("ni_value", cmd_inq.GetDecimal(3));
				cmd_inq1.Parameters.Set("mo_value", cmd_inq.GetDecimal(4));
				cmd_inq1.Parameters.Set("mat_code", cmd_inq.GetString(1));
				cmd_inq1.Parameters.Set("unit_price", bcls_ret3.Tables[0].Rows[0]["UNIT_PRICE"]);
				cmd_inq1.ExecuteNonQuery();
				cmd_inq1.Close();
			}
		}
		cmd_inq.Close();

		

		//先删除
		sqlstr = " delete from TQMTSCB01_MX t1"
			" where DATE_C = @stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tqmtscb01_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,AREA_CODE,STATION_NAME,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,INCLUDE_NI,INCLUDE_CR,INCLUDE_MO,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,UNIT_PRICE_XY,COST_XY,COST_PERT_XY)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR"
			",case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD' else ' ' end" 
			",t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"				
			",t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.mat_act_wt,t1.UNIT_PRICE,sum(devo_wt),round(sum(t1.UNIT_PRICE*devo_wt),2),decode(t2.mat_act_wt,0,0,round(sum(t1.UNIT_PRICE*devo_wt)/t2.mat_act_wt,2))"
			", case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end "
			", round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt), 2)"
			", decode(t2.mat_act_wt, 0, 0, round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt) / t2.mat_act_wt, 2))"
			" from tqmtscb02_mx t2"
			" left join tmmsmzxh01 t1 on t1.HANDLE_DIV!='F' and t1.heat_no=t2.HEATNR"
			" where 1=1"
			" and nvl(devo_wt,0) !=0"
			" and  t2.DATE_C = @stat_date"
			" group by t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR,t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1,t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.mat_act_wt,t1.UNIT_PRICE,t1.UNIT_PRICE_CR,t1.UNIT_PRICE_NI"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 
		
		//插入回收
		sqlstr = " insert into tqmtscb01_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,TYPE)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR"
			",t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1,'回收',t4.MAT_CODE_DR"
			",nvl((select mat_name from tmmsm50 t where t.mat_code = t4.MAT_CODE_DR),'回收废钢')"
			",t2.mat_act_wt,nvl(t4.PRICE,0),0-(t2.CUT_SCRAP_WT+t2.XF_WT),0-round(nvl(t4.PRICE,0)*(t2.CUT_SCRAP_WT+t2.XF_WT),2),decode(t2.mat_act_wt,0,0,0-round(nvl(t4.PRICE,0)*(t2.CUT_SCRAP_WT+t2.XF_WT)/t2.mat_act_wt,2)),'回收'"
			" from  tqmtscb02_mx  t2"			
			" left join "
			"( select t1.ST_NO,t1.KM_CODE as MAT_CODE_DR,tt.UNIT_PRICE as price from tqmtscb05_dr t1  left join "
			" (select KM_CODE,UNIT_PRICE from tqmtscb00_dr where (KM_CODE,date_c) in (select KM_CODE,max(date_c) from tqmtscb00_dr where date_c<=@stat_date group by KM_CODE)) tt "
			" on t1.mat_code_dr = tt.km_code) t4"
			" on  t2.GRADE_ID=t4.ST_NO"
			" where 1=1"
			" and nvl(t4.MAT_CODE_DR,' ')!=' '"
			" and t2.CUT_SCRAP_WT+t2.XF_WT!=0"
			" and t2.DATE_C = @stat_date"
			;

		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();  		

		sqlstr = " insert into tqmtscb01_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,AREA_CODE,STATION_NAME,MAT_CODE_DR,MAT_NAME_DR,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,TYPE)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t1.heat_no"
			",t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",'步骤费',t1.AREA_CODE,t1.DEV_CODE,t3.MAT_CODE,t3.mat_name,t2.mat_act_wt,t3.COST_VALUE,t2.mat_act_wt,round(t3.COST_VALUE*t2.mat_act_wt,2),t3.COST_VALUE,'步骤费'"
			" from   tqmtscb02_mx t2 "
			" left join (select heat_no,dev_code,case when dev_code like 'B%' then 'BOF' when dev_code like 'E%' then 'EAF' when dev_code like 'R%' then 'RH' when dev_code like 'F%' then 'LF'  when dev_code like 'S%' then 'LTS'  when dev_code like 'Z%' then 'IF'  when dev_code like 'V%' then 'VOD'  when dev_code like 'A%' then 'AOD'  when dev_code like 'C%' and heat_no like 'A%' then 'CCM-S'  when dev_code like 'C%' and heat_no like 'B%' then 'CCM-C' else ' ' end AREA_CODE  from tmmsmgy06 tt where tt.HANDLE_DIV = ' ' ) t1  on t1.heat_no = t2.HEATNR"
			" left join (select AREA_CODE,MAT_CODE,'步骤费' as MAT_NAME,COST_VALUE FROM TQMTSCB01_DR WHERE DATE_C IN (SELECT MAX(DATE_C) FROM TQMTSCB01_DR WHERE DATE_C<=@stat_date)) t3 "
			" on t1.AREA_CODE=t3.AREA_CODE"
			" where 1=1"
			" and nvl(t3.MAT_CODE,' ')!=' '"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr1111 = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		


		//插入预熔液步骤费用
		sqlstr_temp = "(SELECT HEAT_NO,DEV_CODE,AREA_CODE,sum(ACTRESULT) ACTRESULT from ( "
			" select t1.HEAT_NO, t1.L2_PROC_NO, t1.DEV_CODE, nvl(ACTRESULT, 0) ACTRESULT"
			" ,case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD'  when t1.dev_code like 'C%' then 'CCM' else ' ' end AREA_CODE "
			" from tmmsmgy06 t1 "
			" LEFT JOIN TMMSM19 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO "
			" where HANDLE_DIV = 'Y' AND t1.L2_PROC_NO like 'F%'  and exists(select 1 from tqmtscb02_mx t3 where t1.heat_no=t3.HEATNR and t3.DATE_C = @stat_date)"
			" union all"
			" select t1.HEAT_NO, t1.L2_PROC_NO, t1.DEV_CODE, nvl(ACTRESULT, 0) ACTRESULT"
			" ,case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD'  when t1.dev_code like 'C%' then 'CCM' else ' ' end AREA_CODE "
			" from tmmsmgy06 t1	"
			" LEFT JOIN TMMSM20 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO	"
			" where HANDLE_DIV = 'Y' AND t1.L2_PROC_NO like 'E%'   and exists(select 1 from tqmtscb02_mx t3 where t1.heat_no=t3.HEATNR and t3.DATE_C = @stat_date)"
			" union all	"
			" select t1.HEAT_NO, t1.L2_PROC_NO, t1.DEV_CODE, nvl(ACTRESULT, 0) ACTRESULT"
			" ,case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD'  when t1.dev_code like 'C%' then 'CCM' else ' ' end AREA_CODE "
			" from tmmsmgy06 t1	"
			" LEFT JOIN TMMSM21 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO	"
			" where HANDLE_DIV = 'Y' AND t1.L2_PROC_NO like 'B%' and exists(select 1 from tqmtscb02_mx t3 where t1.heat_no=t3.HEATNR and t3.DATE_C = @stat_date)"
			" ) group by   HEAT_NO, DEV_CODE,AREA_CODE )  "
			;
		sqlstr = " insert into tqmtscb01_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,AREA_CODE,STATION_NAME,MAT_CODE_DR,MAT_NAME_DR,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,TYPE)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t1.heat_no"
			",t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",'步骤费',t1.AREA_CODE,t1.DEV_CODE,t3.MAT_CODE,t3.mat_name,t2.mat_act_wt,t3.COST_VALUE,decode(all_wt,0,0,round(t2.ACTRESULT*t1.ACTRESULT/all_wt,3)),decode(all_wt,0,0,round(t3.COST_VALUE*t2.ACTRESULT*t1.ACTRESULT/all_wt,2)),case when all_wt!=0 and t2.mat_act_wt!=0 then round(t3.COST_VALUE*t2.ACTRESULT*t1.ACTRESULT/all_wt/t2.mat_act_wt,2) else 0 end,'预熔液步骤费'"
			" from   tqmtscb02_mx t2 "
			" left join"  + sqlstr_temp +  " t1 on t1.heat_no=t2.HEATNR"
			" left join (select AREA_CODE, MAT_CODE,'步骤费' as MAT_NAME,COST_VALUE FROM TQMTSCB01_DR WHERE DATE_C IN (SELECT MAX(DATE_C) FROM TQMTSCB01_DR WHERE DATE_C<=@stat_date)) t3 "
			" on t1.AREA_CODE=t3.AREA_CODE"
			" left join (select heat_no,sum(ACTRESULT) all_wt from " + sqlstr_temp + " group by heat_no) t4 on t4.heat_no =t2.HEATNR " 
			" where 1=1"
			" and t2.RECEIVE_WEIGHT!=0"
			" and nvl(t3.MAT_CODE,' ')!=' '"
			" and t2.ACTRESULT!=0"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 		

		//按折算量算明细消耗
		sqlstr = " delete from TQMTSCB03_MX t1"
			" where  t1.DATE_C = @stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tqmtscb03_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,AREA_CODE,STATION_NAME,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,INCLUDE_NI,INCLUDE_CR,INCLUDE_MO,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,UNIT_PRICE_XY,COST_XY,COST_PERT_XY)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR"
			",case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD' else ' ' end"
			",t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.CONVERSION_OK_WT,t1.UNIT_PRICE,sum(devo_wt),round(sum(t1.UNIT_PRICE*devo_wt),2),decode(t2.CONVERSION_OK_WT,0,0,round(sum(t1.UNIT_PRICE*devo_wt)/t2.CONVERSION_OK_WT,2))"
			", case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end "
			", round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt), 2)"
			", decode(t2.CONVERSION_OK_WT, 0, 0, round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt) / t2.CONVERSION_OK_WT, 2))"
			" from tqmtscb02_mx t2"
			" left join tmmsmzxh01 t1 on t1.HANDLE_DIV!='F' and  t1.heat_no=t2.HEATNR"
			" where 1=1"
			" and nvl(devo_wt,0) !=0"
			" and  t2.DATE_C = @stat_date"
			" group by t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR,t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1,t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.CONVERSION_OK_WT,t1.UNIT_PRICE,t1.UNIT_PRICE_CR,t1.UNIT_PRICE_NI"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//插入回收
		sqlstr = " insert into tqmtscb03_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,TYPE)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR"
			",t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1,'回收',t4.MAT_CODE_DR"
			",nvl((select mat_name from tmmsm50 t where t.mat_code = t4.MAT_CODE_DR),'回收废钢')"
			",t2.CONVERSION_OK_WT,nvl(t4.PRICE,0),0-(t2.CONVERSION_ALLOY_WT),0-round(nvl(t4.PRICE,0)*(t2.CONVERSION_ALLOY_WT),2),decode(t2.CONVERSION_OK_WT,0,0,0-round(nvl(t4.PRICE,0)*(t2.CONVERSION_ALLOY_WT)/t2.CONVERSION_OK_WT,2)),'回收'"
			" from  tqmtscb02_mx  t2"			
			" left join "
			"( select t1.ST_NO,t1.KM_CODE as MAT_CODE_DR,tt.UNIT_PRICE as price from tqmtscb05_dr t1  left join "
			" (select KM_CODE,UNIT_PRICE from tqmtscb00_dr where (KM_CODE,date_c) in (select KM_CODE,max(date_c) from tqmtscb00_dr where date_c<=@stat_date group by KM_CODE)) tt "
			" on t1.mat_code_dr = tt.km_code) t4"
			" on  t2.GRADE_ID=t4.ST_NO"
			" where 1=1" 			
			" and nvl(t4.MAT_CODE_DR,' ')!=' '"
			" and t2.CONVERSION_ALLOY_WT!=0"
			" and t2.DATE_C = @stat_date"
			;

		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tqmtscb03_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,AREA_CODE,STATION_NAME,MAT_CODE_DR,MAT_NAME_DR,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,TYPE)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t1.heat_no"
			",t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",'步骤费',t1.AREA_CODE,t1.DEV_CODE,t3.MAT_CODE,t3.mat_name,t2.CONVERSION_OK_WT,t3.COST_VALUE,t2.CONVERSION_OK_WT,round(t3.COST_VALUE*t2.CONVERSION_OK_WT,2),t3.COST_VALUE,'步骤费'"
			" from   tqmtscb02_mx t2 "
			" left join (select heat_no,dev_code,case when dev_code like 'B%' then 'BOF' when dev_code like 'E%' then 'EAF' when dev_code like 'R%' then 'RH' when dev_code like 'F%' then 'LF'  when dev_code like 'S%' then 'LTS'  when dev_code like 'Z%' then 'IF'  when dev_code like 'V%' then 'VOD'  when dev_code like 'A%' then 'AOD'  when dev_code like 'C%' and heat_no like 'A%' then 'CCM-S' when dev_code like 'C%' and heat_no like 'B%' then 'CCM-C' else ' ' end AREA_CODE  from tmmsmgy06 tt where tt.HANDLE_DIV = ' ' ) t1  on t1.heat_no = t2.HEATNR"
			" left join (select AREA_CODE,MAT_CODE,'步骤费' as MAT_NAME,COST_VALUE FROM TQMTSCB01_DR WHERE DATE_C IN (SELECT MAX(DATE_C) FROM TQMTSCB01_DR WHERE DATE_C<=@stat_date)) t3 "
			" on t1.AREA_CODE=t3.AREA_CODE"
			" where 1=1"
			" and nvl(t3.MAT_CODE,' ')!=' '"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr1111 = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//插入预熔液步骤费用
		sqlstr_temp = "(SELECT HEAT_NO,DEV_CODE,AREA_CODE,sum(ACTRESULT) ACTRESULT from ( "
			" select t1.HEAT_NO, t1.L2_PROC_NO, t1.DEV_CODE, nvl(ACTRESULT, 0) ACTRESULT"
			" ,case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD'  when t1.dev_code like 'C%' then 'CCM' else ' ' end AREA_CODE "
			" from tmmsmgy06 t1 "
			" LEFT JOIN TMMSM19 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO "
			" where HANDLE_DIV = 'Y' AND t1.L2_PROC_NO like 'F%'  and exists(select 1 from tqmtscb02_mx t3 where t1.heat_no=t3.HEATNR and t3.DATE_C = @stat_date)"
			" union all"
			" select t1.HEAT_NO, t1.L2_PROC_NO, t1.DEV_CODE, nvl(ACTRESULT, 0) ACTRESULT"
			" ,case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD'  when t1.dev_code like 'C%' then 'CCM' else ' ' end AREA_CODE "
			" from tmmsmgy06 t1	"
			" LEFT JOIN TMMSM20 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO	"
			" where HANDLE_DIV = 'Y' AND t1.L2_PROC_NO like 'E%'   and exists(select 1 from tqmtscb02_mx t3 where t1.heat_no=t3.HEATNR and t3.DATE_C = @stat_date)"
			" union all	"
			" select t1.HEAT_NO, t1.L2_PROC_NO, t1.DEV_CODE, nvl(ACTRESULT, 0) ACTRESULT"
			" ,case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD'  when t1.dev_code like 'C%' then 'CCM' else ' ' end AREA_CODE "
			" from tmmsmgy06 t1	"
			" LEFT JOIN TMMSM21 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO	"
			" where HANDLE_DIV = 'Y' AND t1.L2_PROC_NO like 'B%' and exists(select 1 from tqmtscb02_mx t3 where t1.heat_no=t3.HEATNR and t3.DATE_C = @stat_date)"
			" ) group by   HEAT_NO, DEV_CODE,AREA_CODE )  "
			;
		sqlstr = " insert into tqmtscb03_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,AREA_CODE,STATION_NAME,MAT_CODE_DR,MAT_NAME_DR,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,TYPE)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t1.heat_no"
			",t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",'步骤费',t1.AREA_CODE,t1.DEV_CODE,t3.MAT_CODE,t3.mat_name,t2.CONVERSION_OK_WT,t3.COST_VALUE,decode(all_wt,0,0,round(t2.ACTRESULT*t1.ACTRESULT/all_wt,3)),decode(all_wt,0,0,round(t3.COST_VALUE*t2.ACTRESULT*t1.ACTRESULT/all_wt,2)),case when all_wt!=0 and t2.CONVERSION_OK_WT!=0 then round(t3.COST_VALUE*t2.ACTRESULT*t1.ACTRESULT/all_wt/t2.CONVERSION_OK_WT,2) else 0 end,'预熔液步骤费'"
			" from   tqmtscb02_mx t2 "
			" left join" + sqlstr_temp + " t1 on t1.heat_no=t2.HEATNR"
			" left join (select AREA_CODE, MAT_CODE,'步骤费' as MAT_NAME,COST_VALUE FROM TQMTSCB01_DR WHERE DATE_C IN (SELECT MAX(DATE_C) FROM TQMTSCB01_DR WHERE DATE_C<=@stat_date)) t3 "
			" on t1.AREA_CODE=t3.AREA_CODE"
			" left join (select heat_no,sum(ACTRESULT) all_wt from " + sqlstr_temp + " group by heat_no) t4 on t4.heat_no =t2.HEATNR "
			" where 1=1"
			" and nvl(t3.MAT_CODE,' ')!=' '"
			" and t2.ACTRESULT!=0"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新回收的铬镍钼成分
		sqlstr = " update tqmtscb01_mx t1 set (INCLUDE_CR,INCLUDE_NI,INCLUDE_MO) = ( SELECT NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0) AT_CR "
			", NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) AT_NI"
			", NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0) AT_MO"
			" from TQMTSB0 t2 where t1.HEATNR = t2.heat_no )"
			" where 1=1"
			" and exists(select 1  from TQMTSB0 t2 where t1.HEATNR = t2.heat_no)"
			" and MAT_TYPE_DESC = '回收'"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//更新回收的铬镍钼成分
		sqlstr = " update tqmtscb03_mx t1 set (INCLUDE_CR,INCLUDE_NI,INCLUDE_MO) = ( SELECT NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0) AT_CR "
			", NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) AT_NI"
			", NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0) AT_MO"
			" from TQMTSB0 t2 where t1.HEATNR = t2.heat_no )"
			" where 1=1"
			" and exists(select 1  from TQMTSB0 t2 where t1.HEATNR = t2.heat_no)"
			" and MAT_TYPE_DESC = '回收'"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();  

		//20250528addbegin
		//按折算量算明细消耗
		sqlstr = " delete from TQMTSCB05_MX t1"
			" where  t1.DATE_C = @stat_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tqmtscb05_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,AREA_CODE,STATION_NAME,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,INCLUDE_NI,INCLUDE_CR,INCLUDE_MO,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,UNIT_PRICE_XY,COST_XY,COST_PERT_XY)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR"
			",case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD' else ' ' end"
			",t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.lts_steel_wt,t1.UNIT_PRICE,sum(devo_wt),round(sum(t1.UNIT_PRICE*devo_wt),2),decode(t2.lts_steel_wt,0,0,round(sum(t1.UNIT_PRICE*devo_wt)/t2.lts_steel_wt,2))"
			", case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end "
			", round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt), 2)"
			", decode(t2.lts_steel_wt, 0, 0, round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt) / t2.lts_steel_wt, 2))"
			" from tqmtscb02_mx t2"
			" left join tmmsmzxh01 t1 on t1.HANDLE_DIV!='F' and  t1.heat_no=t2.HEATNR"
			" where 1=1"
			" and nvl(devo_wt,0) !=0"
			" and  t2.DATE_C = @stat_date"
			" group by t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR,t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1,t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.lts_steel_wt,t1.UNIT_PRICE,t1.UNIT_PRICE_CR,t1.UNIT_PRICE_NI"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//插入回收
		sqlstr = " insert into tqmtscb05_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,TYPE)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR"
			",t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1,'回收',t4.MAT_CODE_DR"
			",nvl((select mat_name from tmmsm50 t where t.mat_code = t4.MAT_CODE_DR),'回收废钢')"
			",t2.lts_steel_wt,nvl(t4.PRICE,0),0-(t2.CONVERSION_ALLOY_WT),0-round(nvl(t4.PRICE,0)*(t2.CONVERSION_ALLOY_WT),2),decode(t2.lts_steel_wt,0,0,0-round(nvl(t4.PRICE,0)*(t2.CONVERSION_ALLOY_WT)/t2.lts_steel_wt,2)),'回收'"
			" from  tqmtscb02_mx  t2"
			" left join "
			"( select t1.ST_NO,t1.KM_CODE as MAT_CODE_DR,tt.UNIT_PRICE as price from tqmtscb05_dr t1  left join "
			" (select KM_CODE,UNIT_PRICE from tqmtscb00_dr where (KM_CODE,date_c) in (select KM_CODE,max(date_c) from tqmtscb00_dr where date_c<=@stat_date group by KM_CODE)) tt "
			" on t1.mat_code_dr = tt.km_code) t4"
			" on  t2.GRADE_ID=t4.ST_NO"
			" where 1=1"
			" and nvl(t4.MAT_CODE_DR,' ')!=' '"
			" and t2.CONVERSION_ALLOY_WT!=0"
			" and t2.DATE_C = @stat_date"
			;

		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tqmtscb05_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,AREA_CODE,STATION_NAME,MAT_CODE_DR,MAT_NAME_DR,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,TYPE)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t1.heat_no"
			",t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",'步骤费',t1.AREA_CODE,t1.DEV_CODE,t3.MAT_CODE,t3.mat_name,t2.lts_steel_wt,t3.COST_VALUE,t2.lts_steel_wt,round(t3.COST_VALUE*t2.lts_steel_wt,2),t3.COST_VALUE,'步骤费'"
			" from   tqmtscb02_mx t2 "
			" left join (select heat_no,dev_code,case when dev_code like 'B%' then 'BOF' when dev_code like 'E%' then 'EAF' when dev_code like 'R%' then 'RH' when dev_code like 'F%' then 'LF'  when dev_code like 'S%' then 'LTS'  when dev_code like 'Z%' then 'IF'  when dev_code like 'V%' then 'VOD'  when dev_code like 'A%' then 'AOD'  when dev_code like 'C%' and heat_no like 'A%' then 'CCM-S' when dev_code like 'C%' and heat_no like 'B%' then 'CCM-C' else ' ' end AREA_CODE  from tmmsmgy06 tt where tt.HANDLE_DIV = ' ' ) t1  on t1.heat_no = t2.HEATNR"
			" left join (select AREA_CODE,MAT_CODE,'步骤费' as MAT_NAME,COST_VALUE FROM TQMTSCB01_DR WHERE DATE_C IN (SELECT MAX(DATE_C) FROM TQMTSCB01_DR WHERE DATE_C<=@stat_date)) t3 "
			" on t1.AREA_CODE=t3.AREA_CODE"
			" where 1=1"
			" and nvl(t3.MAT_CODE,' ')!=' '"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr1111 = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//插入预熔液步骤费用
		sqlstr_temp = "(SELECT HEAT_NO,DEV_CODE,AREA_CODE,sum(ACTRESULT) ACTRESULT from ( "
			" select t1.HEAT_NO, t1.L2_PROC_NO, t1.DEV_CODE, nvl(ACTRESULT, 0) ACTRESULT"
			" ,case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD'  when t1.dev_code like 'C%' then 'CCM' else ' ' end AREA_CODE "
			" from tmmsmgy06 t1 "
			" LEFT JOIN TMMSM19 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO "
			" where HANDLE_DIV = 'Y' AND t1.L2_PROC_NO like 'F%'  and exists(select 1 from tqmtscb02_mx t3 where t1.heat_no=t3.HEATNR and t3.DATE_C = @stat_date)"
			" union all"
			" select t1.HEAT_NO, t1.L2_PROC_NO, t1.DEV_CODE, nvl(ACTRESULT, 0) ACTRESULT"
			" ,case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD'  when t1.dev_code like 'C%' then 'CCM' else ' ' end AREA_CODE "
			" from tmmsmgy06 t1	"
			" LEFT JOIN TMMSM20 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO	"
			" where HANDLE_DIV = 'Y' AND t1.L2_PROC_NO like 'E%'   and exists(select 1 from tqmtscb02_mx t3 where t1.heat_no=t3.HEATNR and t3.DATE_C = @stat_date)"
			" union all	"
			" select t1.HEAT_NO, t1.L2_PROC_NO, t1.DEV_CODE, nvl(ACTRESULT, 0) ACTRESULT"
			" ,case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD'  when t1.dev_code like 'C%' then 'CCM' else ' ' end AREA_CODE "
			" from tmmsmgy06 t1	"
			" LEFT JOIN TMMSM21 t2 on t1.L2_PROC_NO = t2.L2_PROC_NO	"
			" where HANDLE_DIV = 'Y' AND t1.L2_PROC_NO like 'B%' and exists(select 1 from tqmtscb02_mx t3 where t1.heat_no=t3.HEATNR and t3.DATE_C = @stat_date)"
			" ) group by   HEAT_NO, DEV_CODE,AREA_CODE )  "
			;
		sqlstr = " insert into tqmtscb05_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,AREA_CODE,STATION_NAME,MAT_CODE_DR,MAT_NAME_DR,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,TYPE)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t1.heat_no"
			",t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",'步骤费',t1.AREA_CODE,t1.DEV_CODE,t3.MAT_CODE,t3.mat_name,t2.lts_steel_wt,t3.COST_VALUE,decode(all_wt,0,0,round(t2.ACTRESULT*t1.ACTRESULT/all_wt,3)),decode(all_wt,0,0,round(t3.COST_VALUE*t2.ACTRESULT*t1.ACTRESULT/all_wt,2)),case when all_wt!=0 and t2.lts_steel_wt!=0 then round(t3.COST_VALUE*t2.ACTRESULT*t1.ACTRESULT/all_wt/t2.lts_steel_wt,2) else 0 end,'预熔液步骤费'"
			" from   tqmtscb02_mx t2 "
			" left join" + sqlstr_temp + " t1 on t1.heat_no=t2.HEATNR"
			" left join (select AREA_CODE, MAT_CODE,'步骤费' as MAT_NAME,COST_VALUE FROM TQMTSCB01_DR WHERE DATE_C IN (SELECT MAX(DATE_C) FROM TQMTSCB01_DR WHERE DATE_C<=@stat_date)) t3 "
			" on t1.AREA_CODE=t3.AREA_CODE"
			" left join (select heat_no,sum(ACTRESULT) all_wt from " + sqlstr_temp + " group by heat_no) t4 on t4.heat_no =t2.HEATNR "
			" where 1=1"
			" and nvl(t3.MAT_CODE,' ')!=' '"
			" and t2.ACTRESULT!=0"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		//20250528addend
		sqlstr = " DELETE FROM tqmtscb01a_mx"
			" where 1=1"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " DELETE FROM tqmtscb03a_mx"
			" where 1=1"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tqmtscb03a_mx(REC_CREATOR, REC_CREATE_TIME, AOD_BOF_E_DTIME, DATE_C, CAST_SEQ, HEATNR, AREA_CODE, STATION_NAME, TS_SHIFTNO, GRADE_ID, GRADE_TYPE1, F_ROUTE1, CAST_DIV_NO_1, MAT_TYPE_DESC, MAT_CODE_DR, MAT_NAME_DR, INCLUDE_NI, INCLUDE_CR, INCLUDE_MO, QUALIFIED_WT, CONVERSION_PRICE, WEIGHT, COST, COST_PERT, UNIT_PRICE_XY, COST_XY, COST_PERT_XY,TYPE)"
			" select REC_CREATOR, REC_CREATE_TIME, AOD_BOF_E_DTIME, DATE_C, CAST_SEQ, HEATNR, AREA_CODE, STATION_NAME, TS_SHIFTNO, GRADE_ID, GRADE_TYPE1, F_ROUTE1, CAST_DIV_NO_1, MAT_TYPE_DESC, MAT_CODE_DR, MAT_NAME_DR, INCLUDE_NI, INCLUDE_CR, INCLUDE_MO, QUALIFIED_WT, CONVERSION_PRICE, WEIGHT, COST, COST_PERT, UNIT_PRICE_XY, COST_XY, COST_PERT_XY,TYPE"
			" from tqmtscb03_mx"
			" where 1=1"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tqmtscb01a_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,AREA_CODE,STATION_NAME,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,INCLUDE_NI,INCLUDE_CR,INCLUDE_MO,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,UNIT_PRICE_XY,COST_XY,COST_PERT_XY,TYPE)"
			" select REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,AREA_CODE,STATION_NAME,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,INCLUDE_NI,INCLUDE_CR,INCLUDE_MO,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,UNIT_PRICE_XY,COST_XY,COST_PERT_XY,type"
			" from tqmtscb01_mx"
			" where 1=1"
			" and DATE_C = @stat_date"
			;
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " insert into tqmtscb01a_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,AREA_CODE,STATION_NAME,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,INCLUDE_NI,INCLUDE_CR,INCLUDE_MO,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,UNIT_PRICE_XY,COST_XY,COST_PERT_XY,HANDLE_DIV)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR"
			",case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD' else ' ' end"
			",t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.mat_act_wt,t1.UNIT_PRICE,sum(devo_wt),round(sum(t1.UNIT_PRICE*devo_wt),2),decode(t2.mat_act_wt,0,0,round(sum(t1.UNIT_PRICE*devo_wt)/t2.mat_act_wt,2))"
			", case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end "
			", round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt), 2)"
			", decode(t2.mat_act_wt, 0, 0, round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt) / t2.mat_act_wt, 2))"
			",'F'"
			" from tqmtscb02_mx t2"
			" left join tmmsmzxh01 t1 on t1.HANDLE_DIV ='F' and t1.heat_no=t2.HEATNR"
			" where 1=1"
			" and nvl(devo_wt,0) !=0"
			" and  t2.DATE_C = @stat_date"
			" group by t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR,t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1,t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.mat_act_wt,t1.UNIT_PRICE,t1.UNIT_PRICE_CR,t1.UNIT_PRICE_NI"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//插入分摊的量
		sqlstr = " insert into tqmtscb03a_mx(REC_CREATOR,REC_CREATE_TIME,AOD_BOF_E_DTIME,DATE_C,CAST_SEQ,HEATNR,AREA_CODE,STATION_NAME,TS_SHIFTNO,GRADE_ID,GRADE_TYPE1,F_ROUTE1,CAST_DIV_NO_1,MAT_TYPE_DESC,MAT_CODE_DR,MAT_NAME_DR,INCLUDE_NI,INCLUDE_CR,INCLUDE_MO,QUALIFIED_WT,CONVERSION_PRICE,WEIGHT,COST,COST_PERT,UNIT_PRICE_XY,COST_XY,COST_PERT_XY,HANDLE_DIV)"
			" select @rec_creator, @rec_create_time,t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR"
			",case when t1.dev_code like 'B%' then 'BOF' when t1.dev_code like 'E%' then 'EAF' when t1.dev_code like 'R%' then 'RH' when t1.dev_code like 'F%' then 'LF'  when t1.dev_code like 'S%' then 'LTS'  when t1.dev_code like 'Z%' then 'IF'  when t1.dev_code like 'V%' then 'VOD'  when t1.dev_code like 'A%' then 'AOD' else ' ' end"
			",t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1"
			",t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.CONVERSION_OK_WT,t1.UNIT_PRICE,sum(devo_wt),round(sum(t1.UNIT_PRICE*devo_wt),2),decode(t2.CONVERSION_OK_WT,0,0,round(sum(t1.UNIT_PRICE*devo_wt)/t2.CONVERSION_OK_WT,2))"
			", case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end "
			", round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt), 2)"
			", decode(t2.CONVERSION_OK_WT, 0, 0, round(sum((case when substr(t2.GRADE_ID,1,2) in ('1F','1M','4F','4M') then  t1.UNIT_PRICE_CR when substr(t2.GRADE_ID,1,2) in ('1A','1D','4A','4D') then  t1.UNIT_PRICE_NI else 0 end)*devo_wt) / t2.CONVERSION_OK_WT, 2))"
			",'F'"
			" from tqmtscb02_mx t2"
			" left join tmmsmzxh01 t1 on t1.HANDLE_DIV='F' and  t1.heat_no=t2.HEATNR"
			" where 1=1"
			" and nvl(devo_wt,0) !=0"
			" and  t2.DATE_C = @stat_date"
			" group by t2.AOD_BOF_E_DTIME,t2.DATE_C,t2.CAST_SEQ,t2.HEATNR,t1.dev_code,t2.TS_SHIFTNO,t2.GRADE_ID,t2.GRADE_TYPE1,t2.F_ROUTE1,t2.CAST_DIV_NO_1,t1.TYPE_CODE1,t1.mat_code,t1.mat_name,round(t1.NI_VALUE,3),round(t1.CR_VALUE,3),round(t1.MO_VALUE,3),t2.CONVERSION_OK_WT,t1.UNIT_PRICE,t1.UNIT_PRICE_CR,t1.UNIT_PRICE_NI"
			;
		Log::Trace("", __FUNCTION__, "-sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		//插入收得率信息值
		sqlstr = " delete from tqmtscb04_mx"
			" where 1=1"
			" and  DATE_C like @stat_date||'%'"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = "insert into tqmtscb04_mx (REC_CREATOR,REC_CREATE_TIME,DATE_C,HEAT_COUNT,RECEIVE_WEIGHT,MAT_ACT_WT,CR_IN,NI_IN,MO_IN,CR_OUT,NI_OUT,MO_OUT,CR_YEILD,NI_YEILD,MO_YEILD,METAL_CONS_PERT,BUSI_TYPE)"
			" select @rec_creator, @rec_create_time,t1.aod_bof_e_dtime, heat_count, RECEIVE_WEIGHT, MAT_ACT_WT"
			", round(IN_CR/100, 0), round(IN_NI/100, 0), round(IN_MO/100, 0), round(AT_CR/100, 0), round(AT_NI/100, 0), round(AT_MO/100, 0)"
			",decode(IN_CR,0,0,round(AT_CR*100/IN_CR, 2)),decode(IN_NI,0,0,round(AT_NI*100/IN_NI, 2)),decode(IN_MO,0,0,round(AT_MO*100/IN_MO, 2))"
			", decode((RECEIVE_WEIGHT + CUT_SCRAP_WT), 0, 0, round(METAL_CONS * 1000 / (RECEIVE_WEIGHT + CUT_SCRAP_WT), 0))"
			",'日累计收得率'"
			" from("
			" select aod_bof_e_dtime, sum(heat_count) over(order by aod_bof_e_dtime) heat_count"
			" , sum(RECEIVE_WEIGHT) over(order by aod_bof_e_dtime) RECEIVE_WEIGHT"
			" , sum(MAT_ACT_WT) over(order by aod_bof_e_dtime) MAT_ACT_WT"
			" , sum(CUT_SCRAP_WT) over(order by aod_bof_e_dtime) CUT_SCRAP_WT"
			" from("
			" select aod_bof_e_dtime, count(1) heat_count, sum(RECEIVE_WEIGHT) RECEIVE_WEIGHT, sum(MAT_ACT_WT) MAT_ACT_WT, sum(CUT_SCRAP_WT) CUT_SCRAP_WT"
			" from tqmtscb02_mx"
			" where HEATNR like 'A%'"
			" and HEATNR in ( select heat_no from TQMTSB0 )"
			" and aod_bof_e_dtime like @stat_date || '%'"
			" group by aod_bof_e_dtime)"
			" )t1"
			" left join("
			" select aod_bof_e_dtime, sum(IN_CR) over(order by aod_bof_e_dtime) IN_CR"
			" , sum(IN_NI) over(order by aod_bof_e_dtime) IN_NI"
			" , sum(IN_MO) over(order by aod_bof_e_dtime) IN_MO"
			" , sum(METAL_CONS) over(order by aod_bof_e_dtime) METAL_CONS"
			" from ("
			" select aod_bof_e_dtime, sum(include_cr*WEIGHT) IN_CR"
			", sum(case when GRADE_ID in (select st_no from tqmts0x  t where t.elm_std_idx_a  in (select IDX_NO from tqmts02 where SPE_MIN>0 and elm_code='007')) then include_ni*WEIGHT else 0 end) IN_NI"
			", sum(case when GRADE_ID in (select STEEL_GRADE from tqmtscb09_dr where GRADE_SERIES='含钼') then include_mo*WEIGHT else 0 end) IN_MO"
			" , sum(case when MAT_CODE_DR in(select MAT_CODE_DR from tqmtscb08_dr where MAT_TYPE_DESC in('辅料', '步骤费')) then 0 else WEIGHT end) METAL_CONS"
			" from tqmtscb01_mx"
			" where HEATNR like 'A%'"
			" and HEATNR in ( select heat_no from TQMTSB0 )"
			" and MAT_TYPE_DESC not in('步骤费', '回收')"
			" and aod_bof_e_dtime like @stat_date || '%'"
			" group by aod_bof_e_dtime) "
			" )t2 on t1.aod_bof_e_dtime = t2.aod_bof_e_dtime"
			" left join("
			" select aod_bof_e_dtime, sum(AT_CR) over(order by aod_bof_e_dtime) AT_CR"
			" , sum(AT_NI) over(order by aod_bof_e_dtime) AT_NI"
			" , sum(AT_MO) over(order by aod_bof_e_dtime) AT_MO"
			" from ("
			" select t.aod_bof_e_dtime, sum((RECEIVE_WEIGHT + CUT_SCRAP_WT)*NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0)) AT_CR"
			", sum(case when  GRADE_ID in (select st_no from tqmts0x  t where t.elm_std_idx_a  in (select IDX_NO from tqmts02 where SPE_MIN>0 and elm_code='007')) then (RECEIVE_WEIGHT + CUT_SCRAP_WT)*NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) else 0 end) AT_NI"
			", sum(case when t.GRADE_ID in (select STEEL_GRADE from tqmtscb09_dr where GRADE_SERIES='含钼') then (RECEIVE_WEIGHT + CUT_SCRAP_WT)*NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0) else 0 end) AT_MO"
			" from tqmtscb02_mx t left join TQMTSB0 t2 on t.HEATNR = t2.heat_no"
			" where t.HEATNR like 'A%'"
			" and HEATNR in ( select heat_no from TQMTSB0 )"
			" and aod_bof_e_dtime like @stat_date || '%'"
			" group by t.aod_bof_e_dtime)"
			" )t3 on  t1.aod_bof_e_dtime = t3.aod_bof_e_dtime"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		sqlstr = "insert into tqmtscb04_mx (REC_CREATOR,REC_CREATE_TIME,DATE_C,HEAT_COUNT,RECEIVE_WEIGHT,MAT_ACT_WT,CR_IN,NI_IN,MO_IN,CR_OUT,NI_OUT,MO_OUT,CR_YEILD,NI_YEILD,MO_YEILD,METAL_CONS_PERT,BUSI_TYPE)"
			" select @rec_creator, @rec_create_time,t1.aod_bof_e_dtime, heat_count, RECEIVE_WEIGHT, MAT_ACT_WT"
			", round(IN_CR/100, 0), round(IN_NI/100, 0), round(IN_MO/100, 0), round(AT_CR/100, 0), round(AT_NI/100, 0), round(AT_MO/100, 0)"
			",decode(IN_CR,0,0,round(AT_CR*100/IN_CR, 2)),decode(IN_NI,0,0,round(AT_NI*100/IN_NI, 2)),decode(IN_MO,0,0,round(AT_MO*100/IN_MO, 2))"
			", decode((RECEIVE_WEIGHT + CUT_SCRAP_WT), 0, 0, round(METAL_CONS * 1000 / (RECEIVE_WEIGHT + CUT_SCRAP_WT), 0))"
			",'日收得率'"
			" from("
			" select aod_bof_e_dtime, count(1) heat_count, sum(RECEIVE_WEIGHT) RECEIVE_WEIGHT, sum(MAT_ACT_WT) MAT_ACT_WT, sum(CUT_SCRAP_WT) CUT_SCRAP_WT"
			" from tqmtscb02_mx"
			" where HEATNR like 'A%'"
			" and HEATNR in ( select heat_no from TQMTSB0 )"
			" and aod_bof_e_dtime like @stat_date || '%'"
			" group by aod_bof_e_dtime) t1"
			" left join("
			" select aod_bof_e_dtime, sum(include_cr*WEIGHT) IN_CR"
			", sum(case when GRADE_ID  in (select st_no from tqmts0x  t where t.elm_std_idx_a  in (select IDX_NO from tqmts02 where SPE_MIN>0 and elm_code='007')) then include_ni*WEIGHT else 0 end) IN_NI"			
			", sum(case when GRADE_ID in (select STEEL_GRADE from tqmtscb09_dr where GRADE_SERIES='含钼') then include_mo*WEIGHT else 0 end) IN_MO"
			" , sum(case when MAT_CODE_DR in(select MAT_CODE_DR from tqmtscb08_dr where MAT_TYPE_DESC in('辅料', '步骤费')) then 0 else WEIGHT end) METAL_CONS"
			" from tqmtscb01_mx"
			" where HEATNR like 'A%'"
			" and HEATNR in ( select heat_no from TQMTSB0 )"
			" and MAT_TYPE_DESC not in('步骤费', '回收')"
			" and aod_bof_e_dtime like @stat_date || '%'"
			" group by aod_bof_e_dtime) t2 on t1.aod_bof_e_dtime = t2.aod_bof_e_dtime"
			" left join("
			" select t.aod_bof_e_dtime, sum((RECEIVE_WEIGHT + CUT_SCRAP_WT)*NVL(CASE WHEN ELM_006 = -1 THEN 0 ELSE ELM_006 END, 0)) AT_CR"			
			", sum(case when GRADE_ID in (select st_no from tqmts0x  t where t.elm_std_idx_a  in (select IDX_NO from tqmts02 where SPE_MIN>0 and elm_code='007')) then (RECEIVE_WEIGHT + CUT_SCRAP_WT)*NVL(CASE WHEN ELM_007 = -1 THEN 0 ELSE ELM_007 END, 0) else 0 end) AT_NI"
			", sum(case when t.GRADE_ID in (select STEEL_GRADE from tqmtscb09_dr where GRADE_SERIES='含钼') then (RECEIVE_WEIGHT + CUT_SCRAP_WT)*NVL(CASE WHEN ELM_008 = -1 THEN 0 ELSE ELM_008 END, 0) else 0 end) AT_MO"			
			" from tqmtscb02_mx t left join TQMTSB0 t2 on t.HEATNR = t2.heat_no"
			" where t.HEATNR like 'A%'"
			" and HEATNR in ( select heat_no from TQMTSB0 )"
			" and aod_bof_e_dtime like @stat_date || '%'"
			" group by t.aod_bof_e_dtime) t3 on  t1.aod_bof_e_dtime = t3.aod_bof_e_dtime"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stat_date", stat_date);
		cmd_inq.Parameters.Set("rec_creator", s.userid);
		cmd_inq.Parameters.Set("rec_create_time", dateNow);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();		


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
