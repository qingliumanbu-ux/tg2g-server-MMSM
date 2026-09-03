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

/*<remark>=========================================================
/// <summary>
/// 转炉生产炉
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


int f_mmsm21_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测

BM2F_ENTERACE_TELE(cm_e2t8s5_rcv)

int f_cm_e2t8s5_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd_inq_code(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_yl(conn);
	CDbCommand cmd_yz(conn);
	CDbCommand cmd_yzsc(conn);
	CDbCommand cmd_inq_test(conn);
	CDbCommand cmd_kr(conn);
	CModel tmmsm21("TMMSM21"); 
	CModel tmmsm12("TMMSM12");
	CModel ttmsm12("TTMSM12");
	CModel tmmsmkr14("TMMSMKR14");
	EIClass tmmsm21_back;
	CString dev_code="";
	CString heat_no = " ";
	CString sm_plan_no = " ";
	CString proc_no = " ";
	CString end_time_trp = " ";

	//加入函数的表

	tmmsm21_back.Tables.Add();
	if (!tmmsm21_back.Tables[0].Columns.Contains("PROC_DIV"))
	{
		tmmsm21_back.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
	}
	if (!tmmsm21_back.Tables[0].Columns.Contains("PRACT_COLL_MODE"))
	{
		tmmsm21_back.Tables[0].Columns.Add(DT_STRING, "PRACT_COLL_MODE");
	}
	if (!tmmsm21_back.Tables[0].Columns.Contains("FACTORY_DIV"))
	{
		tmmsm21_back.Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
	}
	//tmmsm21_back.Tables[0].Rows.Add();
	tmmsm21_back.Tables[0].Columns.Add(tmmsm21);
	//tmmsm21_back.Tables[0].Rows.Add();

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		tmmsm21.MergeFrom(bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]);
		///工号
		dev_code = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["AGGREGATE_NAME"].ToString().Trim();
		sm_plan_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["ORDER_NUMBER"].ToString().Trim();
		tmmsm21["ID_SJ"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["ID"].ToString().Trim();
		//同工位处理次数
		tmmsm21["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["TREATMENT_COUNTER"].ToString().Trim();
		Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no);
		//根据计划号获取熔炼号和制造命令号
		CString sm_plan_nol2_test = "0";
		cmd_inq_code.SetCommandText(" SELECT HEAT_NO, PONO, SM_PLAN_NO FROM "
			" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
			" UNION "
			" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm21["HEAT_NO"] = cmd_inq_code.GetString(1);
			tmmsm21["PONO"] = cmd_inq_code.GetString(2);
			tmmsm21["SM_PLAN_NO"] = cmd_inq_code.GetString(3);
		}
		Log::Info("", __FUNCTION__, "HEAT_NO-__=[{0}]", tmmsm21["HEAT_NO"].ToString());
		cmd_inq_code.Close();
		cmd_inq_test.Close();
		if (tmmsm21["HEAT_NO"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT T1.PROC_NO,T2.STATION_ID,T2.STATION_NO FROM (  "
				" SELECT PROC_NO, DEV_CODE, DECODE(PRE_SOLUTION_FLAG, '1', '0', '1') C_DIV FROM TPSSM12 WHERE HEAT_NO = '" + tmmsm21["HEAT_NO"].ToString() + "' AND DEV_CODE = '" + dev_code + "'   ) T1 LEFT JOIN TPSSMD1 T2  "
				" on t1.DEV_CODE = t2.DEV_CODE AND T1.C_DIV = T2.C_DIV");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm21["PROC_NO"] = cmd_inq.GetString(1);
				tmmsm21["STATION_ID"] = cmd_inq.GetString(2);
				tmmsm21["STATION_NO"] = cmd_inq.GetString(3);
			}
		}
		else{
			cmd_inq.SetCommandText(" SELECT STATION_ID,STATION_NO FROM TPSSMD1 WHERE DEV_CODE='" + dev_code + "' ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm21["STATION_ID"] = cmd_inq.GetString(1);
				tmmsm21["STATION_NO"] = cmd_inq.GetString(2);
			}
		}
		Log::Info("", __FUNCTION__, "PROC_NO-__=[{0}]", tmmsm21["PROC_NO"].ToString());
		cmd_inq.Close();
		tmmsm21["DEV_CODE"] = dev_code;
		if (tmmsm21["PROC_NO"].ToString() != ""&&tmmsm21["PROC_NO"].ToString() != " "){
		}
		else{
			//处理号
			tmmsm21["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HEAT_NUMBER"].ToString().Trim();
		}

		//二级炉号作为处理号存储到L2_PROC_NO字段中
		tmmsm21["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HEAT_NUMBER"].ToString().Trim();

		//L2计划号
		tmmsm21["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["ORDER_NUMBER"].ToString().Trim();
		//分包号SPLIT_INDICATION
		//班组
		tmmsm21["SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["SHIFT_TEAM"].ToString().Trim();
		tmmsm21["PROD_SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["SHIFT_TEAM"].ToString().Trim();
		Log::Info("", __FUNCTION__, "__like-__=[{0}]", __LINE__);
		//操纵工
		tmmsm21["ASSISTANT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["OPERATOR"].ToString().Trim();
		//空罐重量 EMPTY_LADLE_WEIGHT
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() != 0){
			tmmsm21["EMPTY_LADLE_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal()/1000;
		}
		//实际内部钢种
		tmmsm21["ST_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["GRADE_ACT"].ToString().Trim();
		//如果钢种是dep或者是1打头的则是预熔液
		if (tmmsm21["ST_NO"].ToString() != "DeP"&&tmmsm21["ST_NO"].ToString().Substring(0, 1) != "1"){
			tmmsm21["HEAT_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HEAT_NUMBER"].ToString().Trim();
		}
		//实际钢水重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["WEIGHT_ACT"].ToDecimal() != 0){
			tmmsm21["ACTRESULT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["WEIGHT_ACT"].ToDecimal()/1000;
		}
		//铁水重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HM_WEIGHT"].ToDecimal() != 0){
			tmmsm21["MOLTIRON_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HM_WEIGHT"].ToDecimal()/1000;
		}
		Log::Info("", __FUNCTION__, "__like-__=[{0}]", __LINE__);
		//废料重量 SCRAP_WEIGHT
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["SCRAP_WEIGHT"].ToDecimal() != 0){
			tmmsm21["SCRAP_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["SCRAP_WEIGHT"].ToDecimal()/1000;
		}
		//毛重
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["PIG_WEIGHT"].ToDecimal() != 0){
			tmmsm21["GROSS_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["PIG_WEIGHT"].ToDecimal()/1000;
		}
		//转炉开始时间
		tmmsm21["START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HEAT_START"].ToString().Trim();
		//转炉结束时间
		tmmsm21["END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HEAT_END"].ToString().Trim();
		//吹氧次数 BLOW_NUMBER
		//吹炼时间
		tmmsm21["BLOW_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["BLOW_TIME"].ToDecimal()/60;
		tmmsm21["BLOW_DURATION"] = tmmsm21["BLOW_DURATION"].ToDecimal().Round(2);
		//总吹氧量
		tmmsm21["OXYGEN_FINAL"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["OXYGEN_TOT"].ToString().Trim();
		//溅渣次数 SLAG_SPLASHING_NUMBER
		//顶吹氮量 
		tmmsm21["TOTAL_N2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["NITROGEN_TOT"].ToString().Trim();
		//BLOW_START 最先开始吹氧开始时间
		//BLOW_END 最后吹氧结束时间
		//溅渣开始时间
		tmmsm21["SLAG_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["SLAG_SPLASHING_START"].ToString().Trim();
		//溅渣结束时间
		tmmsm21["SLAG_END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["SLAG_SPLASHING_END"].ToString().Trim();
		//底吹氩量
		tmmsm21["AR_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["ARGON_TUY_TOT"].ToString().Trim();
		//底吹氮量
		tmmsm21["N_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["NITROGEN_TUY_TOT"].ToString().Trim();
		//出钢开始时间
		tmmsm21["TAP_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["TAPPING_START"].ToString().Trim();
		//出钢结束时间
		tmmsm21["TAP_END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["TAPPING_END"].ToString().Trim();
		//除渣开始时间
		tmmsm21["DROSSING_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["SLAGGING_START"].ToString().Trim();
		//除渣结束时间
		tmmsm21["DROSSING_END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["SLAGGING_END"].ToString().Trim();
		//铁水C元素
		tmmsm21["IRON_C"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HM_C"].ToString().Trim();
		//铁水SI元素
		tmmsm21["IRON_SI"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HM_SI"].ToString().Trim();
		//铁水MN元素
		tmmsm21["IRON_MN"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HM_MN"].ToString().Trim();
		//铁水P元素
		tmmsm21["IRON_P"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HM_P"].ToString().Trim();
		//铁水S元素
		tmmsm21["IRON_S"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HM_S"].ToString().Trim();
		//温度
		tmmsm21["IRON_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HM_TEMP"].ToString().Trim();
		//氧枪号
		tmmsm21["O2_LANCE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["OXYGEN_LANCE_NUMBER"].ToString().Trim();
		Log::Info("", __FUNCTION__, "__like-__=[{0}]", __LINE__);
		//铁水处理号
		tmmsm21["IRON_LTREAT_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["HM_TREATMENT_NO"].ToString().Trim();
		//脱硫处理号 DES_TREATMENT_NO
		if (tmmsm21["DES_TREATMENT_NO"].ToString() != " "){
			cmd_kr.SetCommandText(" select HEAT_NO,DES_ID from TMMSMKR14 WHERE DES_ID='" + tmmsm21["DES_TREATMENT_NO"].ToString() + "'  ");
			cmd_kr.ExecuteReader();
			if (cmd_kr.Read())
			{
				tmmsmkr14["HEAT_NO"] = cmd_kr.GetString(1);
				tmmsmkr14["DES_ID"] = cmd_kr.GetString(2);
			}
			if (tmmsmkr14["HEAT_NO"].ToString() == " "){
				tmmsmkr14["HEAT_NO"] = tmmsm21["HEAT_NO"].ToString();
			}
			tmmsmkr14.Update("HEAT_NO","DES_ID");
			Log::Info("", __FUNCTION__, "tmmsmkr14__HEAT_NO=[{0}]", tmmsmkr14["HEAT_NO"].ToString());
			cmd_kr.Close();
		}
		//倾倒温度
		tmmsm21["OUT_STEEL_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["TAP_TEMP"].ToString().Trim();
		//卷板、中板类型 INT_MES_PROD_SUMMARY_BOF.CAST_PURPOSE
		tmmsm21["IRON_LADLE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["CAST_PURPOSE"].ToString().Trim();
		//预溶液熔炼号1 HEATNO_PREMELT1
		//预溶液重量1 WEIGHT_PREMELT1
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["WEIGHT_PREMELT1"].ToDecimal() != 0){
			tmmsm21["WEIGHT_PREMELT1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["WEIGHT_PREMELT1"].ToDecimal()/1000;
		}
		//预溶液熔炼号2 HEATNO_PREMELT2
		//预溶液重量2 WEIGHT_PREMELT2
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["WEIGHT_PREMELT2"].ToDecimal() != 0){
			tmmsm21["WEIGHT_PREMELT2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["WEIGHT_PREMELT2"].ToDecimal()/1000;
		}
		//预溶液熔炼号3 HEATNO_PREMELT3
		//预溶液重量3 WEIGHT_PREMELT3
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["WEIGHT_PREMELT3"].ToDecimal() != 0){
			tmmsm21["WEIGHT_PREMELT3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["WEIGHT_PREMELT3"].ToDecimal()/1000 ;
		}
		//BLIR持续时间 BLIR_DURATION
		tmmsm21["BLIR_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["BLIR_DURATION"].ToDecimal() / 60;
		tmmsm21["BLIR_DURATION"] = tmmsm21["BLIR_DURATION"].ToDecimal().Round(2);
		Log::Info("", __FUNCTION__, "__like-__=[{0}]", __LINE__);
		//补吹持续时间
		tmmsm21["REBLOW_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["REBL_DURATION"].ToDecimal() / 60;
		tmmsm21["REBLOW_DURATION"] = tmmsm21["REBLOW_DURATION"].ToDecimal().Round(2);
		//炉代 BOF_LIFE
		tmmsm21["FURNACE_AGE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["BOF_LIFE"].ToString().Trim();
		//钢包号 
		tmmsm21["LADLE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["STEEL_LADLE_NO"].ToString().Trim();
		//钢包包龄
		tmmsm21["LADLE_AGE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["LADLE_LIFE"].ToString().Trim();
		Log::Info("", __FUNCTION__, "__like-__=[{0}]", __LINE__);
		//空铁水包重量  EMPTY_LADLE_WT
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["EMPTY_LADLE_WT"].ToDecimal() != 0){
			tmmsm21["EMPTY_LADLE_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["EMPTY_LADLE_WT"].ToDecimal() / 1000;
		}
		//顶部氮气消耗 TOP_N2_CONS
		tmmsm21["TOP_N2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["TOP_N2_CONS"].ToString().Trim();
		//补吹次数 REBLOW_COUNT
		tmmsm21["REBLOW_COUNT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["REBLOW_COUNT"].ToString().Trim();
		//钢包离开重量 LADLE_DEPART_WT
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["LADLE_DEPART_WT"].ToDecimal() != 0){
			tmmsm21["LADLE_DEPART_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["LADLE_DEPART_WT"].ToDecimal()/1000;
		}
		//下工序设备代码 NEXT_DEV_CODE
		tmmsm21["NEXT_DEV_CODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["NEXT_DEV_CODE"].ToString().Trim();
		//吹炼时间 MELT_DURATION
		tmmsm21["MELT_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["MELT_DURATION"].ToDecimal() / 60;
		tmmsm21["MELT_DURATION"] = tmmsm21["MELT_DURATION"].ToDecimal().Round(2);
		//辅助时长 ASSIS_DURATION
		tmmsm21["ASSIS_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["ASSIS_DURATION"].ToDecimal() / 60;
		tmmsm21["ASSIS_DURATION"] = tmmsm21["ASSIS_DURATION"].ToDecimal().Round(2);
		Log::Info("", __FUNCTION__, "__like-__=[{0}]", __LINE__);
		//冶炼时间 TAPTOTAP_DURATION
		tmmsm21["TAPTOTAP_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["TAPTOTAP_DURATION"].ToDecimal() / 60;
		tmmsm21["TAPTOTAP_DURATION"] = tmmsm21["TAPTOTAP_DURATION"].ToDecimal().Round(2);
		//总罐重量 GROSS_LADLE_WT
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["GROSS_LADLE_WT"].ToDecimal() != 0){
			tmmsm21["GROSS_LADLE_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_BOF"].Rows[0]["GROSS_LADLE_WT"].ToDecimal()/1000;
		}
		Log::Info("", __FUNCTION__, "__like-__=[{0}]", __LINE__);
		//冶炼模式 SMP_MODE
		Log::Info("", __FUNCTION__, "__like-__=[{0}]", __LINE__);
		tmmsm21["REC_CREATOR"] = s.userid;
		tmmsm21["REC_CREATE_TIME"] = datetime;
		Log::Trace("", "PONO", "冶炼周期上一炉号:PONO[{0}]", tmmsm21["PONO"].ToString());
		//计算冶炼时长
		//cmd_yl.SetCommandText(
		//	" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm21["SLAG_START_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
		//	" TO_DATE('" + tmmsm21["TAP_END_TIME"].ToString() + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
		//	" from dual ");
		//cmd_yl.ExecuteReader();
		//if (cmd_yl.Read())
		//{
		//	tmmsm21["MELT_DURATION"] = cmd_yl.GetDecimal(1);
		//}
		//cmd_yl.Close();
		////tmmsm21["MELT_DURATION"] = tmmsm21["SLAG_START_TIME"].ToDecimal() - tmmsm21["TAP_END_TIME"].ToDecimal();
		//Log::Trace("", "MELT_DURATION", "秒:MELT_DURATION[{0}]", tmmsm21["MELT_DURATION"].ToString());
		//if (tmmsm21["MELT_DURATION"].ToDecimal() != 0){
		//	tmmsm21["MELT_DURATION"] = tmmsm21["MELT_DURATION"].ToDecimal() / 60;
		//	tmmsm21["MELT_DURATION"] = tmmsm21["MELT_DURATION"].ToDecimal().Round(2);
		//	Log::Trace("", "MELT_DURATION", "分:MELT_DURATION[{0}]", tmmsm21["MELT_DURATION"].ToString());
		//}
		//计算冶炼周期
		proc_no = tmmsm21["PROC_NO"].ToString() - 1;
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
		if (end_time_trp != " "){
			cmd_yz.Close();
			cmd_yzsc.SetCommandText(
				" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm21["TAP_END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
				" TO_DATE('" + end_time_trp + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
				" from dual ");
			cmd_yzsc.ExecuteReader();
			if (cmd_yzsc.Read())
			{
				tmmsm21["SMELT_CYCLE"] = cmd_yzsc.GetDecimal(1);
			}
			cmd_yzsc.Close();
			if (tmmsm21["SMELT_CYCLE"].ToDecimal() != 0){
				tmmsm21["SMELT_CYCLE"] = tmmsm21["SMELT_CYCLE"].ToDecimal() / 60;
				tmmsm21["SMELT_CYCLE"] = tmmsm21["SMELT_CYCLE"].ToDecimal().Round(2);
				Log::Trace("", "SMELT_CYCLE", "冶炼周期:SMELT_CYCLE[{0}]", tmmsm21["SMELT_CYCLE"].ToString());
			}
		}
		//反写12表
		tmmsm12["TPD_NO"] = tmmsm21["IRON_LTREAT_NO"].ToString().Trim();
		if (tmmsm21["IRON_LTREAT_NO"].ToString() != " "&&tmmsm21["HEAT_NO"].ToString().Trim()!=""){
			if (tmmsm12.QueryCount("TPD_NO") > 0){
				tmmsm12["HEAT_NO"] = tmmsm21["HEAT_NO"].ToString().Trim();
				tmmsm12.Update("HEAT_NO","TPD_NO");
			}
		}
		f_tableObjectCheck9999(tmmsm21);
		tmmsm21.MergeTo(tmmsm21_back.Tables[0], false);
		tmmsm21_back.Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";
		/*if (tmmsm21["PROC_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "处理号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		Log::Info("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm21["PROC_NO"].ToString());
		if (tmmsm21.QueryCount("L2_PROC_NO"))
		{
			Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "U");
			tmmsm21_back.Tables[0].Rows[0]["PROC_DIV"] = "U";
			tmmsm21_back.Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";
			doFlag = f_mmsm21_proc(&tmmsm21_back, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//tmmsm21.Update("PROC_NO");
		}
		else
		{
			Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "I");
			tmmsm21_back.Tables[0].Rows[0]["PROC_DIV"] = "I";
			tmmsm21_back.Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";
			doFlag = f_mmsm21_proc(&tmmsm21_back, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/*tmmsm21.TrimOrBlank();
			tmmsm21.Insert();*/
		}
		ttmsm12["HEAT_NO"] = tmmsm21["HEAT_NO"];
		ttmsm12["LADLE_GROSS_WT"] = tmmsm21["LADLE_DEPART_WT"];
		ttmsm12["OUT_STEEL_TIME"] = tmmsm21["TAP_START_TIME"];
		ttmsm12["ST_NO"] = tmmsm21["ST_NO"];
		if (ttmsm12.QueryCount("HEAT_NO") > 0&& tmmsm21.QueryCount("HEAT_NO") > 0)//代表转炉生产的钢水，而不是预溶液
		{
			ttmsm12.Update("LADLE_GROSS_WT,OUT_STEEL_TIME,ST_NO", "HEAT_NO");
		}
		//sprintf(s.msg, "%d条记录新增成功！请重新查询！");
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


