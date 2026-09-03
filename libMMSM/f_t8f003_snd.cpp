/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-27 14:13:28
Description: 投料实绩
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t8f003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString v_prod_shift_no = "";//班次
	CString v_prod_shift_group = "";//班组
	CString sqlstr = " ";
	CString heat_no = " ";
	CString deal_flag = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel tmmsm56("TMMSM56");

	try
	{
		CString epex_number = "T8F003";

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();

			sqlstr = " select * from tmmsm56 where  heat_no=@heat_no";
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Set("heat_no", heat_no);
			cmd_sql.ExecuteReader();
			while (cmd_sql.Read())
			{
				tmmsm56.Reset();
				cmd_sql.Fetch(tmmsm56);
				if (tmmsm56["OUT_STOCK_TIME"].ToString().Trim() != "")
				{
					f_epep_get_shift_group("SMDD", tmmsm56["OUT_STOCK_TIME"].ToString(), v_prod_shift_no, v_prod_shift_group, conn);
				}
				if (epex.Initialize(epex_number) < 0)
				{
					sprintf(s.msg, (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("deal_flag", 0, "I") < 0 ||
					epex.SetValue("data_id", 0, tmmsm56["OUT_STOCK_NO"].ToString()) < 0 ||
					epex.SetValue("work_date", 0, tmmsm56["OUT_STOCK_TIME"].ToString().SubstringNE(0, 8)) < 0 ||
					epex.SetValue("factory_id", 0, "6240") < 0 ||
					epex.SetValue("prod_unit_code", 0, tmmsm56["DEV_CODE"].ToString()) < 0 ||
					epex.SetValue("pono", 0, tmmsm56["PONO"].ToString()) < 0 ||
					epex.SetValue("heat_no", 0, tmmsm56["HEAT_NO"].ToString()) < 0 ||
					epex.SetValue("process_no", 0, tmmsm56["SM_PLAN_NOL2"].ToString()) < 0 ||
					epex.SetValue("st_no", 0, tmmsm56["ST_NO"].ToString()) < 0 ||
					epex.SetValue("mat_code", 0, tmmsm56["MAT_CODE"].ToString()) < 0 ||
					epex.SetValue("weigh_no", 0, tmmsm56["WEIGH_NO"].ToString()) < 0 ||
					epex.SetValue("batch_no", 0, tmmsm56["LOT_NO"].ToString()) < 0 ||
					epex.SetValue("wt", 0, tmmsm56["DEVO_WT"].ToDecimal()) < 0 ||
					epex.SetValue("work_time", 0, tmmsm56["OUT_STOCK_TIME"].ToString()) < 0 ||
					epex.SetValue("prod_shift_no", 0, v_prod_shift_no) < 0 ||
					epex.SetValue("prod_group_no", 0, v_prod_shift_group) < 0 ||
					epex.SetValue("prod_date", 0, tmmsm56["RECV_MAT_TIME"].ToString().SubstringNE(0, 8)) < 0)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (epex.SendTele() < 0)
				{
					Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
					sprintf(s.msg, (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				epex.Uninitialize();
			}
			cmd_sql.Close();
		}

		
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


