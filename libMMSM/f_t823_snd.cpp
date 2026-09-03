/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-16 14:13:28
Description: 智慧质量函数
**************************************************/


#include "stdafx.h"
#include "epex.h"

int f_t82306_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量投料
BM2_FUNCTION_EXPORT
int f_t823_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	CString sql_insert = " ";
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_2a(conn);
	CString station_id = " ";
	CString v_proc_div = " ";
	CModel tmmsm2a("TMMSM2A");
	CString id = " ";
	CString heat_no = " ";
	CString sm_plan_nol2 = " ";

	blkNum = bcls_rec->Tables.IndexOf("T823");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("T823");
	}

	if (!bcls_rec->Tables["T823"].Columns.Contains("DEAL_FLAG"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "DEAL_FLAG");
	}

	if (!bcls_rec->Tables["T823"].Columns.Contains("PROC_NO"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "PROC_NO");
	}
	if (!bcls_rec->Tables["T823"].Columns.Contains("PROC_COUNT"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "PROC_COUNT");
	}

	if (!bcls_rec->Tables["T823"].Columns.Contains("HEAT_NO"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "HEAT_NO");
	}

	if (!bcls_rec->Tables["T823"].Columns.Contains("L2_PROC_NO"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "L2_PROC_NO");
	}
	if (!bcls_rec->Tables["T823"].Columns.Contains("PROD_SEQ_NO"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "PROD_SEQ_NO");
	}

	try
	{
		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		sqlstr=("  "
			" select a.SM_PLAN_NOL2,A.L2_PROC_NO, A.HEAT_NO, a.LADLE_ARRIVE_TIME "
			" from TMMSM31 A "
			" LEFT JOIN TMMSMGY05 C ON A.SM_PLAN_NOL2 = C.SM_PLAN_NOL2 "
			" WHERE A.REC_CREATOR != 'QC' "
			" and A.LADLE_ARRIVE_TIME != ' ' AND C.SEND_T823 != '1' and "
			" to_date(LADLE_ARRIVE_TIME, 'yyyy-MM-dd hh24:MI:SS')< SYSDATE - 4 / 24 and a.HEAT_NO='" + heat_no + "' ");
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			sm_plan_nol2 = cmd_sql.GetString(1);
			cmd_2a.SetCommandText(
				" select *  from TMMSM2A WHERE SM_PLAN_NOL2='"+sm_plan_nol2+"' ");
			cmd_2a.ExecuteReader();
			while (cmd_2a.Read())
			{
				cmd_2a.Fetch(tmmsm2a);
				bcls_rec->Tables["T823"].Rows.Add();
				bcls_rec->Tables["T823"].Rows[0]["DEAL_FLAG"] = "I";
				bcls_rec->Tables["T823"].Rows[0]["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
				bcls_rec->Tables["T823"].Rows[0]["HEAT_NO"] = tmmsm2a["HEAT_NO"];
				bcls_rec->Tables["T823"].Rows[0]["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"];
				bcls_rec->Tables["T823"].Rows[0]["MAT_CODE"] = tmmsm2a["MAT_CODE"];
				doFlag = f_t82306_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_2a.Close();
		}
		cmd_sql.Close();


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