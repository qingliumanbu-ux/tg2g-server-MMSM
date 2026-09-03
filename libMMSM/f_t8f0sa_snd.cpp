/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-27 14:13:28
Description: 出钢计划
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t8f0sa_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	/*CModel tmmsm14("TMMSM14");*/

	try
	{
		//初始化
		CString epex_number = "T8F002";
		CString proc_div = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString().Trim();
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取熔炼号
		//tmmsm14["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		//获取处理号
		/*tmmsm14["PROC_NO"] = bcls_rec->Tables[0].Rows[0]["PROC_NO"].ToString().Trim();
		tmmsm14.Query("PROC_NO");
		tmmsm14.TrimOrBlank();*/

		//初始化
		if (epex.Initialize(epex_number) < 0)
		{
			strcpy(s.msg, "初始化电文[" + epex_number + "]失败，原因[" + epex.GetMsg() + "]。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//ID
		/*if (epex.SetValue("ID", 0, epex_number) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
*/
		//处理区分
		//if (epex.SetValue("PROC_DIV", 0, proc_div) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		////计划号
		//if (epex.SetValue("PLANID", 0, tmmsm21["SM_PLAN_NO"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		////厂别代码
		//if (epex.SetValue("FACTORY_DIV", 0, tmmsm21["FACTORY_DIV"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		////制造命令号
		//if (epex.SetValue("PONO", 0, tmmsm21["PONO"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		////熔炼号
		//if (epex.SetValue("HEAT_ID", 0, tmmsm21["HEAT_NO"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		////钢种
		//if (epex.SetValue("ST_NO", 0, tmmsm21["ST_NO"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		////设备号
		//if (epex.SetValue("FACILITY_ID", 0, tmmsm21["STATION_NO"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		////炉次开始时刻
		//if (epex.SetValue("STAR_OF_HEAT", 0, tmmsm21["START_TIME"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		////炉次结束时刻
		//if (epex.SetValue("END_OF_HEAT", 0, tmmsm21["END_TIME"].ToString()) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		////发送时间
		//if (epex.SetValue("SEND_TIME", 0, datetime) < 0)
		//{
		//	sprintf(s.msg, epex.GetMsg());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
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


