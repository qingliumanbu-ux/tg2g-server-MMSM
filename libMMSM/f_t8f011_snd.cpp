/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-10 14:13:28
Description: 连铸炉次实绩
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t8f011_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel tmmsm31("TMMSM31");

	try
	{
		//初始化
		CString epex_number = "T8F011";
		CString proc_div = bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"].ToString().Trim();
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取熔炼号
		tmmsm31["HEAT_NO"] = bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"].ToString().Trim();
		//获取处理号
		tmmsm31["L2_PROC_NO"] = bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"].ToString().Trim();
		tmmsm31.Query("HEAT_NO,L2_PROC_NO");
		tmmsm31.TrimOrBlank();

		//处理区分
		if (epex.SetValue("proc_div", 0, proc_div) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//厂别区分
		if (epex.SetValue("factory_div", 0, tmmsm31["FACTORY_DIV"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产日期
		if (epex.SetValue("prod_time", 0, tmmsm31["PROD_DATE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//流通炉号
		if (epex.SetValue("heat_no_lt", 0, tmmsm31["HEAT_NO"].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}

		//熔炼号L2_PROC_NO
		if (epex.SetValue("heat_no", 0, tmmsm31["L2_PROC_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班次
		if (epex.SetValue("prod_shift_no", 0, tmmsm31["PROD_SHIFT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产班组
		if (epex.SetValue("prod_shift_group", 0, tmmsm31["PROD_SHIFT_GROUP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//制造命令号
		if (epex.SetValue("pono", 0, tmmsm31["PONO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//出钢记号
		if (epex.SetValue("st_no", 0, tmmsm31["ST_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//设备站号
		if (epex.SetValue("station_no", 0, tmmsm31["STATION_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产责任者
		if (epex.SetValue("prod_maker", 0, tmmsm31["ASSISTANT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//连连浇号(CAST号)
		if (epex.SetValue("cast_no", 0, tmmsm31["CAST_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//CAST分割号
		if (epex.SetValue("cast_div_no", 0, tmmsm31["CAST_DIV_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//切断板坯块数
		if (epex.SetValue("cut_slab_num", 0, tmmsm31["CUT_SLAB_NUM"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢包号
		if (epex.SetValue("ladle_no", 0, tmmsm31["LADLE_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢(铁)包到达时刻
		if (epex.SetValue("ladle_arrive_time", 0, tmmsm31["LADLE_ARRIVE_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢(铁)包离开时刻
		if (epex.SetValue("ladle_leave_time", 0, tmmsm31["LADLE_LEAVE_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢包打开时刻
		if (epex.SetValue("open_time", 0, tmmsm31["LADLE_OPEN_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//最终切断时间
		if (epex.SetValue("fin_cut_time", 0, tmmsm31["FIN_CUT_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢水测温时刻
		if (epex.SetValue("steel_temp_time", 0, tmmsm31["LADLE_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢水温度
		if (epex.SetValue("steel_temp", 0, tmmsm31["LADLE_TEMP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//中间包号
		if (epex.SetValue("td_no", 0, tmmsm31["TD_NO_1"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢包到达重量
		if (epex.SetValue("ladle_weight_arr", 0, tmmsm31["LADLE_ARRIVE_WT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//钢包离开重量
		if (epex.SetValue("ladle_weight_dep", 0, tmmsm31["LADLE_LEAVE_WT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//浇铸钢水重量
		/*if (epex.SetValue("cast_steel_wt", 0, tmmsm31[""].ToString()) < 0)
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


