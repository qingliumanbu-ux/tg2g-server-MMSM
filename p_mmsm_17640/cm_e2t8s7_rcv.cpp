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
/// RH炉次报告
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


int f_mmsm23_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm2a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测

BM2F_ENTERACE_TELE(cm_e2t8s7_rcv)

int f_cm_e2t8s7_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);
	CDbCommand cmd_inq_id(conn);
	CDbCommand cmd_yl(conn);
	CDbCommand cmd_yz(conn);
	CDbCommand cmd_yzsc(conn);
	CDbCommand cmd_inq_test(conn);
	CModel tmmsm23("TMMSM23");
	CModel tmmsm2a("TMMSM2A");
	EIClass tmmsm23_back;
	EIClass tmmsm2a_back;
	CString dev_code = "";
	CString sm_plan_no = " ";
	CString cname = " ";
	CString proc_no = " ";
	CString end_time_trp = " ";

	//加入函数的表

	tmmsm23_back.Tables.Add();
	if (!tmmsm23_back.Tables[0].Columns.Contains("PROC_DIV"))
	{
		tmmsm23_back.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
	}
	if (!tmmsm23_back.Tables[0].Columns.Contains("PRACT_COLL_MODE"))
	{
		tmmsm23_back.Tables[0].Columns.Add(DT_STRING, "PRACT_COLL_MODE");
	}
	if (!tmmsm23_back.Tables[0].Columns.Contains("FACTORY_DIV"))
	{
		tmmsm23_back.Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
	}
	//tmmsm23_back.Tables[0].Rows.Add();
	tmmsm23_back.Tables[0].Columns.Add(tmmsm23);
	//tmmsm23_back.Tables[0].Rows.Add();

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

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		tmmsm23.MergeFrom(bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]);
		///工号
		dev_code = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["AGGREGATE_NAME"].ToString();
		sm_plan_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["ORDER_NUMBER"].ToString();
		//熔炼号
		tmmsm23["HEAT_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["HEAT_NUMBER"].ToString();
		tmmsm23["ID_SJ"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["ID"].ToString();
		tmmsm23["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["HEAT_NUMBER"].ToString();
		//同工位处理次数
		tmmsm23["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["TREATMENT_COUNTER"].ToString();

		Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no);
		//根据计划号获取熔炼号和制造命令号
		cmd_inq_code.SetCommandText(" SELECT  PONO, SM_PLAN_NO FROM "
			" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
			" UNION "
			" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm23["PONO"] = cmd_inq_code.GetString(1);
			tmmsm23["SM_PLAN_NO"] = cmd_inq_code.GetString(2);
		}
		cmd_inq_code.Close();

		cmd_inq.SetCommandText(" SELECT T1.PROC_NO,T2.STATION_ID,T2.STATION_NO FROM (  "
			" SELECT PROC_NO, DEV_CODE, DECODE(PRE_SOLUTION_FLAG, '1', '0', '1') C_DIV FROM TPSSM12 WHERE HEAT_NO = '" + tmmsm23["HEAT_NO"].ToString() + "' AND DEV_CODE = '" + dev_code + "' and TREATMENT_COUNTER ='" + tmmsm23["SAME_PROC_NUM"].ToString() + "' ) T1 LEFT JOIN TPSSMD1 T2  "
			" on t1.DEV_CODE = t2.DEV_CODE AND T1.C_DIV = T2.C_DIV");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm23["PROC_NO"] = cmd_inq.GetString(1);
			tmmsm23["STATION_ID"] = cmd_inq.GetString(2);
			tmmsm23["STATION_NO"] = cmd_inq.GetString(3);
		}
		/*else{
			tmmsm23["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["HEAT_NUMBER"].ToString();
			tmmsm23["STATION_ID"] = "R";
			tmmsm23["STATION_NO"] = "4";
		}*/
		Log::Trace("", "PROC_NO", "PROC_NO = {0}", tmmsm23["PROC_NO"].ToString());
		tmmsm23["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["HEAT_NUMBER"].ToString();
		cmd_inq.Close();


		tmmsm23["DEV_CODE"] = dev_code;
		//计划号 
		tmmsm23["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["ORDER_NUMBER"].ToString();

		//空包重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() != 0){
			tmmsm23["EMPTY_LADLE_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal()/1000;
		}
		//分包号SPLIT_INDICATION
		//班组
		tmmsm23["SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["SHIFT_TEAM"].ToString();
		Log::Info("", __FUNCTION__, "__like-__=[{0}]", __LINE__);
		//操纵工
		tmmsm23["ASSISTANT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["OPERATOR"].ToString();
		/*cmd_inq_id.SetCommandText(" select ENAME FROM es.tesuserinfo where cname='"+cname+"' ");
		cmd_inq_id.ExecuteReader();
		if (cmd_inq_id.Read())
		{
			tmmsm23["ASSISTANT"] = cmd_inq_id.GetString(1);
		}
		Log::Trace("", "ASSISTANT", "ASSISTANT = {0}", tmmsm23["ASSISTANT"].ToString());
		cmd_inq_id.Close();*/
		//内部钢种
		tmmsm23["ST_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["GRADE_ACT"].ToString();
		//开始时间
		tmmsm23["START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["HEAT_START"].ToString();
		//结束时间
		tmmsm23["END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["HEAT_END"].ToString();
		//实际重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["WEIGHT_ACT"].ToDecimal() != 0){
			tmmsm23["ACTRESULT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["WEIGHT_ACT"].ToDecimal()/1000;
		}
		//吹氧量(m3)
		tmmsm23["O2_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["OXYGEN_TOT"].ToString();
		//氩气总量
		tmmsm23["AR_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["ARGON_TOT"].ToString();
		//氮气总量
		tmmsm23["N_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["NITROGEN_TOT"].ToString();
		//天然气总量
		tmmsm23["NG_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["NATURALGAS_TOT"].ToString();
		//压缩空气总量 COMPRESSAIR_TOT
		//蒸汽总量 STEAM_TOT
		//工业用水总量
		tmmsm23["WATER_USE_QTY"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["INDUSTRYWATER_TOT"].ToString();
		//生活用水总量 POTABLEWATER_TOT
		//深真空压力 DEEP_VAC_PRESS  
		//深真空时间 DEEP_VAC_DUR  
		tmmsm23["DEEP_VAC_DUR"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["DEEP_VAC_DUR"].ToDecimal()/60;
		tmmsm23["DEEP_VAC_DUR"] = tmmsm23["DEEP_VAC_DUR"].ToDecimal().Round(2);
		//轻搅拌时间 SOFT_STIRRING_DUR
		tmmsm23["SOFT_STIRRING_DUR"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["SOFT_STIRRING_DUR"].ToDecimal() / 60;
		tmmsm23["SOFT_STIRRING_DUR"] = tmmsm23["SOFT_STIRRING_DUR"].ToDecimal().Round(2);
		//定氢量 STEEL_H
		//开始温度
		tmmsm23["START_STEEL_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["TEMP_START"].ToString();
		//结束温度
		tmmsm23["END_STEEL_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["TEMP_END"].ToString();
		//炉座号 
		tmmsm23["FURNACE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["VESSEL_NUMBER"].ToString();
		//吹氧枪枪龄
		tmmsm23["O2_LANCE_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["SNORKEL_LIFE"].ToString();
		//取样方式 SMP_MODE
		//上升流量 ASC_GAS_FLOW
		//脱碳时间 DE_C_DURATION
		tmmsm23["DE_C_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["DE_C_DURATION"].ToDecimal() / 60;
		tmmsm23["DE_C_DURATION"] = tmmsm23["DE_C_DURATION"].ToDecimal().Round(2);
		//钢包离站重量 LADLE_DEPART_WT
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["LADLE_DEPART_WT"].ToDecimal() != 0){
			tmmsm23["LADLE_DEPART_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH"].Rows[0]["LADLE_DEPART_WT"].ToDecimal()/1000;
		}
		//下工序设备代码 NEXT_DEV_CODE
		//真空开始时间

		tmmsm23["REC_CREATOR"] = s.userid;
		tmmsm23["REC_CREATE_TIME"] = datetime;

		//查询熔炼号
		/*cmd_inq.SetCommandText("SELECT HEAT_NO FROM TPSSM12 WHERE PROC_NO =@PROC_NO");
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("PROC_NO", tmmsm23["PROC_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm23["HEAT_NO"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();*/

		//计算冶炼时长
		cmd_yl.SetCommandText(
			" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm23["END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
			" TO_DATE('" + tmmsm23["START_TIME"].ToString() + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
			" from dual ");
		cmd_yl.ExecuteReader();
		if (cmd_yl.Read())
		{
			tmmsm23["MELT_DURATION"] = cmd_yl.GetDecimal(1);
		}
		cmd_yl.Close();
		//tmmsm21["MELT_DURATION"] = tmmsm21["END_TIME"].ToDecimal() - tmmsm21["START_TIME"].ToDecimal()/1000;
		Log::Trace("", "MELT_DURATION", "秒:MELT_DURATION[{0}]", tmmsm23["MELT_DURATION"].ToString());
		if (tmmsm23["MELT_DURATION"].ToDecimal() != 0){
			tmmsm23["MELT_DURATION"] = tmmsm23["MELT_DURATION"].ToDecimal() / 60;
			tmmsm23["MELT_DURATION"] = tmmsm23["MELT_DURATION"].ToDecimal().Round(2);
			Log::Trace("", "MELT_DURATION", "分:MELT_DURATION[{0}]", tmmsm23["MELT_DURATION"].ToString());
		}

		//计算冶炼周期
		proc_no = tmmsm23["PROC_NO"].ToString() - 1;
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
				" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm23["END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
				" TO_DATE('" + end_time_trp + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
				" from dual ");
			cmd_yzsc.ExecuteReader();
			if (cmd_yzsc.Read())
			{
				tmmsm23["SMELT_CYCLE"] = cmd_yzsc.GetDecimal(1);
			}
			cmd_yzsc.Close();
			if (tmmsm23["SMELT_CYCLE"].ToDecimal() != 0){
				tmmsm23["SMELT_CYCLE"] = tmmsm23["SMELT_CYCLE"].ToDecimal() / 60;
				tmmsm23["SMELT_CYCLE"] = tmmsm23["SMELT_CYCLE"].ToDecimal().Round(2);
				Log::Trace("", "SMELT_CYCLE", "冶炼周期:SMELT_CYCLE[{0}]", tmmsm23["SMELT_CYCLE"].ToString());
			}
		}

		tmmsm23.MergeTo(tmmsm23_back.Tables[0], false);
		tmmsm23_back.Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";

		for (int i = 0; i < bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH_DET"].Rows.get_Count(); i++)
		{
			Log::Info("", __FUNCTION__, "HEAT_NO=[{0}]", tmmsm23["HEAT_NO"].ToString());
			tmmsm2a.CopyFrom(tmmsm23);
			//物料代码
			tmmsm2a["MAT_CODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH_DET"].Rows[i]["MATERIAL_CODE"].ToString();
			//批次号
			tmmsm2a["LOT_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH_DET"].Rows[i]["BATCH_NUMBER"].ToString();
			//长度
			tmmsm2a["LEN"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_RH_DET"].Rows[i]["LENGTH"].ToString();
			tmmsm2a["PROC_COUNT"] = i;
			tmmsm2a["REC_CREATOR"] = s.userid;
			tmmsm2a["REC_CREATE_TIME"] = datetime;
			tmmsm2a["DEV_CODE"] = dev_code;
			tmmsm2a["HEAT_NO"] = tmmsm23["HEAT_NO"].ToString();
			tmmsm2a["PROC_NO"] = tmmsm23["PROC_NO"].ToString();
			tmmsm2a["ST_NO"] = tmmsm23["ST_NO"].ToString();
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


		/*if (tmmsm23["PROC_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "处理号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		Log::Info("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm23["PROC_NO"].ToString());
		Log::Info("", __FUNCTION__, "L2_PROC_NO=[{0}]", tmmsm23["L2_PROC_NO"].ToString());
		Log::Info("", __FUNCTION__, "HEAT_NO=[{0}]", tmmsm23["HEAT_NO"].ToString());
		f_tableObjectCheck9999(tmmsm23);
		if (tmmsm23.QueryCount("HEAT_NO")>0)
		{
			Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "U");
			tmmsm23_back.Tables[0].Rows[0]["PROC_DIV"] = "U";
			tmmsm23_back.Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";
			doFlag = f_mmsm23_proc(&tmmsm23_back, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//tmmsm23.Update("PROC_NO");
		}
		else
		{
			Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "I");
			tmmsm23_back.Tables[0].Rows[0]["PROC_DIV"] = "I";
			tmmsm23_back.Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";
			doFlag = f_mmsm23_proc(&tmmsm23_back, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/*tmmsm23.TrimOrBlank();
			tmmsm23.Insert();*/
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


