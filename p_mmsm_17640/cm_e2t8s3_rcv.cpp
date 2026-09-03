/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================SHIFT_GROUP
/// <summary>
/// AOD生产炉次报告
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//参考21的函数
int f_mmsm27_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_t823s2_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//f_t823s1_snd
int f_t823s1_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_t823s3_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量
int f_t823s5_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测
int f_qmts_call_judge(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//消息规则引擎

BM2F_ENTERACE_TELE(cm_e2t8s3_rcv)

int f_cm_e2t8s3_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);
	CDbCommand cmd_yz(conn);
	CDbCommand cmd_yl(conn);
	CDbCommand cmd_yzsc(conn);
	CDbCommand cmd_2a(conn);
	CDbCommand cmd_inq_test(conn);
	CDbCommand cmd_inq_27(conn);
	CDbCommand cmd_tpssm(conn);
	CDbCommand cmd_tpssm12z(conn);
	CModel tmmsm27("TMMSM27");
	CModel tmmsm14("TMMSM14");
	CModel tmmsm12("TMMSM12");
	CModel tmmsm20("TMMSM20");
	CModel tmmsm19("TMMSM19");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm2a_yl("TMMSM2A_YL");
	CModel tmmsm21("TMMSM21");
	CModel ttmsm12("TTMSM12");
	CModel tpssm12z("TPSSM12Z");
	CModel tpssm12zt("TPSSM12ZT");
	EIClass tmmsm27_back;
	CString dev_code = "";
	CString st_no = "";
	CString sm_plan_no2 = "";//炼钢计划号
	CString sql = "";
	CString heat_no = " ";
	CString proc_no = " ";//商议炉炉号
	CString end_time_trp = " ";//上一炉出钢结束时间


	//加入函数的表

	tmmsm27_back.Tables.Add();
	if (!tmmsm27_back.Tables[0].Columns.Contains("PROC_DIV"))
	{
		tmmsm27_back.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
	}
	if (!tmmsm27_back.Tables[0].Columns.Contains("PRACT_COLL_MODE"))
	{
		tmmsm27_back.Tables[0].Columns.Add(DT_STRING, "PRACT_COLL_MODE");
	}
	if (!tmmsm27_back.Tables[0].Columns.Contains("FACTORY_DIV"))
	{
		tmmsm27_back.Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
	}
	//tmmsm27_back.Tables[0].Rows.Add();
	tmmsm27_back.Tables[0].Columns.Add(tmmsm27);
	//tmmsm27_back.Tables[0].Rows.Add();

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		tmmsm27.MergeFrom(bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]);
		///工号
		dev_code = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["AGGREGATE_NAME"].ToString().Trim();
		st_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["GRADE_ACT"].ToString().Trim();
		sm_plan_no2 = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["ORDER_NUMBER"].ToString().Trim();
		tmmsm27["ID_SJ"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["ID"].ToString().Trim();
		//同工位处理次数
		tmmsm27["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["TREATMENT_COUNTER"].ToString().Trim();

		Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no2);
		//根据计划号获取熔炼号和制造命令号
		cmd_inq_code.SetCommandText(" SELECT HEAT_NO, PONO, SM_PLAN_NO FROM "
			" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
			" UNION "
			" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no2 + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm27["HEAT_NO"] = cmd_inq_code.GetString(1);
			tmmsm27["PONO"] = cmd_inq_code.GetString(2);
			tmmsm27["SM_PLAN_NO"] = cmd_inq_code.GetString(3);
		}
		cmd_inq_code.Close();

		cmd_inq.SetCommandText(" SELECT T1.PROC_NO,T2.STATION_ID,T2.STATION_NO FROM (  "
			" SELECT PROC_NO, DEV_CODE, DECODE(PRE_SOLUTION_FLAG, '1', '0', '1') C_DIV FROM TPSSM12 WHERE HEAT_NO = '" + tmmsm27["HEAT_NO"].ToString() + "' AND DEV_CODE = '" + dev_code + "' and TREATMENT_COUNTER ='" + tmmsm27["SAME_PROC_NUM"].ToString() + "' ) T1 LEFT JOIN TPSSMD1 T2  "
			" on t1.DEV_CODE = t2.DEV_CODE AND T1.C_DIV = T2.C_DIV");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm27["PROC_NO"] = cmd_inq.GetString(1);
			tmmsm27["STATION_ID"] = cmd_inq.GetString(2);
			tmmsm27["STATION_NO"] = cmd_inq.GetString(3);
		}
		cmd_inq.Close();


		tmmsm27["DEV_CODE"] = dev_code;

		//熔炼号
		tmmsm27["HEAT_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["HEAT_NUMBER"].ToString().Trim();
		tmmsm27["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["HEAT_NUMBER"].ToString().Trim();
		if (tmmsm27["PROC_NO"].ToString() != " "&&tmmsm27["PROC_NO"].ToString() != ""){

		}
		else{
			tmmsm27["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["HEAT_NUMBER"].ToString().Trim();
		}
		//计划号
		tmmsm27["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["ORDER_NUMBER"].ToString().Trim();
		//分包号SPLIT_INDICATION

		
		//兑入预溶液的空包重
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() != 0){
			tmmsm27["EMPTY_LADLE_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal()/1000;
		}
		//班组
		tmmsm27["PROD_SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["SHIFT_TEAM"].ToString().Trim();
		//操纵工
		tmmsm27["ASSISTANT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["OPERATOR"].ToString().Trim();
		//实际内部钢种
		tmmsm27["ST_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["GRADE_ACT"].ToString().Trim();
		//实际重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["WEIGHT_ACT"].ToDecimal() != 0){
			tmmsm27["ACTRESULT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["WEIGHT_ACT"].ToDecimal()/1000;
		}
		//预溶液重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["EAF_WEIGHT"].ToDecimal() != 0){
			tmmsm27["LADLE_PRE_LIQUID_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["EAF_WEIGHT"].ToDecimal()/1000;
		}
		//废料重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["SCRAP_WEIGHT"].ToDecimal() != 0){
			tmmsm27["SCRAP_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["SCRAP_WEIGHT"].ToDecimal()/1000;
		}
		//开始时刻
		tmmsm27["START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["HEAT_START"].ToString().Trim();
		//结束时刻
		tmmsm27["END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["HEAT_END"].ToString().Trim();
		Log::Trace("", "START_TIME", "开始时刻:START_TIME[{0}]", tmmsm27["START_TIME"].ToString());
		Log::Trace("", "END_TIME", "结束时刻:END_TIME[{0}]", tmmsm27["END_TIME"].ToString());
		//吹炼次数BLOW_NUMBER
		//吹炼时间
		tmmsm27["BLOW_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["BLOW_TIME"].ToDecimal()/60;
		tmmsm27["BLOW_DURATION"] = tmmsm27["BLOW_DURATION"].ToDecimal().Round(2);
		//总吹氧量
		tmmsm27["OXYGEN_FINAL"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["OXYGEN_TOT"].ToString().Trim();
		//吹炼开始时间
		tmmsm27["BLOW_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["BLOW_START"].ToString().Trim();
		//吹炼结束时间
		tmmsm27["BLOW_END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["BLOW_END"].ToString().Trim();
		Log::Trace("", "BLOW_START_TIME", "吹炼开始时间:BLOW_START_TIME[{0}]", tmmsm27["BLOW_START_TIME"].ToString());
		Log::Trace("", "BLOW_END_TIME", "吹炼结束时间:BLOW_END_TIME[{0}]", tmmsm27["BLOW_END_TIME"].ToString());
		//溅渣次数SLAG_SPLASHING_NUMBER
		//顶底吹氮量
		tmmsm27["NITROGEN_TOT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["NITROGEN_TOT"].ToString().Trim();
		//顶底吹氩量
		tmmsm27["TOTAL_AR_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["ARGON_TOT"].ToString().Trim();
		//溅渣开始时间
		tmmsm27["SLAG_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["SLAG_SPLASHING_START"].ToString().Trim();
		//溅渣结束时间
		tmmsm27["SLAG_END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["SLAG_SPLASHING_END"].ToString().Trim();
		//氩气合计
		tmmsm27["AR_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["ARGON_TUY_TOT"].ToString().Trim();
		//氮气合计
		tmmsm27["N_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["NITROGEN_TUY_TOT"].ToString().Trim();
		//出钢开始时间
		tmmsm27["TAP_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["TAPPING_START"].ToString().Trim();
		//出钢结束时间
		tmmsm27["TAP_END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["TAPPING_END"].ToString().Trim();
		Log::Trace("", "TAP_START_TIME", "出钢开始时间:TAP_START_TIME[{0}]", tmmsm27["TAP_START_TIME"].ToString());
		Log::Trace("", "TAP_END_TIME", "出钢结束:TAP_END_TIME[{0}]", tmmsm27["TAP_END_TIME"].ToString());
		//扒渣开始时间
		tmmsm27["DROSSING_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["SLAGGING_START"].ToString().Trim();
		//扒渣结束时间
		tmmsm27["DROSSING_END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["SLAGGING_END"].ToString().Trim();
		Log::Trace("", "DROSSING_START_TIME", "扒渣开始时间:DROSSING_START_TIME[{0}]", tmmsm27["DROSSING_START_TIME"].ToString());
		Log::Trace("", "DROSSING_END_TIME", "扒渣结束时间:DROSSING_END_TIME[{0}]", tmmsm27["DROSSING_END_TIME"].ToString());
		//预溶液熔炼号1 HEATNO_PREMELT1
		//预溶液重量1 WEIGHT_PREMELT1
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["WEIGHT_PREMELT1"].ToDecimal() != 0){
			tmmsm27["WEIGHT_PREMELT1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["WEIGHT_PREMELT1"].ToDecimal()/1000;
		}
		//预溶液熔炼号2 HEATNO_PREMELT2
		//预溶液重量2 WEIGHT_PREMELT2
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["WEIGHT_PREMELT2"].ToDecimal() != 0){
			tmmsm27["WEIGHT_PREMELT2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["WEIGHT_PREMELT2"].ToDecimal()/1000;
		}
		//预溶液熔炼号3 HEATNO_PREMELT3
		//预溶液重量3 WEIGHT_PREMELT3
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["WEIGHT_PREMELT3"].ToDecimal() != 0){
			tmmsm27["WEIGHT_PREMELT3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["WEIGHT_PREMELT3"].ToDecimal()/1000;
		}
		//炉壳号AOD_SHELL_NO
		//炉壳龄AOD_SHELL_LIFE
		//顶枪枪龄TOP_LANCE_LIFE
		//副枪使用次数SUB_LANCE_USE_COUNT
		//钢包号
		tmmsm27["LADLE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["STEEL_LADLE_NO"].ToString().Trim();
		//钢包包龄
		tmmsm27["LADLE_AGE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["LADLE_LIFE"].ToString().Trim();
		//出钢钢包空罐重量 EMPTY_LADLE_WT
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["EMPTY_LADLE_WT"].ToDecimal() != 0){
			tmmsm27["EMPTY_LADLE_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["EMPTY_LADLE_WT"].ToDecimal()/1000;
		}
		//充电温度 CHARGE_TEMP
		//开始时间渣厚 SLAG_THICK_START
		//冶炼模式 SMP_MODE
		//一阶段温度
		tmmsm27["FIRST_BG_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["PHASE1_TEMP"].ToString().Trim();
		//一阶段碳PHASE1_C
		//吹炼结束温度BLOW_END_TEMP
		//吹炼结束碳BLOW_END_C
		//还原周期REDUCING_DURATION
		tmmsm27["REDUCING_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["REDUCING_DURATION"].ToDecimal() / 60;
		tmmsm27["REDUCING_DURATION"] = tmmsm27["REDUCING_DURATION"].ToDecimal().Round(2);
		//预计算碱度CAL_BASICITY
		//还原后碱度REDUCING_BASICITY
		//还原后温度REDUCTION_TEMP
		//还原后硫REDUCTION_S
		//还原后硅REDUCTION_SI
		//补吹次数REBLOW_COUNT
		//出钢温度
		tmmsm27["OUT_STEEL_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["TAPPING_TEMP"].ToString().Trim();
		//熔化持续时间MELT_DURATION
		//辅助时长ASSIS_DURATION
		//出钢周期TAPTOTAP_DURATION
		//出钢渣厚SLAG_THICK_TAPPING
		//钢水罐离开重量LADLE_DEPART_WT
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["LADLE_DEPART_WT"].ToDecimal() != 0){
			tmmsm27["LADLE_DEPART_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["LADLE_DEPART_WT"].ToDecimal()/1000;
		}
		//钢包温度LADLE_TEMP
		//下工序设备代码NEXT_DEV_CODE
		//顶枪抢号TOP_LANCE_NO
		//等待时间
		tmmsm27["WAITING_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["DURATION_WAIT"].ToDecimal() / 60;
		tmmsm27["WAITING_TIME"] = tmmsm27["WAITING_TIME"].ToDecimal().Round(2);
		//等待原因
		tmmsm27["WAIT_REASON"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["REMARK_WAIT"].ToString().Trim();
		//剩余铁水重量 REMAIN_STEEL_WEIGHT
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["REMAIN_STEEL_WEIGHT"].ToDecimal() != 0){
			tmmsm27["REMAIN_STEEL_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["REMAIN_STEEL_WEIGHT"].ToDecimal()/1000;
		}
		//预溶液钢包号1 LADLENO_PREMELT1
		//预溶液钢包号2 LADLENO_PREMELT2
		//预溶液钢包号3 LADLENO_PREMELT3
		//预溶液熔炼号4 HEATNO_PREMELT4
		//预溶液熔炼号5 HEATNO_PREMELT5
		//预溶液熔炼号6 HEATNO_PREMELT6

		//根据预溶液熔炼号去反写表
		if (tmmsm27["HEAT_NO"].ToString() == " " ){
			Log::Trace("", "heat_no", "heat_no没有值无法反写 = {0}", heat_no);
		}
		else
		{
			cmd_tpssm12z.SetCommandText(" update TPSSM12Z set SM_PLAN_NO=' ',PONO=' ',ST_NO=' ',HEAT_NO=' ',SM_PLAN_NOL2=' ' where HEAT_NO='" + tmmsm27["HEAT_NO"].ToString() + "' ");
			cmd_tpssm12z.ExecuteNonQuery();
			cmd_tpssm12z.Close();
			if (tmmsm27["HEATNO_PREMELT1"].ToString() != " "){
				Log::Trace("", "HEATNO_PREMELT1", "HEATNO_PREMELT1 = {0}", tmmsm27["HEATNO_PREMELT1"].ToString().SubstringNE(0, 1));
				if (tmmsm27["HEATNO_PREMELT1"].ToString().SubstringNE(0, 1) == "D"){
					tmmsm14["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm14["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NOL2"].ToString().Trim();
					tmmsm14["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tmmsm14["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tmmsm14["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					Log::Trace("", "PONO", "PONO = {0}", tmmsm14["PONO"].ToString());
					Log::Trace("", "L2_PROC_NO", "L2_PROC_NO = {0}", tmmsm14["L2_PROC_NO"].ToString());
					tmmsm14.Update("HEAT_NO,PONO,SM_PLAN_NO,SM_PLAN_NOL2","L2_PROC_NO");
					tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'D%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						Log::Trace("", "L2_PROC_NO", "tmmsm2a.L2_PROC_NO = {0}", tmmsm2a["L2_PROC_NO"].ToString());
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");

					}
					cmd_2a.Close();
					//调用智慧质量发送电文
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm14["L2_PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm14["HEAT_NO"];
					//doFlag = f_t823s1_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT1"].ToString().SubstringNE(0, 1) == "F"){
					//中频炉反写
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm19["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm19.Query("L2_PROC_NO");
					tmmsm19.TrimOrBlank();
					tmmsm19["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm19["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tmmsm19["SM_PLAN_NOL2"] = sm_plan_no2;
					//update计划表 王程宇提出
					tpssm12z["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tpssm12z["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tpssm12z["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tpssm12z["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NOL2"].ToString().Trim();
					tpssm12z["ST_NO"] = tmmsm27["ST_NO"].ToString().Trim();
					tpssm12z["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tpssm12z.Update("PONO,SM_PLAN_NO,HEAT_NO,SM_PLAN_NOL2,ST_NO","PROC_NO");
					Log::Trace("", "tpssm12z", "tpssm12z");
					if (tmmsm19["HEAT_NO"].ToString() != " "){
						Log::Trace("", "HEAT_NO", "HEAT_NO");
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString()==" "){
							tmmsm19["HEAT_NO1"] = heat_no;
							tmmsm19["SM_PLAN_NO1"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							tmmsm19.Update("HEAT_NO1,SM_PLAN_NOL2,SM_PLAN_NO1,PROC_NO,PONO", "L2_PROC_NO");
						}
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() != " " &&tmmsm19["HEAT_NO1"].ToString() != heat_no){
							tmmsm19["HEAT_NO2"] = heat_no;
							tmmsm19["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NO"].ToString().Trim(); 
							tmmsm19.Update("HEAT_NO2,SM_PLAN_NOL2,SM_PLAN_NOL2,PROC_NO,PONO", "L2_PROC_NO");
						}
					}
					else{
						Log::Trace("", "else", "else");
						tmmsm19["HEAT_NO"] = heat_no;
						tmmsm19["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
						tmmsm19.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PROC_NO,PONO", "L2_PROC_NO");
						tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
						tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
						tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						Log::Trace("", "HEATNO_PREMELT1", "HEATNO_PREMELT1=[{0}]", tmmsm27["HEATNO_PREMELT1"].ToString());
						cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'Z%' ");
						cmd_2a.ExecuteReader();
						while (cmd_2a.Read())
						{
							Log::Trace("", "L2_PROC_NO", "tmmsm2a.L2_PROC_NO = {0}", tmmsm2a["L2_PROC_NO"].ToString());
							tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
							tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
							tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
							tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
							tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
							tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");

						}
						cmd_2a.Close();
					}
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString().Trim();
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString().Trim();
					//doFlag = f_t823s3_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT1"].ToString().SubstringNE(0, 1) == "E"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm20["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm20.Query("L2_PROC_NO");
					tmmsm20.TrimOrBlank();
					tmmsm20["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					if (tmmsm20["HEAT_NO"].ToString() != " "){
						CString heat_noc = " ";
						cmd_inq_27.SetCommandText(" select HEAT_NO from TMMSM27 WHERE HEATNO_PREMELT1='" + tmmsm27["HEATNO_PREMELT1"].ToString() + "' OR HEATNO_PREMELT2='" + tmmsm27["HEATNO_PREMELT1"].ToString() + "' OR HEATNO_PREMELT3='" + tmmsm27["HEATNO_PREMELT1"].ToString() + "' ");
						cmd_inq_27.ExecuteReader();
						if (cmd_inq_27.Read()){
							heat_noc = cmd_inq_27.GetString(1);
						}
						if (heat_noc != heat_no&&heat_noc != " "){
							if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() == " "){
								tmmsm20["HEAT_NO1"] = heat_no;
								tmmsm20.Update("HEAT_NO1,PROC_NO", "L2_PROC_NO");
							}
							if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() != " " &&tmmsm20["HEAT_NO1"].ToString() != heat_no){
								tmmsm20["HEAT_NO2"] = heat_no;
								tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
							}
						}
						else{
							tmmsm20["HEAT_NO"] = heat_no;
							tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm20["HEAT_NO"] = heat_no;
						tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
					}
					tmmsm2a["HEAT_NO"] = heat_no;
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm20["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'E%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
					}
					cmd_2a.Close();
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}
					if (!bcls_rec->Tables["T823S"].Columns.Contains("L2_PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "L2_PROC_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm20["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm20["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tmmsm20["L2_PROC_NO"];
					//doFlag = f_t823s5_snd(bcls_rec, bcls_ret, conn);
					
				}
				else if (tmmsm27["HEATNO_PREMELT1"].ToString().SubstringNE(0, 1) == "B"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm21["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm21.Query("L2_PROC_NO");
					tmmsm21.TrimOrBlank();
					if (tmmsm21["HEAT_NO"].ToString() != " "){
						CString heat_noc = " ";
						cmd_inq_27.SetCommandText(" select HEAT_NO from TMMSM27 WHERE HEATNO_PREMELT1='" + tmmsm27["HEATNO_PREMELT1"].ToString() + "' OR HEATNO_PREMELT2='" + tmmsm27["HEATNO_PREMELT1"].ToString() + "' OR HEATNO_PREMELT3='" + tmmsm27["HEATNO_PREMELT1"].ToString() + "' ");
						cmd_inq_27.ExecuteReader();
						if (cmd_inq_27.Read()){
							heat_noc = cmd_inq_27.GetString(1);
						}
						if (heat_noc != heat_no&&heat_noc != " "){
							if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() == " "){
								tmmsm21["HEAT_NO1"] = heat_no;
								tmmsm21.Update("HEAT_NO1", "L2_PROC_NO");
							}
							if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() != " " &&tmmsm21["HEAT_NO1"].ToString() != heat_no){
								tmmsm21["HEAT_NO2"] = heat_no;
								tmmsm21.Update("HEAT_NO2", "L2_PROC_NO");
							}
						}
						else{
							if (tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "1" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "4" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 3) == "DeP")
							{
								tmmsm21["HEAT_NO"] = heat_no;
								tmmsm21.Update("HEAT_NO", "L2_PROC_NO");
							}
							
						}
					}
					else{
						if (tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "1" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "4" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 3) == "DeP")
						{
							tmmsm21["HEAT_NO"] = heat_no;
							tmmsm21.Update("HEAT_NO", "L2_PROC_NO");
						}
						
					}
					tmmsm2a["HEAT_NO"] = heat_no;
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm21["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'B%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
					}
					cmd_2a.Close();
				}
			}
			//HEATNO_PREMELT2
			Log::Trace("", "HEATNO_PREMELT2", "HEATNO_PREMELT2 = {0}", tmmsm27["HEATNO_PREMELT2"].ToString());
			if ( tmmsm27["HEATNO_PREMELT2"].ToString() != " "){
				if (tmmsm27["HEATNO_PREMELT2"].ToString().SubstringNE(0, 1) == "D"){
					tmmsm14["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm14["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
					tmmsm14["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NOL2"].ToString().Trim();
					tmmsm14["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tmmsm14["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tmmsm14.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PONO", "L2_PROC_NO");
					tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'D%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						Log::Trace("", "L2_PROC_NO", "tmmsm2a.L2_PROC_NO = {0}", tmmsm2a["L2_PROC_NO"].ToString());
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");

					}
					cmd_2a.Close();
					//调用智慧质量发送电文
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm14["L2_PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm14["HEAT_NO"];
					//doFlag = f_t823s1_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT2"].ToString().SubstringNE(0, 1) == "F"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm19["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
					tmmsm19.Query("L2_PROC_NO");
					tmmsm19.TrimOrBlank();
					tmmsm19["PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
					tmmsm19["SM_PLAN_NOL2"] = sm_plan_no2;
					tmmsm19["PONO"] = tmmsm27["PONO"].ToString().Trim();
					//update计划表 王程宇提出
					tpssm12z["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tpssm12z["PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
					tpssm12z["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tpssm12z["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tpssm12z["ST_NO"] = tmmsm27["ST_NO"].ToString().Trim();
					tpssm12z.Update("PONO,SM_PLAN_NO,HEAT_NO,SM_PLAN_NOL2,ST_NO", "PROC_NO");
					if (tmmsm19["HEAT_NO"].ToString() != " "){
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() == " "){
							tmmsm19["HEAT_NO1"] = heat_no;
							tmmsm19["SM_PLAN_NO1"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							tmmsm19.Update("HEAT_NO1,SM_PLAN_NOL2,SM_PLAN_NO1,PROC_NO,PONO", "L2_PROC_NO");
						}
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() != " " &&tmmsm19["HEAT_NO1"].ToString() != heat_no){
							tmmsm19["HEAT_NO2"] = heat_no;
							tmmsm19["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							tmmsm19.Update("HEAT_NO2,SM_PLAN_NOL2,PROC_NO,PONO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm19["HEAT_NO"] = heat_no;
						tmmsm19["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
						tmmsm19.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PROC_NO,PONO", "L2_PROC_NO");
						tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
						tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
						tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
						tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'Z%' ");
						cmd_2a.ExecuteReader();
						while (cmd_2a.Read())
						{
							Log::Trace("", "L2_PROC_NO", "tmmsm2a.L2_PROC_NO = {0}", tmmsm2a["L2_PROC_NO"].ToString());
							tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
							tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
							tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
							tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
							tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
							tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						}
						tmmsm2a.Update("PROC_NO", "PROD_SEQ_NO,HEAT_NO");
						cmd_2a.Close();
						blkNum = bcls_rec->Tables.IndexOf("T823S");
						if (blkNum < 0)
						{
							bcls_rec->Tables.Add("T823S");
						}

						if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
						{
							bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
						}

						if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
						{
							bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
						}

						if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
						{
							bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
						}

						bcls_rec->Tables["T823S"].Rows.Add();
						bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
						bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString().Trim();
						bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString().Trim();
						//doFlag = f_t823s3_snd(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					
				}
				else if (tmmsm27["HEATNO_PREMELT2"].ToString().SubstringNE(0, 1) == "E"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm20["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
					tmmsm20.Query("L2_PROC_NO");
					tmmsm20.TrimOrBlank();
					tmmsm20["PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
					if (tmmsm20["HEAT_NO"].ToString() != " "){
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() == " "){
							tmmsm20["HEAT_NO1"] = heat_no;
							tmmsm20.Update("HEAT_NO1,PROC_NO", "L2_PROC_NO");
						}
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() != " " &&tmmsm20["HEAT_NO1"].ToString() != heat_no){
							tmmsm20["HEAT_NO2"] = heat_no;
							tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm20["HEAT_NO"] = heat_no;
						tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
					}
					tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm20["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'E%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
					}
					cmd_2a.Close();
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}
					if (!bcls_rec->Tables["T823S"].Columns.Contains("L2_PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "L2_PROC_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm20["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm20["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tmmsm20["L2_PROC_NO"];
					//doFlag = f_t823s5_snd(bcls_rec, bcls_ret, conn);
				}
				else if (tmmsm27["HEATNO_PREMELT2"].ToString().SubstringNE(0, 1) == "B"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm21["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT2"].ToString().Trim();
					tmmsm21.Query("L2_PROC_NO");
					tmmsm21.TrimOrBlank();
					if (tmmsm21["HEAT_NO"].ToString() != " "){
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() == " "){
							tmmsm21["HEAT_NO1"] = heat_no;
							tmmsm21.Update("HEAT_NO1", "L2_PROC_NO");
						}
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() != " " &&tmmsm21["HEAT_NO1"].ToString() != heat_no){
							tmmsm21["HEAT_NO2"] = heat_no;
							tmmsm21.Update("HEAT_NO2", "L2_PROC_NO");
						}
					}
					else{
						if (tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "1" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "4" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 3) == "DeP")
						{
							tmmsm21["HEAT_NO"] = heat_no;
							tmmsm21.Update("HEAT_NO", "L2_PROC_NO");
						}
						
					}
				}
			}
			//HEATNO_PREMELT3
			Log::Trace("", "HEATNO_PREMELT3", "HEATNO_PREMELT3 = {0}", tmmsm27["HEATNO_PREMELT3"].ToString());
			if (tmmsm27["HEATNO_PREMELT3"].ToString() != "" || tmmsm27["HEATNO_PREMELT3"].ToString() != " "){
				if (tmmsm27["HEATNO_PREMELT3"].ToString().SubstringNE(0, 1) == "D"){
					tmmsm14["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm14["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
					tmmsm14["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NOL2"].ToString().Trim();
					tmmsm14["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tmmsm14["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tmmsm14.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PONO", "L2_PROC_NO");
					tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm14["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'D%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						Log::Trace("", "L2_PROC_NO", "tmmsm2a.L2_PROC_NO = {0}", tmmsm2a["L2_PROC_NO"].ToString());
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");

					}
					cmd_2a.Close();
					//调用智慧质量发送电文
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm14["L2_PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm14["HEAT_NO"];
					//doFlag = f_t823s1_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT3"].ToString().SubstringNE(0, 1) == "F"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm19["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
					tmmsm19.Query("L2_PROC_NO");
					tmmsm19.TrimOrBlank();
					tmmsm19["PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
					tmmsm19["SM_PLAN_NOL2"] = sm_plan_no2;
					tmmsm19["PONO"] = tmmsm27["PONO"].ToString().Trim();
					//update计划表 王程宇提出
					tpssm12z["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tpssm12z["PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
					tpssm12z["HEAT_NO"] = heat_no;
					tpssm12z["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tpssm12z["ST_NO"] = tmmsm27["ST_NO"].ToString().Trim();
					tpssm12z.Update("PONO,SM_PLAN_NO,HEAT_NO,SM_PLAN_NOL2,ST_NO", "PROC_NO");
					if (tmmsm19["HEAT_NO"].ToString() != " "){
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() == " "){
							tmmsm19["HEAT_NO1"] = heat_no;
							tmmsm19["SM_PLAN_NO1"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							tmmsm19.Update("HEAT_NO1,SM_PLAN_NOL2,SM_PLAN_NO1,PROC_NO,PONO", "L2_PROC_NO");
						}
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() != " " &&tmmsm19["HEAT_NO1"].ToString() != heat_no){
							tmmsm19["HEAT_NO2"] = heat_no;
							tmmsm19["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							tmmsm19.Update("HEAT_NO2,SM_PLAN_NOL2,SM_PLAN_NOL2,PROC_NO,PONO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm19["HEAT_NO"] = heat_no;
						tmmsm19["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
						tmmsm19.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PROC_NO,PONO", "L2_PROC_NO");
						tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
						tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
						tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
						tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'Z%' ");
						cmd_2a.ExecuteReader();
						while (cmd_2a.Read())
						{
							tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
							tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
							tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
							tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
							tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
							tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						}
						tmmsm2a.Update("PROC_NO", "PROD_SEQ_NO,HEAT_NO");
						cmd_2a.Close();
					}
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString().Trim();
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString().Trim();
					//doFlag = f_t823s3_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT3"].ToString().SubstringNE(0, 1) == "E"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm20["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
					tmmsm20.Query("L2_PROC_NO");
					tmmsm20.TrimOrBlank();
					tmmsm20["PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
					if (tmmsm20["HEAT_NO"].ToString() != " "){
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() == " "){
							tmmsm20["HEAT_NO1"] = heat_no;
							tmmsm20.Update("HEAT_NO1,PROC_NO", "L2_PROC_NO");
						}
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() != " " &&tmmsm20["HEAT_NO1"].ToString() != heat_no){
							tmmsm20["HEAT_NO2"] = heat_no;
							tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm20["HEAT_NO"] = heat_no;
						tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
					}
					tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm20["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'E%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
					}
					cmd_2a.Close();
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}
					if (!bcls_rec->Tables["T823S"].Columns.Contains("L2_PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "L2_PROC_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm20["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm20["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tmmsm20["L2_PROC_NO"];
					//doFlag = f_t823s5_snd(bcls_rec, bcls_ret, conn);
				}
				else if (tmmsm27["HEATNO_PREMELT3"].ToString().SubstringNE(0, 1) == "B"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm21["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT3"].ToString().Trim();
					tmmsm21.Query("L2_PROC_NO");
					tmmsm21.TrimOrBlank();
					if (tmmsm21["HEAT_NO"].ToString() != " "){
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() == " "){
							tmmsm21["HEAT_NO1"] = heat_no;
							tmmsm21.Update("HEAT_NO1", "L2_PROC_NO");
						}
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() != " " &&tmmsm21["HEAT_NO1"].ToString() != heat_no){
							tmmsm21["HEAT_NO2"] = heat_no;
							tmmsm21.Update("HEAT_NO2", "L2_PROC_NO");
						}
					}
					else{

						if (tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "1" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "4" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 3) == "DeP")
						{
							tmmsm21["HEAT_NO"] = heat_no;
							tmmsm21.Update("HEAT_NO", "L2_PROC_NO");
						}
					}
				}
			}
			//HEATNO_PREMELT4
			Log::Trace("", "HEATNO_PREMELT4", "HEATNO_PREMELT4= {0}", tmmsm27["HEATNO_PREMELT4"].ToString());
			if (tmmsm27["HEATNO_PREMELT4"].ToString() != "" || tmmsm27["HEATNO_PREMELT4"].ToString() != " "){
				if (tmmsm27["HEATNO_PREMELT4"].ToString().SubstringNE(0, 1) == "D"){
					tmmsm14["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm14["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					tmmsm14["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NOL2"].ToString().Trim();
					tmmsm14["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tmmsm14["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tmmsm14.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PONO", "L2_PROC_NO");
					tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'D%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						Log::Trace("", "L2_PROC_NO", "tmmsm2a.L2_PROC_NO = {0}", tmmsm2a["L2_PROC_NO"].ToString());
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");

					}
					cmd_2a.Close();
					//调用智慧质量发送电文
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm14["L2_PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm14["HEAT_NO"];
					//doFlag = f_t823s1_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT4"].ToString().SubstringNE(0, 1) == "F"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm19["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					tmmsm19.Query("L2_PROC_NO");
					tmmsm19.TrimOrBlank();
					tmmsm19["PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					tmmsm19["SM_PLAN_NOL2"] = sm_plan_no2;
					tmmsm19["PONO"] = tmmsm27["PONO"].ToString().Trim();
					//update计划表 王程宇提出
					tpssm12z["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tpssm12z["PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					tpssm12z["HEAT_NO"] = heat_no;
					tpssm12z["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tpssm12z["ST_NO"] = tmmsm27["ST_NO"].ToString().Trim();
					tpssm12z.Update("PONO,SM_PLAN_NO,HEAT_NO,SM_PLAN_NOL2,ST_NO", "PROC_NO");
					if (tmmsm19["HEAT_NO"].ToString() != " "){
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() == " "){
							tmmsm19["HEAT_NO1"] = heat_no;
							tmmsm19["SM_PLAN_NO1"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							tmmsm19.Update("HEAT_NO1,SM_PLAN_NOL2,SM_PLAN_NO1,PROC_NO,PONO", "L2_PROC_NO");
						}
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() != " " &&tmmsm19["HEAT_NO1"].ToString() != heat_no){
							tmmsm19["HEAT_NO2"] = heat_no;
							tmmsm19["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							tmmsm19.Update("HEAT_NO2,SM_PLAN_NOL2,SM_PLAN_NOL2,PROC_NO,PONO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm19["HEAT_NO"] = heat_no;
						tmmsm19["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
						tmmsm19.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PROC_NO,PONO", "L2_PROC_NO");
						tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
						tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
						tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
						tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'Z%' ");
						cmd_2a.ExecuteReader();
						while (cmd_2a.Read())
						{
							tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
							tmmsm2a["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
							tmmsm2a_yl["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
							tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
							tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
							tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						}
						tmmsm2a.Update("PROC_NO", "PROD_SEQ_NO,HEAT_NO");
						cmd_2a.Close();
					}
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString().Trim();
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString().Trim();
					//doFlag = f_t823s3_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT4"].ToString().SubstringNE(0, 1) == "E"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm20["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					tmmsm20.Query("L2_PROC_NO");
					tmmsm20.TrimOrBlank();
					tmmsm20["PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					if (tmmsm20["HEAT_NO"].ToString() != " "){
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() == " "){
							tmmsm20["HEAT_NO1"] = heat_no;
							tmmsm20.Update("HEAT_NO1,PROC_NO", "L2_PROC_NO");
						}
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() != " " &&tmmsm20["HEAT_NO1"].ToString() != heat_no){
							tmmsm20["HEAT_NO2"] = heat_no;
							tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm20["HEAT_NO"] = heat_no;
						tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");	
					}
					tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm20["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'E%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
					}
					cmd_2a.Close();
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}
					if (!bcls_rec->Tables["T823S"].Columns.Contains("L2_PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "L2_PROC_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm20["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm20["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tmmsm20["L2_PROC_NO"];
					//doFlag = f_t823s5_snd(bcls_rec, bcls_ret, conn);
				}
				else if (tmmsm27["HEATNO_PREMELT4"].ToString().SubstringNE(0, 1) == "B"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm21["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					tmmsm21.Query("L2_PROC_NO");
					tmmsm21.TrimOrBlank();
					if (tmmsm21["HEAT_NO"].ToString() != " "){
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() == " "){
							tmmsm21["HEAT_NO1"] = heat_no;
							tmmsm21.Update("HEAT_NO1", "L2_PROC_NO");
						}
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() != " " &&tmmsm21["HEAT_NO1"].ToString() != heat_no){
							tmmsm21["HEAT_NO2"] = heat_no;
							tmmsm21.Update("HEAT_NO2", "L2_PROC_NO");
						}
					}
					else{
						if (tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "1" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "4" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 3) == "DeP")
						{
							tmmsm21["HEAT_NO"] = heat_no;
							tmmsm21.Update("HEAT_NO", "L2_PROC_NO");
						}
					}
				}
			}
			//HEATNO_PREMELT5
			Log::Trace("", "HEATNO_PREMELT5", "HEATNO_PREMELT5= {0}", tmmsm27["HEATNO_PREMELT5"].ToString());
			if (tmmsm27["HEATNO_PREMELT5"].ToString() != "" || tmmsm27["HEATNO_PREMELT5"].ToString() != " "){
				if (tmmsm27["HEATNO_PREMELT5"].ToString().SubstringNE(0, 1) == "D"){
					tmmsm14["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm14["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT5"].ToString().Trim();
					tmmsm14["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NOL2"].ToString().Trim();
					tmmsm14["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tmmsm14["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tmmsm14.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PONO", "L2_PROC_NO");
					tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'D%' ");
					cmd_2a.ExecuteReader();
					Log::Trace("", "HEATNO_PREMELT5", "HEATNO_PREMELT5= {0}", tmmsm27["HEATNO_PREMELT5"].ToString());
					while (cmd_2a.Read())
					{
						Log::Trace("", "L2_PROC_NO", "tmmsm2a.L2_PROC_NO = {0}", tmmsm2a["L2_PROC_NO"].ToString());
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm27["HEATNO_PREMELT1"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						Log::Trace("", "HEATNO_PREMELT5", "HEATNO_PREMELT5= {0}", tmmsm27["HEATNO_PREMELT5"].ToString());
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						Log::Trace("", "HEATNO_PREMELT5", "HEATNO_PREMELT5= {0}", tmmsm27["HEATNO_PREMELT5"].ToString());

					}
					cmd_2a.Close();
					//调用智慧质量发送电文
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm14["L2_PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm14["HEAT_NO"];
					//doFlag = f_t823s1_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT5"].ToString().SubstringNE(0, 1) == "F"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm19["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT5"].ToString().Trim();
					tmmsm19.Query("L2_PROC_NO");
					tmmsm19.TrimOrBlank();
					tmmsm19["PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					tmmsm19["SM_PLAN_NOL2"] = sm_plan_no2;
					tmmsm19["PONO"] = tmmsm27["PONO"].ToString().Trim();
					//update计划表 王程宇提出
					tpssm12z["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tpssm12z["PROC_NO"] = tmmsm27["HEATNO_PREMELT5"].ToString().Trim();
					tpssm12z["HEAT_NO"] = heat_no;
					tpssm12z["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tpssm12z["ST_NO"] = tmmsm27["ST_NO"].ToString().Trim();
					tpssm12z.Update("PONO,SM_PLAN_NO,HEAT_NO,SM_PLAN_NOL2,ST_NO", "PROC_NO");
					Log::Trace("", "HEATNO_PREMELT5", "HEATNO_PREMELT5= {0}", tmmsm27["HEATNO_PREMELT5"].ToString());
					if (tmmsm19["HEAT_NO"].ToString() != " "){
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() == " "){
							tmmsm19["HEAT_NO1"] = heat_no;
							tmmsm19["SM_PLAN_NO1"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							Log::Trace("", "HEAT_NO1", "HEAT_NO1= {0}", tmmsm27["HEAT_NO"].ToString());
							tmmsm19.Update("HEAT_NO1,SM_PLAN_NOL2,SM_PLAN_NO1,PROC_NO,PONO", "L2_PROC_NO");
							Log::Trace("", "HEAT_NO", "HEAT_NO2= {0}", tmmsm27["HEAT_NO"].ToString());
						}
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() != " " &&tmmsm19["HEAT_NO1"].ToString() != heat_no){
							tmmsm19["HEAT_NO2"] = heat_no;
							tmmsm19["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							//20250508下面update中列名重复修改
							tmmsm19.Update("HEAT_NO2,SM_PLAN_NOL2,SM_PLAN_NO1,PROC_NO,PONO", "L2_PROC_NO");
							Log::Trace("", "HEATNO_PREMELT5", "HEATNO_1= {0}", tmmsm19["HEAT_NO1"].ToString());
						}
					}
					else{
						tmmsm19["HEAT_NO"] = heat_no;
						tmmsm19["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
						Log::Trace("", "HEATNO_PREMELT5", "HEATNO_PREMELT5= {0}", tmmsm27["HEATNO_PREMELT5"].ToString());
						tmmsm19.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PROC_NO,PONO", "L2_PROC_NO");
						tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
						tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT5"].ToString().Trim();
						tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
						tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'Z%' ");
						cmd_2a.ExecuteReader();
						while (cmd_2a.Read())
						{
							tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
							tmmsm2a["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
							tmmsm2a_yl["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
							tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
							tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
							tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
							Log::Trace("", "HEATNO_PREMELT5", "HEATNO_PREMELT5= {0}", tmmsm27["HEATNO_PREMELT5"].ToString());
						}
						tmmsm2a.Update("PROC_NO", "PROD_SEQ_NO,HEAT_NO");
						Log::Trace("", "HEATNO_PREMELT5", "HEATNO_PREMELT5= {0}", tmmsm27["HEATNO_PREMELT5"].ToString());
						cmd_2a.Close();
					}
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString().Trim();
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString().Trim();
					//doFlag = f_t823s3_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT5"].ToString().SubstringNE(0, 1) == "E"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm20["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT5"].ToString().Trim();
					tmmsm20.Query("L2_PROC_NO");
					tmmsm20.TrimOrBlank();
					tmmsm20["PROC_NO"] = tmmsm27["HEATNO_PREMELT5"].ToString().Trim();
					if (tmmsm20["HEAT_NO"].ToString() != " "){
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() == " "){
							tmmsm20["HEAT_NO1"] = heat_no;
							tmmsm20.Update("HEAT_NO1,PROC_NO", "L2_PROC_NO");
						}
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() != " " &&tmmsm20["HEAT_NO1"].ToString() != heat_no){
							tmmsm20["HEAT_NO2"] = heat_no;
							tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm20["HEAT_NO"] = heat_no;
						tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
					}
					tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT5"].ToString().Trim();
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
					cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm20["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'E%' ");
					cmd_2a.ExecuteReader();
					while (cmd_2a.Read())
					{
						tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
						tmmsm2a["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						tmmsm2a_yl["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
						tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
					}
					cmd_2a.Close();
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}
					if (!bcls_rec->Tables["T823S"].Columns.Contains("L2_PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "L2_PROC_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm20["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm20["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tmmsm20["L2_PROC_NO"];
					//doFlag = f_t823s5_snd(bcls_rec, bcls_ret, conn);
				}
				else if (tmmsm27["HEATNO_PREMELT5"].ToString().SubstringNE(0, 1) == "B"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm21["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT5"].ToString().Trim();
					tmmsm21.Query("L2_PROC_NO");
					tmmsm21.TrimOrBlank();
					if (tmmsm21["HEAT_NO"].ToString() != " "){
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() == " "){
							tmmsm21["HEAT_NO1"] = heat_no;
							tmmsm21.Update("HEAT_NO1", "L2_PROC_NO");
						}
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() != " " &&tmmsm21["HEAT_NO1"].ToString() != heat_no){
							tmmsm21["HEAT_NO2"] = heat_no;
							tmmsm21.Update("HEAT_NO2", "L2_PROC_NO");
						}
					}
					else{
						if (tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "1" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "4" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 3) == "DeP")
						{
							tmmsm21["HEAT_NO"] = heat_no;
							tmmsm21.Update("HEAT_NO", "L2_PROC_NO");
						}
					}
				}
			}
			//HEATNO_PREMELT6
			Log::Trace("", "HEATNO_PREMELT6", "HEATNO_PREMELT6= {0}", tmmsm27["HEATNO_PREMELT6"].ToString());
			if (tmmsm27["HEATNO_PREMELT6"].ToString() != "" || tmmsm27["HEATNO_PREMELT6"].ToString() != " "){
				if (tmmsm27["HEATNO_PREMELT6"].ToString().SubstringNE(0, 1) == "D"){
					tmmsm14["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm14["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT6"].ToString().Trim();
					tmmsm14["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NOL2"].ToString().Trim();
					tmmsm14["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tmmsm14["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tmmsm14.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PONO", "L2_PROC_NO");
					//调用智慧质量发送电文
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm14["L2_PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm14["HEAT_NO"];
					//doFlag = f_t823s1_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT6"].ToString().SubstringNE(0, 1) == "F"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm19["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT6"].ToString().Trim();
					tmmsm19.Query("L2_PROC_NO");
					tmmsm19.TrimOrBlank();
					tmmsm19["PROC_NO"] = tmmsm27["HEATNO_PREMELT4"].ToString().Trim();
					tmmsm19["SM_PLAN_NOL2"] = sm_plan_no2;
					//update计划表 王程宇提出
					tpssm12z["PONO"] = tmmsm27["PONO"].ToString().Trim();
					tpssm12z["PROC_NO"] = tmmsm27["HEATNO_PREMELT6"].ToString().Trim();
					tpssm12z["HEAT_NO"] = heat_no;
					tpssm12z["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
					tpssm12z["ST_NO"] = tmmsm27["ST_NO"].ToString().Trim();
					tpssm12z.Update("PONO,SM_PLAN_NO,HEAT_NO,SM_PLAN_NOL2,ST_NO", "PROC_NO");
					if (tmmsm19["HEAT_NO"].ToString() != " "){
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() == " "){
							tmmsm19["HEAT_NO1"] = heat_no;
							tmmsm19["SM_PLAN_NO1"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							tmmsm19.Update("HEAT_NO1,SM_PLAN_NOL2,SM_PLAN_NO1,PROC_NO", "L2_PROC_NO");
						}
						if (tmmsm19["HEAT_NO"].ToString() != heat_no && tmmsm19["HEAT_NO1"].ToString() != " " &&tmmsm19["HEAT_NO1"].ToString() != heat_no){
							tmmsm19["HEAT_NO2"] = heat_no;
							tmmsm19["SM_PLAN_NOL2"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
							tmmsm19.Update("HEAT_NO2,SM_PLAN_NOL2,SM_PLAN_NOL2,PROC_NO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm19["HEAT_NO"] = heat_no;
						tmmsm19["SM_PLAN_NO"] = tmmsm27["SM_PLAN_NO"].ToString().Trim();
						tmmsm19.Update("HEAT_NO,SM_PLAN_NOL2,SM_PLAN_NO,PROC_NO", "L2_PROC_NO");
						tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
						tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT6"].ToString().Trim();
						tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
						tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm19["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'Z%' ");
						cmd_2a.ExecuteReader();
						while (cmd_2a.Read())
						{
							tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
							tmmsm2a["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
							tmmsm2a_yl["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
							tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
							tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
							tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						}
						tmmsm2a.Update("PROC_NO", "PROD_SEQ_NO,HEAT_NO");
						cmd_2a.Close();
					}
					blkNum = bcls_rec->Tables.IndexOf("T823S");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("T823S");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
					}

					if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
					{
						bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
					}

					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString().Trim();
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString().Trim();
					//doFlag = f_t823s3_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT6"].ToString().SubstringNE(0, 1) == "E"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm20["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT6"].ToString().Trim();
					tmmsm20.Query("L2_PROC_NO");
					tmmsm20.TrimOrBlank();
					tmmsm20["PROC_NO"] = tmmsm27["HEATNO_PREMELT6"].ToString().Trim();
					if (tmmsm20["HEAT_NO"].ToString() != " "){
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() == " "){
							tmmsm20["HEAT_NO1"] = heat_no;
							tmmsm20.Update("HEAT_NO1,PROC_NO", "L2_PROC_NO");
						}
						if (tmmsm20["HEAT_NO"].ToString() != heat_no && tmmsm20["HEAT_NO1"].ToString() != " " &&tmmsm20["HEAT_NO1"].ToString() != heat_no){
							tmmsm20["HEAT_NO2"] = heat_no;
							tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
						}
					}
					else{
						tmmsm20["HEAT_NO"] = heat_no;
						tmmsm20.Update("HEAT_NO,PROC_NO", "L2_PROC_NO");
						tmmsm2a["HEAT_NO"] = tmmsm27["HEAT_NO"].ToString().Trim();
						tmmsm2a["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT6"].ToString().Trim();
						tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString().Trim();
						tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
						cmd_2a.SetCommandText(" select PROD_SEQ_NO from TMMSM2A WHERE L2_PROC_NO='" + tmmsm20["L2_PROC_NO"].ToString() + "' AND  DEV_CODE LIKE 'E%' ");
						cmd_2a.ExecuteReader();
						while (cmd_2a.Read())
						{
							tmmsm2a["PROD_SEQ_NO"] = cmd_2a.GetString(1);
							tmmsm2a["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
							tmmsm2a_yl["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString().Trim();
							tmmsm2a_yl["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString().Trim();
							tmmsm2a.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
							tmmsm2a_yl.Update("PROC_NO,HEAT_NO", "PROD_SEQ_NO,L2_PROC_NO");
						}
						cmd_2a.Close();
						blkNum = bcls_rec->Tables.IndexOf("T823S");
						if (blkNum < 0)
						{
							bcls_rec->Tables.Add("T823S");
						}

						if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
						{
							bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
						}

						if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
						{
							bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
						}

						if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
						{
							bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
						}
						if (!bcls_rec->Tables["T823S"].Columns.Contains("L2_PROC_NO"))
						{
							bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "L2_PROC_NO");
						}

						bcls_rec->Tables["T823S"].Rows.Add();
						bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
						bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm20["PROC_NO"];
						bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm20["HEAT_NO"];
						bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tmmsm20["L2_PROC_NO"];
						//doFlag = f_t823s5_snd(bcls_rec, bcls_ret, conn);
					}
				}
				else if (tmmsm27["HEATNO_PREMELT6"].ToString().SubstringNE(0, 1) == "B"){
					CString heat_no = tmmsm27["HEAT_NO"].ToString().Trim();
					tmmsm21["L2_PROC_NO"] = tmmsm27["HEATNO_PREMELT6"].ToString().Trim();
					tmmsm21.Query("L2_PROC_NO");
					tmmsm21.TrimOrBlank();
					if (tmmsm21["HEAT_NO"].ToString() != " "){
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() == " "){
							tmmsm21["HEAT_NO1"] = heat_no;
							tmmsm21.Update("HEAT_NO1", "L2_PROC_NO");
						}
						if (tmmsm21["HEAT_NO"].ToString() != heat_no && tmmsm21["HEAT_NO1"].ToString() != " " &&tmmsm21["HEAT_NO1"].ToString() != heat_no){
							tmmsm21["HEAT_NO2"] = heat_no;
							tmmsm21.Update("HEAT_NO2", "L2_PROC_NO");
						}
					}
					else{
						if (tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "1" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 1) == "4" || tmmsm21["ST_NO"].ToString().SubstringNE(0, 3) == "DeP")
						{
							tmmsm21["HEAT_NO"] = heat_no;
							tmmsm21.Update("HEAT_NO", "L2_PROC_NO");
						}
					}
				}
			}
		}

		//备注
		tmmsm27["REMARK"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_AOD"].Rows[0]["COMMENT_POST"].ToString().Trim();

		tmmsm27["REC_CREATOR"] = s.userid;
		tmmsm27["REC_CREATE_TIME"] = datetime;

		//查询熔炼号
		/*cmd_inq.SetCommandText("SELECT HEAT_NO FROM TPSSM12 WHERE PROC_NO =@PROC_NO");
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("PROC_NO", tmmsm27["PROC_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm27["HEAT_NO"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();*/


		//计算冶炼时长
		cmd_yl.SetCommandText(
			" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm27["TAP_END_TIME"].ToString()+ "', 'YYYYMMDDhh24miss') - "
			" TO_DATE('" + tmmsm27["START_TIME"].ToString() + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
			" from dual ");
		cmd_yl.ExecuteReader();
		Log::Trace("", "PROC_NO", "冶炼周期上一炉号:PROC_NO[{0}]", proc_no);
		if (cmd_yl.Read())
		{
			tmmsm27["MELT_DURATION"] = cmd_yl.GetDecimal(1);
		}
		cmd_yl.Close();
		//tmmsm27["MELT_DURATION"] = tmmsm27["TAP_END_TIME"].ToDateTime() - tmmsm27["START_TIME"].ToDateTime();
		Log::Trace("", "MELT_DURATION", "秒:MELT_DURATION[{0}]", tmmsm27["MELT_DURATION"].ToString());
		if (tmmsm27["MELT_DURATION"].ToDecimal() != 0){
			tmmsm27["MELT_DURATION"] = tmmsm27["MELT_DURATION"].ToDecimal() / 60;
			tmmsm27["MELT_DURATION"] = tmmsm27["MELT_DURATION"].ToDecimal().Round(2);
			Log::Trace("", "MELT_DURATION", "分:MELT_DURATION[{0}]", tmmsm27["MELT_DURATION"].ToString());
		}

		//计算冶炼周期
		proc_no = tmmsm27["PROC_NO"].ToString() - 1;
		cmd_yz.SetCommandText(
			" SELECT  END_TIME_REAL FROM  TPSSM12 WHERE DEV_CODE = '" + dev_code + "' AND PROC_NO  = '" + proc_no + "' "
			" UNION ALL "
			" SELECT  END_TIME_REAL FROM  TPSSM42 WHERE DEV_CODE = '" + dev_code + "' AND PROC_NO = '" + proc_no + "' ");
		cmd_yz.ExecuteReader();
		Log::Trace("", "PROC_NO", "冶炼周期上一炉号:PROC_NO[{0}]", proc_no);
		if (cmd_yz.Read())
		{
			end_time_trp = cmd_yz.GetString(1);
		}
		Log::Trace("", "end_time_trp", "冶炼周期上一炉出钢结束时间:end_time_trp[{0}]", end_time_trp);
		cmd_yz.Close();
		if (end_time_trp != " "){
			cmd_yzsc.SetCommandText(
				" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm27["TAP_END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
				" TO_DATE('" + end_time_trp + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
				" from dual ");
			cmd_yzsc.ExecuteReader();
			if (cmd_yzsc.Read())
			{
				tmmsm27["SMELT_CYCLE"] = cmd_yzsc.GetDecimal(1);
			}
			cmd_yzsc.Close();
			if (tmmsm27["SMELT_CYCLE"].ToDecimal() != 0){
				tmmsm27["SMELT_CYCLE"] = tmmsm27["SMELT_CYCLE"].ToDecimal() / 60;
				tmmsm27["SMELT_CYCLE"] = tmmsm27["SMELT_CYCLE"].ToDecimal().Round(2);
				Log::Trace("", "SMELT_CYCLE", "冶炼周期:SMELT_CYCLE[{0}]", tmmsm27["SMELT_CYCLE"].ToString());
			}
		}

		tmmsm27.MergeTo(tmmsm27_back.Tables[0], false);
		/*if (tmmsm27["PROC_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "处理号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		Log::Info("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm27["PROC_NO"].ToString());

		f_tableObjectCheck9999(tmmsm27);
		if (tmmsm27.QueryCount("L2_PROC_NO,SM_PLAN_NOL2"))
		{
			Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "U");
			tmmsm27_back.Tables[0].Rows[0]["PROC_DIV"] = "U";
			tmmsm27_back.Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";
			tmmsm27_back.Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";
			doFlag = f_mmsm27_proc(&tmmsm27_back, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}
		else
		{
			Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "I");
			tmmsm27.Print();
			tmmsm27_back.Tables[0].Rows[0]["PROC_DIV"] = "I";
			tmmsm27_back.Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";
			tmmsm27_back.Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";
			doFlag = f_mmsm27_proc(&tmmsm27_back, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//tmmsm27.Insert();
		}
		ttmsm12["HEAT_NO"] = tmmsm27["HEAT_NO"];
		ttmsm12["OUT_STEEL_TIME"] = tmmsm27["TAP_START_TIME"];
		ttmsm12["LADLE_GROSS_WT"] = tmmsm27["LADLE_DEPART_WT"];
		ttmsm12["ST_NO"] = tmmsm27["ST_NO"];
		if (ttmsm12.QueryCount("HEAT_NO") > 0)
		{
			ttmsm12.Update("LADLE_GROSS_WT,OUT_STEEL_TIME,ST_NO", "HEAT_NO");
		}
		//王程宇提出的逻辑
		if (tmmsm27["LADLENO_PREMELT1"].ToString()!=" "){
			cmd_tpssm.SetCommandText(" select * from TPSSM12ZT where LADLE_NO='" + tmmsm27["LADLENO_PREMELT1"].ToString() + "' ");
			cmd_tpssm.ExecuteReader();
			if (cmd_tpssm.Read()){
				cmd_tpssm.Fetch(tpssm12zt);
				if (tpssm12zt["PROC_NO"].ToString() == tmmsm27["HEATNO_PREMELT1"].ToString() || tpssm12zt["PROC_NO"].ToString() == tmmsm27["HEATNO_PREMELT2"].ToString() || tpssm12zt["PROC_NO"].ToString() == tmmsm27["HEATNO_PREMELT3"].ToString()){
					tpssm12zt["PROC_NO"] = " ";
					tpssm12zt.Update("PROC_NO","LADLE_NO");
				}
				if (tpssm12zt["PROC_NO2"].ToString() == tmmsm27["HEATNO_PREMELT1"].ToString() || tpssm12zt["PROC_NO2"].ToString() == tmmsm27["HEATNO_PREMELT2"].ToString() || tpssm12zt["PROC_NO2"].ToString() == tmmsm27["HEATNO_PREMELT3"].ToString()){
					tpssm12zt["PROC_NO2"] = " ";
					tpssm12zt.Update("PROC_NO2", "LADLE_NO");
				}
				if (tpssm12zt["PROC_NO3"].ToString() == tmmsm27["HEATNO_PREMELT1"].ToString() || tpssm12zt["PROC_NO3"].ToString() == tmmsm27["HEATNO_PREMELT2"].ToString() || tpssm12zt["PROC_NO3"].ToString() == tmmsm27["HEATNO_PREMELT3"].ToString()){
					tpssm12zt["PROC_NO3"] = " ";
					tpssm12zt.Update("PROC_NO3", "LADLE_NO");
				}
				if (tpssm12zt["PROC_NO4"].ToString() == tmmsm27["HEATNO_PREMELT1"].ToString() || tpssm12zt["PROC_NO4"].ToString() == tmmsm27["HEATNO_PREMELT2"].ToString() || tpssm12zt["PROC_NO4"].ToString() == tmmsm27["HEATNO_PREMELT3"].ToString()){
					tpssm12zt["PROC_NO4"] = " ";
					tpssm12zt.Update("PROC_NO4", "LADLE_NO");
				}
			}
		}
		//sprintf(s.msg, "%d条记录新增成功！请重新查询！");

		//规则引擎
		EIClass iblk_yq;
		if (iblk_yq.Tables.Contains("RULE_CONFIG") == false)
		{
			iblk_yq.Tables[0].set_TableName("RULE_CONFIG");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "PROJECT_ENAME");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "ENV_TYPE");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "VERSION");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "CUSTOM_CONFIG");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "CODE_CLASS");
			iblk_yq.Tables["RULE_CONFIG"].Rows.Add();
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["PROJECT_ENAME"] = "TASK";//固定值，不变
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["ENV_TYPE"] = "0";//测试--0，正式--1
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["VERSION"] = "20241201";//固定值，不变
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["CUSTOM_CONFIG"] = "T";//固定值，不变
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["CODE_CLASS"] = "EPIJG0";//固定值，不变
		}
		if (iblk_yq.Tables.Contains("PROJECT_CONFIG") == false)
		{
			iblk_yq.Tables.Add();
			iblk_yq.Tables[1].set_TableName("PROJECT_CONFIG");
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "MESSAGE_CLASS");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "UNIT_CODE");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "MAT_NO");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "HEAT_NO");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "DEV_CODE");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "ST_NO");//三列必须有
		}
		if (iblk_yq.Tables.Contains("DATA_CUSTOM") == false)
		{
			Log::Trace("", "", "inBlock 初始化表3为 DATA_CUSTOM ");

			iblk_yq.Tables.Add("DATA_CUSTOM");
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "UNIT_CODE");//该列必须有
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "HEAT_NO");//参与计算的列
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "DEV_CODE");//参与计算的列
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "ST_NO");//参与计算的列
		}

		iblk_yq.Tables["PROJECT_CONFIG"].Rows.Add();
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["MESSAGE_CLASS"] = "SJAOD01";//任务池准入条件的比对值
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["UNIT_CODE"] = "H000";//不定机组
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["HEAT_NO"] = heat_no;
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["DEV_CODE"] = dev_code;
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["ST_NO"] = st_no;


		iblk_yq.Tables["DATA_CUSTOM"].Rows.Add();
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["UNIT_CODE"] = "H000";//不定机组
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["HEAT_NO"] = heat_no;
		Log::Trace("", "", "heat_no={0}", heat_no);
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["DEV_CODE"] = dev_code;
		Log::Trace("", "", "dev_code={0}", dev_code);
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["ST_NO"] = st_no;
		Log::Trace("", "", "st_no={0}", st_no);

		/*doFlag = f_qmts_xxyq(&iblk_yq, bcls_ret, conn);
		if (doFlag < 0)
		{
		Log::Trace("", "", "判定结果保存失败 ");
		doFlag = 0;
		s.flag = 0;
		}
		Log::Trace("", "", "判定结果保存成功 ");*/

		doFlag = f_qmts_call_judge(&iblk_yq, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", "", "引擎失败 ");
			doFlag = 0;
			s.flag = 0;
		}
		Log::Trace("", "", "引擎成功 ");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


