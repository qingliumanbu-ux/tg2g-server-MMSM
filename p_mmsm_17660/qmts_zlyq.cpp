/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-08-24 19:54:28
Description: 质量引擎每日7点推送
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmts_zlyq)
//BM2_FUNCTION_EXPORT

int f_qmts_call_judge(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//消息规则引擎

int f_qmts_zlyq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		//规则引擎
		EIClass iblk_yq;
		if (iblk_yq.Tables.Contains("RULE_CONFIG") == false)
		{
			iblk_yq.Tables[0].set_TableName("RULE_CONFIG");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "PROJECT_ENAME");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "ENV_TYPE");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "VERSION");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "CUSTOM_CONFIG");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "CODE_CLASS");
			iblk_yq.Tables["RULE_CONFIG"].Rows.Add();
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["PROJECT_ENAME"] = "TASK";//固定值，不变
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["ENV_TYPE"] = "0";//测试--0，正式--1
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["VERSION"] = "20241201";//固定值，不变
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["CUSTOM_CONFIG"] = "T";//固定值，不变
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["CODE_CLASS"] = "EPIJG0";//固定值，不变
		}
		if (iblk_yq.Tables.Contains("PROJECT_CONFIG") == false)
		{
			iblk_yq.Tables.Add();
			iblk_yq.Tables[1].set_TableName("PROJECT_CONFIG");
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "MESSAGE_CLASS");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "UNIT_CODE");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "MAT_NO");//三列必须有
		}
		if (iblk_yq.Tables.Contains("DATA_CUSTOM") == false)
		{
			Log::Trace("", "", "inBlock 初始化表3为 DATA_CUSTOM ");

			iblk_yq.Tables.Add("DATA_CUSTOM");
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "UNIT_CODE");//该列必须有
			//iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "HEAT_NO");//参与计算的列
			//iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "ST_SAMPLE_NO");//参与计算的列
		}

		iblk_yq.Tables["PROJECT_CONFIG"].Rows.Add();
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["MESSAGE_CLASS"] = "BB_ZLYQ_SEVEN";//任务池准入条件的比对值
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["UNIT_CODE"] = "H000";//不定机组


		iblk_yq.Tables["DATA_CUSTOM"].Rows.Add();
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["UNIT_CODE"] = "H000";//不定机组
		//iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"].ToString();
		//iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"].ToString();
		doFlag = f_qmts_call_judge(&iblk_yq, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", "", "引擎失败 ");
			doFlag = 0;
			s.flag = 0;
		}
		Log::Trace("", "", "引擎成功 ");
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