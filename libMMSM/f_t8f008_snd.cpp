/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-10 14:13:28
Description: VOD发送
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t8f008_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel tmmsm25("TMMSM25");

	try
	{
		//初始化
		CString epex_number = "T8F008";
		CString proc_div = bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"].ToString().Trim();
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取熔炼号
		tmmsm25["HEAT_NO"] = bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"].ToString().Trim();
		tmmsm25["L2_PROC_NO"] = bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"].ToString().Trim();
		tmmsm25.Query("HEAT_NO,L2_PROC_NO");
		tmmsm25.TrimOrBlank();

		//处理区分
		if (epex.SetValue("proc_div", 0, proc_div) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//厂别区分
		if (epex.SetValue("factory_div", 0, tmmsm25["FACTORY_DIV"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产日期
		if (epex.SetValue("prod_time", 0, tmmsm25["PROD_DATE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//流通炉号L2_PROC_NO
		if (epex.SetValue("heat_no_lt", 0, tmmsm25["HEAT_NO"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//熔炼号
		if (epex.SetValue("heat_no", 0, tmmsm25["L2_PROC_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班次
		if (epex.SetValue("prod_shift_no", 0, tmmsm25["PROD_SHIFT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班组
		if (epex.SetValue("prod_shift_group", 0, tmmsm25["PROD_SHIFT_GROUP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//制造命令号
		if (epex.SetValue("pono", 0, tmmsm25["PONO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//出钢记号
		if (epex.SetValue("st_no", 0, tmmsm25["ST_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//设备站号
		if (epex.SetValue("station_no", 0, tmmsm25["STATION_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢包号
		if (epex.SetValue("ladle_no", 0, tmmsm25["LADLE_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//最终钢水量--
		/*if (epex.SetValue("fin_ladle_w_l", 0, tmmsm25[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//底吹氩气耗量(nm3)
		/*if (epex.SetValue("bttm_ar_comsume", 0, tmmsm25[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//底吹氮气耗量(nm3)
		/*if (epex.SetValue("bttm_n_comsume", 0, tmmsm25[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//蒸汽消耗量(nm3)--不确定
		if (epex.SetValue("steam_comsume", 0, tmmsm25["STEAM_TOT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//氧气总耗量(nm3)
		if (epex.SetValue("o2_sum_comsume", 0, tmmsm25["TOTAL_O2_CONS"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//作业开始时刻--不确定
		if (epex.SetValue("start_time", 0, tmmsm25["START_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//作业结束时刻
		if (epex.SetValue("end_time", 0, tmmsm25["END_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//处理时间(min)
		/*if (epex.SetValue("proc_time", 0, tmmsm25[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//高作业时间(min)
		/*if (epex.SetValue("sum_time", 0, tmmsm25[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//高真空时间(min)
		if (epex.SetValue("high_vacuum_time", 0, tmmsm25["VACUUM_PUMP_DURATION"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//吹氧时间(min)
		if (epex.SetValue("o2_blow_time", 0, tmmsm25["O2_BLOW_DURATION"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

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


