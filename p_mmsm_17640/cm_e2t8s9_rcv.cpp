/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      VOD炉次报告
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
/// VOD炉次报告接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
#include "epex.h" 


//外部函数声明
int f_mmsm25_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测

BM2F_ENTERACE_TELE(cm_e2t8s9_rcv)

int f_cm_e2t8s9_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd_inq_id(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_test(conn);
	CModel tmmsm25("TMMSM25");
	CString heat_no = " ";
	CString dev_code = "";
	CString sm_plan_no = " ";
	CString cname = " ";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		tmmsm25.MergeFrom(bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]); 
		///工号
		dev_code = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["AGGREGATE_NAME"].ToString();
		sm_plan_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["ORDER_NUMBER"].ToString();
		//熔炼号
		tmmsm25["HEAT_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HEAT_NUMBER"].ToString();
		tmmsm25["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HEAT_NUMBER"].ToString();
		tmmsm25["ID_SJ"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["ID"].ToString();
		//同工位处理次数
		tmmsm25["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["TREATMENT_COUNTER"].ToString();

		Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no);
		//根据计划号获取熔炼号和制造命令号
		cmd_inq_code.SetCommandText(" SELECT  PONO, SM_PLAN_NO FROM "
			" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
			" UNION "
			" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm25["PONO"] = cmd_inq_code.GetString(1);
			tmmsm25["SM_PLAN_NO"] = cmd_inq_code.GetString(2);
		}
		cmd_inq_code.Close();

		cmd_inq.SetCommandText(" SELECT T1.PROC_NO,T2.STATION_ID,T2.STATION_NO FROM (  "
			" SELECT PROC_NO, DEV_CODE, DECODE(PRE_SOLUTION_FLAG, '1', '0', '1') C_DIV FROM TPSSM12 WHERE HEAT_NO = '" + tmmsm25["HEAT_NO"].ToString() + "' AND DEV_CODE = '" + dev_code + "' and TREATMENT_COUNTER ='" + tmmsm25["SAME_PROC_NUM"].ToString() + "' ) T1 LEFT JOIN TPSSMD1 T2  "
			" on t1.DEV_CODE = t2.DEV_CODE AND T1.C_DIV = T2.C_DIV");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm25["PROC_NO"] = cmd_inq.GetString(1);
			tmmsm25["STATION_ID"] = cmd_inq.GetString(2);
			tmmsm25["STATION_NO"] = cmd_inq.GetString(3);
		}
		Log::Trace("", "PROC_NO", "PROC_NO = {0}", tmmsm25["PROC_NO"].ToString());
		cmd_inq.Close();
		if (tmmsm25["PROC_NO"].ToString() == " "){
			tmmsm25["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HEAT_NUMBER"].ToString();

		}
		tmmsm25["DEV_CODE"] = dev_code;
		//计划号 
		tmmsm25["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["ORDER_NUMBER"].ToString();

		//分包号
		tmmsm25["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["SPLIT_INDICATION"].ToString(); 
		//空罐重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() != 0){
			tmmsm25["EMPTY_LADLE_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() / 1000;
		}
		//班组
		tmmsm25["PROD_SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["SHIFT_TEAM"].ToString();
		//操作工
		tmmsm25["ASSISTANT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["OPERATOR"].ToString();
	    /*cmd_inq_id.SetCommandText(" select ENAME FROM es.tesuserinfo where cname='" + cname + "' ");
		cmd_inq_id.ExecuteReader();
		if (cmd_inq_id.Read())
		{
			tmmsm25["ASSISTANT"] = cmd_inq_id.GetString(1);
		}
		Log::Trace("", "ASSISTANT", "ASSISTANT = {0}", tmmsm25["ASSISTANT"].ToString());
		cmd_inq_id.Close();*/
		//炉次开始时间
		tmmsm25["START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HEAT_START"].ToString();
		Log::Trace("", "", "START_TIME = {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HEAT_START"].ToString());
		//炉次结束时间
		tmmsm25["END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HEAT_END"].ToString();
		Log::Trace("", "", "END_TIME = {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HEAT_END"].ToString());

		//实际内部钢种
		tmmsm25["ST_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["GRADE_ACT"].ToString();
		//实际重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["WEIGHT_ACT"].ToDecimal() != 0){
			tmmsm25["ACTRESULT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["WEIGHT_ACT"].ToDecimal() / 1000;
		}
		
		//总氧量
		tmmsm25["OXYGEN_FINAL"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["OXYGEN_TOT"].ToString();
		//总氩量
		tmmsm25["AR_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["ARGON_TOT"].ToString();
		//总氮量
		tmmsm25["EAF_TOTAL_N2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["NITROGEN_TOT"].ToString();
		//总天然气量
		tmmsm25["NATURALGAS_TOT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["NATURALGAS_TOT"].ToString();
		//总压缩空气量
		tmmsm25["COMPRESSAIR_TOT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["COMPRESSAIR_TOT"].ToString();
		//总蒸汽量
		tmmsm25["STEAM_TOT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["STEAM_TOT"].ToString();
		//总工业水量
		tmmsm25["WATER_USE_QTY"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["INDUSTRYWATER_TOT"].ToString();
		//总饮用水量
		tmmsm25["POTABLEWATER_TOT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["POTABLEWATER_TOT"].ToString();
		//开始渣重
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["SLAG_WT_START"].ToDecimal() != 0){
			tmmsm25["SLAG_WT_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["SLAG_WT_START"].ToDecimal() / 1000;
		}
		

		//净空
		tmmsm25["HEADROOM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HEADROOM"].ToString();
	
		//氧枪模式
		tmmsm25["O2_LANCE_MODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["O2_LANCE_MODE"].ToString();
		
		//VCD结束温度
		tmmsm25["VCD_END_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["VCD_END_TEMP"].ToString();
		Log::Trace("", "", "VCD_END_TEMP = {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["VCD_END_TEMP"].ToString());
		//VCD结束碳含量
		tmmsm25["VCD_END_C"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["VCD_END_C"].ToString();
		Log::Trace("", "", "VCD_END_C = {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["VCD_END_C"].ToString());


		//真空泵持续时间
		tmmsm25["VACUUM_PUMP_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["VACUUM_PUMP_DURATION"].ToDecimal()/60;
		tmmsm25["VACUUM_PUMP_DURATION"] = tmmsm25["VACUUM_PUMP_DURATION"].ToDecimal().Round(2);
		//10分钟沸腾时的真空度
		tmmsm25["VACUUM_DEGREE_BOIL_10MIN"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["VACUUM_DEGREE_BOIL_10MIN"].ToString();
		//沸腾时最高真空度
		tmmsm25["HIGHEST_VACUUM_DEGREE_BOIL"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HIGHEST_VACUUM_DEGREE_BOIL"].ToString();
		//沸腾时间
		tmmsm25["BOIL_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["BOIL_DURATION"].ToDecimal()/60;
		tmmsm25["BOIL_DURATION"] = tmmsm25["BOIL_DURATION"].ToDecimal().Round(2);
		//精抽沸腾时间
		tmmsm25["FINE_VACUUM_BOIL_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["FINE_VACUUM_BOIL_DURATION"].ToDecimal() / 60;
		tmmsm25["FINE_VACUUM_BOIL_DURATION"] = tmmsm25["FINE_VACUUM_BOIL_DURATION"].ToDecimal().Round(2);
		//精抽减压时间
		tmmsm25["FINE_VACUUM_REDUCING_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["FINE_VACUUM_REDUCING_DURATION"].ToDecimal() / 60;
		tmmsm25["FINE_VACUUM_REDUCING_DURATION"] = tmmsm25["FINE_VACUUM_REDUCING_DURATION"].ToDecimal().Round(2);
		//减压时最高真空度
		tmmsm25["HIGHEST_VACUUM_DEGREE_REDUCING"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["HIGHEST_VACUUM_DEGREE_REDUCING"].ToString();
		//减压时间
		tmmsm25["REDUCING_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["REDUCING_DURATION"].ToDecimal() / 60;
		tmmsm25["REDUCING_DURATION"] = tmmsm25["REDUCING_DURATION"].ToDecimal().Round(2);
		//减压结束温度
		tmmsm25["REDUC_END_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["REDUC_END_TEMP"].ToString();
		//计算氧气消耗量
		tmmsm25["CAL_O2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["CAL_O2_CONS"].ToString();
		//总氧气消耗量
		tmmsm25["TOTAL_O2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["TOTAL_O2_CONS"].ToString();
		//氧气吹炼时间
		tmmsm25["O2_BLOW_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["O2_BLOW_DURATION"].ToDecimal() / 60;
		tmmsm25["O2_BLOW_DURATION"] = tmmsm25["O2_BLOW_DURATION"].ToDecimal().Round(2);
		//离开钢包重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["LADLE_DEPART_WT"].ToDecimal() != 0){
			tmmsm25["LADLE_DEPART_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["LADLE_DEPART_WT"].ToDecimal() / 1000;
		}
		//结束渣厚
		tmmsm25["SLAG_THICK_END"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["SLAG_THICK_END"].ToString();
	    //钢包号
		tmmsm25["LADLE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["STEEL_LADLE_NO"].ToString();
		//钢包包龄
		tmmsm25["LADLE_AGE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["LADLE_LIFE"].ToString();
		//钢包到达温度
		tmmsm25["LADLE_ARRIVE_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["LADLE_ARRIVE_TEMP"].ToString();
		//钢包到达重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["LADLE_ARRIVE_WT"].ToDecimal() != 0){
			tmmsm25["LADLE_ARRIVE_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["LADLE_ARRIVE_WT"].ToDecimal() / 1000;
		}
		//钢包到达时的钢重
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["LADLE_ARRIVE_STEEL_WT"].ToDecimal() != 0){
			tmmsm25["LADLE_ARRIVE_STEEL_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["LADLE_ARRIVE_STEEL_WT"].ToDecimal() / 1000;
		}
		//开始渣厚
		tmmsm25["SLAG_THICK_START"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["SLAG_THICK_START"].ToString();
		//钢包离开温度
		tmmsm25["LADLE_DEPART_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["LADLE_DEPART_TEMP"].ToString();
		//熔化时间
		tmmsm25["MELT_DURATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["MELT_DURATION"].ToDecimal() / 60;
		tmmsm25["MELT_DURATION"] = tmmsm25["MELT_DURATION"].ToDecimal().Round(2);
		//下一个设备代码
		tmmsm25["NEXT_DEV_CODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["NEXT_DEV_CODE"].ToString();
		//氧枪位置
		tmmsm25["O2_LANCE_POS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_VOD"].Rows[0]["O2_LANCE_POS"].ToString();

		//tmmsm25["STATION_ID"] = "V";//设备类型

		tmmsm25["REC_CREATOR"] = s.userid;
		tmmsm25["REC_CREATE_TIME"] = datetime;


		bcls_rec->Tables.Clear();
		tmmsm25.MergeTo(bcls_rec->Tables.Add());

		/*if (tmmsm25["PROC_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "VOD处理号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/

		if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		}
		if (!bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
		}
		/*if (!bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "STATION_ID");
		}*/

		bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";//厂别
		//bcls_rec->Tables[0].Rows[0]["STATION_ID"] = "V";//设备类型
		f_tableObjectCheck9999(tmmsm25);
		if (tmmsm25.QueryCount("HEAT_NO"))
		{
			Log::Trace("", "", "123456");
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "U";//标记为修改
			doFlag = f_mmsm25_proc(bcls_rec, bcls_ret, conn);
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
			doFlag = f_mmsm25_proc(bcls_rec, bcls_ret, conn);
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


