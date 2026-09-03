/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   XMY
Version:    1.0
Date:     2024-04-25
Description: 过钢量重新下发
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsmggl_xf)

int f_mmsmggl_xf(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	/* 业务变量 */
	CString	datetime("");

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString v_heat_no = " ";
	CString l2_proc_no = " ";
	CString dev_code = " ";
	int   blkNum;
	CModel tmmsmgy06("TMMSMGY06");
	CModel hmmsmgy06("HMMSMGY06");
	CDbCommand cmd_1(conn);
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd_inq(conn);

	try
	{
		//调用发送电文
		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMSND");
		}

		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
		}

		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}
		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("DEV_CODE"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "DEV_CODE");
		}
		if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("XF_MIN"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "XF_MIN");
		}
		bcls_rec->Tables["MMSMSND"].Rows.Add();
		Log::Trace("", "v_heat_no", "v_heat_no = {0}]", v_heat_no);
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			v_heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();
			Log::Trace("", "v_heat_no", "v_heat_no = {0}]", v_heat_no);
			cmd_inq.SetCommandText(" select L2_PROC_NO,DEV_CODE from TMMSMGY06 where  HEAT_NO = '" + v_heat_no + "' and AFFIRM_FLAG='1' "
				" ");
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				l2_proc_no = cmd_inq.GetString(1);
				dev_code = cmd_inq.GetString(2);
				tmmsmgy06["HEAT_NO"] = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();
				tmmsmgy06["L2_PROC_NO"] = l2_proc_no;
				tmmsmgy06["DEV_CODE"] = dev_code;
				bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM21S";
				bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
				bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();
				bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = l2_proc_no;
				bcls_rec->Tables["MMSMSND"].Rows[0]["DEV_CODE"] = dev_code;
				bcls_rec->Tables["MMSMSND"].Rows[0]["XF_MIN"] = "1";
				doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE");
				tmmsmgy06.TrimOrBlank();
				hmmsmgy06.CopyFrom(tmmsmgy06);
				hmmsmgy06["FIN_CONFM_FLAG"] = "1";
				//FIN_CONFM_FLAG 1表示最后发送的是-，2表示最后发送的是正的
				cmd_1.SetCommandText(" SELECT LPAD(TO_CHAR(HMMSMGY06_ID.NEXTVAL), 9, '0') AS ID FROM DUAl ");
				cmd_1.ExecuteReader();
				if (cmd_1.Read())
				{
					hmmsmgy06["ID"] = cmd_1.GetDecimal(1);
				}
				cmd_1.Close();
				hmmsmgy06.Insert();
				bcls_rec->Tables["MMSMSND"].Rows[i]["XF_MIN"] = "2";
				doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", "datetime", "datetime = {0}]", datetime);
				tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE");
				tmmsmgy06.TrimOrBlank();
				hmmsmgy06.CopyFrom(tmmsmgy06);
				hmmsmgy06["FIN_CONFM_FLAG"] = "2";
				cmd_1.SetCommandText(" SELECT LPAD(TO_CHAR(HMMSMGY06_ID.NEXTVAL), 9, '0') AS ID FROM DUAl ");
				cmd_1.ExecuteReader();
				if (cmd_1.Read())
				{
					hmmsmgy06["ID"] = cmd_1.GetDecimal(1);
				}
				cmd_1.Close();
				//FIN_CONFM_FLAG 1表示最后发送的是-，2表示最后发送的是正的
				hmmsmgy06.Insert();
				tmmsmgy06["TC_SEND_FLAG"] = "2";
				tmmsmgy06["DATI_MSG_SENT"] = datetime;
				tmmsmgy06.Update("TC_SEND_FLAG,DATI_MSG_SENT", "HEAT_NO,L2_PROC_NO,DEV_CODE");
			}
			
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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

}
