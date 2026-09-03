/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-10 14:13:28
Description: 能源AOD发送
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t8f007_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel tmmsm27("TMMSM27");

	try
	{
		//初始化
		CString epex_number = "T8F007";
		CString proc_div = bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"].ToString().Trim();
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取熔炼号
		tmmsm27["HEAT_NO"] = bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"].ToString();
		//获取处理号
		tmmsm27["PROC_NO"] = bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"].ToString();
		tmmsm27["L2_PROC_NO"] = bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"].ToString();
		tmmsm27.Query("HEAT_NO,PROC_NO,L2_PROC_NO");
		tmmsm27.TrimOrBlank();


		//处理区分
		if (epex.SetValue("proc_div", 0, proc_div) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//厂别区分
		if (epex.SetValue("factory_div", 0, tmmsm27["FACTORY_DIV"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产日期
		if (epex.SetValue("prod_time", 0, tmmsm27["PROD_DATE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//流通炉号L2_PROC_NO
		if (epex.SetValue("heat_no_lt", 0, tmmsm27["HEAT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//熔炼号
		if (epex.SetValue("heat_no", 0, tmmsm27["L2_PROC_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班次
		if (epex.SetValue("prod_shift_no", 0, tmmsm27["PROD_SHIFT_NO"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班组
		if (epex.SetValue("prod_shift_group", 0, tmmsm27["PROD_SHIFT_GROUP"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//制造命令号
		if (epex.SetValue("pono", 0, tmmsm27["PONO"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//出钢记号
		if (epex.SetValue("st_no", 0, tmmsm27["ST_NO"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//设备站号
		if (epex.SetValue("station_no", 0, tmmsm27["STATION_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢包号
		if (epex.SetValue("ladle_no", 0, tmmsm27["LADLE_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢水重量
		if (epex.SetValue("out_steel_wt", 0, tmmsm27["LADLE_PRE_LIQUID_WT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//环流氩气耗量(nm3)
		if (epex.SetValue("cir_ar_comsume", 0, tmmsm27["TOTAL_AR_CONS"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//环流氮气耗量(nm3)
		if (epex.SetValue("cir_n_comsume", 0, tmmsm27["TOTAL_N2_CONS"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//底吹氩气耗量(nm3)
		if (epex.SetValue("bttm_ar_comsume", 0, tmmsm27["AR_SUM_COMSUME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//底吹氮气耗量(nm3)
		if (epex.SetValue("bttm_n_comsume", 0, tmmsm27["N_SUM_COMSUME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//氧气总耗量(nm3)
		if (epex.SetValue("o2_sum_comsume", 0, tmmsm27["OXYGEN_FINAL"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//作业开始时刻
		if (epex.SetValue("start_time", 0, tmmsm27["START_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//作业结束时刻
		if (epex.SetValue("end_time", 0, tmmsm27["END_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//处理时间(min)
		/*if (epex.SetValue("proc_time", 0, tmmsm27[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//总作业时间(min)
		/*if (epex.SetValue("sum_time", 0, tmmsm27[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//吹氧时间(min)
		/*if (epex.SetValue("o2_blow_time", 0, tmmsm27["BLOW_DURATION"].ToString()) < 0)
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


