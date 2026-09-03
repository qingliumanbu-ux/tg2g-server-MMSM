/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-01-17
Description:石灰采购订单接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(cm_b02108_rcv)

int f_cm_b02108_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{

	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString div_flag = "";
	CString matPlmsCode = "";
	CString matCode = "";
	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsm81v("TMMSM81V");
	CModel tmmsm81v_old("TMMSM81V");
	try
	{
		tmmsm81v.Reset();
		tmmsm81v.MergeFrom(bcls_rec->Tables["B02108"].Rows[0]);
		tmmsm81v["C_ORDERID"] = bcls_rec->Tables["B02108"].Rows[0]["BUY_ORDER_NO"];
		tmmsm81v["VENDOR_CODE"] = bcls_rec->Tables["B02108"].Rows[0]["SUPPLIER_CODE"];
		tmmsm81v["VENDOR_NAME"] = bcls_rec->Tables["B02108"].Rows[0]["SUPPLIER_NAME"];
		tmmsm81v["MAT_ACT_WT"] = bcls_rec->Tables["B02108"].Rows[0]["PLAN_WGT"];
		if (tmmsm81v["C_ORDERID"].ToString().Trim() == "")
		{
			sprintf(s.msg, "采购订单号为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm81v.QueryCount("C_ORDERID,MAT_CODE")<1)
		{
			tmmsm81v["REC_CREATOR"] = s.userid;
			tmmsm81v["REC_CREATE_TIME"] = datetime;
			tmmsm81v["RECV_TIME"] = datetime;
			sqlstr = "  SELECT max(SEQ_NO)+1 FROM TMMSM81V ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm81v["SEQ_NO"] = cmd_inq.GetDecimal(1);
			}
			tmmsm81v.TrimOrBlank();
			tmmsm81v.Insert();
		}
		else
		{
			tmmsm81v["REC_REVISOR"] = s.userid;
			tmmsm81v["REC_REVISE_TIME"] = datetime;
			tmmsm81v.Update("REC_REVISOR,REC_REVISE_TIME,CONSIGN_ADVICE_NO,ORDER_ID,VENDOR_CODE,VENDOR_NAME,DATE_START,DATE_END,PLAN_WGT,MAT_ACT_WT", "C_ORDERID,MAT_CODE");
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