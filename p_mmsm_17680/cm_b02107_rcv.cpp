/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-01-17
Description:磅单号与质检组批对应关系接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(cm_b02107_rcv)

int f_cm_b02107_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{

	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString div_flag = "";
	CString matPlmsCode = "";
	CString matCode = "";
	CModel tmmsm81("TMMSM81");
	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		return 0;	//不需要接收 郭松要求20240518

		div_flag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();
		tmmsm81["QUALITY_BATCH_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ENTR_NO"].ToString();
		tmmsm81["WEIGH_NO"] = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString();
		Log::Trace("", __FUNCTION__, "WEIGH_NO=[{0}] QUALITY_BATCH_NO=[{1}]",
			tmmsm81["WEIGH_NO"].ToString(), tmmsm81["QUALITY_BATCH_NO"].ToString());

		if ("" == div_flag.Trim())
		{
			strcpy(s.msg, "处理标记不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == tmmsm81["QUALITY_BATCH_NO"].ToString().Trim())
		{
			strcpy(s.msg, "质检批号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (0 < tmmsm81.QueryCount("WEIGH_NO"))
		{
			tmmsm81["REC_REVISE_TIME"] = datetime;
			tmmsm81["REC_REVISOR"] = s.userid;
			tmmsm81.Update("QUALITY_BATCH_NO,REC_REVISE_TIME,REC_REVISOR", "WEIGH_NO");
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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

	cmd_inq.Close();

	return doFlag;
}