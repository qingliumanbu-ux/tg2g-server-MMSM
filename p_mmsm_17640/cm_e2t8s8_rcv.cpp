/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      LTS炉次报告
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
/// LTS炉次报告接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
#include "epex.h" 


//外部函数声明
int f_mmsm26_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmts_call_judge(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//消息规则引擎
int f_qmts_xxyq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//消息引擎判定结果


BM2F_ENTERACE_TELE(cm_e2t8s8_rcv)

int f_cm_e2t8s8_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd_inq_test(conn);
	CDbCommand cmd_cy(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tmmsm26("TMMSM26");
	CString heat_no = " ";
	CString dev_code = "";
	CString sm_plan_no = " ";
	CString st_no = "";


	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		tmmsm26.MergeFrom(bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]);
		///工号
		dev_code = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["AGGREGATE_NAME"].ToString();
		sm_plan_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["ORDER_NUMBER"].ToString();
		heat_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["HEAT_NUMBER"].ToString();
		st_no = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["GRADE_ACT"].ToString();

		//熔炼号
		tmmsm26["HEAT_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["HEAT_NUMBER"].ToString();
		tmmsm26["ID_SJ"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["ID"].ToString();
		tmmsm26["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["HEAT_NUMBER"].ToString();
		//同工位处理次数
		tmmsm26["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["TREATMENT_COUNTER"].ToString();
		//L2_PROC_NO
		Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no);
		//根据计划号获取熔炼号和制造命令号
		cmd_inq_code.SetCommandText(" SELECT  PONO, SM_PLAN_NO FROM "
			" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
			" UNION "
			" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm26["PONO"] = cmd_inq_code.GetString(1);
			tmmsm26["SM_PLAN_NO"] = cmd_inq_code.GetString(2);
		}
		cmd_inq_code.Close();


		cmd_inq.SetCommandText(" SELECT T1.PROC_NO,T2.STATION_ID,T2.STATION_NO FROM (  "
			" SELECT PROC_NO, DEV_CODE, DECODE(PRE_SOLUTION_FLAG, '1', '0', '1') C_DIV FROM TPSSM12 WHERE HEAT_NO = '" + tmmsm26["HEAT_NO"].ToString() + "' AND DEV_CODE = '" + dev_code + "' and TREATMENT_COUNTER ='" + tmmsm26["SAME_PROC_NUM"].ToString() + "' ) T1 LEFT JOIN TPSSMD1 T2  "
			" on t1.DEV_CODE = t2.DEV_CODE AND T1.C_DIV = T2.C_DIV");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm26["PROC_NO"] = cmd_inq.GetString(1);
			tmmsm26["STATION_ID"] = cmd_inq.GetString(2);
			tmmsm26["STATION_NO"] = cmd_inq.GetString(3);
		}
		if (tmmsm26["PROC_NO"].ToString() == " "){
			tmmsm26["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["HEAT_NUMBER"].ToString();
		}
		Log::Trace("", "PROC_NO", "PROC_NO = {0}", tmmsm26["PROC_NO"].ToString());
		cmd_inq.Close();


		tmmsm26["DEV_CODE"] = dev_code;
		//计划号 
		tmmsm26["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["ORDER_NUMBER"].ToString();
		//分包号
		tmmsm26["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["SPLIT_INDICATION"].ToString();
		//钢水重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["STEEL_WEIGHT"].ToDecimal() != 0){
			tmmsm26["STEEL_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["STEEL_WEIGHT"].ToDecimal() / 1000;
		}
		//空罐重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() != 0){
			tmmsm26["EMPTY_LADLE_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["EMPTY_LADLE_WEIGHT"].ToDecimal() / 1000;
		}
		//班组
		tmmsm26["PROD_SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["SHIFT_TEAM"].ToString();
		//操作工
		tmmsm26["ASSISTANT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["OPERATOR"].ToString();


		//实际内部钢种
		tmmsm26["ST_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["GRADE_ACT"].ToString();

		//炉次开始时间
		tmmsm26["START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["HEAT_START"].ToString();
		Log::Trace("", "", "START_TIME = {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["HEAT_START"].ToString());
		//炉次结束时间
		tmmsm26["END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["HEAT_END"].ToString();
		Log::Trace("", "", "END_TIME = {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["HEAT_END"].ToString());


		//总氩量
		tmmsm26["AR_SUM_COMSUME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["ARGON_TOT"].ToString();
		//吹氩时间
		tmmsm26["AR_BLOW_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["ARGON_TIME"].ToString();
		if (tmmsm26["AR_BLOW_TIME"].ToString() != "0" || tmmsm26["AR_BLOW_TIME"].ToString() != " "){
			//计算吹氩时间（L2没有传值给我们）
			cmd_cy.SetCommandText(
				" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm26["END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
				" TO_DATE('" + tmmsm26["START_TIME"].ToString() + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
				" from dual ");
			cmd_cy.ExecuteReader();
			if (cmd_cy.Read())
			{
				tmmsm26["AR_BLOW_TIME"] = cmd_cy.GetDecimal(1);
			}
			cmd_cy.Close();
			//tmmsm26["AR_BLOW_TIME"] = tmmsm26["END_TIME"].ToDecimal() - tmmsm26["START_TIME"].ToDecimal()/1000;
			Log::Trace("", "AR_BLOW_TIME", "秒:AR_BLOW_TIME[{0}]", tmmsm26["AR_BLOW_TIME"].ToString());
			if (tmmsm26["AR_BLOW_TIME"].ToDecimal() != 0){
				tmmsm26["AR_BLOW_TIME"] = tmmsm26["AR_BLOW_TIME"].ToDecimal() / 60;
				tmmsm26["AR_BLOW_TIME"] = tmmsm26["AR_BLOW_TIME"].ToDecimal().Round(2);
				Log::Trace("", "AR_BLOW_TIME", "分:AR_BLOW_TIME[{0}]", tmmsm26["AR_BLOW_TIME"].ToString());
			}
		}

		//总氮量
		tmmsm26["EAF_TOTAL_N2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["ARGON_TOT"].ToString();
		//氮
		tmmsm26["EAF_TOTAL_N2_CONS"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["N2_TOT"].ToString();
		//吹氮时间
		tmmsm26["N2_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["N2_TIME"].ToString();
		//钢包号
		tmmsm26["LADLE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["STEEL_LADLE_NO"].ToString();
		//钢包包龄
		tmmsm26["LADLE_AGE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["LADLE_LIFE"].ToString();
		//钢包到达重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["LADLE_ARRIVE_WT"].ToDecimal() != 0){
			tmmsm26["LADLE_ARRIVE_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["LADLE_ARRIVE_WT"].ToDecimal() / 1000;
		}
		//钢包到达时的钢重
		tmmsm26["LADLE_ARRIVE_STEEL_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["LADLE_ARRIVE_STEEL_WT"].ToDecimal() / 1000;
		//钢包离开时间
		tmmsm26["LADLE_DEPART_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["LADLE_DEPART_TIME"].ToString();
		Log::Trace("", "", "LADLE_DEPART_TIME = {0}", bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["LADLE_DEPART_TIME"].ToString());
		//钢包离开时的重量
		if (bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["LADLE_DEPART_WT"].ToDecimal() != 0){
			tmmsm26["LADLE_LEAVE_WT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["LADLE_DEPART_WT"].ToDecimal() / 1000;
		}

		//下一个设备代码
		tmmsm26["NEXT_DEV_CODE"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_LTS"].Rows[0]["NEXT_DEV_CODE"].ToString();

		tmmsm26["REC_CREATOR"] = s.userid;
		tmmsm26["REC_CREATE_TIME"] = datetime;


		bcls_rec->Tables.Clear();
		tmmsm26.MergeTo(bcls_rec->Tables.Add());

		/*if (tmmsm26["PROC_NO"].ToString().Trim() == "")
		{
		sprintf(s.msg, "LTS处理号不能为空");
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
		if (!bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "STATION_ID");
		}

		bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";//厂别

		if (tmmsm26.QueryCount("L2_PROC_NO"))
		{
			Log::Trace("", "", "123456");
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "U";//标记为修改
			doFlag = f_mmsm26_proc(bcls_rec, bcls_ret, conn);
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
			doFlag = f_mmsm26_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//sprintf(s.msg, "%d条记录新增成功！请重新查询！");

		tpcommit(0);//提交
		tpbegin(0, 0);//重新开始

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
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["MESSAGE_CLASS"] = "SJLTS01";//任务池准入条件的比对值
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["UNIT_CODE"] = "H000";//不定机组
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["HEAT_NO"] = heat_no;
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["DEV_CODE"] = dev_code;
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["ST_NO"] = st_no;


		iblk_yq.Tables["DATA_CUSTOM"].Rows.Add();
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["UNIT_CODE"] = "H000";//不定机组
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["HEAT_NO"] = heat_no;
		Log::Trace("", "", "heat_no={0}",heat_no);

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


