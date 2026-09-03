/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      LF炉次报告
Version:     1.0
Date:        2023-10-26
Description: 实绩接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// LF炉次报告接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



//外部函数声明
int f_mmsm24_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm2a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测
int f_qmts_xxyq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//消息引擎判定结果
int f_qmts_call_judge(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//消息规则引擎


BM2F_ENTERACE_TELE(cm_e2t8s6_rcv)

int f_cm_e2t8s6_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd_yz(conn);
	CDbCommand cmd_yl(conn);
	CDbCommand cmd_yzsc(conn);
	CDbCommand cmd_inq_code(conn);
	CModel tmmsm24("TMMSM24");
	CModel tmmsm2a("TMMSM2A");
	EIClass tmmsm2a_back;
	EIClass tmmsm24_back;
	CString heat_no = " ";
	CString dev_code = "";
	CString st_no = "";
	CString sm_plan_no = " ";
	CString proc_no = " ";
	CString end_time_trp = " ";
	CDbCommand cmd_inq_test(conn);
	//加入函数的表

	blkNum = tmmsm2a_back.Tables.IndexOf("MMSM2A");
	tmmsm2a_back.Tables.Add("MMSM2A");
	tmmsm2a_back.Tables["MMSM2A"].Columns.Add(tmmsm2a);
	if (!tmmsm2a_back.Tables["MMSM2A"].Columns.Contains("PROC_DIV"))
	{
		Log::Trace("", "", "PROC_DIV");
		tmmsm2a_back.Tables["MMSM2A"].Columns.Add(DT_STRING, "PROC_DIV");
	}
	if (!tmmsm2a_back.Tables["MMSM2A"].Columns.Contains("ACJC_RELATION_ID"))
	{
		tmmsm2a_back.Tables["MMSM2A"].Columns.Add(DT_STRING, "ACJC_RELATION_ID");
	}
	if (!tmmsm2a_back.Tables["MMSM2A"].Columns.Contains("NEW_WT"))
	{
		tmmsm2a_back.Tables["MMSM2A"].Columns.Add(DT_STRING, "NEW_WT");
	}
	if (!tmmsm2a_back.Tables["MMSM2A"].Columns.Contains("NEW_HANDWORK_MARK"))
	{
		tmmsm2a_back.Tables["MMSM2A"].Columns.Add(DT_STRING, "NEW_HANDWORK_MARK");
	}
	if (!tmmsm2a_back.Tables["MMSM2A"].Columns.Contains("NEW_COLL_MODE"))
	{
		tmmsm2a_back.Tables["MMSM2A"].Columns.Add(DT_STRING, "NEW_COLL_MODE");
	}

	tmmsm24_back.Tables.Add();
	if (!tmmsm24_back.Tables[0].Columns.Contains("PROC_DIV"))
	{
		tmmsm24_back.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
	}
	if (!tmmsm24_back.Tables[0].Columns.Contains("FACTORY_DIV"))
	{
		tmmsm24_back.Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
	}

	tmmsm24_back.Tables[0].Columns.Add(tmmsm24);


	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		tmmsm24.MergeFrom(bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]);
		///工号
		dev_code = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["AGGREGATE_NAME"].ToString();
		st_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["GRADE_ACT"].ToString();
		sm_plan_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["ORDER_NUMBER"].ToString();
		heat_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["HEAT_NUMBER"].ToString();
		//熔炼号
		tmmsm24["HEAT_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["HEAT_NUMBER"].ToString();
		tmmsm24["ID_SJ"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["ID"].ToString();
		//同工位处理次数
		tmmsm24["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["TREATMENT_COUNTER"].ToString();

		Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no);
		//根据计划号获取熔炼号和制造命令号
		cmd_inq_code.SetCommandText(" SELECT HEAT_NO, PONO, SM_PLAN_NO FROM "
			" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
			" UNION "
			" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm24["PONO"] = cmd_inq_code.GetString(2);
			tmmsm24["SM_PLAN_NO"] = cmd_inq_code.GetString(3);
		}
		cmd_inq_code.Close();

		if (tmmsm24["HEAT_NO"].ToString() != " "){
			cmd_inq.SetCommandText(" SELECT T1.PROC_NO,T2.STATION_ID,T2.STATION_NO FROM (  "
				" SELECT PROC_NO, DEV_CODE, DECODE(PRE_SOLUTION_FLAG, '1', '0', '1') C_DIV FROM TPSSM12 WHERE HEAT_NO = '" + tmmsm24["HEAT_NO"].ToString() + "' AND DEV_CODE = '" + dev_code + "' and TREATMENT_COUNTER ='" + tmmsm24["SAME_PROC_NUM"].ToString() + "' ) T1 LEFT JOIN TPSSMD1 T2  "
				" on t1.DEV_CODE = t2.DEV_CODE AND T1.C_DIV = T2.C_DIV");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm24["PROC_NO"] = cmd_inq.GetString(1);
				tmmsm24["STATION_ID"] = cmd_inq.GetString(2);
				tmmsm24["STATION_NO"] = cmd_inq.GetString(3);
			}
		}
		else{
			cmd_inq.SetCommandText(" SELECT STATION_ID,STATION_NO FROM TPSSMD1 WHERE DEV_CODE='" + dev_code + "' ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm24["STATION_ID"] = cmd_inq.GetString(1);
				tmmsm24["STATION_NO"] = cmd_inq.GetString(2);
			}
		}
		Log::Trace("", "PROC_NO", "PROC_NO = {0}", tmmsm24["PROC_NO"].ToString());
		cmd_inq.Close();

		if (tmmsm24["PROC_NO"].ToString() != " "&&tmmsm24["PROC_NO"].ToString() != ""){

		}
		else{
			tmmsm24["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["HEAT_NUMBER"].ToString();
		}
		tmmsm24["DEV_CODE"] = dev_code;
		//计划号 INT_MES_PROD_SUMMARY_LF.ORDER_NUMBER
		tmmsm24["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["ORDER_NUMBER"].ToString();
		tmmsm24["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["HEAT_NUMBER"].ToString();
		//分包号
		tmmsm24["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SPLIT_INDICATION"].ToString();
		//铁水重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["STEEL_WEIGHT"].ToDecimal() != 0){
			tmmsm24["MOLTIRON_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["STEEL_WEIGHT"].ToDecimal()/1000;
		}

		//空罐重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() != 0){
			tmmsm24["EMPTY_LADLE_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() / 1000;
		}
		//班组
		tmmsm24["PROD_SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SHIFT_TEAM"].ToString();
		//操作工
		tmmsm24["ASSISTANT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["OPERATOR"].ToString();
		//实际出钢记号
		tmmsm24["ST_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["GRADE_ACT"].ToString();


		//开始时间
		tmmsm24["START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["HEAT_START"].ToString();
		//结束时间
		tmmsm24["END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["HEAT_END"].ToString();


		//加热次数
		tmmsm24["HEAT_COUNT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["HEATING_NUMBER"].ToString();
		//加热时间
		tmmsm24["BIL_MELT_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["HEAT_TIME"].ToString();
		//电耗
		tmmsm24["POWER_CONSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWER"].ToString();

		//最初加热开始时间
		tmmsm24["FIRST_HEATING_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["FIRST_HEATING_START"].ToString();
		//最后加热结束时间
		tmmsm24["LAST_HEATING_END"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LAST_HEATING_END"].ToString();
		//氩气合计
		tmmsm24["AR_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["ARGON_TOT"].ToString();
		//吹氩次数
		tmmsm24["ARGON_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["ARGON_TIME"].ToString();
		//最初通电开始时间
		tmmsm24["FIRST_WIRE_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["FIRST_WIRE_START"].ToString();
		//最后通电开始时间
		tmmsm24["LAST_WIRE_END"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LAST_WIRE_END"].ToString();

		//渣厚
		tmmsm24["SLAG_HEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SLAG_HEIGHT"].ToString();
		//空间
		tmmsm24["FREEBOARD"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["FREEBOARD"].ToString();
		//软搅持续时间
		tmmsm24["SOFT_WHISK_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SOFT_STIRRING_DUR"].ToString();
		//开始温度
		tmmsm24["START_STEEL_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["TEMP_START"].ToString();
		//结束温度
		tmmsm24["END_STEEL_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["TEMP_END"].ToString();
		//钢包号
		tmmsm24["LADLE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["STEEL_LADLE_NO"].ToString();
		//钢包龄
		tmmsm24["LADLE_AGE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LADLE_LIFE"].ToString();

		//钢包到达重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LADLE_ARRIVE_WT"].ToDecimal() != 0){
			tmmsm24["LADLE_ARRIVE_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LADLE_ARRIVE_WT"].ToDecimal() / 1000;
		}
		
		//钢包空罐重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["EMPTY_LADLE_WT"].ToDecimal() != 0){
			tmmsm24["EMPTY_LADLE_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["EMPTY_LADLE_WT"].ToDecimal() / 1000;
		}
		//钢包到达铁水重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LADLE_ARRIVE_STEEL_WT"].ToDecimal() != 0){
			tmmsm24["LADLE_ARRIVE_STEEL_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LADLE_ARRIVE_STEEL_WT"].ToDecimal() / 1000;
		}
		//炉次开始渣厚
		tmmsm24["SLAG_THICK_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SLAG_THICK_START"].ToString();
		//炉渣开始重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SLAG_WT_START"].ToDecimal() != 0){
			tmmsm24["SLAG_WT_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SLAG_WT_START"].ToDecimal()/1000;
		}
		//钢包到达温度
		tmmsm24["LADLE_ARRIVE_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LADLE_ARRIVE_TEMP"].ToString();
		//通电开始时间1
		tmmsm24["ELEC_START_TIME1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWER_ON_TIME1"].ToString();
		//通电持续时间1
		tmmsm24["POWER_ON_DURATION1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWER_ON_DURATION1"].ToDecimal()/60;
		tmmsm24["POWER_ON_DURATION1"] = tmmsm24["POWER_ON_DURATION1"].ToDecimal().Round(2);
		//耗电量1
		tmmsm24["ELEC_CONSUMPTION_1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["ELEC_CONSUMPTION_1"].ToString();
		//通电结束时间1
		tmmsm24["ELEC_END_TIME1"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWEROFF_TIME1"].ToString();
		//通电开始时间2
		tmmsm24["ELEC_START_TIME2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWER_ON_TIME2"].ToString();
		//通电持续时间2
		tmmsm24["POWER_ON_DURATION2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWER_ON_DURATION2"].ToDecimal()/60;
		tmmsm24["POWER_ON_DURATION2"] = tmmsm24["POWER_ON_DURATION2"].ToDecimal().Round(2);
		//耗电量2
		tmmsm24["ELEC_CONSUMPTION_2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["ELEC_CONSUMPTION_2"].ToString();
		//通电结束时间2
		tmmsm24["ELEC_END_TIME2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWEROFF_TIME2"].ToString();
		//通电开始时间3
		tmmsm24["ELEC_START_TIME3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWER_ON_TIME3"].ToString();
		//通电持续时间3
		tmmsm24["POWER_ON_DURATION3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWER_ON_DURATION3"].ToDecimal()/60;
		tmmsm24["POWER_ON_DURATION3"] = tmmsm24["POWER_ON_DURATION3"].ToDecimal().Round(2);
		//耗电量3
		tmmsm24["ELEC_CONSUMPTION_3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["ELEC_CONSUMPTION_3"].ToString();
		//通电结束时间3
		tmmsm24["ELEC_END_TIME3"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["POWEROFF_TIME3"].ToString();
		//炉渣结束重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SLAG_WT_END"].ToDecimal() != 0){
			tmmsm24["SLAG_WT_END"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SLAG_WT_END"].ToDecimal() / 1000;
		}
		//钢包离开重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LADLE_DEPART_WT"].ToDecimal() != 0){
			tmmsm24["LADLE_DEPART_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["LADLE_DEPART_WT"].ToDecimal() / 1000;
		}
		//下工序设备代码
		tmmsm24["NEXT_DEV_CODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["NEXT_DEV_CODE"].ToString();

		tmmsm24["STRONG_STIR_DUR"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["STRONG_STIR_DUR"].ToString();
		tmmsm24["MIDSTRONG_STIR_DUR"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["MIDSTRONG_STIR_DUR"].ToString();
		//MIDDLE_STIR_FLOW
		tmmsm24["MIDDLE_STIR_FLOW"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["MIDDLE_STIR_FLOW"].ToString();
		tmmsm24["MIDDLE_STIR_DUR"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["MIDDLE_STIR_DUR"].ToString();
		tmmsm24["STRONG_STIR_FLOW"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["STRONG_STIR_FLOW"].ToString();
		tmmsm24["MIDSTRONG_STIR_FLOW"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["MIDSTRONG_STIR_FLOW"].ToString();
		tmmsm24["SOFT_STIR_FLOW"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["SOFT_STIR_FLOW"].ToString();
		tmmsm24["BAS_ONLINE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF"].Rows[0]["BAS_ONLINE"].ToString();

		tmmsm24["REC_CREATOR"] = s.userid;
		tmmsm24["REC_CREATE_TIME"] = datetime;

		//计算冶炼时长
		cmd_yl.SetCommandText(
			" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm24["END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
			" TO_DATE('" + tmmsm24["START_TIME"].ToString() + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
			" from dual ");
		cmd_yl.ExecuteReader();
		if (cmd_yl.Read())
		{
			tmmsm24["MELT_DURATION"] = cmd_yl.GetDecimal(1);
		}
		cmd_yl.Close();
		//tmmsm21["MELT_DURATION"] = tmmsm21["END_TIME"].ToDecimal() - tmmsm21["START_TIME"].ToDecimal()/1000;
		Log::Trace("", "MELT_DURATION", "秒:MELT_DURATION[{0}]", tmmsm24["MELT_DURATION"].ToString());
		if (tmmsm24["MELT_DURATION"].ToDecimal() != 0){
			tmmsm24["MELT_DURATION"] = tmmsm24["MELT_DURATION"].ToDecimal() / 60;
			tmmsm24["MELT_DURATION"] = tmmsm24["MELT_DURATION"].ToDecimal().Round(2);
			Log::Trace("", "MELT_DURATION", "分:MELT_DURATION[{0}]", tmmsm24["MELT_DURATION"].ToString());
		}

		//计算冶炼周期
		proc_no = tmmsm24["PROC_NO"].ToString() - 1;
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
		Log::Trace("", "end_time_trp", "冶炼周期上一炉结束时间:end_time_trp[{0}]", end_time_trp);
		if (end_time_trp != " "){
			cmd_yz.Close();
			cmd_yzsc.SetCommandText(
				" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm24["END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
				" TO_DATE('" + end_time_trp + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
				" from dual ");
			cmd_yzsc.ExecuteReader();
			if (cmd_yzsc.Read())
			{
				tmmsm24["SMELT_CYCLE"] = cmd_yzsc.GetDecimal(1);
			}
			cmd_yzsc.Close();
			if (tmmsm24["SMELT_CYCLE"].ToDecimal() != 0){
				tmmsm24["SMELT_CYCLE"] = tmmsm24["SMELT_CYCLE"].ToDecimal() / 60;
				tmmsm24["SMELT_CYCLE"] = tmmsm24["SMELT_CYCLE"].ToDecimal().Round(2);
				Log::Trace("", "SMELT_CYCLE", "冶炼周期:SMELT_CYCLE[{0}]", tmmsm24["SMELT_CYCLE"].ToString());
			}
		}

		//bcls_rec->Tables.Clear();
		tmmsm24.MergeTo(bcls_rec->Tables.Add());

		/*if (tmmsm24["PROC_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "LF处理号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		f_tableObjectCheck9999(tmmsm24);
		Log::Info("", __FUNCTION__, "like=[{0}]", __LINE__);
		for (int i = 0; i < bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF_DET"].Rows.get_Count(); i++)
		{
			Log::Info("", __FUNCTION__, "like=[{0}]", __LINE__);
			tmmsm2a.CopyFrom(tmmsm24);
			//物料代码
			tmmsm2a["MAT_CODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF_DET"].Rows[i]["MATERIAL_CODE"].ToString();
			//批次号
			tmmsm2a["LOT_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF_DET"].Rows[i]["BATCH_NUMBER"].ToString();
			//长度
			tmmsm2a["LEN"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LF_DET"].Rows[i]["LENGTH"].ToString();
			tmmsm2a["PROC_COUNT"] = i;
			tmmsm2a["REC_CREATOR"] = s.userid;
			tmmsm2a["REC_CREATE_TIME"] = datetime;
			tmmsm2a["DEV_CODE"] = dev_code;
			tmmsm2a["HEAT_NO"] = tmmsm24["HEAT_NO"].ToString();
			tmmsm2a["PROC_NO"] = tmmsm24["PROC_NO"].ToString();
			tmmsm2a["ST_NO"] = tmmsm24["ST_NO"].ToString();
			tmmsm2a.MergeTo(tmmsm2a_back.Tables["MMSM2A"], false);
			if (tmmsm2a.QueryCount("PROC_NO,PROC_COUNT"))
			{
				Log::Trace("", "", "U");
				tmmsm2a_back.Tables["MMSM2A"].Rows[i]["PROC_DIV"] = "U";//标记为修改
				doFlag = f_mmsm2a_proc(&tmmsm2a_back, bcls_ret, conn);
				if (doFlag < 0)
				{
					strcpy(s.msg, "调用函数报错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				tmmsm2a_back.Tables["MMSM2A"].Rows.Add();
				Log::Trace("", "", "I");
				Log::Info("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm2a_back.Tables["MMSM2A"].Rows[i]["PROC_NO"].ToString());
				tmmsm2a_back.Tables["MMSM2A"].Rows[i]["PROC_DIV"] = "I";//标记为新增
				Log::Info("", __FUNCTION__, "like=[{0}]", __LINE__);
				doFlag = f_mmsm2a_proc(&tmmsm2a_back, bcls_ret, conn);
				if (doFlag < 0)
				{
					strcpy(s.msg, "调用函数报错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			
		}

		tmmsm24.MergeTo(tmmsm24_back.Tables[0], false);
		tmmsm24_back.Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";
		if (tmmsm24.QueryCount("HEAT_NO")>0)
		{
			Log::Trace("", "", "U");
			tmmsm24_back.Tables[0].Rows[0]["PROC_DIV"] = "U";//标记为修改
			doFlag = f_mmsm24_proc(&tmmsm24_back, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//tmmsm24.Update("MIDDLE_STIR_FLOW,STRONG_STIR_DUR,MIDSTRONG_STIR_DUR,MIDDLE_STIR_DUR,STRONG_STIR_FLOW,MIDSTRONG_STIR_FLOW,SOFT_STIR_FLOW,BAS_ONLINE","HEAT_NO");

		}
		else
		{
			Log::Trace("", "", "I");
			tmmsm24_back.Tables[0].Rows[0]["PROC_DIV"] = "I";//标记为新增
			doFlag = f_mmsm24_proc(&tmmsm24_back, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
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
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["MESSAGE_CLASS"] = "SJLF01";//任务池准入条件的比对值
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


