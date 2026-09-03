/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-10 14:13:28
Description: 能源动态转炉发送
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t8f004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel tmmsm21("TMMSM21");

	try
	{
		//初始化
		CString epex_number = "T8F004";
		CString proc_div = bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"].ToString().Trim();
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取熔炼号
		tmmsm21["HEAT_NO"] = bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"].ToString().Trim();
		tmmsm21["L2_PROC_NO"] = bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"].ToString().Trim();
		Log::Info("", __FUNCTION__, "L2_PROC_NO-__=[{0}]", tmmsm21["L2_PROC_NO"].ToString());
		tmmsm21.Query("L2_PROC_NO");
		tmmsm21.TrimOrBlank();


		//处理区分
		if (epex.SetValue("proc_div", 0, proc_div) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//厂别区分
		if (epex.SetValue("factory_div", 0, tmmsm21["FACTORY_DIV"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产日期
		if (epex.SetValue("prod_time", 0, tmmsm21["PROD_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//流通炉号
		if (epex.SetValue("heat_no_lt", 0, tmmsm21["HEAT_NO"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//熔炼号
		if (epex.SetValue("heat_no", 0, tmmsm21["L2_PROC_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班次
		if (epex.SetValue("prod_shift_no", 0, tmmsm21["SHIFT_GROUP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班组
		if (epex.SetValue("prod_shift_group", 0, tmmsm21["PROD_SHIFT_GROUP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//制造命令号
		if (epex.SetValue("pono", 0, tmmsm21["PONO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//出钢记号
		if (epex.SetValue("st_no", 0, tmmsm21["ST_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//设备站号
		if (epex.SetValue("station_no", 0, tmmsm21["STATION_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//废钢投入量
		if (epex.SetValue("scrap_weight", 0, tmmsm21["SCRAP_WEIGHT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//铁水量
		if (epex.SetValue("moltiron_wt", 0, tmmsm21["MOLTIRON_WT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//铁包号
		if (epex.SetValue("iron_ladle_no", 0, tmmsm21["IRON_LADLE_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢包号
		if (epex.SetValue("ladle_no", 0, tmmsm21["LADLE_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢水重量
		if (epex.SetValue("out_steel_wt", 0, tmmsm21["MOLTIRON_WT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//炉次开始时刻
		if (epex.SetValue("start_of_heat", 0, tmmsm21["START_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//出钢开始时刻
		if (epex.SetValue("start_time", 0, tmmsm21["TAP_START_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//出钢结束时刻
		if (epex.SetValue("end_time", 0, tmmsm21["TAP_END_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//炉次结束时刻
		if (epex.SetValue("end_of_heat", 0, tmmsm21["END_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//出钢持续时间
		/*if (epex.SetValue("steel_time", 0, tmmsm21[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//总氧气消耗量
		if (epex.SetValue("o2_sum_comsume", 0, tmmsm21["OXYGEN_FINAL"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//底吹氩气总消耗量
		if (epex.SetValue("bttm_ar_comsume_sum", 0, tmmsm21["AR_SUM_COMSUME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//底吹氮气总消耗量
		if (epex.SetValue("bttm_n_comsume_sum", 0, tmmsm21["N_SUM_COMSUME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//溅渣开始时刻
		if (epex.SetValue("slag_start_time", 0, tmmsm21["SLAG_START_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//溅渣结束时刻
		if (epex.SetValue("slag_end_time", 0, tmmsm21["SLAG_END_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//总溅渣时间
		/*if (epex.SetValue("spatter_slag_time", 0, tmmsm21[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
*/
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


