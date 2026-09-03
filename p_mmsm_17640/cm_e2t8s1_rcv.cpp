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
/// DES生产实绩表电文
/// 
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


int f_mmsm14_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE_TELE(cm_e2t8s1_rcv)

int f_cm_e2t8s1_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd_inq_test(conn);
	CDbCommand cmd_inq_code(conn);
	CModel tmmsm14("TMMSM14");
	EIClass tmmsm14_back;
	CString dev_code = "";
	CString sm_plan_no2 = "";//炼钢计划号

	//加入函数的表

	blkNum = tmmsm14_back.Tables.IndexOf("PARA");
	if (blkNum < 0)
	{
		tmmsm14_back.Tables.Add();
		tmmsm14_back.Tables.Add("PARA");
	}
	if (!tmmsm14_back.Tables["PARA"].Columns.Contains("PROC_DIV"))
	{
		tmmsm14_back.Tables["PARA"].Columns.Add(DT_STRING, "PROC_DIV");
	}
	if (!tmmsm14_back.Tables["PARA"].Columns.Contains("PRACT_COLL_MODE"))
	{
		tmmsm14_back.Tables["PARA"].Columns.Add(DT_STRING, "PRACT_COLL_MODE");
	}
	if (!tmmsm14_back.Tables["PARA"].Columns.Contains("FACTORY_DIV"))
	{
		tmmsm14_back.Tables["PARA"].Columns.Add(DT_STRING, "FACTORY_DIV");
	}
	//tmmsm14_back.Tables[0].Rows.Add();
	tmmsm14_back.Tables[0].Columns.Add(tmmsm14);
	//tmmsm14_back.Tables[0].Rows.Add();
	tmmsm14_back.Tables["PARA"].Rows.Add();

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		tmmsm14.MergeFrom(bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]);
		///工号
		dev_code = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["AGGREGATE_NAME"].ToString();
		sm_plan_no2 = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["ORDER_NUMBER"].ToString();
		tmmsm14["ID_SJ"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["ID"].ToString();
		//同工位处理次数
		tmmsm14["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["TREATMENT_COUNTER"].ToString();
		Log::Trace("", "dev_code", "dev_code = {0}", dev_code);
		//根据计划号获取熔炼号和制造命令号
		cmd_inq_code.SetCommandText(" SELECT HEAT_NO,PONO,SM_PLAN_NO FROM TPSSM11 WHERE SM_PLAN_NOL2_TEST ='" + sm_plan_no2 + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm14["HEAT_NO"] = cmd_inq_code.GetString(1);
			tmmsm14["PONO"] = cmd_inq_code.GetString(2);
			tmmsm14["SM_PLAN_NO"] = cmd_inq_code.GetString(3);
		}
		cmd_inq_code.Close();

		//根据工位获取设备类型和站号
		Log::Trace("", "dev_code", "dev_code = {0}", dev_code);
		//查询设备站号和设备代码
		cmd_inq_code.SetCommandText(" select STATION_ID,STATION_NO  from TPSSMD1 where DEV_CODE=@DEV_CODE ");
		cmd_inq_code.Parameters.Clear();
		cmd_inq_code.Parameters.Set("DEV_CODE", dev_code);
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm14["STATION_ID"] = cmd_inq_code.GetString(1);
			tmmsm14["STATION_NO"] = cmd_inq_code.GetString(2);

		}
		cmd_inq_code.Close();
		tmmsm14["DEV_CODE"] = dev_code;

		
		//处理号
		tmmsm14["PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["HEAT_NUMBER"].ToString();
		tmmsm14["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["HEAT_NUMBER"].ToString();
		//计划号
		tmmsm14["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["ORDER_NUMBER"].ToString();
		//分包号SPLIT_INDICATION
		//内部钢种
		tmmsm14["ST_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["STEELGRADE"].ToString();
		//PROC_COUNT INT_MES_PROD_SUMMARY_DES.TMSEQCOUNT
		tmmsm14["PROC_COUNT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["TMSEQCOUNT"].ToString();
		//铁水罐号
		tmmsm14["IRON_LADLE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["HMLADLENO"].ToString();

		//铁水罐重LADLE_NET_WEIGHT  单位为吨 
		tmmsm14["LADLE_NET_WEIGHT"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["LADLE_NET_WEIGHT"].ToDecimal()/1000 ;
		
		//初始硫含量
		tmmsm14["INIT_S"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["INITIALSULPHUR"].ToString();
		//最终硫含量
		tmmsm14["AFT_S"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["LASTSULPHUR"].ToString();
		//初始温度
		tmmsm14["DE_S_PREV_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["INITTEMP"].ToString();
		//初始温度时刻
		tmmsm14["DE_S_PREV_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["DTINITTEMP"].ToString();
		//处理后温度
		tmmsm14["DE_S_REP_TEMP"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["AFTER_TM_TEMP"].ToString();
		//处理后温度时刻
		tmmsm14["DE_S_REP_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["DT_AFTER_TM_TEMP"].ToString();
		//氧枪号
		tmmsm14["O2_LANCE_NO"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["LANCEID"].ToString();
		//处理开始时刻
		tmmsm14["START_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["DTTMSTART"].ToString();
		//处理结束时刻
		tmmsm14["END_TIME"] = bcls_rec->Tables["INT_MES_PROD_SUMMARY_DES"].Rows[0]["DTTMFINISH"].ToString();
		//信息计数MSGCOUNT
		//时间戳TIME_STAMPS
		tmmsm14["REC_CREATOR"] = s.userid;
		tmmsm14["REC_CREATE_TIME"] = datetime;

		//未明确？？
		//喷吹类型？？
		tmmsm14.MergeTo(tmmsm14_back.Tables[0],false);
		//处理号会在 tmmsm14_proc 函数中做处理
		if (tmmsm14["PROC_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "处理号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Info("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm14["PROC_NO"].ToString());
		if (tmmsm14.QueryCount("PROC_NO"))
		{
			Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]","U");
			tmmsm14_back.Tables["PARA"].Rows[0]["PROC_DIV"] = "U";
			tmmsm14_back.Tables["PARA"].Rows[0]["PRACT_COLL_MODE"] = "0";
			tmmsm14_back.Tables["PARA"].Rows[0]["FACTORY_DIV"] = "LG1";
			doFlag = f_mmsm14_proc(&tmmsm14_back, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//tmmsm14.Update("PROC_NO");
		}
		else
		{
			Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "I");
			tmmsm14_back.Tables["PARA"].Rows[0]["PROC_DIV"] = "I";
			tmmsm14_back.Tables["PARA"].Rows[0]["PRACT_COLL_MODE"] = "0";
			tmmsm14_back.Tables["PARA"].Rows[0]["FACTORY_DIV"] = "LG1";
			doFlag = f_mmsm14_proc(&tmmsm14_back, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/*tmmsm14.TrimOrBlank();
			tmmsm14.Insert();*/
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


