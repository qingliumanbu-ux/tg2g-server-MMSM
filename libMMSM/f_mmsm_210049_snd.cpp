/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-13
Description: 炼钢倒罐实绩处理
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h" 

BM2_FUNCTION_EXPORT
int f_mmsm_210049_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	CString v_proc_div = "";
	CDbCommand cmd_sql(conn);
	CModel tmmsmgy06("TMMSMGY06");
	CModel hmmsmgy06("TMMSMGY06_SED");
	CModel tmmsmgy05("TMMSMGY05");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd_inq(conn);
	CString table_name = " ";
	CDbCommand cmd_inqsj(conn);
	CString remark_1 = " ";
	CString remark_2 = " ";
	CString remark_3 = " ";
	/* 创建电文处理对象 */
	EPEX epex(&s, conn);

	try{
		for (int i = 0; i < bcls_rec->Tables["MMSMSND"].Rows.get_Count(); i++)
		{
			if (epex.Initialize("210049") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsmgy05["HEAT_NO"] = bcls_rec->Tables["MMSMSND"].Rows[i]["HEAT_NO"].ToString().Trim();
			tmmsmgy06.MergeFrom(bcls_rec->Tables["MMSMSND"].Rows[i]);
			hmmsmgy06.MergeFrom(bcls_rec->Tables["MMSMSND"].Rows[i]);
			Log::Trace("", "like", "HEAT_NO = {0}]", tmmsmgy06["HEAT_NO"].ToString());
			Log::Trace("", "DEV_CODE", "DEV_CODE = {0}]", tmmsmgy06["DEV_CODE"].ToString());
			Log::Trace("", "DEV_CODE", "DEV_CODE = {0}]", tmmsmgy06["DEV_CODE"].ToString());
			if (bcls_rec->Tables["MMSMSND"].Columns.Contains("DEAL_FLAG")){
				CString deal_flag = bcls_rec->Tables["MMSMSND"].Rows[0]["DEAL_FLAG"].ToString().Trim();
				if (deal_flag != "D"){
					hmmsmgy06["REC_CREATE_TIME"] = datetime;
					hmmsmgy06.Insert();
				}
				else{
					tmmsmgy06["DURATION_TIME"] = 0-tmmsmgy06["DURATION_TIME"].ToDecimal();
				}
			}
			else{
				hmmsmgy06["REC_CREATE_TIME"] = datetime;
				hmmsmgy06.Insert();
			}
			CString xf_min = " ";
			if (bcls_rec->Tables["MMSMSND"].Columns.Contains("XF_MIN")){
				xf_min = bcls_rec->Tables["MMSMSND"].Rows[i]["XF_MIN"].ToString();
			}
			Log::Trace("", "xf_min", "xf_min = {0}]", xf_min);
			if (epex.SetValue("ZCHO_TMMSMCT", 0, tmmsmgy06) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Trace("", "like", "like = {0}]", __LINE__);
			//电文号
			if (epex.SetValue("bapiheader", "msgtype", 0, "210049") < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", "like", "like = {0}]", __LINE__);
			if (v_proc_div == "U" || v_proc_div == "I"){
				v_proc_div = "1";
			}
			else if (v_proc_div == "D"){
				v_proc_div = "2";
			}
			//ZCHO_TMMSMCT.PONO
			if (epex.SetValue("ZCHO_TMMSMCT", "PONO", 0, tmmsmgy06["PONO"].ToString()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//ZCHO_TMMSMCT.PROD_SHIFT_NO
			if (epex.SetValue("ZCHO_TMMSMCT", "PROD_SHIFT_NO", 0, tmmsmgy06["PROD_SHIFT_NO"].ToString()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", "like", "like = {0}]", __LINE__);
			//APP_THROW_AI_DATE

			tmmsmgy05.Query("HEAT_NO");
			tmmsmgy05.TrimOrBlank();
			CString recv_mat_time = " ";
			if (tmmsmgy05["RECV_MAT_TIME"].ToString() != " "){
				recv_mat_time = tmmsmgy05["RECV_MAT_TIME"].ToString().Substring(0, 8);
			}
			if (epex.SetValue("ZCHO_TMMSMCT", "APP_THROW_AI_DATE", 0, recv_mat_time) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
			}
			Log::Trace("", "like", "like = {0}]", __LINE__);

			//生产日期 ZCHO_TMMSMCT.PROD_DATE
			Log::Trace("", "START_TIME", "START_TIME = {0}]", tmmsmgy06["START_TIME"].ToString());
			if (tmmsmgy06["PROD_DATE"].ToString().Trim() == ""&&tmmsmgy06["START_TIME"].ToString()!=" "){
				tmmsmgy06["PROD_DATE"] = tmmsmgy06["START_TIME"].ToString().Substring(0,8);
				Log::Trace("", "PROD_DATE", "PROD_DATE = {0}]", tmmsmgy06["PROD_DATE"].ToString());
			}

			if (epex.SetValue("ZCHO_TMMSMCT", "PROD_DATE", 0, tmmsmgy06["PROD_DATE"].ToString()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//机组代码（工位） ZCHO_TMMSMCT.PROC_UNIT
			if (epex.SetValue("ZCHO_TMMSMCT", "PROC_UNIT", 0, tmmsmgy06["DEV_CODE"].ToString()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
			}
			//炉次开始时刻 ZCHO_TMMSMCT.START_OF_HEAT
			if (epex.SetValue("ZCHO_TMMSMCT", "START_OF_HEAT", 0, tmmsmgy06["START_TIME"].ToString()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//炉次结束时刻 ZCHO_TMMSMCT.END_OF_HEAT
			if (epex.SetValue("ZCHO_TMMSMCT", "END_OF_HEAT", 0, tmmsmgy06["END_TIME"].ToString()) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsmgy06["DEV_CODE"].ToString().SubstringNE(0, 1) == "F"){
				table_name = "TMMSM24";
				cmd_inqsj.SetCommandText(" select POWER_CONSUME,AR_SUM_COMSUME from TMMSM24 WHERE HEAT_NO='" + tmmsmgy06["HEAT_NO"].ToString() + "' ");
				cmd_inqsj.ExecuteReader();
				if (cmd_inqsj.Read())
				{
					//如果是LF备注1则为电，2为吹氩
					remark_1 = cmd_inqsj.GetString(1);
					remark_2 = cmd_inqsj.GetString(2);
				}
				cmd_inqsj.Close();
			}
			else if (tmmsmgy06["DEV_CODE"].ToString().SubstringNE(0, 1) == "Z"){
				table_name = "TMMSM19";
				cmd_inqsj.SetCommandText(" select POWER_CONSUME from TMMSM19 WHERE HEAT_NO='" + tmmsmgy06["HEAT_NO"].ToString() + "' ");
				cmd_inqsj.ExecuteReader();
				if (cmd_inqsj.Read())
				{
					//如果是电炉备注1则为电
					remark_1 = cmd_inqsj.GetString(1);
				}
				cmd_inqsj.Close();
			}
			else if (tmmsmgy06["DEV_CODE"].ToString().SubstringNE(0, 1) == "E"){
				table_name = "TMMSM20";
				cmd_inqsj.SetCommandText(" select POWER_CONSUME,OXYGEN_FINAL,TOP_N2_CONS from TMMSM20 WHERE HEAT_NO='" + tmmsmgy06["HEAT_NO"].ToString() + "' ");
				cmd_inqsj.ExecuteReader();
				if (cmd_inqsj.Read())
				{
					//如果是电炉备注1则为吹氧量，2为电量，3为顶吹氮量
					remark_1 = cmd_inqsj.GetString(1);
					remark_2 = cmd_inqsj.GetString(2);
					remark_3 = cmd_inqsj.GetString(3);
				}
				cmd_inqsj.Close();
			}
			else if (tmmsmgy06["DEV_CODE"].ToString().SubstringNE(0, 1) == "B"){
				table_name = "TMMSM21";
				cmd_inqsj.SetCommandText(" select OXYGEN_FINAL,TOTAL_N2_CONS,AR_SUM_COMSUME from TMMSM21 WHERE HEAT_NO='" + tmmsmgy06["HEAT_NO"].ToString() + "' ");
				cmd_inqsj.ExecuteReader();
				if (cmd_inqsj.Read())
				{
					//如果是转炉备注1则为吹氧量，2为顶吹氮量，3为底吹氩量
					remark_1 = cmd_inqsj.GetString(1);
					remark_2 = cmd_inqsj.GetString(2);
					remark_3 = cmd_inqsj.GetString(3);
				}
				cmd_inqsj.Close();
			}
			else if (tmmsmgy06["DEV_CODE"].ToString().SubstringNE(0, 1) == "R"){
				table_name = "TMMSM23";
				cmd_inqsj.SetCommandText(" select O2_SUM_COMSUME,N_SUM_COMSUME,AR_SUM_COMSUME from TMMSM23 WHERE HEAT_NO='" + tmmsmgy06["HEAT_NO"].ToString() + "' ");
				cmd_inqsj.ExecuteReader();
				if (cmd_inqsj.Read())
				{
					//如果是VOD备注1则为吹氧量，2为吹氮量,3为吹氩量
					remark_1 = cmd_inqsj.GetString(1);
					remark_2 = cmd_inqsj.GetString(2);
					remark_3 = cmd_inqsj.GetString(3);
				}
				cmd_inqsj.Close();
			}
			else if (tmmsmgy06["DEV_CODE"].ToString().SubstringNE(0, 1) == "V"){
				table_name = "TMMSM25";
				cmd_inqsj.SetCommandText(" select OXYGEN_FINAL,AR_SUM_COMSUME from TMMSM25 WHERE HEAT_NO='" + tmmsmgy06["HEAT_NO"].ToString() + "' ");
				cmd_inqsj.ExecuteReader();
				if (cmd_inqsj.Read())
				{
					//如果是VOD备注1则为吹氧量，2为吹氩量
					remark_1 = cmd_inqsj.GetString(1);
					remark_2 = cmd_inqsj.GetString(2);
				}
				cmd_inqsj.Close();
			}
			else if (tmmsmgy06["DEV_CODE"].ToString().SubstringNE(0, 1) == "S"){
				table_name = "TMMSM26";
			}
			else if (tmmsmgy06["DEV_CODE"].ToString().SubstringNE(0, 1) == "A"){
				table_name = "TMMSM27";
				cmd_inqsj.SetCommandText(" select OXYGEN_FINAL,NITROGEN_TOT,TOTAL_AR_CONS from TMMSM27 WHERE HEAT_NO='" + tmmsmgy06["HEAT_NO"].ToString() + "' ");
				cmd_inqsj.ExecuteReader();
				if (cmd_inqsj.Read())
				{
					//如果是AOD备注1则为吹氧量，2为顶底吹氮量，3为顶底吹氩量（m3/t）
					remark_1 = cmd_inqsj.GetString(1);
					remark_2 = cmd_inqsj.GetString(2);
					remark_3 = cmd_inqsj.GetString(3);
				}
				cmd_inqsj.Close();
			}
			else if (tmmsmgy06["DEV_CODE"].ToString().SubstringNE(0, 1) == "C"){
				table_name = "TMMSM31";
			}
			Log::Trace("", "table_name", "table_name = {0}]", table_name);
		    //ZCHO_TMMSMCT.REMARK_1
			if (epex.SetValue("ZCHO_TMMSMCT", "REMARK_1", 0, remark_1) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("ZCHO_TMMSMCT", "REMARK_2", 0, remark_2) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("ZCHO_TMMSMCT", "REMARK_3", 0, remark_3) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", "like", "like = {0}]", __LINE__);
			if (xf_min == "1"){
				CDecimal duration_time = 0;
				duration_time = tmmsmgy06["DURATION_TIME"].ToDecimal() - tmmsmgy06["DURATION_TIME"].ToDecimal() - tmmsmgy06["DURATION_TIME"].ToDecimal();
				Log::Trace("", "duration_time", "duration_time = {0}]", duration_time);
				//持续时间1 ZCHO_TMMSMCT.DURATION_TIME
				if (epex.SetValue("ZCHO_TMMSMCT", "DURATION_TIME", 0, duration_time) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else{
				if (tmmsmgy06["TOGETHER_TIME"].ToDecimal() != 0){
					CDecimal together_time = 0;
					together_time = tmmsmgy06["TOGETHER_TIME"].ToDecimal() - tmmsmgy06["TOGETHER_TIME"].ToDecimal() - tmmsmgy06["TOGETHER_TIME"].ToDecimal();
					//持续时间1 ZCHO_TMMSMCT.DURATION_TIME
					if (epex.SetValue("ZCHO_TMMSMCT", "DURATION_TIME", 0, together_time) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else{
					Log::Trace("", "like", "like = {0}]", __LINE__);
					if (epex.SetValue("ZCHO_TMMSMCT", "DURATION_TIME", 0, tmmsmgy06["DURATION_TIME"].ToDecimal()) < 0)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			if (epex.SendTele() < 0)
			{
				strcpy(s.msg, "电文发送失败。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//Log::Trace("", __FUNCTION__, "调用框架发送电文结束");
			/* 释放 */
			epex.Uninitialize();
			Log::Trace("", "like", "like = {0}]", __LINE__);
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();
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
