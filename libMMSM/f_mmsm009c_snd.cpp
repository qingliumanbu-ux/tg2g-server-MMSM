/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-11 14:13:28
Description: 集控大屏数据添加
**************************************************/

#include "stdafx.h"
#include "epex.h"
int f_tableObjectCheck9999(ITableObject2& obj);//字段超长检测

BM2_FUNCTION_EXPORT
int f_mmsm009c_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_sql(conn);
	//cmd_inq
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_insert(conn);
	CDbCommand cmd_sql_count(conn);
	CDbCommand cmd_sql_delete(conn);
	CDbCommand cmd_sql_insert(conn);
	CString datetime(" ");
	CString table_name = " ";
	CString v_proc_div = " ";
	CString sql_insert = "";
	CModel tmmsm27("TMMSM27");
	CModel tmmsm21("TMMSM21");
	CModel tmmsm20("TMMSM20");
	CModel tmmsm24("TMMSM24");
	CModel tmmsm26("TMMSM26");
	CModel tmmsm23("TMMSM23");
	CModel tmmsm25("TMMSM25");
	CModel tmmsm31("TMMSM31");
	CModel tmmsm33("TMMSM33");
	CModel tmmsm14("TMMSM14");
	CModel tmmsm19("TMMSM19");
	CModel aod("DA_AOD_PRO_SUMMARY");
	CModel bof("DA_BOF_PRO_SUMMARY");//转炉
	CModel eaf("DA_EAF_PRO_SUMMARY");//电炉
	CModel lf("DA_LF_PRO_SUMMARY");//LF
	CModel lts("DA_LTS_PROD_SUMMARY");//LTS
	CModel rh("DA_RH_PRO_SUMMARY");//RH
	CModel vod("DA_VOD_PRO_SUMMARY");//VOD
	CModel ccm("DA_CCM_PRO_SUMMARY");//连铸
	CModel slab("DA_CCM_SLAB_SUMMARY");//切断
	CModel des("DA_DES_PRO_SUMMARY01");//脱硫
	CModel lif("DA_IF_PRO_SUMMARY");//IF

	//CDateTime datetime = CDateTime::Now();

	//DateTime convertedDate = DateTime.Parse(dateString);

	try
	{
		table_name = bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "table_name=[{0}]", table_name);
		v_proc_div = bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"].ToString().Trim();
		if (table_name == "DA_AOD_PRO_SUMMARY"){
			
			tmmsm27["L2_PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"].ToString().Trim();
			tmmsm27["HEAT_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"].ToString().Trim();
			tmmsm27["ID_SJ"] = bcls_rec->Tables["JKDP"].Rows[0]["ID_SJ"].ToString().Trim();
			tmmsm27.Query("HEAT_NO,L2_PROC_NO");
			Log::Trace("", __FUNCTION__, "HEAT_NO=[{0}]", tmmsm27["HEAT_NO"].ToString());
			tmmsm27.TrimOrBlank();
			aod["HEATNUMBER"] = tmmsm27["L2_PROC_NO"].ToString().Trim();

			if (v_proc_div == "I" || v_proc_div == "U"){
				if (aod.QueryCount("HEATNUMBER"))
				{
					aod.Delete("HEATNUMBER");
				}
				//关键字--未定
				aod["ID"] = tmmsm27["ID_SJ"].ToString();
				//工位名
				aod["AGGREGATECODE"] = tmmsm27["DEV_CODE"].ToString();
				//炉次号
				aod["HEATNUMBER"] = tmmsm27["L2_PROC_NO"].ToString();
				//计划号
				aod["PLANID"] = tmmsm27["SM_PLAN_NOL2"].ToString();
				//分包标志
				aod["SPLITINDICATION"] = tmmsm27["SPLIT_INDICATION"].ToString();
				Log::Trace("", __FUNCTION__, "SPLITINDICATION=[{0}]", aod["SPLITINDICATION"].ToString());
				//处理次数
				aod["TREATMENTCOUNTER"] = tmmsm27["SAME_PROC_NUM"].ToString();
				//空包重量
				aod["EMPTYLADLEWEIGHT"] = tmmsm27["EMPTY_LADLE_WEIGHT"].ToDecimal() * 1000;
				//班
				aod["SHIFTTEAM"] = tmmsm27["PROD_SHIFT_GROUP"].ToString();
				//操作员
				aod["OPERATOR"] = tmmsm27["ASSISTANT"].ToString();
				//实际钢种
				aod["GRADEACT"] = tmmsm27["ST_NO"].ToString();
				Log::Trace("", __FUNCTION__, "GRADEACT=[{0}]", aod["GRADEACT"].ToString());
				//实际出钢量
				aod["WEIGHTACT"] = tmmsm27["ACTRESULT"].ToDecimal() * 1000;
				Log::Trace("", __FUNCTION__, "WEIGHTACT=[{0}]", aod["WEIGHTACT"].ToString());
				//EAF钢量(预溶液量)
				if (tmmsm27["LADLE_PRE_LIQUID_WT"].ToString() == " "){
					tmmsm27["LADLE_PRE_LIQUID_WT"] = "0";
				}
				Log::Trace("", __FUNCTION__, "LADLE_PRE_LIQUID_WT=[{0}]", tmmsm27["LADLE_PRE_LIQUID_WT"].ToString());
				aod["EAFWEIGHT"] = tmmsm27["LADLE_PRE_LIQUID_WT"].ToDecimal() * 1000;
				Log::Trace("", __FUNCTION__, "EAFWEIGHT=[{0}]", aod["EAFWEIGHT"].ToString());
				//废钢重量
				aod["SCRAPWEIGHT"] = tmmsm27["SCRAP_WEIGHT"].ToDecimal() * 1000;
				Log::Trace("", __FUNCTION__, "SCRAPWEIGHT=[{0}]", aod["SCRAPWEIGHT"].ToString());
				//炉次开始时间
				Log::Trace("", __FUNCTION__, "START_TIME=[{0}]", tmmsm27["START_TIME"].ToString());
				if (tmmsm27["START_TIME"].ToString() != " "){
					CDateTime start_time = CDateTime::Parse(tmmsm27["START_TIME"].ToString());
					Log::Trace("", __FUNCTION__, "start_time=[{0}]", start_time.ToString());
					aod["HEATSTART"] = start_time;
				}
				else{
					aod["HEATSTART"] = "19000101000000";
				}
				Log::Trace("", __FUNCTION__, "start_time=[{0}]", aod["HEATSTART"].ToString());
				//炉次结束时间
				if (tmmsm27["END_TIME"].ToString() != " "){
					CDateTime end_time = CDateTime::Parse(tmmsm27["END_TIME"].ToString());
					aod["HEATEND"] = end_time;
				}
				else{
					aod["HEATEND"] = "19000101000000";
				}
				//吹氧次数
				aod["BLOWNUMBER"] = tmmsm27["BLOW_NUMBER"].ToString();
				//吹氧时间
				aod["BLOWTIME"] = tmmsm27["BLOW_DURATION"].ToDecimal() * 60;
				//吹氧量
				aod["OXYGENTOT"] = tmmsm27["OXYGEN_FINAL"].ToString();
				Log::Trace("", __FUNCTION__, "吹氧量=[{0}]", aod["OXYGENTOT"].ToString());
				//第一次吹氧开始时间
				if (tmmsm27["BLOW_START_TIME"].ToString() != " "){
					CDateTime blow_start_time = CDateTime::Parse(tmmsm27["BLOW_START_TIME"].ToString());
					aod["BLOWSTART"] = blow_start_time;
				}
				else{
					aod["BLOWSTART"] = "19000101000000";
				}
				//最后一次吹氧结束时间
				if (tmmsm27["BLOW_END_TIME"].ToString() != " "){
					CDateTime blow_end_time = CDateTime::Parse(tmmsm27["BLOW_END_TIME"].ToString());
					aod["BLOWEND"] = blow_end_time;
				}
				else{
					aod["BLOWEND"] = "19000101000000";
				}
				//溅渣次数
				aod["SLAGSPLASHINGNUMBER"] = tmmsm27["SLAG_SPLASHING_NUMBER"].ToString();
				Log::Trace("", __FUNCTION__, "SLAGSPLASHINGNUMBER=[{0}]", aod["SLAGSPLASHINGNUMBER"].ToString());
				//顶底吹氮量
				if (tmmsm27["NITROGEN_TOT"].ToString() == " "){
					aod["NITROGENTOT"] = "0";
				}
				else{
					aod["NITROGENTOT"] = tmmsm27["NITROGEN_TOT"].ToString();
				}
				Log::Trace("", __FUNCTION__, "NITROGENTOT=[{0}]", aod["NITROGENTOT"].ToString());
				//顶底吹氩量
				if (tmmsm27["TOTAL_AR_CONS"].ToString() == " "){
					tmmsm27["TOTAL_AR_CONS"] = "0";
				}
				else{
					aod["ARGONTOT"] = tmmsm27["TOTAL_AR_CONS"].ToString();
				}
				Log::Trace("", __FUNCTION__, "ARGONTOT=[{0}]", aod["ARGONTOT"].ToString());
				//溅渣开始时间
				if (tmmsm27["SLAG_START_TIME"].ToString() != " " && tmmsm27["SLAG_START_TIME"].ToDecimal() >= 20000101000000){
					Log::Trace("", __FUNCTION__, "SLAGSPLASHINGSTART=[{0}]", tmmsm27["SLAG_START_TIME"].ToString());
					CDateTime slag_start_time = CDateTime::Parse(tmmsm27["SLAG_START_TIME"].ToString());
					aod["SLAGSPLASHINGSTART"] = slag_start_time;
				}
				else{
					aod["SLAGSPLASHINGSTART"] = "19000101000000";
				}
				//aod["SLAGSPLASHINGSTART"] = tmmsm27["SLAG_START_TIME"].ToString("yyyy-MM-dd HH:mm:ss");
				Log::Trace("", __FUNCTION__, "SLAGSPLASHINGSTART=[{0}]", aod["SLAGSPLASHINGSTART"].ToString());
				//溅渣结束时间
				if (tmmsm27["SLAG_END_TIME"].ToString() != " " && tmmsm27["SLAG_END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime slag_end_time = CDateTime::Parse(tmmsm27["SLAG_END_TIME"].ToString());
					aod["SLAGSPLASHINGEND"] = slag_end_time;
				}
				else{
					aod["SLAGSPLASHINGEND"] = "19000101000000";
				}
				//底吹氩量
				aod["ARGONTUYTOT"] = tmmsm27["AR_SUM_COMSUME"].ToString();
				//出钢开始时间
				if (tmmsm27["TAP_START_TIME"].ToString() != " " && tmmsm27["TAP_START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime tap_start_time = CDateTime::Parse(tmmsm27["TAP_START_TIME"].ToString());
					aod["TAPPINGSTART"] = tap_start_time;
				}
				else{
					aod["TAPPINGSTART"] = "19000101000000";
				}
				//出钢结束时间
				if (tmmsm27["TAP_END_TIME"].ToString() != " " && tmmsm27["TAP_END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime tap_end_time = CDateTime::Parse(tmmsm27["TAP_END_TIME"].ToString());
					aod["TAPPINGEND"] = tap_end_time;
				}
				else{
					aod["TAPPINGEND"] = "19000101000000";
				}
				//除渣开始时间
				if (tmmsm27["DROSSING_START_TIME"].ToString() != " " && tmmsm27["DROSSING_START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime drossing_start_time = CDateTime::Parse(tmmsm27["DROSSING_START_TIME"].ToString());
					aod["SLAGGINGSTART"] = drossing_start_time;
				}
				else{
					aod["SLAGGINGSTART"] = "19000101000000";
				}
				//除渣结束时间
				if (tmmsm27["DROSSING_END_TIME"].ToString() != " " && tmmsm27["DROSSING_END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime drossing_end_time = CDateTime::Parse(tmmsm27["DROSSING_END_TIME"].ToString());
					aod["SLAGGINGEND"] = drossing_end_time;
				}
				else{
					aod["SLAGGINGEND"] = "19000101000000";
				}
				//记录插入时间
				/*Log::Trace("", __FUNCTION__, "datetime=[{0}]", datetime);
				CDateTime timestamp = CDateTime::Parse(tmmsm27["REC_CREATE_TIME"].ToString());
				aod["TIMESTAMP"] = timestamp;
				Log::Trace("", __FUNCTION__, "TIMESTAMP=[{0}]", aod["TIMESTAMP"].ToString());*/
				//原材料管理模块读取时间
				//aod["RMREADTIME"] = tmmsm27[""].ToString();
				////记录读取标志
				//aod["RMREAD"] = tmmsm27[""].ToString();
				////能源介质管理模块读取时间
				//aod["EMREADTIME"] = tmmsm27[""].ToString();
				////能源介质管理模块读取时间
				//aod["EMREADTIME"] = tmmsm27[""].ToString();
				////记录读取标志
				//aod["EMREAD"] = tmmsm27[""].ToString();
				////产品管理模块读取时间
				//aod["PMREADTIME"] = tmmsm27[""].ToString();
				////记录读取标志
				//aod["PMREAD"] = tmmsm27[""].ToString();
				////质量管理模块读取时间
				//aod["QMREADTIME"] = tmmsm27[""].ToString();
				////记录读取标志-质量
				//aod["QMREAD"] = tmmsm27[""].ToString();
				////过程跟踪管理模块读取时间-质量
				//aod["DMREADTIME"] = tmmsm27[""].ToString();
				////记录读取标志-质量
				//aod["DMREAD"] = tmmsm27[""].ToString();
				//预溶液1炉号
				aod["HEATNO_PREMELT1"] = tmmsm27["HEATNO_PREMELT1"].ToString();
				//预溶液1重量
				aod["WEIGHT_PREMELT1"] = tmmsm27["WEIGHT_PREMELT1"].ToDecimal() * 1000;
				//预溶液2炉号
				aod["HEATNO_PREMELT2"] = tmmsm27["HEATNO_PREMELT2"].ToString();
				//预溶液2重量
				aod["WEIGHT_PREMELT2"] = tmmsm27["WEIGHT_PREMELT2"].ToDecimal() * 1000;
				//预溶液3炉号
				aod["HEATNO_PREMELT3"] = tmmsm27["HEATNO_PREMELT3"].ToString();
				//预溶液3重量
				aod["WEIGHT_PREMELT3"] = tmmsm27["WEIGHT_PREMELT3"].ToDecimal() * 1000;
				//AOD炉壳编号
				aod["AOD_SHELL_NO"] = tmmsm27["AOD_SHELL_NO"].ToString();
				//AOD炉壳寿命
				aod["AOD_SHELL_LIFE"] = tmmsm27["AOD_SHELL_LIFE"].ToString();
				//顶枪寿命
				aod["TOP_LANCE_LIFE"] = tmmsm27["TOP_LANCE_LIFE"].ToString();
				//使用副枪次数
				aod["SUB_LANCE_USE_COUNT"] = tmmsm27["SUB_LANCE_USE_COUNT"].ToString();
				//钢包号
				aod["STEEL_LADLE_NO"] = tmmsm27["LADLE_NO"].ToString();
				//包龄
				aod["LADLE_LIFE"] = tmmsm27["LADLE_AGE"].ToString();
				//空包重量(出钢)
				aod["EMPTY_LADLE_WT"] = tmmsm27["EMPTY_LADLE_WEIGHT"].ToDecimal() * 1000;
				//入炉温度1
				aod["CHARGE_TEMP"] = tmmsm27["CHARGE_TEMP"].ToString();
				//处理前渣厚
				aod["SLAG_THICK_START"] = tmmsm27["SLAG_THICK_START"].ToString();
				//SMP模式
				aod["SMP_MODE"] = tmmsm27["SMP_MODE"].ToString();
				//I期温度
				aod["PHASE1_TEMP"] = tmmsm27["FIRST_BG_TEMP"].ToString();
				//I期碳
				aod["PHASE1_C"] = tmmsm27["PHASE1_C"].ToString();
				//吹止温度
				aod["BLOW_END_TEMP"] = tmmsm27["BLOW_END_TEMP"].ToString();
				//吹止碳
				aod["BLOW_END_C"] = tmmsm27["BLOW_END_C"].ToString();
				//还原周期
				aod["REDUCING_DURATION"] = tmmsm27["REDUCING_DURATION"].ToDecimal() * 60;
				//预计算碱度
				aod["CAL_BASICITY"] = tmmsm27["CAL_BASICITY"].ToString();
				//还原碱度(实绩)
				aod["REDUCING_BASICITY"] = tmmsm27["REDUCING_BASICITY"].ToString();
				//还原后温度
				aod["REDUCTION_TEMP"] = tmmsm27["REDUCTION_TEMP"].ToString();
				//还原S
				aod["REDUCTION_S"] = tmmsm27["REDUCTION_S"].ToString();
				//还原Si
				aod["REDUCTION_SI"] = tmmsm27["REDUCTION_SI"].ToString();
				//后吹次数
				aod["REBLOW_COUNT"] = tmmsm27["REBLOW_NUM"].ToString();
				//出钢温度
				aod["TAPPING_TEMP"] = tmmsm27["OUT_STEEL_TEMP"].ToString();
				//冶炼时长
				aod["MELT_DURATION"] = tmmsm27["MELT_DURATION"].ToDecimal() * 60;
				//辅助时长
				aod["ASSIS_DURATION"] = tmmsm27["ASSIS_DURATION"].ToString();
				//出钢周期
				aod["TAPTOTAP_DURATION"] = tmmsm27["TAPTOTAP_DURATION"].ToString();
				//出钢渣厚
				aod["SLAG_THICK_TAPPING"] = tmmsm27["SLAG_THICK_TAPPING"].ToString();
				//出钢重量
				/*aod["TAPPING_WT"] = tmmsm27[""].ToString();*/
				//出站总重量
				aod["LADLE_DEPART_WT"] = tmmsm27["LADLE_DEPART_WT"].ToDecimal() * 1000;
				//钢包钢水温度
				aod["LADLE_TEMP"] = tmmsm27["LADLE_TEMP"].ToString();
				//下道设备号
				aod["NEXT_DEV_CODE"] = tmmsm27["NEXT_DEV_CODE"].ToString();
				//记录读取标志-－产销模块
				//aod["CXSJREAD"] = tmmsm27[""].ToString();
				////产销模块读取时间，由产销模块填入'
				//aod["CXSJREADTIME"] = tmmsm27[""].ToString();
				//顶枪编号
				aod["TOP_LANCE_NO"] = tmmsm27["TOP_LANCE_NO"].ToString();
				//等待周期
				aod["DURATION_WAIT"] = tmmsm27["WAITING_TIME"].ToDecimal() * 60;
				//等待原因
				aod["REMARK_WAIT"] = tmmsm27["WAIT_REASON"].ToString();
				//上炉残钢量
				aod["REMAIN_STEEL_WEIGHT"] = tmmsm27["REMAIN_STEEL_WEIGHT"].ToDecimal() * 1000;
				Log::Trace("", __FUNCTION__, "REMAIN_STEEL_WEIGHT=[{0}]", aod["REMAIN_STEEL_WEIGHT"].ToString());
				////记录读取标志 NY模块
				//aod["NYREAD"] = tmmsm27[""].ToString();
				////NY模块读取时间
				//aod["NYREADTIME"] = tmmsm27[""].ToString();
				//备注
				aod["COMMENT_POST"] = tmmsm27["REMARK"].ToString();
				sql_insert =" insert into DA_AOD_PRO_SUMMARY (ID, AGGREGATECODE, HEATNUMBER, PLANID, SPLITINDICATION, TREATMENTCOUNTER, "
					" EMPTYLADLEWEIGHT, SHIFTTEAM, OPERATOR, GRADEACT, WEIGHTACT, EAFWEIGHT, SCRAPWEIGHT, "
					" HEATSTART, HEATEND, BLOWNUMBER, BLOWTIME, OXYGENTOT, BLOWSTART, BLOWEND, "
					" SLAGSPLASHINGNUMBER, NITROGENTOT, ARGONTOT, SLAGSPLASHINGSTART, SLAGSPLASHINGEND, "
					" ARGONTUYTOT, NITROGENTUYTOT, TAPPINGSTART, TAPPINGEND, SLAGGINGSTART, SLAGGINGEND, "
					"  HEATNO_PREMELT1, WEIGHT_PREMELT1, "
					" HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, AOD_SHELL_NO, "
					" AOD_SHELL_LIFE, TOP_LANCE_LIFE, SUB_LANCE_USE_COUNT, STEEL_LADLE_NO, LADLE_LIFE, "
					" EMPTY_LADLE_WT, CHARGE_TEMP, SLAG_THICK_START, SMP_MODE, PHASE1_TEMP, PHASE1_C, "
					" BLOW_END_TEMP, BLOW_END_C, REDUCING_DURATION, CAL_BASICITY, REDUCING_BASICITY, "
					" REDUCTION_TEMP, REDUCTION_S, REDUCTION_SI, REBLOW_COUNT, TAPPING_TEMP, MELT_DURATION, "
					" ASSIS_DURATION, TAPTOTAP_DURATION, SLAG_THICK_TAPPING, LADLE_DEPART_WT, "
					" LADLE_TEMP, NEXT_DEV_CODE, TOP_LANCE_NO, DURATION_WAIT, "
					" REMARK_WAIT, REMAIN_STEEL_WEIGHT, LADLENO_PREMELT1, LADLENO_PREMELT2, LADLENO_PREMELT3, "
					" COMMENT_POST) "
					" values('" + aod["ID"].ToString() + "','" + aod["AGGREGATECODE"].ToString() + "','" + aod["HEATNUMBER"].ToString() + "','" + aod["PLANID"].ToString() + "','" + aod["SPLITINDICATION"].ToString() + "','" + aod["TREATMENTCOUNTER"].ToString() + "', "
					" '" + aod["EMPTYLADLEWEIGHT"].ToString() + "','" + aod["SHIFTTEAM"].ToString() + "','" + aod["OPERATOR"].ToString() + "','" + aod["GRADEACT"].ToString() + "','" + aod["WEIGHTACT"].ToString() + "','" + aod["EAFWEIGHT"].ToString() + "','" + aod["SCRAPWEIGHT"].ToString() + "',  "
					"  to_date('" + aod["HEATSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + aod["HEATEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + aod["BLOWNUMBER"].ToString() + "','" + aod["BLOWTIME"].ToString() + "','" + aod["OXYGENTOT"].ToString() + "',to_date('" + aod["BLOWSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + aod["BLOWEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),  "
					" '" + aod["SLAGSPLASHINGNUMBER"].ToString() + "','" + aod["NITROGENTOT"].ToString() + "','" + aod["ARGONTOT"].ToString() + "',to_date('" + aod["SLAGSPLASHINGSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + aod["SLAGSPLASHINGEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + aod["ARGONTUYTOT"].ToString() + "','" + tmmsm27["N_SUM_COMSUME"].ToString() + "',to_date('" + aod["TAPPINGSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + aod["TAPPINGEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + aod["SLAGGINGSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + aod["SLAGGINGEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + aod["HEATNO_PREMELT1"].ToString() + "','" + aod["WEIGHT_PREMELT1"].ToString() + "', "
					" '" + aod["HEATNO_PREMELT2"].ToString() + "','" + aod["WEIGHT_PREMELT2"].ToString() + "','" + aod["HEATNO_PREMELT3"].ToString() + "','" + aod["WEIGHT_PREMELT3"].ToString() + "','" + aod["AOD_SHELL_NO"].ToString() + "', "
					" '" + aod["AOD_SHELL_LIFE"].ToString() + "','" + aod["TOP_LANCE_LIFE"].ToString() + "','" + aod["SUB_LANCE_USE_COUNT"].ToString() + "','" + aod["STEEL_LADLE_NO"].ToString() + "','" + aod["LADLE_LIFE"].ToString() + "', "
					" '" + aod["EMPTY_LADLE_WT"].ToString() + "','" + aod["CHARGE_TEMP"].ToString() + "','" + aod["SLAG_THICK_START"].ToString() + "','" + aod["SMP_MODE"].ToString() + "','" + aod["PHASE1_TEMP"].ToString() + "','" + aod["PHASE1_C"].ToString() + "' ,"
					" '" + aod["BLOW_END_TEMP"].ToString() + "','" + aod["BLOW_END_C"].ToString() + "','" + aod["REDUCING_DURATION"].ToString() + "','" + aod["CAL_BASICITY"].ToString() + "','" + aod["REDUCING_BASICITY"].ToString() + "', "
					" '" + aod["REDUCTION_TEMP"].ToString() + "','" + aod["REDUCTION_S"].ToString() + "','" + aod["REDUCTION_SI"].ToString() + "','" + aod["REBLOW_COUNT"].ToString() + "','" + aod["TAPPING_TEMP"].ToString()+"','" + aod["MELT_DURATION"].ToString() + "', "
					" '" + aod["ASSIS_DURATION"].ToString() + "','" + aod["TAPTOTAP_DURATION"].ToString() + "','" + aod["SLAG_THICK_TAPPING"].ToString() + "','" + aod["LADLE_DEPART_WT"].ToString() + "', "
					" '" + aod["LADLE_TEMP"].ToString() + "','" + aod["NEXT_DEV_CODE"].ToString() + "','" + aod["TOP_LANCE_NO"].ToString() + "','" + aod["DURATION_WAIT"].ToString() + "', "
					" '" + aod["REMARK_WAIT"].ToString() + "','" + aod["REMAIN_STEEL_WEIGHT"].ToString() + "','" + tmmsm27["LADLENO_PREMELT1"].ToString() + "','" + tmmsm27["LADLENO_PREMELT2"].ToString() + "','" + tmmsm27["LADLENO_PREMELT3"].ToString() + "', "
					" '" + aod["COMMENT_POST"].ToString() + "' "
					" ) ";
				cmd_insert.SetCommandText(sql_insert);
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.ExecuteNonQuery();
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.Close();
				//aod.Insert();
			}
			else{
				if (aod.QueryCount("HEATNUMBER"))
				{
					aod.Delete("HEATNUMBER");
				}
			}
		}
		else if (table_name == "DA_BOF_PRO_SUMMARY"){
			Log::Trace("", __FUNCTION__, "DA_BOF_PRO_SUMMARY=[111]");
			tmmsm21["L2_PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"].ToString();
			tmmsm21["HEAT_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "DA_BOF_PRO_SUMMARY=[111]");
			Log::Trace("", __FUNCTION__, "L2_PROC_NO=[{0}]", tmmsm21["L2_PROC_NO"].ToString());
			tmmsm21["ID_SJ"] = bcls_rec->Tables["JKDP"].Rows[0]["ID_SJ"].ToString().Trim();
			tmmsm21.Query("L2_PROC_NO,HEAT_NO");
			tmmsm21.TrimOrBlank();
			if (v_proc_div == "I" || v_proc_div == "U"){
				bof["HEATNUMBER"] = tmmsm21["PROC_NO"].ToString().Trim();
				if (bof.QueryCount("HEATNUMBER"))
				{
					bof.Delete("HEATNUMBER");
				}
				//ID
				CString id = tmmsm21["ID_SJ"].ToString();
				//工位名
				CString aggregatecode = tmmsm21["DEV_CODE"].ToString();
				//炉次号
				CString heatnumber = tmmsm21["PROC_NO"].ToString();
				//计划号
				CString planid = tmmsm21["SM_PLAN_NO"].ToString();
				//分包标志
				CString splitindication = tmmsm21["SPLIT_INDICATION"].ToString();
				//处理次数
				CString treatmentcounter = tmmsm21["SAME_PROC_NUM"].ToString();
				//空包重量
				CDecimal emptyladleweight = tmmsm21["EMPTY_LADLE_WEIGHT"].ToDecimal() * 1000;
				//班
				CString shiftteam = tmmsm21["PROD_SHIFT_GROUP"].ToString();
				//操作员
				CString operator1 = tmmsm21["ASSISTANT"].ToString();
				//实际钢种
				CString gradeact = tmmsm21["ST_NO"].ToString();
				//实际出钢量
				CDecimal weightact = tmmsm21["ACTRESULT"].ToDecimal() * 1000;
				//铁水重量
				CDecimal hmweight = tmmsm21["MOLTIRON_WT"].ToDecimal() * 1000;
				//废钢重量
				CDecimal scrapweight = tmmsm21["SCRAP_WEIGHT"].ToDecimal() * 1000;
				//生铁重量
				CDecimal pigweight = tmmsm21["GROSS_WT"].ToDecimal() * 1000;
				//炉次开始时间
				CString start_time = tmmsm21["START_TIME"].ToString();
				//炉次结束时间
				CString end_time = tmmsm21["END_TIME"].ToString();
				//吹氧次数
				CString blownumber = tmmsm21["BLOW_NUMBER"].ToString();
				//吹氧时间
				CDecimal blowtime = tmmsm21["BLOW_DURATION"].ToDecimal() * 60;
				//吹氧量
				CString oxygentot = tmmsm21["OXYGEN_FINAL"].ToString();
				Log::Trace("", __FUNCTION__, "OXYGENTOT=[{0}]", bof["OXYGENTOT"].ToString());
				//第一次吹氧开始时间
				if (tmmsm21["BLOW_START_TIME1"].ToString() != " " && tmmsm21["BLOW_START_TIME1"].ToDecimal() >= 20000101000000){
					Log::Trace("", __FUNCTION__, "BLOW_START_TIME1=[{0}]", tmmsm21["BLOW_START_TIME1"].ToString());
					if (tmmsm21["BLOW_START_TIME1"].ToString() != " "){
						CDateTime blow_start_time = CDateTime::Parse(tmmsm21["BLOW_START_TIME1"].ToString());
						bof["BLOWSTART"] = blow_start_time;
					}
					else{
						bof["BLOWSTART"] = "19000101000000";
					}
				}
				else{
					bof["BLOWSTART"] = "19000101000000";
				}
				Log::Trace("", __FUNCTION__, "BLOWSTART=[{0}]", bof["BLOWSTART"].ToString());
				//最后一次吹氧结束时间
				if (tmmsm21["BLOW_END_TIME1"].ToString() != " " && tmmsm21["BLOW_END_TIME1"].ToDecimal() >= 20000101000000){
					CDateTime blow_end_time = CDateTime::Parse(tmmsm21["BLOW_END_TIME1"].ToString());
					bof["BLOWEND"] = blow_end_time;
				}
				else{
					bof["BLOWEND"] = "19000101000000";
				}
				//溅渣次数
				CString slagsplashingnumber = tmmsm21["SLAG_SPLASHING_NUMBER"].ToString();
				//顶吹氮量
				CString nitrogentot = tmmsm21["TOTAL_N2_CONS"].ToString();
				//溅渣开始时间
				if (tmmsm21["SLAG_START_TIME"].ToString() != " "  && tmmsm21["SLAG_START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime slag_start_time = CDateTime::Parse(tmmsm21["SLAG_START_TIME"].ToString());
					bof["SLAGSPLASHINGSTART"] = slag_start_time;
				}
				else{
					bof["SLAGSPLASHINGSTART"] = "19000101000000";
				}
				//溅渣结束时间
				if (tmmsm21["SLAG_END_TIME"].ToString() != " "  && tmmsm21["SLAG_END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime slag_end_time = CDateTime::Parse(tmmsm21["SLAG_END_TIME"].ToString());
					bof["SLAGSPLASHINGEND"] = slag_end_time;
				}
				else{
					bof["SLAGSPLASHINGEND"] = "19000101000000";
				}
				//底吹氩量
				CString argontuytot = tmmsm21["AR_SUM_COMSUME"].ToString();
				Log::Trace("", __FUNCTION__, "ARGONTUYTOT=[{0}]", bof["ARGONTUYTOT"].ToString());
				//底吹氮量
				CString nitrogentuytot = tmmsm21["N_SUM_COMSUME"].ToString();
				//出钢开始时间
				if (tmmsm21["TAP_START_TIME"].ToString() != " "  && tmmsm21["TAP_START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime tap_start_time = CDateTime::Parse(tmmsm21["TAP_START_TIME"].ToString());
					bof["TAPPINGSTART"] = tap_start_time;
				}
				else{
					bof["TAPPINGSTART"] = "19000101000000";
				}
				//出钢结束时间
				if (tmmsm21["TAP_END_TIME"].ToString() != " "  && tmmsm21["TAP_END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime tap_end_time = CDateTime::Parse(tmmsm21["TAP_END_TIME"].ToString());
					bof["TAPPINGEND"] = tap_end_time;
				}
				else{
					bof["TAPPINGEND"] = "19000101000000";
				}
				//除渣开始时间
				if (tmmsm21["DROSSING_START_TIME"].ToString() != " "  && tmmsm21["DROSSING_START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime drossing_start_time = CDateTime::Parse(tmmsm21["DROSSING_START_TIME"].ToString());
					bof["SLAGGINGSTART"] = drossing_start_time;
				}
				else{
					bof["SLAGGINGSTART"] = "19000101000000";
				}
				//除渣结束时间
				if (tmmsm21["DROSSING_END_TIME"].ToString() != " "  && tmmsm21["DROSSING_END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime drossing_end_time = CDateTime::Parse(tmmsm21["DROSSING_END_TIME"].ToString());
					bof["SLAGGINGEND"] = drossing_end_time;
				}
				else{
					bof["SLAGGINGEND"] = "19000101000000";
				}
				//铁水成分C
				CString hmc = tmmsm21["IRON_C"].ToString();
				//铁水成分Si
				CString hmsi = tmmsm21["IRON_SI"].ToString();
				//铁水成分Mn
				CString hmmn = tmmsm21["IRON_MN"].ToString();
				//铁水成分P
				
				CString hmp = tmmsm21["IRON_P"].ToString();
				//铁水成分S
				CString hms = tmmsm21["IRON_S"].ToString();
				//铁水温度
				CString hmtemp = tmmsm21["IRON_TEMP"].ToString();
				
				//氧枪号
				Log::Trace("", __FUNCTION__, "O2_LANCE_NO=[{0}]", tmmsm21["O2_LANCE_NO"].ToString());
				if (tmmsm21["O2_LANCE_NO"].ToString() == " "){
					CDecimal o2_lance_no = 0;
					bof["OXYGENLANCENUMBER"] = o2_lance_no;
				}
				else
				{
					bof["OXYGENLANCENUMBER"] = tmmsm21["O2_LANCE_NO"].ToString();
				}
				CString hm_treatment_no = tmmsm21["IRON_LTREAT_NO"].ToString();
				//脱硫处理号
				CString des_treatment_no = tmmsm21["DES_TREATMENT_NO"].ToString();
				//倾倒温度
				CString tap_temp = tmmsm21["OUT_STEEL_TEMP"].ToString();
				//卷板、中板类型，仅从二级收集，尚未在MES中使用
				CString cast_purpose = tmmsm21["IRON_LADLE_NO"].ToString();
				//转炉炉龄
				CString bof_life = tmmsm21["FURNACE_AGE"].ToString();
				//钢包号
				CString steel_ladle_no = tmmsm21["LADLE_NO"].ToString();
				//空铁水包重量
				CDecimal empty_ladle_wt = tmmsm21["EMPTY_LADLE_WEIGHT"].ToDecimal() * 1000;
				//顶吹氮量
				CString top_n2_cons = "0";
				if (tmmsm21["TOTAL_N2_CONS"].ToString().GetLength() > 4){

				}
				else{
					top_n2_cons = tmmsm21["TOTAL_N2_CONS"].ToString();
				}
				//出站总重量
				CDecimal ladle_depart_wt = tmmsm21["LADLE_DEPART_WT"].ToDecimal() * 1000;
				//下道设备号
				CString next_dev_code = tmmsm21["NEXT_DEV_CODE"].ToString();
				//冶炼时长
				CDecimal melt_duration = tmmsm21["MELT_DURATION"].ToDecimal() * 60;
				//辅助时长
				CDecimal assis_duration = tmmsm21["ASSIS_DURATION"].ToDecimal() * 60;
				//出钢周期
				CString taptotap_duration = tmmsm21["TAPTOTAP_DURATION"].ToString();
				//满铁水包重量 t
				CDecimal gross_ladle_wt = tmmsm21["GROSS_LADLE_WT"].ToDecimal() * 1000;
				CString heatno_premelt1 = tmmsm21["HEATNO_PREMELT1"].ToString();
				CDecimal weight_premelt1 = tmmsm21["WEIGHT_PREMELT1"].ToDecimal() * 1000;
				CString heatno_premelt2 = tmmsm21["HEATNO_PREMELT2"].ToString();
				CDecimal weight_premelt2 = tmmsm21["WEIGHT_PREMELT2"].ToDecimal() * 1000;
				CString heatno_premelt3 = tmmsm21["HEATNO_PREMELT3"].ToString();
				CDecimal weight_premelt3 = tmmsm21["WEIGHT_PREMELT3"].ToDecimal() * 1000;
				//SMP_MODE
				CString smp_mode = tmmsm21["SMP_MODE"].ToString();
				if (tmmsm21["BLOW_START"].ToString() == " "){
					tmmsm21["BLOW_START"] = "19000101000000";
				}
				if (tmmsm21["BLOW_END"].ToString() == " "){
					tmmsm21["BLOW_END"] = "19000101000000";
				}
				////记录读取标志 NY模块
				//bof["NYREAD"] = tmmsm21[""].ToString();
				//bof.Insert();
				sql_insert = " insert into DA_BOF_PRO_SUMMARY (ID, AGGREGATECODE, HEATNUMBER, PLANID, SPLITINDICATION, TREATMENTCOUNTER, SHIFTTEAM, "
					" OPERATOR, EMPTYLADLEWEIGHT, GRADEACT, WEIGHTACT, HMWEIGHT, SCRAPWEIGHT, PIGWEIGHT, "
					" HEATSTART, HEATEND, BLOWNUMBER, BLOWTIME, OXYGENTOT, BLOWSTART, BLOWEND, "
					" SLAGSPLASHINGNUMBER, NITROGENTOT, SLAGSPLASHINGSTART, SLAGSPLASHINGEND, ARGONTUYTOT, "
					" NITROGENTUYTOT, TAPPINGSTART, TAPPINGEND, SLAGGINGSTART, SLAGGINGEND, HMC, HMSI, HMMN, "
					" HMP, HMS, HMTEMP, OXYGENLANCENUMBER, "
					" HM_TREATMENT_NO, DES_TREATMENT_NO, TAP_TEMP, CAST_PURPOSE, "
					" BOF_LIFE, STEEL_LADLE_NO, EMPTY_LADLE_WT, TOP_N2_CONS,  "
					"  LADLE_DEPART_WT, NEXT_DEV_CODE, MELT_DURATION, ASSIS_DURATION, "
					" TAPTOTAP_DURATION, SMP_MODE, GROSS_LADLE_WT,HEATNO_PREMELT1,WEIGHT_PREMELT1,HEATNO_PREMELT2,WEIGHT_PREMELT2,HEATNO_PREMELT3,WEIGHT_PREMELT3) "
					" values('"+id+"','"+aggregatecode+"','"+heatnumber+"','"+planid+"','"+splitindication+"','"+treatmentcounter+"','"+shiftteam+"', "
					" '" + operator1 + "','" + emptyladleweight.ToString() + "','" + gradeact + "','" + weightact.ToString() + "','" + hmweight.ToString() + "','" + scrapweight.ToString() + "','" + pigweight.ToString() + "', "
					" to_date('" + start_time + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + end_time + "', 'yyyy-MM-dd hh24:MI:SS'),'" + blownumber + "','" + blowtime.ToString() + "','" + oxygentot + "',to_date('" + tmmsm21["BLOW_START"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + tmmsm21["BLOW_END"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + slagsplashingnumber + "','" + nitrogentot + "',to_date('" + bof["SLAGSPLASHINGSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + bof["SLAGSPLASHINGEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + argontuytot + "', "
					" '" + nitrogentuytot + "',to_date('" + bof["TAPPINGSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + bof["TAPPINGEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + bof["SLAGGINGSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + bof["SLAGGINGEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + hmc + "','" + hmsi + "','" + hmmn + "', "
					" '" + hmp + "','" + hms + "','" + hmtemp + "','" + bof["OXYGENLANCENUMBER"].ToString() + "',"
					" '" + hm_treatment_no + "','" + des_treatment_no + "','" + tap_temp + "','" + cast_purpose + "',"
					" '" + bof_life + "','" + steel_ladle_no + "','" + empty_ladle_wt.ToString() + "','" + top_n2_cons + "', "
					" '" + ladle_depart_wt.ToString() + "','" + next_dev_code + "','" + melt_duration.ToString() + "','" + assis_duration.ToString() + "', "
					" '" + taptotap_duration + "','" + smp_mode + "','" + gross_ladle_wt.ToString() + "',  "
					" '" + heatno_premelt1 + "','" + weight_premelt1.ToString() + "','" + heatno_premelt2 + "','" + weight_premelt2.ToString() + "','" + heatno_premelt3 + "','" + weight_premelt3.ToString() + "' "
					" ) ";
				cmd_insert.SetCommandText(sql_insert);
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.ExecuteNonQuery();
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.Close();
			}
			else{
				if (bof.QueryCount("HEATNUMBER"))
				{
					bof.Delete("HEATNUMBER");
				}
			}
		}
		else if (table_name == "DA_EAF_PRO_SUMMARY"){
			tmmsm20["PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"].ToString();
			tmmsm20["HEAT_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"].ToString();
			tmmsm20["ID_SJ"] = bcls_rec->Tables["JKDP"].Rows[0]["ID_SJ"].ToString().Trim();
			tmmsm20["L2_PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"].ToString().Trim();
			Log::Trace("", "", "tmmsm20[L2_PROC_NO] = [{0}]", tmmsm20["L2_PROC_NO"].ToString());
			tmmsm20.Query("L2_PROC_NO");
			tmmsm20.TrimOrBlank();
			eaf["HEATNUMBER"] = tmmsm20["L2_PROC_NO"].ToString().Trim();
			Log::Trace("", "", "HEATNUMBER = [{0}]", eaf["HEATNUMBER"].ToString());
			if (v_proc_div == "I" || v_proc_div == "U"){
				if (eaf.QueryCount("HEATNUMBER"))
				{
					eaf.Delete("HEATNUMBER");
				}
				CString id = tmmsm20["ID_SJ"].ToString();
				//工位名
				CString aggregatecode = tmmsm20["DEV_CODE"].ToString();
				//炉次号
				CString heatnumber = tmmsm20["L2_PROC_NO"].ToString();
				//计划号
				CString planid = tmmsm20["SM_PLAN_NO"].ToString();
				//分包标志
				CString splitindication = tmmsm20["SPLIT_INDICATION"].ToString();
				//处理次数
				CString treatmentcounter = tmmsm20["SAME_PROC_NUM"].ToString();
				//空包重量
				CDecimal emptyladleweight = tmmsm20["EMPTY_LADLE_WEIGHT"].ToDecimal() * 1000;
				//班
				CString shiftteam = tmmsm20["PROD_SHIFT_GROUP"].ToString();
				//操作员
				CString operator1 = tmmsm20["ASSISTANT"].ToString();
				//实际钢种
				CString gradeact = tmmsm20["ST_NO"].ToString();
				//实际出钢量
				CDecimal weightact = tmmsm20["ACTRESULT"].ToDecimal() * 1000;
				//炉次开始时间
				if (tmmsm20["START_TIME"].ToString() != " "  && tmmsm20["START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime start_time = CDateTime::Parse(tmmsm20["START_TIME"].ToString());
					eaf["HEATSTART"] = start_time;
				}
				else{
					eaf["HEATSTART"] = "19000101000000";
				}
				//炉次结束时间
				if (tmmsm20["END_TIME"].ToString() != " "  && tmmsm20["END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime end_time = CDateTime::Parse(tmmsm20["END_TIME"].ToString());
					eaf["HEATEND"] = end_time;
				}
				else{
					eaf["HEATEND"] = "19000101000000";
				}
				//加热次数
				CString heatingnumber = tmmsm20["HEAT_COUNT"].ToString();
				//加热时间
				CDecimal heattime = tmmsm20["BIL_MELT_TIME"].ToDecimal() * 60;
				//耗电量
				CString power = tmmsm20["POWER_CONSUME"].ToString();
				//第一次加热开始时间
				if (tmmsm20["FIRST_HEATING_START"].ToString() != " " && tmmsm20["FIRST_HEATING_START"].ToDecimal() >= 20000101000000){
					CDateTime first_heating_start = CDateTime::Parse(tmmsm20["FIRST_HEATING_START"].ToString());
					eaf["FIRSTHEATINGSTART"] = first_heating_start;
				}
				else{
					eaf["FIRSTHEATINGSTART"] = "19000101000000";
				}
				//最后一次加热结束时间
				if (tmmsm20["LAST_HEATING_END"].ToString() != " " && tmmsm20["LAST_HEATING_END"].ToDecimal() >= 20000101000000){
					CDateTime last_heating_end = CDateTime::Parse(tmmsm20["LAST_HEATING_END"].ToString());
					eaf["LASTHEATINGEND"] = last_heating_end;
				}
				else{
					eaf["LASTHEATINGEND"] = "19000101000000";
				}
				//转炉来钢水重量
				CDecimal bofweight = tmmsm20["BOF_WEIGHT"].ToDecimal() * 1000;
				//废钢重量
				CDecimal scrapweight = tmmsm20["SCRAP_WEIGHT"].ToDecimal() * 1000;
				//碳粉重量
				CDecimal cpowderweight = tmmsm20["C_POWDER_WEIGHT"].ToDecimal() * 1000;
				//吹氧次数
				CString oxygennumber = tmmsm20["OXYGEN_NUMBER"].ToString();
				//吹氧时间
				CDecimal blowtime = tmmsm20["BLOW_DURATION"].ToDecimal() * 60;
				//吹氧量
				CString oxygentotal = tmmsm20["OXYGEN_FINAL"].ToString();
				//第一次吹氧时间
				if (tmmsm20["FIRST_BLOW_START"].ToString() != " "  && tmmsm20["FIRST_BLOW_START"].ToDecimal() >= 20000101000000){
					CDateTime first_blow_start = CDateTime::Parse(tmmsm20["FIRST_BLOW_START"].ToString());
					eaf["FIRSTBLOWSTART"] = first_blow_start;
				}
				else{
					eaf["FIRSTBLOWSTART"] = "19990101110101";
				}
				//最后一次吹氧时间
				if (tmmsm20["LAST_BLOW_END"].ToString() != " "  && tmmsm20["LAST_BLOW_END"].ToDecimal() >= 20000101000000){
					CDateTime last_blow_end = CDateTime::Parse(tmmsm20["LAST_BLOW_END"].ToString());
					eaf["LASTBLOWEND"] = last_blow_end;
				}
				else{
					eaf["FIRSTBLOWSTART"] = "19990101110101";
				}
				//出钢开始时间
				if (tmmsm20["TAP_START_TIME"].ToString() != " "  && tmmsm20["TAP_START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime tap_start_time = CDateTime::Parse(tmmsm20["TAP_START_TIME"].ToString());
					eaf["TAPPINGSTART"] = tap_start_time;
				}
				else{
					eaf["TAPPINGSTART"] = "19000101000000";
				}
				//出钢结束时间
				if (tmmsm20["TAP_END_TIME"].ToString() != " "  && tmmsm20["TAP_END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime tap_end_time = CDateTime::Parse(tmmsm20["TAP_END_TIME"].ToString());
					eaf["TAPPINGEND"] = tap_end_time;
				}
				else{
					eaf["TAPPINGEND"] = "19000101000000";
				}
				//顶吹氮量
				CString nitrogentopamount = tmmsm20["TOP_N2_CONS"].ToString();
				//顶吹氮时间
				CDecimal nitrogentoptime = tmmsm20["NITROGEN_TOP_TIME"].ToDecimal() * 1000;
				//底吹氮量
				CString nitrogenbottomamount = tmmsm20["BTTM_N_COMSUME"].ToString();
				//底吹氮时间
				CDecimal nitrogenbottomtime = tmmsm20["NITROGEN_BOTTOM_TIME"].ToDecimal() * 60;
				//记录插入时间
				Log::Trace("", __FUNCTION__, "REC_CREATE_TIME=[{0}]", tmmsm20["REC_CREATE_TIME"].ToString());
				CDateTime timestamp = CDateTime::Parse(tmmsm20["REC_CREATE_TIME"].ToString());
				eaf["TIMESTAMP"] = timestamp;
				//eaf炉壳编号
				Log::Trace("", __FUNCTION__, "EAF_SHELL_NO=[{0}]", tmmsm20["EAF_SHELL_NO"].ToString());
				CString eaf_shell_no = tmmsm20["EAF_SHELL_NO"].ToString();
				//eaf炉壳寿命
				Log::Trace("", __FUNCTION__, "EAF_SHELL_LIFE=[{0}]", tmmsm20["EAF_SHELL_LIFE"].ToString());
				CString eaf_shell_life = tmmsm20["EAF_SHELL_LIFE"].ToString();
				//
				CString heatno_premelt1 = tmmsm20["HEATNO_PREMELT1"].ToString();
				CDecimal weight_premelt1 = tmmsm20["WEIGHT_PREMELT1"].ToDecimal() * 1000;
				CString heatno_premelt2 = tmmsm20["HEATNO_PREMELT2"].ToString();
				CDecimal weight_premelt2 = tmmsm20["WEIGHT_PREMELT2"].ToDecimal() * 1000;
				CString heatno_premelt3 = tmmsm20["HEATNO_PREMELT3"].ToString();
				CDecimal weight_premelt3 = tmmsm20["WEIGHT_PREMELT3"].ToDecimal() * 1000;
				//装料1开始时刻
				if (tmmsm20["CHARGE_START_TIME1"].ToString() != " " && tmmsm20["CHARGE_START_TIME1"].ToDecimal() >= 20000101000000){
					CDateTime charge_start_time1 = CDateTime::Parse(tmmsm20["CHARGE_START_TIME1"].ToString());
					eaf["CHARGE_START_TIME1"] = charge_start_time1;
				}
				else{
					eaf["CHARGE_START_TIME1"] = "19000101000000";
				}
				//装料1用时
				CDecimal charge_duration1 = tmmsm20["CHARGE_DURATION1"].ToDecimal() * 60;
				//送电开始时刻1
				if (tmmsm20["POWER_ON_TIME1"].ToString() != " " && tmmsm20["POWER_ON_TIME1"].ToDecimal() >= 20000101000000){
					CDateTime power_on_time1 = CDateTime::Parse(tmmsm20["POWER_ON_TIME1"].ToString());
					eaf["POWER_ON_TIME1"] = power_on_time1;
				}
				else{
					eaf["POWER_ON_TIME1"] = "19000101000000";
				}
				//送电时长
				CString power_on_duration1 = tmmsm20["POWER_ON_DURATION1"].ToString();
				//装料2开始时刻 
				if (tmmsm20["CHARGE_START_TIME2"].ToString() != " " && tmmsm20["CHARGE_START_TIME2"].ToDecimal() >= 20000101000000){
					CDateTime charge_start_time2 = CDateTime::Parse(tmmsm20["CHARGE_START_TIME2"].ToString());
					eaf["CHARGE_START_TIME2"] = charge_start_time2;
				}
				else{
					eaf["CHARGE_START_TIME2"] = "19000101000000";
				}
				//装料2用时
				CDecimal charge_duration2 = tmmsm20["CHARGE_DURATION2"].ToDecimal() * 60;
				//送电开始时刻2
				if (tmmsm20["POWER_ON_TIME2"].ToString() != " " && tmmsm20["POWER_ON_TIME2"].ToDecimal() >= 20000101000000){
					CDateTime power_on_time2 = CDateTime::Parse(tmmsm20["POWER_ON_TIME2"].ToString());
					eaf["POWER_ON_TIME2"] = power_on_time2;
				}
				else{
					eaf["POWER_ON_TIME2"] = "19000101000000";
				}
				//送电时长2
				CString power_on_duration2 = tmmsm20["POWER_ON_DURATION2"].ToString();
				//装料3开始时刻
				if (tmmsm20["CHARGE_START_TIME3"].ToString() != " " && tmmsm20["CHARGE_START_TIME3"].ToDecimal() >= 20000101000000){
					CDateTime charge_start_time3 = CDateTime::Parse(tmmsm20["CHARGE_START_TIME3"].ToString());
					eaf["CHARGE_START_TIME3"] = charge_start_time3;
				}
				else{
					eaf["CHARGE_START_TIME3"] = "19000101000000";
				}
				//装料3用时
				CDecimal charge_duration3 = tmmsm20["CHARGE_DURATION3"].ToDecimal() * 60;
				//送电开始时刻3
				if (tmmsm20["POWER_ON_TIME3"].ToString() != " " && tmmsm20["POWER_ON_TIME3"].ToDecimal() >= 20000101000000){
					CDateTime power_on_time3 = CDateTime::Parse(tmmsm20["POWER_ON_TIME3"].ToString());
					eaf["POWER_ON_TIME3"] = power_on_time3;
				}
				else{
					eaf["POWER_ON_TIME3"] = "19000101000000";
				}
				//送电时长3
				CString power_on_duration3 = tmmsm20["POWER_ON_DURATION3"].ToString();
				//还原开始时刻
				if (tmmsm20["REDUCING_START_TIME"].ToString() != " " && tmmsm20["REDUCING_START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime reducing_start_time = CDateTime::Parse(tmmsm20["REDUCING_START_TIME"].ToString());
					eaf["REDUCING_START_TIME"] = reducing_start_time;
				}
				else{
					eaf["REDUCING_START_TIME"] = "19000101000000";
				}
				//还原周期
				CDecimal reducing_duration = tmmsm20["REDUCING_DURATION"].ToDecimal() * 60;
				//出钢温度1
				CString tapping_temp1 = tmmsm20["OUT_STEEL_TEMP"].ToString();
				
				//钢（铁）包号1
				CString ladle_no1 = tmmsm20["LADLE_NO"].ToString();
				//包1包龄
				CString ladle_life1 = tmmsm20["LADLE_AGE"].ToString();
				//包1包况
				CString ladle_status1 = tmmsm20["LADLE_STATUS"].ToString();
				//冶炼模式
				CString smelt_mode = tmmsm20["SMELT_MODE1"].ToString();
				//总送电次数
				CString power_on_times = tmmsm20["POWER_ON_TIMES"].ToString();
				//炉门氧量
				
				CString eaf_door_o2_cons = tmmsm20["EAF_DOOR_O2_CONS"].ToString();
				//炉壁集速氧量
				CString eaf_wall_o2_cons = tmmsm20["EAF_WALL_O2_CONS"].ToString();
				//还原气体类型
				CString reducing_gas_type = tmmsm20["REDUCING_GAS_TYPE"].ToString();
				//炉壁氧枪使用模式
				/*if (tmmsm20["LW_O2_USE_MODE"].ToString() != " "){
					eaf["LW_O2_USE_MODE"] = tmmsm20["LW_O2_USE_MODE"].ToString();
				}*/
				//冶炼时长
				CDecimal melt_duration = tmmsm20["MELT_DURATION"].ToDecimal() * 60;
				//出钢周期
				CDecimal taptotap_duration = tmmsm20["TAPTOTAP_DURATION"].ToDecimal() * 60;
				//下道设备号
				CString next_dev_code = tmmsm20["NEXT_DEV_CODE"].ToString();
				//工艺路线
				CString prod_route = tmmsm20["PROD_ROUTE"].ToString();
				//EV工艺
				CString heatno_aod3 = tmmsm20["HEATNO_AOD3"].ToString();
				Log::Trace("", __FUNCTION__, "111=[{0}]" );
				f_tableObjectCheck9999(eaf);
				eaf.Print();
				//eaf.Insert();
				sql_insert = " insert into DA_EAF_PRO_SUMMARY (ID, AGGREGATECODE, HEATNUMBER, PLANID, SPLITINDICATION, TREATMENTCOUNTER, "
					" EMPTYLADLEWEIGHT, SHIFTTEAM, OPERATOR, GRADEACT, WEIGHTACT, HEATSTART, HEATEND, "
					" HEATINGNUMBER, HEATTIME, POWER, FIRSTHEATINGSTART, LASTHEATINGEND, BOFWEIGHT, "
					" SCRAPWEIGHT, CPOWDERWEIGHT, OXYGENNUMBER, BLOWTIME, OXYGENTOTAL, FIRSTBLOWSTART, "
					" LASTBLOWEND, TAPPINGSTART, TAPPINGEND, NITROGENTOPAMOUNT, NITROGENTOPTIME, "
					" NITROGENBOTTOMAMOUNT, NITROGENBOTTOMTIME, "
					" HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, "
					" HEATNO_PREMELT3, WEIGHT_PREMELT3, EAF_SHELL_NO, EAF_SHELL_LIFE, CHARGE_START_TIME1, "
					" CHARGE_DURATION1, POWER_ON_TIME1, POWER_ON_DURATION1, CHARGE_START_TIME2, "
					" CHARGE_DURATION2, POWER_ON_TIME2, POWER_ON_DURATION2, CHARGE_START_TIME3, "
					" CHARGE_DURATION3, POWER_ON_TIME3, POWER_ON_DURATION3, REDUCING_START_TIME, "
					" REDUCING_DURATION, TAPPING_TEMP1, LADLE_NO1, LADLE_LIFE1, LADLE_STATUS1, SMELT_MODE, "
					" POWER_ON_TIMES, EAF_DOOR_O2_CONS, EAF_WALL_O2_CONS, REDUCING_GAS_TYPE, "
					" MELT_DURATION, TAPTOTAP_DURATION, NEXT_DEV_CODE, PROD_ROUTE, "
					" HEATNO_AOD3) "
					" values( "
					" '" + id + "','" + aggregatecode + "','" + eaf["HEATNUMBER"].ToString() + "','" + planid + "','" + splitindication + "','" + treatmentcounter + "', "
					" '" + emptyladleweight.ToString() + "','" + shiftteam + "','" + operator1 + "','" + gradeact + "','" + weightact.ToString() + "',to_date('" + eaf["HEATSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + eaf["HEATEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + heatingnumber + "','" + heattime.ToString() + "','" + power + "',to_date('" + eaf["FIRSTHEATINGSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + eaf["LASTHEATINGEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + bofweight.ToString() + "', "
					" '" + scrapweight.ToString() + "','" + cpowderweight.ToString() + "','" + oxygennumber + "','" + blowtime.ToString() + "','" + oxygentotal + "',to_date('" + eaf["FIRSTBLOWSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" to_date('" + eaf["LASTBLOWEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + eaf["TAPPINGSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + eaf["TAPPINGEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + nitrogentopamount + "','" + nitrogentoptime.ToString() + "', "
					" '" + nitrogenbottomamount + "','" + nitrogenbottomtime.ToString() + "','" + heatno_premelt1 + "','" + weight_premelt1.ToString() + "','" + heatno_premelt2 + "','" + weight_premelt2.ToString() + "',"
					" '" + heatno_premelt3 + "','" + weight_premelt3.ToString() + "','" + eaf_shell_no + "','" + eaf_shell_life + "',to_date('" + eaf["CHARGE_START_TIME1"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + charge_duration1.ToString() + "',to_date('" + eaf["POWER_ON_TIME1"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + power_on_duration1 + "',to_date('" + eaf["CHARGE_START_TIME2"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + charge_duration2.ToString() + "',to_date('" + eaf["POWER_ON_TIME2"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + power_on_duration2 + "',to_date('" + eaf["CHARGE_START_TIME3"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + charge_duration3.ToString() + "',to_date('" + eaf["POWER_ON_TIME3"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + power_on_duration3 + "',to_date('" + eaf["REDUCING_START_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),"
					" '" + reducing_duration.ToString() + "','" + tapping_temp1 + "','" + ladle_no1 + "','" + ladle_life1 + "','" + ladle_status1 + "','" + smelt_mode + "', "
					" '" + power_on_times + "','" + eaf_door_o2_cons + "','" + eaf_wall_o2_cons + "','" + reducing_gas_type + "', "
					" '" + melt_duration.ToString() + "','" + taptotap_duration.ToString() + "','" + next_dev_code + "','" + prod_route + "', "
					" '" + heatno_aod3+"' "
					" ) ";
				cmd_insert.SetCommandText(sql_insert);
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.ExecuteNonQuery();
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.Close();
			}
			else{
				if (eaf.QueryCount("HEATNUMBER"))
				{
					eaf.Delete("HEATNUMBER");
				}
			}
		}
		else if (table_name == "DA_LF_PRO_SUMMARY"){
			tmmsm24["PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"].ToString().Trim();
			tmmsm24["L2_PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"].ToString().Trim();
			tmmsm24["HEAT_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"].ToString().Trim();
			tmmsm24["ID_SJ"] = bcls_rec->Tables["JKDP"].Rows[0]["ID_SJ"].ToString().Trim();
			tmmsm24.Query("PROC_NO,HEAT_NO,L2_PROC_NO");
			tmmsm24.TrimOrBlank();
			lf["HEATNUMBER"] = tmmsm24["HEAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "HEAT_NO=[{0}]", tmmsm24["HEAT_NO"].ToString());
			if (v_proc_div == "I" || v_proc_div == "U"){
				if (lf.QueryCount("HEATNUMBER"))
				{
					lf.Delete("HEATNUMBER");
				}
				//关键字--未定
				CString id = tmmsm24["ID_SJ"].ToString();
				//工位名
				CString aggregatecode = tmmsm24["DEV_CODE"].ToString();
				Log::Trace("", __FUNCTION__, "00=[{}]");
				//炉次号
				CString heatnumber = tmmsm24["HEAT_NO"].ToString();
				//计划号
				CString planid = tmmsm24["SM_PLAN_NOL2"].ToString();
				//分包标志
				CString splitindication = tmmsm24["SPLIT_INDICATION"].ToString();
				//处理次数
				CString treatmentcounter = tmmsm24["SAME_PROC_NUM"].ToString();
				Log::Trace("", __FUNCTION__, "1112121=[{}]");
				//处理号 五位？？
				//lf["PROCESSNUMBER"] = tmmsm24["PROC_NO"].ToString();
				//钢水量
				Log::Trace("", __FUNCTION__, "333=[{}]");
				CDecimal steelweight = tmmsm24["MOLTIRON_WT"].ToDecimal() * 1000;
				//空包重量
				Log::Trace("", __FUNCTION__, "2323=[{}]");
				CDecimal emptyladleweight = tmmsm24["EMPTY_LADLE_WEIGHT"].ToDecimal() * 1000;
				//班
				CString shiftteam = tmmsm24["PROD_SHIFT_GROUP"].ToString();
				//操作员
				CString operator1 = tmmsm24["ASSISTANT"].ToString();
				//实际钢种
				CString gradeact1 = tmmsm24["ST_NO"].ToString();
				//炉次开始时间
				Log::Trace("", __FUNCTION__, "11=[{}]");
				if (tmmsm24["START_TIME"].ToString() != " "  && tmmsm24["START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime start_time = CDateTime::Parse(tmmsm24["START_TIME"].ToString());
					lf["HEATSTART"] = start_time;
				}
				else{
					lf["HEATSTART"] = "19000101000000";
				}
				//炉次结束时间
				if (tmmsm24["END_TIME"].ToString() != " "  && tmmsm24["END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime end_time = CDateTime::Parse(tmmsm24["END_TIME"].ToString());
					lf["HEATEND"] = end_time;
				}
				else{
					lf["HEATEND"] = "19000101000000";
				}
				//加热次数
				CString heatingnumber = tmmsm24["HEAT_COUNT"].ToString();
				//加热时间
				CString heattime = tmmsm24["BIL_MELT_TIME"].ToString();
				//耗电量
				CString power = tmmsm24["POWER_CONSUME"].ToString();
				//LADLE_DEPART_WT
				if (tmmsm24["LADLE_DEPART_WT"].ToDecimal() != 0){
					lf["LADLE_DEPART_WT"] = tmmsm24["LADLE_DEPART_WT"].ToDecimal() * 1000;
				}
				if (tmmsm24["EMPTY_LADLE_WT"].ToDecimal() != 0){
					lf["EMPTY_LADLE_WT"] = tmmsm24["EMPTY_LADLE_WT"].ToDecimal() * 1000;
				}
				if (tmmsm24["SLAG_WT_END"].ToDecimal() != 0){
					lf["SLAG_WT_END"] = tmmsm24["SLAG_WT_END"].ToDecimal() * 1000;
				}
				//第一次加热开始时间
				if (tmmsm24["FIRST_HEATING_START"].ToString() != " "  && tmmsm24["FIRST_HEATING_START"].ToDecimal() >= 20000101000000){
					CDateTime first_heating_start = CDateTime::Parse(tmmsm24["FIRST_HEATING_START"].ToString());
					lf["FIRSTHEATINGSTART"] = first_heating_start;
				}
				else{
					lf["FIRSTHEATINGSTART"] = "19000101000000";
				}
				//最后一次加热结束时间
				if (tmmsm24["LAST_HEATING_END"].ToString() != " "  && tmmsm24["LAST_HEATING_END"].ToDecimal() >= 20000101000000){
					CDateTime last_heating_end = CDateTime::Parse(tmmsm24["LAST_HEATING_END"].ToString());
					lf["LASTHEATINGEND"] = last_heating_end;
				}
				else{
					lf["LASTHEATINGEND"] = "19000101000000";
				}
				//吹氩量
				CString argontot = tmmsm24["AR_SUM_COMSUME"].ToString();
				//吹氩时间
				Log::Trace("", __FUNCTION__, "222=[{}]");
				CString argontime = tmmsm24["ARGON_TIME"].ToString();
				//第一次喂丝时间
				if (tmmsm24["FIRST_WIRE_START"].ToString() != " "  && tmmsm24["FIRST_WIRE_START"].ToDecimal() >= 20000101000000){
					CDateTime first_wire_start = CDateTime::Parse(tmmsm24["FIRST_WIRE_START"].ToString());
					lf["FIRSTWIRESTART"] = first_wire_start;
				}
				else{
					lf["FIRSTWIRESTART"] = "19000101000000";
				}
				//最后一次喂丝时间
				if (tmmsm24["LAST_WIRE_END"].ToString() != " "  && tmmsm24["LAST_WIRE_END"].ToDecimal() >= 20000101000000){
					CDateTime last_wire_end = CDateTime::Parse(tmmsm24["LAST_WIRE_END"].ToString());
					lf["LASTWIREEND"] = last_wire_end;
				}
				else{
					lf["LASTWIREEND"] = "19000101000000";
				}
				//记录插入时间
				/*CDateTime timestamp = CDateTime::Parse(tmmsm24["REC_CREATE_TIME"].ToString());
				lf["TIMESTAMP"] = timestamp;*/
				//渣厚
				CString slag_height = tmmsm24["SLAG_HEIGHT"].ToString();
				//空间
				CString freeboard = tmmsm24["FREEBOARD"].ToString();
				//软搅时间
				CString soft_stirring_dur = tmmsm24["SOFT_WHISK_TIME"].ToString();
				//开始温度
				CString temp_start = tmmsm24["START_STEEL_TEMP"].ToString();
				//结束温度
				CString temp_end = tmmsm24["END_STEEL_TEMP"].ToString();
				//炉次备注
				CString comments = tmmsm24["REMARK"].ToString();
				CString steel_ladle_no = tmmsm24["LADLE_NO"].ToString();
				CString ladle_life = tmmsm24["LADLE_AGE"].ToString();
				CDecimal ladle_arrive_wt = tmmsm24["LADLE_ARRIVE_WT"].ToDecimal() * 1000;
				CDecimal empty_ladle_wt = tmmsm24["EMPTY_LADLE_WT"].ToDecimal() * 1000;
				CDecimal ladle_arrive_steel_wt = tmmsm24["LADLE_ARRIVE_STEEL_WT"].ToDecimal() * 1000;
				CString slag_thick_start = tmmsm24["SLAG_THICK_START"].ToString();
				CDecimal slag_wt_start = tmmsm24["SLAG_WT_START"].ToDecimal() * 1000;
				CString ladle_arrive_temp = tmmsm24["LADLE_ARRIVE_TEMP"].ToString();
				CString elec_consumption_1 = tmmsm24["ELEC_CONSUMPTION_1"].ToString();
				CDecimal power_on_duration2 = tmmsm24["POWER_ON_DURATION2"].ToDecimal() * 60;
				CString elec_consumption_2 = tmmsm24["ELEC_CONSUMPTION_2"].ToString();
				CDecimal power_on_duration3 = tmmsm24["POWER_ON_DURATION3"].ToDecimal() * 60;
				CString elec_consumption_3 = tmmsm24["ELEC_CONSUMPTION_3"].ToString();
				CString next_dev_code = tmmsm24["NEXT_DEV_CODE"].ToString();
				CString strong_stir_dur = tmmsm24["STRONG_STIR_DUR"].ToString();
				CString midstrong_stir_dur = tmmsm24["MIDSTRONG_STIR_DUR"].ToString();
				CString middle_stir_dur = tmmsm24["MIDDLE_STIR_DUR"].ToString();
				CString strong_stir_flow = tmmsm24["STRONG_STIR_FLOW"].ToString();
				CString midstrong_stir_flow = tmmsm24["MIDSTRONG_STIR_FLOW"].ToString();
				CString middle_stir_flow = tmmsm24["MIDDLE_STIR_FLOW"].ToString();
				CString bas_online = tmmsm24["BAS_ONLINE"].ToString();
				CString power_on_duration1 = tmmsm24["POWER_ON_DURATION1"].ToString();
				CString soft_stir_flow = tmmsm24["SOFT_STIR_FLOW"].ToString();
				if (tmmsm24["ELEC_START_TIME1"].ToString() == " "){
					tmmsm24["ELEC_START_TIME1"] = "19000101000000";
				}
				if (tmmsm24["ELEC_END_TIME1"].ToString() == " "){
					tmmsm24["ELEC_END_TIME1"] = "19000101000000";
				}
				if (tmmsm24["ELEC_START_TIME2"].ToString() == " "){
					tmmsm24["ELEC_START_TIME2"] = "19000101000000";
				}
				if (tmmsm24["ELEC_END_TIME2"].ToString() == " "){
					tmmsm24["ELEC_END_TIME2"] = "19000101000000";
				}
				if (tmmsm24["ELEC_START_TIME3"].ToString() == " "){
					tmmsm24["ELEC_START_TIME3"] = "19000101000000";
				}
				if (tmmsm24["ELEC_END_TIME3"].ToString() == " "){
					tmmsm24["ELEC_END_TIME3"] = "19000101000000";
				}
				////记录读取标志 NY模块
				//lf["NYREAD"] = tmmsm24[""].ToString();
				sql_insert = " insert into DA_LF_PRO_SUMMARY (ID, AGGREGATECODE, HEATNUMBER, PLANID, SPLITINDICATION, TREATMENTCOUNTER, "
					" STEELWEIGHT, EMPTYLADLEWEIGHT, SHIFTTEAM, OPERATOR, GRADEACT, HEATSTART, HEATEND, "
					" HEATINGNUMBER, HEATTIME, POWER, FIRSTHEATINGSTART, LASTHEATINGEND, ARGONTOT, ARGONTIME, "
					" FIRSTWIRESTART, LASTWIREEND, SLAG_HEIGHT, FREEBOARD, "
					" SOFT_STIRRING_DUR, TEMP_START, TEMP_END, STEEL_LADLE_NO, LADLE_LIFE, LADLE_ARRIVE_WT, "
					" EMPTY_LADLE_WT, LADLE_ARRIVE_STEEL_WT, SLAG_THICK_START, SLAG_WT_START, "
					" LADLE_ARRIVE_TEMP, POWER_ON_TIME1, POWER_ON_DURATION1, ELEC_CONSUMPTION_1, "
					" POWEROFF_TIME1, POWER_ON_TIME2, POWER_ON_DURATION2, ELEC_CONSUMPTION_2, POWEROFF_TIME2, "
					" POWER_ON_TIME3, POWER_ON_DURATION3, ELEC_CONSUMPTION_3, POWEROFF_TIME3, SLAG_WT_END, "
					" LADLE_DEPART_WT, NEXT_DEV_CODE, "
					" STRONG_STIR_DUR, MIDSTRONG_STIR_DUR, MIDDLE_STIR_DUR, STRONG_STIR_FLOW, "
					" MIDSTRONG_STIR_FLOW, MIDDLE_STIR_FLOW, BAS_ONLINE,SOFT_STIR_FLOW) "
					" values('" + id + "','" + aggregatecode + "','" + heatnumber + "','" + planid + "','" + splitindication + "','" + treatmentcounter + "',"
					" '" + steelweight.ToString() + "','" + emptyladleweight.ToString() + "','" + shiftteam + "','" + operator1 + "','" + gradeact1 + "',to_date('" + lf["HEATSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" to_date('" + lf["HEATEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + heatingnumber + "','" + heattime + "','" + power + "',to_date('" + lf["FIRSTHEATINGSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" to_date('" + lf["LASTHEATINGEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + argontot + "','" + argontime + "',to_date('" + lf["FIRSTWIRESTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" to_date('" + lf["LASTWIREEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + slag_height + "','" + freeboard + "','" + soft_stirring_dur + "','" + temp_start + "','" + temp_end + "', "
					" '" + steel_ladle_no + "','" + ladle_life + "','" + ladle_arrive_wt.ToString() + "','" + empty_ladle_wt.ToString() + "', "
					" '" + ladle_arrive_steel_wt.ToString() + "','" + slag_thick_start + "','" + slag_wt_start.ToString() + "','" + ladle_arrive_temp + "', "
					" to_date('" + tmmsm24["ELEC_START_TIME1"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + power_on_duration1 + "','" + elec_consumption_1 + "', "
					" to_date('" + tmmsm24["ELEC_END_TIME1"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + tmmsm24["ELEC_START_TIME2"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + power_on_duration2 .ToString()+ "', "
					" '" + elec_consumption_2 + "',to_date('" + tmmsm24["ELEC_END_TIME2"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + tmmsm24["ELEC_START_TIME3"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + power_on_duration3.ToString() + "','" + elec_consumption_3 + "',to_date('" + tmmsm24["ELEC_END_TIME3"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + lf["SLAG_WT_END"] .ToString()+ "', "
					" '" + lf["LADLE_DEPART_WT"].ToString() + "','" + next_dev_code + "','" + strong_stir_dur + "','" + midstrong_stir_dur + "','" + middle_stir_dur + "','" + strong_stir_flow + "', "
					" '" + midstrong_stir_flow + "','" + middle_stir_flow + "','" + bas_online + "','" + soft_stir_flow + "' "
					" ) ";
				cmd_insert.SetCommandText(sql_insert);
				cmd_insert.Parameters.Set("id", id);
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.ExecuteNonQuery();
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.Close();
			}
			else{
				if (lf.QueryCount("HEATNUMBER"))
				{
					lf.Delete("HEATNUMBER");
				}
			}
			
		}
		else if (table_name == "DA_LTS_PROD_SUMMARY"){
			tmmsm26["PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm26["PROC_NO"].ToString());//关键字--未定
			tmmsm26["HEAT_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"].ToString();
			tmmsm26["ID_SJ"] = bcls_rec->Tables["JKDP"].Rows[0]["ID_SJ"].ToString().Trim();
			tmmsm26["L2_PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"].ToString().Trim();
			tmmsm26.Query("HEAT_NO,PROC_NO,L2_PROC_NO");
			tmmsm26.TrimOrBlank();
			CString heat_number = tmmsm26["HEAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "HEAT_NUMBER=[{0}]", lts["HEAT_NUMBER"].ToString());//关键字--未定
			if (v_proc_div == "I" || v_proc_div == "U"){
				CDecimal count = 0;
				cmd_sql_count.SetCommandText(" select count(*) from DA_LTS_PROD_SUMMARY where HEAT_NUMBER='" + heat_number + "' ");
				cmd_sql_count.ExecuteReader();
				if (cmd_sql_count.Read())
				{
					count = cmd_sql_count.GetDecimal(1);
				}
				cmd_sql_count.Close();
				if (count != 0){
					cmd_sql_delete.SetCommandText(" delete from DA_LTS_PROD_SUMMARY where HEAT_NUMBER='" + heat_number + "' ");
					cmd_sql_delete.ExecuteNonQuery();
					cmd_sql_delete.Close();
				}
				CString id = tmmsm26["ID_SJ"].ToString();
				CString aggregate_name = tmmsm26["DEV_CODE"].ToString();
				//计划号
				CString order_number = tmmsm26["SM_PLAN_NO"].ToString();
				//分包标志
				CString split_indication = tmmsm26["SPLIT_INDICATION"].ToString();
				//处理次数
				CString treatment_counter = tmmsm26["SAME_PROC_NUM"].ToString();
				//钢水量
				CDecimal steel_weight = tmmsm26["STEEL_WT"].ToDecimal() * 1000;
				//空包重量
				CDecimal empty_ladle_weight = tmmsm26["EMPTY_LADLE_WEIGHT"].ToDecimal() * 1000;
				//班
				CString shift_team = tmmsm26["PROD_SHIFT_GROUP"].ToString();
				//操作员
				CString operator1 = tmmsm26["ASSISTANT"].ToString();
				//实际钢种
				CString grade_act = tmmsm26["ST_NO"].ToString();
				//炉次开始时间
				if (tmmsm26["START_TIME"].ToString() != " " && tmmsm26["START_TIME"].ToDecimal() >= 20000101000000){
					tmmsm26["START_TIME"] = CDateTime::Parse(tmmsm26["START_TIME"].ToString());
				}
				else{
					tmmsm26["START_TIME"] = "19000101000000";
				}
				//炉次结束时间
				if (tmmsm26["END_TIME"].ToString() != " " && tmmsm26["END_TIME"].ToDecimal() >= 20000101000000){
					tmmsm26["END_TIME"] = CDateTime::Parse(tmmsm26["END_TIME"].ToString());
				}
				else{
					tmmsm26["END_TIME"] = "19000101000000";
				}
				//吹氩量
				CString argon_tot = tmmsm26["AR_SUM_COMSUME"].ToString();
				//吹氩时间
				CString argon_time = tmmsm26["AR_BLOW_TIME"].ToString();
				//吹氮量
				CString n2_tot = tmmsm26["EAF_TOTAL_N2_CONS"].ToString();
				//吹氮时间
				CString n2_time = tmmsm26["N2_TIME"].ToString();
				Log::Trace("", __FUNCTION__, "REC_CREATE_TIME=[{0}]", tmmsm26["REC_CREATE_TIME"].ToString());//记录插入时间
				/*CDateTime timestamp = CDateTime::Parse(tmmsm26["REC_CREATE_TIME"].ToString());
				lts["TIMESTAMP"] = timestamp;*/
				//钢包号
				CString steel_ladle_no = tmmsm26["LADLE_NO"].ToString();
				//包龄
				CString ladle_life = tmmsm26["LADLE_AGE"].ToString();
				//到站钢包重量
				CString ladle_arrive_wt = tmmsm26["LADLE_ARRIVE_WT"].ToString();
				//到站钢水重量
				CDecimal ladle_arrive_steel_wt = tmmsm26["LADLE_ARRIVE_STEEL_WT"].ToDecimal() * 1000;
				//出站总重量
				CDecimal ladle_depart_wt = tmmsm26["LADLE_LEAVE_WT"].ToDecimal() * 1000;
				//钢(铁)包离开时刻
				if (tmmsm26["LADLE_DEPART_TIME"].ToString() != " " && tmmsm26["LADLE_DEPART_TIME"].ToDecimal() >= 20000101000000){
					CDateTime ladle_depart_time = CDateTime::Parse(tmmsm26["LADLE_DEPART_TIME"].ToString());
					lts["LADLE_DEPART_TIME"] = ladle_depart_time;
				}
				else{
					lts["LADLE_DEPART_TIME"] = "19000101000000";
				}
				//下道设备号
				CString next_dev_code = tmmsm26["NEXT_DEV_CODE"].ToString();
				sql_insert = " insert into DA_LTS_PROD_SUMMARY (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, TREATMENT_COUNTER, "
					" STEEL_WEIGHT, EMPTY_LADLE_WEIGHT, SHIFT_TEAM, OPERATOR, GRADE_ACT, HEAT_START, "
					" HEAT_END, ARGON_TOT, ARGON_TIME, N2_TOT, N2_TIME, "
					" STEEL_LADLE_NO, LADLE_LIFE, LADLE_ARRIVE_WT, LADLE_ARRIVE_STEEL_WT, LADLE_DEPART_WT, "
					" LADLE_DEPART_TIME, NEXT_DEV_CODE) "
					" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "','" + treatment_counter + "',"
					" '" + steel_weight.ToString() + "','" + empty_ladle_weight.ToString() + "','" + shift_team + "','" + operator1 + "','" + grade_act + "',to_date('" + tmmsm26["START_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" to_date('" + tmmsm26["END_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + argon_tot + "','" + argon_time + "','" + n2_tot + "','" + n2_time + "', "
					" '" + steel_ladle_no + "','" + ladle_life + "','" + ladle_arrive_wt + "','" + ladle_arrive_steel_wt.ToString() + "','" + ladle_depart_wt.ToString() + "', "
					" to_date('" + lts["LADLE_DEPART_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + next_dev_code + "' "
					" ) ";
				cmd_insert.SetCommandText(sql_insert);
				cmd_insert.Parameters.Set("id", id);
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.ExecuteNonQuery();
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.Close();
			}
			else{
				if (lts.QueryCount("HEAT_NUMBER"))
				{
					lts.Delete("HEAT_NUMBER");
				}
			}
		}
		else if (table_name == "DA_RH_PRO_SUMMARY"){
			tmmsm23["PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"].ToString().Trim();
			tmmsm23["HEAT_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"].ToString().Trim();
			tmmsm23["ID_SJ"] = bcls_rec->Tables["JKDP"].Rows[0]["ID_SJ"].ToString().Trim();
			tmmsm23["L2_PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"].ToString().Trim();
			tmmsm23.Query("PROC_NO,HEAT_NO,L2_PROC_NO");
			tmmsm23.TrimOrBlank();
			CString heatnumber = tmmsm23["HEAT_NO"].ToString();
			if (v_proc_div == "I" || v_proc_div == "U"){
				CDecimal count = 0;
				cmd_sql_count.SetCommandText(" select * from DA_RH_PRO_SUMMARY where HEATNUMBER='" + heatnumber + "' ");
				cmd_sql_count.ExecuteReader();
				if (cmd_sql_count.Read())
				{
					count = cmd_sql_count.GetDecimal(1);
				}
				cmd_sql_count.Close();
				if (count != 0){
					cmd_sql_delete.SetCommandText(" delete from DA_RH_PRO_SUMMARY where HEATNUMBER='" + heatnumber + "' ");
					cmd_sql_delete.ExecuteNonQuery();
					cmd_sql_delete.Close();
				}
				CString id = tmmsm23["ID_SJ"].ToString();
				//工位名
				CString aggregatecode = tmmsm23["DEV_CODE"].ToString();
				//计划号
				CString planid = tmmsm23["SM_PLAN_NO"].ToString();
				//分包号
				CString splitindication = tmmsm23["SPLIT_INDICATION"].ToString();
				//空包重量
				CDecimal empty_ladle_weight = tmmsm23["EMPTY_LADLE_WEIGHT"].ToDecimal() * 1000;
				//班
				CString shift_team = tmmsm23["PROD_SHIFT_GROUP"].ToString();
				//操作员
				CString operator1 = tmmsm23["ASSISTANT"].ToString();
				//实际钢种
				CString grade_act = tmmsm23["ST_NO"].ToString();
				//实际重量
				CDecimal weight_act = tmmsm23["ACTRESULT"].ToDecimal() * 1000;
				//吹氧量
				CString oxygen_tot = tmmsm23["O2_SUM_COMSUME"].ToString();
				//吹氩量
				CString argon_tot = tmmsm23["AR_SUM_COMSUME"].ToString();
				//吹氮量
				CString nitrogen_tot = tmmsm23["N_SUM_COMSUME"].ToString();
				//炉次开始时间
				if (tmmsm23["START_TIME"].ToString() != " " && tmmsm23["START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime start_time = CDateTime::Parse(tmmsm23["START_TIME"].ToString());
					rh["HEATSTART"] = start_time;
				}
				else{
					rh["HEATSTART"] = "19000101000000";
				}
				//炉次结束时间
				if (tmmsm23["END_TIME"].ToString() != " " && tmmsm23["END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime end_time = CDateTime::Parse(tmmsm23["END_TIME"].ToString());
					rh["HEATEND"] = end_time;
				}
				else{
					rh["HEATEND"] = "19000101000000";
				}
				//处理次数
				CString treatmentcounter = tmmsm23["SAME_PROC_NUM"].ToString();
				CString naturalgas_tot = tmmsm23["NG_COMSUME"].ToString();
				//压缩空气量
				CString compressair_tot = tmmsm23["COMPRESSAIR_TOT"].ToString();
				//蒸汽量
				CString steam_tot = tmmsm23["STEAM_TOT"].ToString();
				//工业水量
				CString industrywater_tot = tmmsm23["WATER_USE_QTY"].ToString();
				//饮用水量
				CString potablewater_tot = tmmsm23["POTABLEWATER_TOT"].ToString();
				//高真空度
				CString deep_vac_press = tmmsm23["DEEP_VAC_PRESS"].ToString();
				//高真空保持时间
				CString deep_vac_dur = tmmsm23["DEEP_VAC_DUR"].ToString();
				//弱搅时间
				CDecimal soft_stirring_dur = tmmsm23["SOFT_STIRRING_DUR"].ToDecimal() * 60;
				//定氢值
				CString steel_h = tmmsm23["STEEL_H"].ToString();
				//开始温度
				CString temp_start = tmmsm23["START_STEEL_TEMP"].ToString();
				//结束温度
				CString temp_end = tmmsm23["END_STEEL_TEMP"].ToString();
				//出站总重量
				CString ladle_depart_wt = tmmsm23["LADLE_DEPART_WT"].ToString();
				//备注
				CString comments = tmmsm23["REMARK"].ToString();
				sql_insert = " insert into DA_RH_PRO_SUMMARY (ID, AGGREGATECODE, HEATNUMBER, PLANID, SPLITINDICATION, EMPTY_LADLE_WEIGHT, SHIFT_TEAM, "
					" OPERATOR, GRADE_ACT, WEIGHT_ACT, OXYGEN_TOT, ARGON_TOT, NITROGEN_TOT, "
					"  HEATSTART, HEATEND, TREATMENTCOUNTER, "
					" NATURALGAS_TOT, COMPRESSAIR_TOT, STEAM_TOT, INDUSTRYWATER_TOT, POTABLEWATER_TOT, "
					"  DEEP_VAC_PRESS, DEEP_VAC_DUR, SOFT_STIRRING_DUR, STEEL_H, TEMP_START, "
					" TEMP_END, LADLE_DEPART_WT, COMMENTS) "
					" values('" + id+ "','" + aggregatecode + "','" + heatnumber + "','" + planid + "','" + splitindication + "','" + empty_ladle_weight.ToString() + "','" + shift_team + "', "
					" '" + operator1 + "','" + grade_act + "','" + weight_act.ToString() + "','" + oxygen_tot + "','" + argon_tot + "','" + nitrogen_tot + "', "
					" to_date('" + rh["HEATSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + rh["HEATEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + treatmentcounter +"', "
					" '" + naturalgas_tot + "','" + compressair_tot + "','" + steam_tot + "','" + industrywater_tot + "','" + potablewater_tot + "', "
					" '" + deep_vac_press + "','" + deep_vac_dur + "','" + soft_stirring_dur.ToString() + "','" + steel_h + "','" + temp_start + "', "
					" '" + temp_end + "','" + ladle_depart_wt + "','" + comments + "' "
					" ) ";
				cmd_insert.SetCommandText(sql_insert);
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.ExecuteNonQuery();
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.Close();
			}
			else{
				if (rh.QueryCount("HEATNUMBER"))
				{
					rh.Delete("HEATNUMBER");
				}
			}
		}
		else if (table_name == "DA_VOD_PRO_SUMMARY"){
			tmmsm25["PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"].ToString().Trim();
			tmmsm25["HEAT_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"].ToString().Trim();
			tmmsm25["ID_SJ"] = bcls_rec->Tables["JKDP"].Rows[0]["ID_SJ"].ToString().Trim();
			tmmsm25["L2_PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "HEAT_NO=[{0}]", tmmsm25["HEAT_NO"].ToString());//关键字--未定
			tmmsm25.Query("HEAT_NO,L2_PROC_NO");
			tmmsm25.TrimOrBlank();
			vod["HEATNUMBER"] = tmmsm25["HEAT_NO"].ToString();
			if (vod.QueryCount("HEATNUMBER"))
			{
				vod.Delete("HEATNUMBER");
			}
			Log::Trace("", __FUNCTION__, "HEATNUMBER=[{0}]", vod["HEATNUMBER"].ToString());//关键字--未定
			if (v_proc_div == "I" || v_proc_div == "U"){
				CString id = tmmsm25["ID_SJ"].ToString();
				CString heatnumber = vod["HEATNUMBER"].ToString();
				//工位名
				CString aggregatecode = tmmsm25["DEV_CODE"].ToString();
				//订单号
				CString splitindication = tmmsm25["SPLIT_INDICATION"].ToString();
				//处理次数
				CString treatmentcounter = tmmsm25["SAME_PROC_NUM"].ToString();
				//班
				CString shiftteam = tmmsm25["PROD_SHIFT_GROUP"].ToString();
				//操作员
				CString operator1 = tmmsm25["ASSISTANT"].ToString();
				//空包重量
				CDecimal emptyladleweight = tmmsm25["EMPTY_LADLE_WEIGHT"].ToDecimal() * 1000;
				//实际钢种
				CString gradeact = tmmsm25["ST_NO"].ToString();
				//实际出钢量
				CDecimal weightact = tmmsm25["ACTRESULT"].ToDecimal() * 1000;
				//炉次开始时间
				if (tmmsm25["START_TIME"].ToString() != " "  && tmmsm25["START_TIME"].ToDecimal() >= 20000101000000){
					CDateTime start_time = CDateTime::Parse(tmmsm25["START_TIME"].ToString());
					vod["HEATSTART"] = start_time;
				}
				else{
					vod["HEATSTART"] = "19000101000000";
				}
				//炉次结束时间
				if (tmmsm25["END_TIME"].ToString() != " "  && tmmsm25["END_TIME"].ToDecimal() >= 20000101000000){
					CDateTime end_time = CDateTime::Parse(tmmsm25["END_TIME"].ToString());
					vod["HEATEND"] = end_time;
				}
				else{
					vod["HEATEND"] = "19000101000000";
				}
				//吹氧量
				CString oxygentot = tmmsm25["OXYGEN_FINAL"].ToString();
				CString naturalgas_tot = tmmsm25["NATURALGAS_TOT"].ToString();
				//压缩空气量
				CString compressair_tot = tmmsm25["COMPRESSAIR_TOT"].ToString();
				// 蒸汽量
				CString steam_tot = tmmsm25["STEAM_TOT"].ToString();
				// 工业用水量
				CString industrywater_tot = tmmsm25["WATER_USE_QTY"].ToString();
				//记录插入时间
				Log::Trace("", __FUNCTION__, "HEATNUMBER=[{0}]", vod["HEATNUMBER"].ToString());//关键字--未定
				//钢包号
				CString steel_ladle_no = tmmsm25["LADLE_NO"].ToString();
				//包龄
				CString ladle_life = tmmsm25["LADLE_AGE"].ToString();
				//到站温度
				CString ladle_arrive_temp = tmmsm25["LADLE_ARRIVE_TEMP"].ToString();
				//到站钢包重量
				CString ladle_arrive_wt = tmmsm25["LADLE_ARRIVE_WT"].ToString();
				//到站钢水重量
				CString ladle_arrive_steel_wt = tmmsm25["LADLE_ARRIVE_STEEL_WT"].ToString();
				//处理前渣厚
				CString slag_thick_start = tmmsm25["SLAG_THICK_START"].ToString();
				//处理前渣重
				CDecimal slag_wt_start = tmmsm25["SLAG_WT_START"].ToDecimal() * 1000;
				//净空(实绩)
				CString headroom = tmmsm25["HEADROOM"].ToString();
				//氧枪模式 为什么一直报不存在
				CString o2_lance_mode = tmmsm25["O2_LANCE_MODE"].ToString();
				//沸腾期结束温度
				CString vcd_end_temp = " ";
				if (tmmsm25["VCD_END_TEMP"].ToString()!=" "){
					vcd_end_temp = tmmsm25["VCD_END_TEMP"].ToString().Trim();
				}
				else{
					vcd_end_temp = "0";
				}
				//沸腾期结束C
				CString vcd_end_c = tmmsm25["VCD_END_C"].ToString().Trim();
				Log::Trace("", __FUNCTION__, "VCD_END_C=[{0}]", vod["VCD_END_C"].ToString());
				//抽真空时间
				CDecimal vacuum_pump_duration = tmmsm25["VACUUM_PUMP_DURATION"].ToDecimal() * 60;
				//沸腾期10min真空度
				CString vacuum_degree_boil_10min = tmmsm25["VACUUM_DEGREE_BOIL_10MIN"].ToString();
				Log::Trace("", __FUNCTION__, "VACUUM_DEGREE_BOIL_10MIN=[{0}]", vod["VACUUM_DEGREE_BOIL_10MIN"].ToString());
				//沸腾极限真空度
				CString highest_vacuum_degree_boil = tmmsm25["HIGHEST_VACUUM_DEGREE_BOIL"].ToString();
				//沸腾时间
				CDecimal boil_duration = tmmsm25["BOIL_DURATION"].ToDecimal() * 60;
				//高真空沸腾时间
				CString fine_vacuum_boil_duration = tmmsm25["FINE_VACUUM_BOIL_DURATION"].ToString();
				Log::Trace("", __FUNCTION__, "FINE_VACUUM_BOIL_DURATION=[{0}]", vod["FINE_VACUUM_BOIL_DURATION"].ToString());
				//高真空还原时间
				if (tmmsm25["FINE_VACUUM_REDUCING_DURATION"].ToString()!=" "){
					vod["FINE_VACUUM_REDUCING_DURATION"] = tmmsm25["FINE_VACUUM_REDUCING_DURATION"].ToDecimal() * 60;
				}
				else{
					vod["FINE_VACUUM_REDUCING_DURATION"] = "19000101000000";
				}
				//还原极限真空度
				if (tmmsm25["HIGHEST_VACUUM_DEGREE_REDUCING"].ToString() != " "){
					vod["HIGHEST_VACUUM_DEGREE_REDUCING"] = tmmsm25["HIGHEST_VACUUM_DEGREE_REDUCING"].ToString();
				}
				else{
					vod["HIGHEST_VACUUM_DEGREE_REDUCING"] = "19000101000000";
				}
				//还原周期
				if (tmmsm25["REDUCING_DURATION"].ToString() != " "){
					vod["REDUCING_DURATION"] = tmmsm25["REDUCING_DURATION"].ToDecimal() * 60;
				}
				//还原结束温度
				if (tmmsm25["REDUC_END_TEMP"].ToString() != " "){
					vod["REDUC_END_TEMP"] = tmmsm25["REDUC_END_TEMP"].ToString();
				}
				//二级算氧量
				if (tmmsm25["CAL_O2_CONS"].ToString() != " "){
					vod["CAL_O2_CONS"] = tmmsm25["CAL_O2_CONS"].ToString();
				}
				Log::Trace("", __FUNCTION__, "CAL_O2_CONS=[{0}]", tmmsm25["CAL_O2_CONS"].ToString());
				//氧气消耗量
				if (tmmsm25["TOTAL_O2_CONS"].ToString() != " "){
					vod["TOTAL_O2_CONS"] = tmmsm25["TOTAL_O2_CONS"].ToString();
				}
				//吹氧时间
				if (tmmsm25["O2_BLOW_DURATION"].ToString() != " "){
					vod["O2_BLOW_DURATION"] = tmmsm25["O2_BLOW_DURATION"].ToDecimal() * 60;
				}
				Log::Trace("", __FUNCTION__, "O2_BLOW_DURATION=[{0}]", tmmsm25["O2_BLOW_DURATION"].ToString());
				//出站总重量
				CDecimal ladle_depart_wt = tmmsm25["LADLE_DEPART_WT"].ToDecimal() * 1000;
				//出站渣厚
				CString slag_thick_end = tmmsm25["SLAG_THICK_END"].ToString();
				//出站温度
				CString ladle_depart_temp = tmmsm25["LADLE_DEPART_TEMP"].ToString();
				Log::Trace("", __FUNCTION__, "LADLE_DEPART_TEMP=[{0}]", tmmsm25["LADLE_DEPART_TEMP"].ToString());
				//冶炼时长
				Log::Trace("", __FUNCTION__, "MELT_DURATION=[{0}]", tmmsm25["MELT_DURATION"].ToString());
				CString melt_duration = tmmsm25["MELT_DURATION"].ToString();
				//吹氩量
				CString argontuytot = tmmsm25["AR_SUM_COMSUME"].ToString();
				//下道设备号
				CString next_dev_code = tmmsm25["NEXT_DEV_CODE"].ToString();
				Log::Trace("", __FUNCTION__, "NEXT_DEV_CODE=[{0}]", tmmsm25["NEXT_DEV_CODE"].ToString());
				vod.TrimOrBlank();
				vod.Print();
				Log::Trace("", "", "line = {0}", __LINE__);
				sql_insert = " insert into DA_VOD_PRO_SUMMARY (ID, AGGREGATECODE, HEATNUMBER, SPLITINDICATION, TREATMENTCOUNTER, "
					" SHIFTTEAM, OPERATOR, EMPTYLADLEWEIGHT, GRADEACT, WEIGHTACT, HEATSTART, HEATEND, "
					" OXYGENTOT, NATURALGAS_TOT, COMPRESSAIR_TOT, STEAM_TOT, "
					" INDUSTRYWATER_TOT,  STEEL_LADLE_NO, LADLE_LIFE, LADLE_ARRIVE_TEMP, "
					" LADLE_ARRIVE_WT, LADLE_ARRIVE_STEEL_WT, SLAG_THICK_START, SLAG_WT_START, HEADROOM, "
					" O2_LANCE_MODE, VCD_END_TEMP, VCD_END_C, VACUUM_PUMP_DURATION, VACUUM_DEGREE_BOIL_10MIN, "
					" HIGHEST_VACUUM_DEGREE_BOIL, BOIL_DURATION, FINE_VACUUM_BOIL_DURATION, "
					" FINE_VACUUM_REDUCING_DURATION, HIGHEST_VACUUM_DEGREE_REDUCING, REDUCING_DURATION, "
					" REDUC_END_TEMP, CAL_O2_CONS, TOTAL_O2_CONS, O2_BLOW_DURATION, LADLE_DEPART_WT, "
					" SLAG_THICK_END, LADLE_DEPART_TEMP, MELT_DURATION, NEXT_DEV_CODE,ORDERNUMBER,ARGONTUYTOT) "
					" values( "
					" '" + id + "','" + aggregatecode + "','" + heatnumber + "','" + splitindication + "','" + treatmentcounter + "', "
					" '" + shiftteam + "','" + operator1 + "','" + emptyladleweight.ToString() + "','" + gradeact + "','" + weightact.ToString() + "',to_date('" + vod["HEATSTART"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + vod["HEATEND"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + oxygentot + "','" + naturalgas_tot + "','" + compressair_tot + "','" + steam_tot + "', "
					" '" + industrywater_tot + "','" + steel_ladle_no + "','" + ladle_life + "','" + ladle_arrive_temp + "', "
					" '" + ladle_arrive_wt + "','" + ladle_arrive_steel_wt + "','" + slag_thick_start + "','" + slag_wt_start.ToString() + "','" + headroom + "',"
					" '" + o2_lance_mode + "','" + vcd_end_temp + "','" + vcd_end_c + "','" + vacuum_pump_duration.ToString() + "','" + vacuum_degree_boil_10min + "', "
					" '" + highest_vacuum_degree_boil + "','" + boil_duration.ToString() + "','" + fine_vacuum_boil_duration + "', "
					" '" + vod["FINE_VACUUM_REDUCING_DURATION"].ToString() + "','" + vod["HIGHEST_VACUUM_DEGREE_REDUCING"].ToString() + "','" + vod["REDUCING_DURATION"].ToString() + "',"
					" '" + vod["REDUC_END_TEMP"].ToString() + "','" + vod["CAL_O2_CONS"].ToString() + "','" + vod["TOTAL_O2_CONS"].ToString() + "','" + vod["O2_BLOW_DURATION"].ToString() + "','" + ladle_depart_wt.ToString() + "'， "
					" '" + slag_thick_end + "','" + ladle_depart_temp + "','" + melt_duration + "','" + next_dev_code + "',' ','" + argontuytot + "' "
					" ) ";
				cmd_insert.SetCommandText(sql_insert);
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.ExecuteNonQuery();
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.Close();
			}
			else{
				if (vod.QueryCount("HEATNUMBER"))
				{
					vod.Delete("HEATNUMBER");
				}
			}
		}
		else if (table_name == "DA_CCM_PRO_SUMMARY"){
			tmmsm31["PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"].ToString().Trim();
			tmmsm31["HEAT_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"].ToString().Trim();
			tmmsm31["L2_PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"].ToString().Trim();
			tmmsm31.Query("HEAT_NO,L2_PROC_NO");
			tmmsm31.TrimOrBlank();
			CString heatnumber  = tmmsm31["HEAT_NO"].ToString();
			Log::Trace("", __FUNCTION__, "HEATNUMBER=[{0}]", ccm["HEATNUMBER"].ToString());//关键字--未定
			CDecimal count = 0;
			cmd_sql_count.SetCommandText(" select * from DA_CCM_PRO_SUMMARY where HEATNUMBER='"+heatnumber+"' ");
			cmd_sql_count.ExecuteReader();
			if (cmd_sql_count.Read())
			{
				count = cmd_sql_count.GetDecimal(1);
			}
			cmd_sql_count.Close();
			if (count != 0){
				cmd_sql_delete .SetCommandText(" delete from DA_CCM_PRO_SUMMARY where HEATNUMBER='" + heatnumber + "' ");
				cmd_sql_delete.ExecuteNonQuery();
				cmd_sql_delete.Close();
			}
			CString id = tmmsm31["ID_SJ"].ToString();
			CString aggregatecode = tmmsm31["DEV_CODE"].ToString();
			//计划号
			CString planid = tmmsm31["SM_PLAN_NOL2"].ToString();
			//分包标志
			CString splitindication = tmmsm31["SPLIT_INDICATION"].ToString();
			//处理次数
			CString treatmentcounter = tmmsm31["SAME_PROC_NUM"].ToString();
			//班
			CString shiftteam = tmmsm31["PROD_SHIFT_GROUP"].ToString();
			//操作员名称
			CString operatorname = tmmsm31["ASSISTANT"].ToString();
			//当前炉次在浇次中的实际顺序号
			CDecimal heatincast = 0;
			if (tmmsm31["CAST_NO"].ToString() == " "){
				heatincast = 0;
			}
			else{
				heatincast = tmmsm31["CAST_NO"].ToDecimal();
			}
			//浇次炉数
			CString castcounter = tmmsm31["CAST_DIV_NO"].ToString();
			//钢种
			CString grade = tmmsm31["ST_NO"].ToString();
			//钢包号
			CString ladlenumber = tmmsm31["LADLE_NO"].ToString();
			//最新一次的钢包测温温度
			CString ladletemp = tmmsm31["LADLE_TEMP"].ToString();
			//最新一次点钢包测温时间
			Log::Trace("", __FUNCTION__, "LADLE_TIME=[{0}]", tmmsm31["LADLE_TIME"].ToString());//最新一次点钢包测温时间
			if (tmmsm31["LADLE_TIME"].ToString() != " "  && tmmsm31["LADLE_TIME"].ToDecimal() >= 20000101000000){
				tmmsm31["LADLE_TIME"] = CDateTime::Parse(tmmsm31["LADLE_TIME"].ToString());
			}
			else{
				tmmsm31["LADLE_TIME"] = "19000101000000";
			}
			//钢包到达回转台时间
			if (tmmsm31["LADLE_ARRIVE_TIME"].ToString() != " "  && tmmsm31["LADLE_ARRIVE_TIME"].ToDecimal() >= 20000101000000){
				tmmsm31["LADLE_ARRIVE_TIME"] = CDateTime::Parse(tmmsm31["LADLE_ARRIVE_TIME"].ToString());
			}
			else{
				tmmsm31["LADLE_ARRIVE_TIME"] = "19000101000000";
			}
			//大包打开滑动水口时间
			if (tmmsm31["LADLE_OPEN_TIME"].ToString() != " "  && tmmsm31["LADLE_OPEN_TIME"].ToDecimal() >= 20000101000000){
				tmmsm31["LADLE_OPEN_TIME"] = CDateTime::Parse(tmmsm31["LADLE_OPEN_TIME"].ToString());
			}
			else{
				tmmsm31["LADLE_OPEN_TIME"] = "19000101000000";
			}
			//钢包离开回转台时间
			if (tmmsm31["LADLE_LEAVE_TIME"].ToString() != " "  && tmmsm31["LADLE_LEAVE_TIME"].ToDecimal() >= 20000101000000){
				tmmsm31["LADLE_LEAVE_TIME"] = CDateTime::Parse(tmmsm31["LADLE_LEAVE_TIME"].ToString());
			}
			else{
				tmmsm31["LADLE_LEAVE_TIME"] = "19000101000000";
			}
			//最后一块板坯切割时间
			if (tmmsm31["FIN_CUT_TIME"].ToString() != " "  && tmmsm31["FIN_CUT_TIME"].ToDecimal() >= 20000101000000){
				tmmsm31["FIN_CUT_TIME"] = CDateTime::Parse(tmmsm31["FIN_CUT_TIME"].ToString());
			}
			else{
				tmmsm31["FIN_CUT_TIME"] = "19000101000000";
			}
			//钢包到达回转台时净重
			CDecimal ladlearrivenetweight = tmmsm31["LADLE_ARRIVE_WT"].ToDecimal() * 1000;
			//钢包离开回转台时净重
			CDecimal ladledepartnetweight = tmmsm31["LADLE_LEAVE_WT"].ToDecimal() * 1000;
			//回转台保温罩使用炉数
			CDecimal shroudcounter = 0;
			if (tmmsm31["SHROUD_COUNTER"].ToString() == " "){
				shroudcounter = 0;
			}
			else{
				shroudcounter = tmmsm31["SHROUD_COUNTER"].ToDecimal();
			}
			//中包号
			CString tundishnumber1 = tmmsm31["TD_NO_1"].ToString();
			//中包号 当一个浇铸炉次中换了中间包，此字段有效
			CString tundishnumber2 = tmmsm31["TD_NO_2"].ToString();
			//铸流1标识
			CString moldnumber1 = tmmsm31["MOLD_NO1"].ToString();
			//铸流2标识
			CString moldnumber2 = tmmsm31["MOLD_NO2"].ToString();
			//中包1保护渣类型
			CString tundishpowertype1 = tmmsm31["TUNDISH_POWDER_TYPE_1"].ToString();
			//中包2保护渣类型
			CString tundishpowertype2 = tmmsm31["TUNDISH_POWDER_TYPE_2"].ToString();
			//总量
			CDecimal tundishpoweramount1 = tmmsm31["TUNDISH_POWDER_AMOUNT_1"].ToDecimal() * 1000;
			//--
			CDecimal moldpoweramountstart = tmmsm31["MOLD_POWDER_AMOUNT_START"].ToDecimal() * 1000;
			//滑动水口使用的炉数
			CString sencounter1 = tmmsm31["SEN_COUNTER_1"].ToString();
			//滑动水口使用的炉数
			CString sencounter2 = tmmsm31["SEN_COUNTER_2"].ToString();
			Log::Trace("", __FUNCTION__, "SENCOUNTER2=[{0}]", ccm["SENCOUNTER2"].ToString());//本炉次生产的坯子数
			//本炉次生产的坯子数
			CString slabsproduced = tmmsm31["CUT_SLAB_NUM"].ToString();
			//记录插入时间
			Log::Trace("", __FUNCTION__, "REC_CREATE_TIME=[{0}]", tmmsm31["REC_CREATE_TIME"].ToString());//记录插入时间
			/*CDateTime timestamp = CDateTime::Parse(tmmsm31["REC_CREATE_TIME"].ToString());
			ccm["TIMESTAMP"] = timestamp;*/
			//结晶器水口类型
			//ccm["MOLD_NOZZLE_TYPE"] = tmmsm31["MOLD_POWDER_TYPE_CAST"].ToString();
			//ccm.Insert();
			//LADLE_CLOSE_TIME
			sql_insert = " insert into DA_CCM_PRO_SUMMARY ( "
				" ID, AGGREGATECODE, HEATNUMBER, PLANID, SPLITINDICATION, TREATMENTCOUNTER, SHIFTTEAM, "
				" OPERATORNAME, HEATINCAST, CASTCOUNTER, GRADE, LADLENUMBER, LADLETEMP, LADLETIME, "
				" LADLEARRIVETIME, LADLEOPENTIME, LADLEDEPARTTIME, LASTSLABCUTTIME, LADLEARRIVENETWEIGHT, "
				" LADLEDEPARTNETWEIGHT, SHROUDCOUNTER, TUNDISHNUMBER1, TUNDISHNUMBER2, MOLDNUMBER1, "
				" MOLDNUMBER2, TUNDISHPOWERTYPE1, TUNDISHPOWERTYPE2, TUNDISHPOWERAMOUNT1, "
				" MOLDPOWERAMOUNTSTART, SENCOUNTER1, SENCOUNTER2, SLABSPRODUCED,LADLE_CLOSE_TIME) "
				" values('" + id + "','" + aggregatecode + "','" + heatnumber + "','" + planid + "','" + splitindication + "','" + treatmentcounter + "','" + shiftteam + "', "
				" '" + operatorname + "','" + heatincast.ToString() + "','" + castcounter + "','" + grade + "','" + ladlenumber + "','" + ladletemp + "',to_date('" + tmmsm31["LADLE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),  "
				" to_date('" + tmmsm31["LADLE_ARRIVE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + tmmsm31["LADLE_OPEN_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + tmmsm31["LADLE_LEAVE_TIME"].ToString() + "','yyyy-MM-dd hh24:MI:SS'), to_date('" + tmmsm31["FIN_CUT_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), '" + ladlearrivenetweight.ToString() + "', "
				" '" + ladledepartnetweight.ToString() + "','" + shroudcounter.ToString() + "','" + tundishnumber1 + "','" + tundishnumber2 + "','" + moldnumber1 + "', "
				" '" + moldnumber2 + "','" + tundishpowertype1 + "','" + tundishpowertype2 + "','" + tundishpoweramount1.ToString() + "', "
				" '" + moldpoweramountstart.ToString() + "','" + sencounter1 + "','" + sencounter2 + "','" + slabsproduced + "', "
				" to_date('" + tmmsm31["LADLE_CLOSE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS') "
				" ) ";
			cmd_insert.SetCommandText(sql_insert);
			cmd_insert.Parameters.Set("id", id);
			Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
			cmd_insert.ExecuteNonQuery();
			Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
			cmd_insert.Close();
		}
		else if (table_name == "DA_CCM_SLABSUMMARY"){
			tmmsm33["PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"].ToString().Trim();
			tmmsm33["HEAT_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"].ToString().Trim();
			tmmsm33.Query("PROC_NO,HEAT_NO");
			tmmsm33.TrimOrBlank();
			slab["HEATNUMBER"] = tmmsm33["HEAT_NO"].ToString();
			if (slab.QueryCount("HEATNUMBER"))
			{
				slab.Delete("HEATNUMBER");
			}
			//关键字--未定
			//计划号
			slab["PLANID"] = tmmsm33["PONO"].ToString();
			//炉次号
			slab["HEATNUMBER"] = tmmsm33["HEAT_NO"].ToString();
			////分包标志
			//slab["SPLITINDICATION"] = tmmsm33[""].ToString();
			////处理次数
			//slab["TREATMENTCOUNTER"] = tmmsm33[""].ToString();
			////铸流号
			//slab["STRANDNUMBER"] = tmmsm33[""].ToString();
			////实际板坯号
			//slab["SLABNUMBER"] = tmmsm33[""].ToString();
			////虚拟板坯号
			//slab["VIRTUALSLABID"] = tmmsm33[""].ToString();
			////板坯喷印号
			//slab["MARKINGNUMBER"] = tmmsm33[""].ToString();
			////最后一块板坯标志
			//slab["SLABFINAL"] = tmmsm33[""].ToString();
			////坯子切割时间
			//slab["SLABCUTTIME"] = tmmsm33[""].ToString();
			////取样完成
			//slab["SAMPLECUTDONE"] = tmmsm33[""].ToString();
			////计划板坯长度
			//slab["AIMLENGTH"] = tmmsm33[""].ToString();
			////实际板坯长度
			//slab["ACTUALLENGTH"] = tmmsm33[""].ToString();
			////实际板坯厚度
			//slab["THICKNESS"] = tmmsm33[""].ToString();
			////板坯头宽度
			//slab["WIDTHHEAD"] = tmmsm33[""].ToString();
			////板坯尾宽度
			//slab["WIDTHTAIL"] = tmmsm33[""].ToString();
			//计算重量
			slab["WEIGHTCALC"] = tmmsm33["L2_THEORY_WT"].ToString();
			//记录插入时间
			//slab["TIMESTAMP"] = datetime;
			//产品管理模块读取时间，由产品管理模块填入
			//slab["PMREADTIME"] = tmmsm33[""].ToString();
			////记录读取标志 －产品管理模块
			//slab["PMREAD"] = tmmsm33[""].ToString();
			////质量管理模块读取时间，由质量管理模块填入
			//slab["QMREADTIME"] = tmmsm33[""].ToString();
			////记录读取标志 －质量管理模块
			//slab["QMREAD"] = tmmsm33[""].ToString();
			////过程跟踪模块读取时间，由质量管理模块填入
			//slab["DMREADTIME"] = tmmsm33[""].ToString();
			////记录读取标志 －过程跟踪模块
			//slab["DMREAD"] = tmmsm33[""].ToString();
			////记录读取标志 NY模块
			//slab["NYREAD"] = tmmsm33[""].ToString();
			////NY模块读取时间'
			//slab["NYREADTIME"] = tmmsm33[""].ToString();
			slab.Insert();
		}
		else if (table_name == "DA_DES_PRO_SUMMARY"){
			tmmsm14["PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"].ToString().Trim();
			Log::Trace("", "", "PROC_NO = [{0}]", tmmsm14["PROC_NO"].ToString());
			tmmsm14.Query("PROC_NO");
			tmmsm14.Print();
			tmmsm14.TrimOrBlank();
			CString plan_id = tmmsm14["SM_PLAN_NOL2"].ToString().Trim();
			CString heat_number = tmmsm14["L2_PROC_NO"].ToString().Trim();
			CDecimal count = 0;
			cmd_sql_count.SetCommandText(" select * from DA_DES_PRO_SUMMARY01 where HEAT_NUMBER='" + heat_number + "' ");
			cmd_sql_count.ExecuteReader();
			if (cmd_sql_count.Read())
			{
				count = cmd_sql_count.GetDecimal(1);
			}
			cmd_sql_count.Close();
			if (count != 0){
				cmd_sql_delete.SetCommandText(" delete from DA_DES_PRO_SUMMARY01 where HEAT_NUMBER='" + heat_number + "' ");
				cmd_sql_delete.ExecuteNonQuery();
				cmd_sql_delete.Close();
			}
			CString id = tmmsm14["ID_SJ"].ToString();
			Log::Trace("", "", "ID_SJ = [{0}]", tmmsm14["ID_SJ"].ToString());
			CString split_indication = tmmsm14["SPLIT_INDICATION"].ToString();
			//ST_NO
			CString steelgrade = tmmsm14["ST_NO"].ToString();
			//DE_S_PREV_TEMP
			CString inittemp = tmmsm14["DE_S_PREV_TEMP"].ToString();
			//LADLE_NET_WEIGHT
			CDecimal ladle_net_weight = tmmsm14["LADLE_NET_WEIGHT"].ToDecimal()*1000;
			//INITIALSULPHUR
			CString initialsulphur = tmmsm14["INIT_S"].ToString();
			//LASTSULPHUR
			CString lastsulphur = tmmsm14["AFT_S"].ToString();
			//铁水罐号
			CString hmladleno = tmmsm14["IRON_LADLE_NO"].ToString();
			//铁水重量
			CString hm_weight = tmmsm14["LADLE_NET_WEIGHT"].ToString();
			//处理前温度
			CString hm_temp_begin = tmmsm14["DE_S_PREV_TEMP"].ToString();
			//处理后温度
			CString hm_temp_end = tmmsm14["DE_S_REP_TEMP"].ToString();
			//工位
			CString aggregate_name = tmmsm14["DEV_CODE"].ToString();
			//开始处理时间
			/*if (tmmsm14["DE_S_PREV_TIME"].ToString() != " "  && tmmsm14["DE_S_PREV_TIME"].ToDecimal() >= 20000101000000){
				CDateTime de_s_prev_time = CDateTime::Parse(tmmsm14["DE_S_PREV_TIME"].ToString());
				des["BEGIN_TIME"] = de_s_prev_time;
			}
			else{
				des["BEGIN_TIME"] = "19000101000000";
			}
			//结束处理时间
			if (tmmsm14["DE_S_REP_TIME"].ToString() != " "  && tmmsm14["DE_S_REP_TIME"].ToDecimal() >= 20000101000000){
				CDateTime de_s_rep_time = CDateTime::Parse(tmmsm14["DE_S_REP_TIME"].ToString());
				des["END_TIME"] = de_s_rep_time;
			}
			else{
				des["END_TIME"] = "19000101000000";
			}*/
			//最后一次采样号
			//des["LAST_SAMPLE_ID"] = tmmsm14[""].ToString();
			//班次
			CString shiftno = tmmsm14["PROD_SHIFT_NO"].ToString();
			//班别
			CString shiftname = tmmsm14["PROD_SHIFT_GROUP"].ToString();
			//操作员
			CString operator1 = tmmsm14["ASSISTANT"].ToString();
			//处理号
			CString treatment_number = tmmsm14["PROC_NO"].ToString();
			CString treatment_counter = tmmsm14["SAME_PROC_NUM"].ToString();
			//AFTER_TM_TEMP
			CString after_tm_temp = tmmsm14["DE_S_REP_TEMP"].ToString();
			//DT_AFTER_TM_TEMP
			CString dt_after_tm_temp = tmmsm14["DE_S_REP_TIME"].ToString();
			//O2_LANCE_NO
			CString lanceid = tmmsm14["O2_LANCE_NO"].ToString();
			//MSGCOUNT
			CString msgcount = tmmsm14["MSGCOUNT"].ToString();
			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
			sql_insert = " insert into DA_DES_PRO_SUMMARY01 (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, TREATMENT_COUNTER, "
				" STEELGRADE, HMLADLENO, LADLE_NET_WEIGHT, INITIALSULPHUR, LASTSULPHUR, INITTEMP, "
				" DTINITTEMP, AFTER_TM_TEMP, DT_AFTER_TM_TEMP,  LANCEID, DTTMSTART, "
				" DTTMFINISH, MSGCOUNT, TIME_STAMPS) "
				" values( "
				" '" + id + "','" + aggregate_name + "','" + heat_number + "','" + plan_id + "','" + split_indication + "','" + treatment_counter + "', "
				" '" + steelgrade + "','" + hmladleno + "','" + ladle_net_weight.ToString() + "','" + initialsulphur + "','" + lastsulphur + "','" + inittemp + "', "
				" to_date('" + tmmsm14["DE_S_PREV_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + after_tm_temp + "',to_date('" + dt_after_tm_temp + "', 'yyyy-MM-dd hh24:MI:SS'),'" + lanceid + "',to_date('" + tmmsm14["START_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'), "
				" to_date('" + tmmsm14["END_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + msgcount + "',to_date('" + datetime + "', 'yyyy-MM-dd hh24:MI:SS') "
				" ) ";
			cmd_insert.SetCommandText(sql_insert);
			Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
			cmd_insert.ExecuteNonQuery();
			Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
			cmd_insert.Close();
		}
		else if (table_name == "DA_IF_PRO_SUMMARY"){
			tmmsm19["PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"].ToString().Trim();
			tmmsm19["L2_PROC_NO"] = bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"].ToString().Trim();
			tmmsm19.Query("L2_PROC_NO");
			tmmsm19.TrimOrBlank();
			lif["HEAT_NUMBER"] = tmmsm27["L2_PROC_NO"].ToString().Trim();
			if (v_proc_div == "I" || v_proc_div == "U"){
				CString plan_id = tmmsm19["SM_PLAN_NOL2"].ToString().Trim();
				CString heat_number = tmmsm19["L2_PROC_NO"].ToString().Trim();
				CDecimal count = 0;
				cmd_sql_count.SetCommandText(" select * from DA_IF_PRO_SUMMARY where HEAT_NUMBER='" + heat_number + "' ");
				cmd_sql_count.ExecuteReader();
				if (cmd_sql_count.Read())
				{
					count = cmd_sql_count.GetDecimal(1);
				}
				cmd_sql_count.Close();
				if (count != 0){
					cmd_sql_delete.SetCommandText(" delete from DA_IF_PRO_SUMMARY where HEAT_NUMBER='" + heat_number + "' ");
					cmd_sql_delete.ExecuteNonQuery();
					cmd_sql_delete.Close();
				}
				CString id = tmmsm19["ID_SJ"].ToString();
				CString aggregate_name = tmmsm19["DEV_CODE"].ToString().Trim();
				heat_number = tmmsm19["L2_PROC_NO"].ToString();
				CString plan_number = tmmsm19["SM_PLAN_NOL2"].ToString();
				CString split_indication = tmmsm19["SPLIT_INDICATION"].ToString();
				CString ladle_number = tmmsm19["LADLE_NO"].ToString();
				CDecimal empty_ladle_weight = tmmsm19["EMPTY_LADLE_WEIGHT"].ToDecimal()*1000;
				CString shift_team = tmmsm19["PROD_SHIFT_GROUP"].ToString();
				CString operator1 = tmmsm19["ASSISTANT"].ToString();
				CString grade_act = tmmsm19["ST_NO"].ToString();
				CDecimal weight_act = tmmsm19["ACTRESULT"].ToDecimal() * 1000;
				CString heat_start = tmmsm19["START_TIME"].ToString();
				if (heat_start == " "){
					heat_start = "19990101110101";
				}
				CString heat_end = tmmsm19["END_TIME"].ToString();
				if (heat_end == " "){
					heat_end = "19990101110101";
				}
				CString heating_number = tmmsm19["HEAT_COUNT"].ToString();
				CDecimal heating_time = tmmsm19["BIL_MELT_TIME"].ToDecimal() * 60;
				CString power = tmmsm19["POWER_CONSUME"].ToString();
				CString first_heating_start = tmmsm19["FIRST_HEATING_START"].ToString();
				if (first_heating_start == " "){
					first_heating_start = "19990101110101";
				}
				CString last_heating_end = tmmsm19["LAST_HEATING_END"].ToString();
				if (last_heating_end == " "){
					last_heating_end = "19990101110101";
				}
				CDecimal premelt_weight = tmmsm19["LADLE_PRE_LIQUID_WT"].ToDecimal() * 1000;
				CString c_powder_weight = tmmsm19["C_POWDER_WEIGHT"].ToString();
				CString oxygen_number = tmmsm19["OXYGEN_NUMBER"].ToString();
				CDecimal blow_time = tmmsm19["BLOW_DURATION"].ToDecimal() * 60;
				CString oxygen_total = tmmsm19["OXYGEN_FINAL"].ToString();
				CString first_blow_start = tmmsm19["FIRST_BLOW_START"].ToString();
				if (first_blow_start == " "){
					first_blow_start = "19990101110101";
				}
				CString last_blow_end = tmmsm19["LAST_BLOW_END"].ToString();
				if (last_blow_end == " "){
					last_blow_end = "19990101110101";
				}
				CString tapping_start = tmmsm19["TAP_START_TIME"].ToString();
				if (tapping_start == " "){
					tapping_start = "19990101110101";
				}
				CString tapping_end = tmmsm19["TAP_END_TIME"].ToString();
				if (tapping_end == " "){
					tapping_end = "19990101110101";
				}
				CString nitrogen_top_amount = tmmsm19["NITROGEN_TOP_AMOUNT"].ToString();
				CDecimal nitrogen_top_time = tmmsm19["NITROGEN_TOP_TIME"].ToDecimal() * 60;
				CString nitrogen_bottom_amount = tmmsm19["BTTM_N_COMSUME"].ToString();
				CDecimal nitrogen_bottom_time = tmmsm19["NITROGEN_BOTTOM_TIME"].ToDecimal() * 60;
				CDecimal scrap_weight = tmmsm19["SCRAP_WEIGHT"].ToDecimal() * 1000;
				CString charge_start_time = tmmsm19["CHARGE_START_TIME"].ToString();
				if (charge_start_time == " "){
					charge_start_time = "19990101110101";
				}
				CString power_on_time = tmmsm19["POWER_START_TIME"].ToString();
				if (power_on_time == " "){
					power_on_time = "19990101110101";
				}
				CDecimal remain_wt = tmmsm19["REMAIN_WT"].ToDecimal() * 1000;
				CString taptotap_duration = tmmsm19["TAPTOTAP_DURATION"].ToString();
				CString next_dev_code = tmmsm19["NEXT_DEV_CODE"].ToString();
				CString melt_mode = tmmsm19["MELT_MODE"].ToString();
				CDecimal full_ladle_weight = tmmsm19["FULL_LADLE_WEIGHT"].ToDecimal() * 1000;
				CString heat_life = tmmsm19["FURNACE_AGE"].ToString();
				CString firebrick_factory = tmmsm19["WORK_MAKER"].ToString();
				sql_insert = " insert into DA_IF_PRO_SUMMARY (ID, AGGREGATE_NAME, HEAT_NUMBER, PLAN_NUMBER, SPLIT_INDICATION, LADLE_NUMBER, "
					" EMPTY_LADLE_WEIGHT, SHIFT_TEAM, OPERATOR, GRADE_ACT, WEIGHT_ACT, HEAT_START, HEAT_END, "
					" HEATING_NUMBER, HEATING_TIME, POWER, FIRST_HEATING_START, LAST_HEATING_END, "
					" PREMELT_WEIGHT, C_POWDER_WEIGHT, OXYGEN_NUMBER, BLOW_TIME, OXYGEN_TOTAL, "
					" FIRST_BLOW_START, LAST_BLOW_END, TAPPING_START, TAPPING_END, NITROGEN_TOP_AMOUNT, "
					" NITROGEN_TOP_TIME, NITROGEN_BOTTOM_AMOUNT, NITROGEN_BOTTOM_TIME, SCRAP_WEIGHT, "
					" CHARGE_START_TIME, POWER_ON_TIME, "
					" REMAIN_WT, TAPTOTAP_DURATION, NEXT_DEV_CODE, MELT_MODE, FULL_LADLE_WEIGHT, HEAT_LIFE, FIREBRICK_FACTORY) "
					" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + plan_number + "','" + split_indication + "','" + ladle_number + "', "
					" '" + empty_ladle_weight.ToString() + "','" + shift_team + "','" + operator1 + "','" + grade_act + "','" + weight_act.ToString() + "',to_date('" + heat_start + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + heat_end + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + heating_number + "','" + heating_time.ToString() + "','" + power + "',to_date('" + first_heating_start + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + last_heating_end + "', 'yyyy-MM-dd hh24:MI:SS'), "
					" '" + premelt_weight.ToString() + "','" + c_powder_weight + "','" + oxygen_number + "','" + blow_time.ToString() + "','" + oxygen_total + "', "
					" to_date('" + first_blow_start + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + last_blow_end + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + tapping_start + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + tapping_end + "', 'yyyy-MM-dd hh24:MI:SS'),'" + nitrogen_top_amount + "', "
					" '" + nitrogen_top_time.ToString() + "','" + nitrogen_bottom_amount + "','" + nitrogen_bottom_time.ToString() + "','" + scrap_weight.ToString() + "', "
					" to_date('" + charge_start_time + "', 'yyyy-MM-dd hh24:MI:SS'),to_date('" + power_on_time + "', 'yyyy-MM-dd hh24:MI:SS'),'" + remain_wt.ToString() + "','" + taptotap_duration + "','" + next_dev_code + "','" + melt_mode + "', "
					" '" + full_ladle_weight.ToString() + "','" + heat_life + "','" + firebrick_factory + "' "
					" ) ";
				cmd_insert.SetCommandText(sql_insert);
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.ExecuteNonQuery();
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.Close();
			}
			else{
				if (aod.QueryCount("HEAT_NUMBER"))
				{
					aod.Delete("HEAT_NUMBER");
				}
			}
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
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


