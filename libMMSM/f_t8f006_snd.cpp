/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-10 14:13:28
Description: RH实绩
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t8f006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel tmmsm23("TMMSM23");

	try
	{
		//初始化
		CString epex_number = "T8F006";
		CString proc_div = bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"].ToString().Trim();
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取熔炼号
		tmmsm23["HEAT_NO"] = bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"].ToString().Trim();
		tmmsm23["PROC_NO"] = bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"].ToString().Trim();
		tmmsm23["L2_PROC_NO"] = bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"].ToString().Trim();
		tmmsm23.Query("HEAT_NO,L2_PROC_NO");
		tmmsm23.TrimOrBlank();

		//处理区分
		if (epex.SetValue("proc_div", 0, proc_div) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//厂别区分
		if (epex.SetValue("factory_div", 0, tmmsm23["FACTORY_DIV"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产日期
		if (epex.SetValue("prod_time", 0, tmmsm23["PROD_DATE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//流通炉号L2_PROC_NO
		if (epex.SetValue("heat_no_lt", 0, tmmsm23["HEAT_NO"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//熔炼号
		if (epex.SetValue("heat_no", 0, tmmsm23["L2_PROC_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班次
		if (epex.SetValue("prod_shift_no", 0, tmmsm23["PROD_SHIFT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班组
		if (epex.SetValue("prod_shift_group", 0, tmmsm23["PROD_SHIFT_GROUP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//制造命令号
		if (epex.SetValue("pono", 0, tmmsm23["PONO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//出钢记号
		if (epex.SetValue("st_no", 0, tmmsm23["ST_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//设备站号
		if (epex.SetValue("station_no", 0, tmmsm23["STATION_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//最终钢水量
		/*if (epex.SetValue("fin_ladle_w_l", 0, tmmsm23[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//钢包到达时刻
		/*if (epex.SetValue("ladle_arrive_time", 0, tmmsm23[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//处理开始时刻
		if (epex.SetValue("proc_start_t", 0, tmmsm23["START_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//处理结束时刻
		if (epex.SetValue("proc_end_t", 0, tmmsm23["END_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//吹氩量
		if (epex.SetValue("ARGON_TOT", 0, tmmsm23["AR_SUM_COMSUME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//吹氮量
		if (epex.SetValue("NITROGEN_TOT", 0, tmmsm23["N_SUM_COMSUME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//蒸汽消耗量
		if (epex.SetValue("steam_comsume", 0, tmmsm23["STEAM_TOT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//氧气消耗量
		if (epex.SetValue("o2_sum_comsume", 0, tmmsm23["O2_SUM_COMSUME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//工业水量
		if (epex.SetValue("INDUSTRYWATER_TOT", 0, tmmsm23["WATER_USE_QTY"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//饮用水量
		if (epex.SetValue("POTABLEWATER_TOT", 0, tmmsm23["POTABLEWATER_TOT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//到站温度--不确定
		if (epex.SetValue("in_temp", 0, tmmsm23["START_STEEL_TEMP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//出站温度--不确定
		if (epex.SetValue("fin_temp", 0, tmmsm23["END_STEEL_TEMP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//总作业时间
		/*if (epex.SetValue("sum_time", 0, tmmsm23[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//最终氢含量
		/*if (epex.SetValue("h2", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//高真空时间
		if (epex.SetValue("high_vacuum_time", 0, tmmsm23["DEEP_VAC_DUR"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//软吹时间
		/*if (epex.SetValue("soft_stiring_time", 0, tmmsm23[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		if (epex.SendTele() < 0)
		{
			Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		epex.Uninitialize();
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


