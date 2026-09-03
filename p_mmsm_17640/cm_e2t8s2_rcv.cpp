/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      IF(合金熔化炉)生产炉报（INT_MES_PROD_SUMMARY_IF）
Version:     1.0
Date:        2023-10-23
Description: 实绩接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// IF生产实绩表电文
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明
int f_mmsm19_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测 

BM2F_ENTERACE_TELE(cm_e2t8s2_rcv)

int f_cm_e2t8s2_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd_yzsc(conn);
	CDbCommand cmd_yl(conn);
	CDbCommand cmd_inq_test(conn);
	CDbCommand cmd_ps(conn);
	CDbCommand cmd_12z(conn);
	CModel tmmsm19("TMMSM19");
	CModel tpssm12zt("TPSSM12ZT");
	CModel tpssm12z("TPSSM12Z");
	CString heat_no = " ";
	CString dev_code = " ";
	CString sm_plan_no2 = "";//炼钢计划号
	CString proc_no = " ";//商议炉炉号
	CString end_time_trp =" ";//上一炉出钢结束时间

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		tmmsm19.MergeFrom(bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]);
		///工号
		dev_code = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["AGGREGATE_NAME"].ToString();

		//INT_MES_PROD_SUMMARY_IF.PLAN_NUMBER
		sm_plan_no2 = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["PLAN_NUMBER"].ToString();
		tmmsm19["ID_SJ"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["ID"].ToString();

		//同工位处理次数
		//tmmsm19["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["TREATMENT_COUNTER"].ToString();

		Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no2);
		//根据计划号获取熔炼号和制造命令号
		cmd_inq_code.SetCommandText(" SELECT HEAT_NO,PONO,SM_PLAN_NO FROM TPSSM11 WHERE SM_PLAN_NOL2 ='" + sm_plan_no2 + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm19["HEAT_NO"] = cmd_inq_code.GetString(1);
			tmmsm19["PONO"] = cmd_inq_code.GetString(2);
			tmmsm19["SM_PLAN_NO"] = cmd_inq_code.GetString(3);
		}

		cmd_inq.SetCommandText(" SELECT STATION_ID,STATION_NO FROM TPSSMD1 WHERE DEV_CODE='" + dev_code + "' ");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm19["STATION_ID"] = cmd_inq.GetString(1);
			tmmsm19["STATION_NO"] = cmd_inq.GetString(2);
		}
		Log::Trace("", "STATION_ID", "STATION_ID = {0}", tmmsm19["STATION_ID"].ToString());
		cmd_inq.Close();
		tmmsm19["DEV_CODE"] = dev_code;
		//处理号
		if (tmmsm19["PROC_NO"].ToString()==" "){
			tmmsm19["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["HEAT_NUMBER"].ToString();
		}
		//tmmsm19["HEAT_NO"] = heat_no;
		//tmmsm19["DEV_CODE"] = dev_code;

		//二级炉号作为处理号存储到L2_PROC_NO字段中
		tmmsm19["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["HEAT_NUMBER"].ToString();
		tmmsm19["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["HEAT_NUMBER"].ToString();
		Log::Trace("", "INT_MES_PROD_SUMMARY_IF", "INT_MES_PROD_SUMMARY_IF[{0}]", bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["HEAT_NUMBER"].ToString());
		Log::Trace("", "L2_PROC_NO", "L2_PROC_NO[{0}]", tmmsm19["L2_PROC_NO"].ToString());
		//计划号
		tmmsm19["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["PLAN_NUMBER"].ToString();
		//分包号
		tmmsm19["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["SPLIT_INDICATION"];
		//包号
		tmmsm19["LADLE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["LADLE_NUMBER"].ToString();
		//空包重量 
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() != 0)
			tmmsm19["EMPTY_LADLE_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal()/1000;
		//班组
		tmmsm19["PROD_SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["SHIFT_TEAM"].ToString();
		//操作工
		tmmsm19["ASSISTANT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["OPERATOR"].ToString();
		//实际钢种
		tmmsm19["ST_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["GRADE_ACT"].ToString();
		//实际钢水重量  
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["WEIGHT_ACT"].ToDecimal()!= 0)
			tmmsm19["ACTRESULT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["WEIGHT_ACT"].ToDecimal() / 1000;
		//加热开始时刻
		tmmsm19["START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["HEAT_START"].ToString();
		//加热结束时刻
		tmmsm19["END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["HEAT_END"].ToString();
		//加热次数
		tmmsm19["HEAT_COUNT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["HEATING_NUMBER"];
		//加热时间
		tmmsm19["BIL_MELT_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["HEATING_TIME"].ToDecimal()/60;
		tmmsm19["BIL_MELT_TIME"] = tmmsm19["BIL_MELT_TIME"].ToDecimal().Round(2);
		//电耗
		tmmsm19["POWER_CONSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["POWER"];
		//首次加热开始时刻
		tmmsm19["FIRST_HEATING_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["FIRST_HEATING_START"].ToString();
		//最后一次加热结束时刻
		tmmsm19["LAST_HEATING_END"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["LAST_HEATING_END"].ToString();
		//上炉钢水余量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["PREMELT_WEIGHT"].ToDecimal() != 0)
			tmmsm19["LADLE_PRE_LIQUID_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["PREMELT_WEIGHT"].ToDecimal() / 1000;
		//碳粉重
		tmmsm19["C_POWDER_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["C_POWDER_WEIGHT"];
		//吹氧次数
		tmmsm19["OXYGEN_NUMBER"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["OXYGEN_NUMBER"];
		//吹氧时间
		tmmsm19["BLOW_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["BLOW_TIME"].ToDecimal()/60;
		tmmsm19["BLOW_DURATION"] = tmmsm19["BLOW_DURATION"].ToDecimal().Round(2);
		//吹氧量
		tmmsm19["OXYGEN_FINAL"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["OXYGEN_TOTAL"];
		//吹氧开始时刻
		tmmsm19["FIRST_BLOW_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["FIRST_BLOW_START"].ToString();
		//吹氧结束时刻
		tmmsm19["LAST_BLOW_END"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["LAST_BLOW_END"].ToString();
		//出钢开始时刻
		tmmsm19["TAP_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["TAPPING_START"].ToString();
		//出钢结束时刻
		tmmsm19["TAP_END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["TAPPING_END"].ToString();
		//顶吹氮量
		tmmsm19["NITROGEN_TOP_AMOUNT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["NITROGEN_TOP_AMOUNT"];
		//顶吹氮时长
		tmmsm19["NITROGEN_TOP_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["NITROGEN_TOP_TIME"].ToDecimal()/60;
		tmmsm19["NITROGEN_TOP_TIME"] = tmmsm19["NITROGEN_TOP_TIME"].ToDecimal().Round(2);
		//底吹氮量
		tmmsm19["BTTM_N_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["NITROGEN_BOTTOM_AMOUNT"];
		//底吹氮时长
		tmmsm19["NITROGEN_BOTTOM_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["NITROGEN_BOTTOM_TIME"].ToDecimal() / 60;
		tmmsm19["NITROGEN_BOTTOM_TIME"] = tmmsm19["NITROGEN_BOTTOM_TIME"].ToDecimal().Round(2);
		//废钢量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["SCRAP_WEIGHT"].ToDecimal() != 0)
			tmmsm19["SCRAP_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["SCRAP_WEIGHT"].ToDecimal() / 1000;
		//装料开始时刻
		tmmsm19["CHARGE_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["CHARGE_START_TIME"].ToString();
		//通电开始时刻
		tmmsm19["POWER_START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["POWER_ON_TIME"].ToString();
		//剩余重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["REMAIN_WT"].ToDecimal() != 0)
			tmmsm19["REMAIN_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["REMAIN_WT"].ToDecimal() / 1000;
		//出钢时间
		tmmsm19["TAPTOTAP_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["TAPTOTAP_DURATION"];
		//下一个设备代码
		tmmsm19["NEXT_DEV_CODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["NEXT_DEV_CODE"].ToString();
		//合包模式
		tmmsm19["MELT_MODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["MELT_MODE"].ToString();
		//满钢包重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["FULL_LADLE_WEIGHT"].ToDecimal() != 0)
			tmmsm19["FULL_LADLE_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["FULL_LADLE_WEIGHT"].ToDecimal() / 1000;
		//炉龄
		tmmsm19["FURNACE_AGE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["HEAT_LIFE"];
		//耐材厂商
		tmmsm19["WORK_MAKER"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_IF"].Rows[0]["FIREBRICK_FACTORY"].ToString();
		tmmsm19["REC_CREATOR"] = s.userid;
		tmmsm19["REC_CREATE_TIME"] = datetime;
		//计算冶炼时长
		cmd_yl.SetCommandText(
			" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm19["TAP_END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
			" TO_DATE('" + tmmsm19["POWER_START_TIME"].ToString() + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
			" from dual ");
		cmd_yl.ExecuteReader();
		Log::Trace("", "PROC_NO", "冶炼周期上一炉号:PROC_NO[{0}]", proc_no);
		Log::Trace("", "TAP_END_TIME", "TAP_END_TIME:TAP_END_TIME[{0}]", tmmsm19["TAP_END_TIME"].ToString());
		Log::Trace("", "POWER_START_TIME", "POWER_START_TIME[{0}]", tmmsm19["POWER_START_TIME"].ToString());
		if (cmd_yl.Read())
		{
			tmmsm19["MELT_DURATION"] = cmd_yl.GetDecimal(1);
			Log::Trace("", "MELT_DURATION", "MELT_DURATION[{0}]", tmmsm19["MELT_DURATION"].ToString());
		}
		cmd_yl.Close();
		//tmmsm19["MELT_DURATION"] = tmmsm19["TAP_END_TIME"].ToDecimal() - tmmsm19["CHARGE_START_TIME"].ToDecimal();
		Log::Trace("", "MELT_DURATION", "秒:MELT_DURATION[{0}]", tmmsm19["MELT_DURATION"].ToString());
		if (tmmsm19["MELT_DURATION"].ToDecimal() != 0 && tmmsm19["MELT_DURATION"].ToDecimal()>0 && tmmsm19["MELT_DURATION"].ToDecimal()<9999){
			tmmsm19["MELT_DURATION"] = tmmsm19["MELT_DURATION"].ToDecimal() / 60;    
			tmmsm19["MELT_DURATION"] = tmmsm19["MELT_DURATION"].ToDecimal().Round(2);
			Log::Trace("", "MELT_DURATION", "分:MELT_DURATION[{0}]", tmmsm19["MELT_DURATION"].ToString());
		}
		else{
			tmmsm19["MELT_DURATION"] = "0";
		}
		//计算冶炼周期
		proc_no = tmmsm19["L2_PROC_NO"].ToString() - 1;
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
		Log::Trace("", "end_time_trp", "冶炼周期上一炉出钢结束时间:TAP_END_TIME[{0}]", tmmsm19["TAP_END_TIME"].ToString());
		cmd_yz.Close();
		//计算秒
		if (end_time_trp != " "){
			cmd_yzsc.SetCommandText(
				" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm19["TAP_END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
				" TO_DATE('" + end_time_trp + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
				" from dual ");
			cmd_yzsc.ExecuteReader();
			if (cmd_yzsc.Read())
			{
				tmmsm19["SMELT_CYCLE"] = cmd_yzsc.GetDecimal(1);
			}
			cmd_yzsc.Close();
		}

		//tmmsm19["SMELT_CYCLE"] = tmmsm19["TAP_END_TIME"].ToDecimal() - end_time_trp;
		if (tmmsm19["SMELT_CYCLE"].ToDecimal() > 0){
			tmmsm19["SMELT_CYCLE"] = tmmsm19["SMELT_CYCLE"].ToDecimal() / 60;
			tmmsm19["SMELT_CYCLE"] = tmmsm19["SMELT_CYCLE"].ToDecimal().Round(2);
			Log::Trace("", "SMELT_CYCLE", "冶炼周期:SMELT_CYCLE[{0}]", tmmsm19["SMELT_CYCLE"].ToString());
		}
		//更改tpssm12zt表
		if (tmmsm19["FULL_LADLE_WEIGHT"].ToString() != " "&&tmmsm19["FULL_LADLE_WEIGHT"].ToDecimal() != 0){
			CString archive_flag = " ";
			cmd_12z.SetCommandText(" select ARCHIVE_FLAG from TPSSM12Z WHERE PROC_NO='"+tmmsm19["L2_PROC_NO"].ToString()+"' ");
			cmd_12z.ExecuteReader();
			if (cmd_12z.Read()){
				archive_flag = cmd_12z.GetString(1);
			}
			if (archive_flag != "1"){
				tpssm12z["ARCHIVE_FLAG"] = "1";
				tpssm12z["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString();
				tpssm12z.Update("ARCHIVE_FLAG", "PROC_NO");
				tpssm12zt["LADLE_NO"] = tmmsm19["LADLE_NO"].ToString();
				CString code = " ";
				cmd_ps.SetCommandText(" SELECT CODE FROM TEP0002 WHERE CODE_CLASS='PSAL2N' AND CODE_DESC_5_CONTENT='" + tmmsm19["MELT_MODE"].ToString() + "' ");
				cmd_ps.ExecuteReader();
				if (cmd_ps.Read()){
					code = cmd_ps.GetString(1);
				}
				cmd_ps.Close();
				Log::Trace("", "MELT_MODE", "MELT_MODE[{0}],code[{1}]", tmmsm19["MELT_MODE"].ToString(), code);
				if (tpssm12zt.QueryCount("LADLE_NO") > 0){
					tpssm12zt.Query("LADLE_NO");
					tpssm12zt.TrimOrBlank();
					tpssm12zt["SMELT_MODE2"] = code;
					if (tpssm12zt["PROC_NO"].ToString() == " "){
						tpssm12zt["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString();
						tpssm12zt.Update("PROC_NO,SMELT_MODE2", "LADLE_NO");
					}
					if (tpssm12zt["PROC_NO"].ToString() != " "&&tpssm12zt["PROC_NO2"].ToString() == " "&&tpssm12zt["PROC_NO"].ToString() != tmmsm19["L2_PROC_NO"].ToString()){
						tpssm12zt["PROC_NO2"] = tmmsm19["L2_PROC_NO"].ToString();
						tpssm12zt.Update("PROC_NO2,SMELT_MODE2", "LADLE_NO");
					}
					if (tpssm12zt["PROC_NO"].ToString() != " "&&tpssm12zt["PROC_NO2"].ToString() != " "&&tpssm12zt["PROC_NO3"].ToString() == " "&&tpssm12zt["PROC_NO"].ToString() != tmmsm19["L2_PROC_NO"].ToString() && tpssm12zt["PROC_NO2"].ToString() != tmmsm19["L2_PROC_NO"].ToString()){
						tpssm12zt["PROC_NO3"] = tmmsm19["L2_PROC_NO"].ToString();
						tpssm12zt.Update("PROC_NO3,SMELT_MODE2", "LADLE_NO");
					}
					if (tpssm12zt["PROC_NO"].ToString() != " "&&tpssm12zt["PROC_NO2"].ToString() != " "&&tpssm12zt["PROC_NO3"].ToString() != " "&&tpssm12zt["PROC_NO4"].ToString() == " "&&tpssm12zt["PROC_NO"].ToString() != tmmsm19["L2_PROC_NO"].ToString() && tpssm12zt["PROC_NO2"].ToString() != tmmsm19["L2_PROC_NO"].ToString() && tpssm12zt["PROC_NO3"].ToString() != tmmsm19["L2_PROC_NO"].ToString()){
						tpssm12zt["PROC_NO4"] = tmmsm19["L2_PROC_NO"].ToString();
						tpssm12zt.Update("PROC_NO4,SMELT_MODE2", "LADLE_NO");
					}
					if (tpssm12zt["PROC_NO"].ToString() != " "&&tpssm12zt["PROC_NO2"].ToString() != " "&&tpssm12zt["PROC_NO3"].ToString() != " "&&tpssm12zt["PROC_NO4"].ToString() != " "&&tpssm12zt["PROC_NO"].ToString() != tmmsm19["L2_PROC_NO"].ToString() && tpssm12zt["PROC_NO2"].ToString() != tmmsm19["L2_PROC_NO"].ToString() && tpssm12zt["PROC_NO3"].ToString() != tmmsm19["L2_PROC_NO"].ToString() && tpssm12zt["PROC_NO4"].ToString() != tmmsm19["L2_PROC_NO"].ToString()){
						if (tpssm12zt["REMARK"].ToString().GetLength() > 480)
						{ 
							tpssm12zt["REMARK"] = "";
						}
						
						tpssm12zt["REMARK"] = tpssm12zt["REMARK"].ToString() + tmmsm19["L2_PROC_NO"].ToString() + ",";
						tpssm12zt.Update("REMARK", "LADLE_NO");
					}
				}
				else{
					tpssm12zt["SMELT_MODE2"] = code;
					tpssm12zt["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString();
					tpssm12zt.Insert();
				}
			}
			else{
				Log::Trace("", "archive_flag", "archive_flag[{0}],proc[{1}]", archive_flag, tmmsm19["L2_PROC_NO"].ToString());
			}
		}	
		f_tableObjectCheck9999(tmmsm19);
		bcls_rec->Tables.Clear();
		tmmsm19.MergeTo(bcls_rec->Tables.Add());
		/*if (tmmsm19["PROC_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "IF处理号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		//bcls_rec->Tables[0].Columns.Add(tmmsm19);
		if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		}
		if (!bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
		}
		if (!bcls_rec->Tables[0].Columns.Contains("FLAG_NO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "FLAG_NO");
		}
		bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";//厂别
		bcls_rec->Tables[0].Rows[0]["FLAG_NO"] = "2";//
		if (tmmsm19.Query("PROC_NO"))
		{
			Log::Trace("", "", "123456");
			Log::Trace("", "PROC_NO", "PROC_NO", tmmsm19["L2_PROC_NO"].ToString());
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "U";//标记为修改
			doFlag = f_mmsm19_proc(bcls_rec, bcls_ret, conn);
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
			doFlag = f_mmsm19_proc(bcls_rec, bcls_ret, conn);
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


