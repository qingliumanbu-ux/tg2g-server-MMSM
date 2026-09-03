/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      电炉炉次报告
Version:     1.0
Date:        2023-10-25
Description: 实绩接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 电炉炉次报告接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明
int f_mmsm20_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测

BM2F_ENTERACE_TELE(cm_e2t8s4_rcv)

int f_cm_e2t8s4_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd_yz(conn);
	CDbCommand cmd_yzsc(conn);
	CDbCommand cmd_inq_test(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);
	CModel tmmsm20("TMMSM20");

	CString heat_no = " ";
	CString dev_code = " ";
	CString sm_plan_no2 = " ";
	CString proc_no = " ";
	CString end_time_trp = " ";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		tmmsm20.MergeFrom(bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]);
		///工号
		dev_code = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["AGGREGATE_NAME"].ToString();
		sm_plan_no2 = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["ORDER_NUMBER"].ToString();
		tmmsm20["ID_SJ"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["ID"].ToString();
		//同工位处理次数
		tmmsm20["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["TREATMENT_COUNTER"].ToString();
		Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no2);
		//根据计划号获取熔炼号和制造命令号
		cmd_inq_code.SetCommandText(" SELECT HEAT_NO, PONO, SM_PLAN_NO FROM "
			" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
			" UNION "
			" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no2 + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm20["HEAT_NO"] = cmd_inq_code.GetString(1);
			tmmsm20["PONO"] = cmd_inq_code.GetString(2);
			tmmsm20["SM_PLAN_NO"] = cmd_inq_code.GetString(3);
		}
		cmd_inq_code.Close();

		cmd_inq.SetCommandText(" SELECT STATION_ID,STATION_NO FROM TPSSMD1 WHERE DEV_CODE='" + dev_code + "' ");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			//tmmsm20["PROC_NO"] = cmd_inq.GetString(1);
			tmmsm20["STATION_ID"] = cmd_inq.GetString(1);
			tmmsm20["STATION_NO"] = cmd_inq.GetString(2);
		}
		cmd_inq.Close();
		tmmsm20["DEV_CODE"] = dev_code;
		
		tmmsm20["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEAT_NUMBER"].ToString();


		//二级炉号作为处理号存储到L2_PROC_NO字段中
		tmmsm20["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEAT_NUMBER"].ToString();

		//计划号
		tmmsm20["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["ORDER_NUMBER"].ToString();
		//分包号
		tmmsm20["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["SPLIT_INDICATION"].ToString();
		//空包重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() != 0){
			tmmsm20["EMPTY_LADLE_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal()/1000;
			Log::Trace("", "", "EMPTY_LADLE_WEIGHT = {0}", tmmsm20["EMPTY_LADLE_WEIGHT"].ToDecimal());
		}
		tmmsm20["PROD_SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["SHIFT_TEAM"].ToString();
		//操作工
		tmmsm20["ASSISTANT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["OPERATOR"].ToString();
		//出钢记号
		tmmsm20["ST_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["GRADE_ACT"].ToString();
		//实际重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["WEIGHT_ACT"].ToDecimal() != 0){
			tmmsm20["ACTRESULT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["WEIGHT_ACT"].ToDecimal()/1000 ;
		}
		//开始时间
		tmmsm20["START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEAT_START"].ToString();
		Log::Trace("", "", "START_TIME = {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEAT_START"].ToString());
		//结束时间
		tmmsm20["END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEAT_END"].ToString();
		Log::Trace("", "", "END_TIME = {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEAT_END"].ToString());
		//加热次数
		tmmsm20["HEAT_COUNT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEATING_NUMBER"].ToString();
		//加热时间
		tmmsm20["BIL_MELT_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEATING_TIME"].ToDecimal() / 60;
		tmmsm20["BIL_MELT_TIME"] = tmmsm20["BIL_MELT_TIME"].ToDecimal().Round(2);
		//耗电量
		tmmsm20["POWER_CONSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["POWER"].ToString();
		
		//最初加热开始时间
		tmmsm20["FIRST_HEATING_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["FIRST_HEATING_START"].ToString();
		//最后加热结束时间
		tmmsm20["LAST_HEATING_END"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["LAST_HEATING_END"].ToString();
		//转炉来钢水重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["BOF_WEIGHT"].ToDecimal() != 0){
			tmmsm20["BOF_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["BOF_WEIGHT"].ToDecimal() ;
		}
		//废钢重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["SCRAP_WEIGHT"].ToDecimal() != 0){
			tmmsm20["SCRAP_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["SCRAP_WEIGHT"].ToDecimal()/1000;
		}
		//碳粉重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["C_POWDER_WEIGHT"].ToDecimal() != 0){
			tmmsm20["C_POWDER_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["C_POWDER_WEIGHT"].ToDecimal()/1000;
		}
		//吹氧次数
		tmmsm20["OXYGEN_NUMBER"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["OXYGEN_NUMBER"].ToString();
		//吹氧时间
		tmmsm20["BLOW_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["BLOW_DURATION"].ToDecimal() / 60;
		tmmsm20["BLOW_DURATION"] = tmmsm20["BLOW_DURATION"].ToDecimal().Round(2);
		//吹氧量
		tmmsm20["OXYGEN_FINAL"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["OXYGEN_TOTAL"].ToString();
		
		//最先吹氧开始时间
		tmmsm20["FIRST_BLOW_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["FIRST_BLOW_START"].ToString();
		//最后吹氧结束时间
		tmmsm20["LAST_BLOW_END"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["LAST_BLOW_END"].ToString();
		//出钢开始时间
		tmmsm20["TAP_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["TAPPING_START"].ToString();
		//出钢结束时间
		tmmsm20["TAP_END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["TAPPING_END"].ToString();
		
		//顶吹氮气量
		tmmsm20["TOP_N2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["NITROGEN_TOP_AMOUNT"].ToString();
		
		Log::Trace("", "", "TOP_N2_CONS= {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["NITROGEN_TOP_AMOUNT"].ToString());
		//顶吹氮气时间
		tmmsm20["NITROGEN_TOP_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["NITROGEN_TOP_TIME"].ToDecimal() / 60;
		tmmsm20["NITROGEN_TOP_TIME"] = tmmsm20["NITROGEN_TOP_TIME"].ToDecimal().Round(2);
		//底吹氮气量
		tmmsm20["BTTM_N_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["NITROGEN_BOTTOM_AMOUNT"].ToString();
		Log::Trace("", "", "BTTM_N_COMSUME= {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["NITROGEN_BOTTOM_AMOUNT"].ToString());
		
		//底吹氮气时间
		tmmsm20["NITROGEN_BOTTOM_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["NITROGEN_BOTTOM_TIME"].ToDecimal() / 60;
		tmmsm20["NITROGEN_BOTTOM_TIME"] = tmmsm20["NITROGEN_BOTTOM_TIME"].ToDecimal().Round(2);
		//预溶液熔炼号1
		tmmsm20["HEATNO_PREMELT1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEATNO_PREMELT1"].ToString();
		//预溶液重量1
		Log::Trace("", "", "WEIGHT_PREMELT1= {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["WEIGHT_PREMELT1"].ToDecimal());
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["WEIGHT_PREMELT1"].ToDecimal() != 0){
			tmmsm20["WEIGHT_PREMELT1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["WEIGHT_PREMELT1"].ToDecimal()/1000;
			Log::Trace("", "", "WEIGHT_PREMELT1_1= {0}", tmmsm20["WEIGHT_PREMELT1"].ToDecimal());
		}
		//预溶液熔炼号2
		tmmsm20["HEATNO_PREMELT2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEATNO_PREMELT2"].ToString();
		//预溶液重量2
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["WEIGHT_PREMELT2"].ToDecimal() != 0){
			tmmsm20["WEIGHT_PREMELT2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["WEIGHT_PREMELT2"].ToDecimal()/1000;
		}
		//预溶液熔炼号3
		tmmsm20["HEATNO_PREMELT3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEATNO_PREMELT3"].ToString();
		//预溶液重量3
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["WEIGHT_PREMELT3"].ToDecimal() != 0){
			tmmsm20["WEIGHT_PREMELT3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["WEIGHT_PREMELT3"].ToDecimal()/1000;
		}
		//电炉炉壳编号
		tmmsm20["EAF_SHELL_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["EAF_SHELL_NO"].ToString();
		
		//电炉炉壳寿命
		tmmsm20["EAF_SHELL_LIFE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["EAF_SHELL_LIFE"].ToString();
		//装料1开始时刻
		tmmsm20["CHARGE_START_TIME1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["CHARGE_START_TIME1"].ToString();
		//装料1用时
		tmmsm20["CHARGE_DURATION1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["CHARGE_DURATION1"].ToDecimal() / 60;
		tmmsm20["CHARGE_DURATION1"] = tmmsm20["CHARGE_DURATION1"].ToDecimal().Round(2);
		//送电开始时刻1
		tmmsm20["POWER_ON_TIME1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["POWER_ON_TIME1"].ToString();
		//送电时长1
		tmmsm20["POWER_ON_DURATION1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["POWER_ON_DURATION1"].ToString();
		//装料2开始时刻
		tmmsm20["CHARGE_START_TIME2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["CHARGE_START_TIME2"].ToString();
		//装料2用时
		tmmsm20["CHARGE_DURATION2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["CHARGE_DURATION2"].ToDecimal() / 60;
		tmmsm20["CHARGE_DURATION2"] = tmmsm20["CHARGE_DURATION2"].ToDecimal().Round(2);
		//送电开始时刻2
		tmmsm20["POWER_ON_TIME2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["POWER_ON_TIME2"].ToString();
		//送电时长2
		tmmsm20["POWER_ON_DURATION2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["POWER_ON_DURATION2"].ToString();
		//装料3开始时刻
		tmmsm20["CHARGE_START_TIME3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["CHARGE_START_TIME3"].ToString();
		//装料3用时
		tmmsm20["CHARGE_DURATION3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["CHARGE_DURATION3"].ToDecimal() / 60;
		tmmsm20["CHARGE_DURATION3"] = tmmsm20["CHARGE_DURATION3"].ToDecimal().Round(2);
		//送电开始时刻3 ddd
		tmmsm20["POWER_ON_TIME3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["POWER_ON_TIME3"].ToString();
		//送电时长3
		tmmsm20["POWER_ON_DURATION3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["POWER_ON_DURATION3"].ToString();
		//还原开始时间
		tmmsm20["REDUCING_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["REDUCING_START_TIME"].ToString();
		//还原周期
		tmmsm20["REDUCING_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["REDUCING_DURATION"].ToDecimal() / 60;
		tmmsm20["REDUCING_DURATION"] = tmmsm20["REDUCING_DURATION"].ToDecimal().Round(2);
		//出钢持续时间
		tmmsm20["TAP_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["TAPPING_DURATION1"].ToDecimal() / 60;
		tmmsm20["TAP_DURATION"] = tmmsm20["TAP_DURATION"].ToDecimal().Round(2);
		
		//出钢温度1
		tmmsm20["OUT_STEEL_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["TAPPING_TEMP1"].ToString();
		Log::Trace("", "", "OUT_STEEL_TEMP={0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["TAPPING_TEMP1"].ToString());
		//钢包号
		tmmsm20["LADLE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["LADLE_NO1"].ToString();
		Log::Trace("", "", "OUT_STEEL_TEMP={0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["LADLE_NO1"].ToString());
		//钢包寿命
		tmmsm20["LADLE_AGE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["LADLE_LIFE1"].ToString();
		Log::Trace("", "", "WSL6");
		//钢包状态
		tmmsm20["LADLE_STATUS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["LADLE_STATUS1"].ToString();
		Log::Trace("", "", "WSL1");
		//冶炼模式
		tmmsm20["SMELT_MODE1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["SMELT_MODE"].ToString();
		Log::Trace("", "", "WSL");
		//总送电次数
		tmmsm20["POWER_ON_TIMES"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["POWER_ON_TIMES"].ToString();
		//炉门氧量
		tmmsm20["EAF_DOOR_O2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["EAF_DOOR_O2_CONS"].ToString();
		//炉壁集速氧量
		tmmsm20["EAF_WALL_O2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["EAF_WALL_O2_CONS"].ToString();
		//还原气体类型
		tmmsm20["REDUCING_GAS_TYPE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["REDUCING_GAS_TYPE"].ToString();
		//炉壁氧枪使用模式
		tmmsm20["LW_O2_USE_MODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["LW_O2_USE_MODE"].ToString();
		//冶炼时长
		tmmsm20["MELT_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["MELT_DURATION"].ToDecimal() / 60;
		tmmsm20["MELT_DURATION"] = tmmsm20["MELT_DURATION"].ToDecimal().Round(2);
		//出钢周期
		tmmsm20["TAPTOTAP_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["TAPTOTAP_DURATION"].ToDecimal() / 60;
		tmmsm20["TAPTOTAP_DURATION"] = tmmsm20["TAPTOTAP_DURATION"].ToDecimal().Round(2);
		//下工序设备代码
		tmmsm20["NEXT_DEV_CODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["NEXT_DEV_CODE"].ToString();
		//工艺路线
		tmmsm20["PROD_ROUTE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["PROD_ROUTE"].ToString();
		//EV工艺炉号
		tmmsm20["HEATNO_AOD3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_EAF"].Rows[0]["HEATNO_AOD3"].ToString();


		tmmsm20["REC_CREATOR"] = s.userid;
		tmmsm20["REC_CREATE_TIME"] = datetime;

		//计算冶炼周期
		proc_no = tmmsm20["PROC_NO"].ToString() - 1;
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
		if (end_time_trp != ' '){
			cmd_yz.Close();
			cmd_yzsc.SetCommandText(
				" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm20["TAP_END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
				" TO_DATE('" + end_time_trp + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
				" from dual ");
			cmd_yzsc.ExecuteReader();
			if (cmd_yzsc.Read())
			{
				tmmsm20["SMELT_CYCLE"] = cmd_yzsc.GetDecimal(1);
			}
			cmd_yzsc.Close();
			if (tmmsm20["SMELT_CYCLE"].ToDecimal() != 0){
				tmmsm20["SMELT_CYCLE"] = tmmsm20["SMELT_CYCLE"].ToDecimal() / 60;
				tmmsm20["SMELT_CYCLE"] = tmmsm20["SMELT_CYCLE"].ToDecimal().Round(2);
				Log::Trace("", "SMELT_CYCLE", "冶炼周期:SMELT_CYCLE[{0}]", tmmsm20["SMELT_CYCLE"].ToString());
			}
		}

		bcls_rec->Tables.Clear();
		tmmsm20.MergeTo(bcls_rec->Tables.Add());

		f_tableObjectCheck9999(tmmsm20);

		if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		}
		if (!bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
		}

		bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";//厂别

		if (tmmsm20.QueryCount("L2_PROC_NO,SM_PLAN_NOL2"))
		{
			Log::Trace("", "", "123456");
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "U";//标记为修改
			doFlag = f_mmsm20_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{
			Log::Trace("", "", "123");
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "I";//标记为新增
			doFlag = f_mmsm20_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
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


