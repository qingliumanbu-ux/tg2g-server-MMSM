/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      563167
Version:     1.0
Date:        2024-01-12
Description: 交料信息申请
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_mmsm_21c002_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	//电文号
	CString v_deal_div("");
	//电文变量
	EPEX epex(&s, conn);
	/* 实体类定义 */
	CModel tmmsm67("TMMSM67");

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", "", "", "[{aaaa}]", bcls_rec->Tables[0].Rows.get_Count());
		if (bcls_rec->Tables[0].Columns.Contains("DEAL_FLAG"))
			v_deal_div = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString().Trim();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			/* 判断是否存在指定块 */


			//电文初始化
			if (epex.Initialize("21C002") < 0)
			{
				CString ls = epex.GetMsg();
				strcpy(s.msg, "初始化电文21c002失败，原因[" + ls + "]。");

				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmmsm67.Reset();
			//测接口暂时借用
			tmmsm67["PURCHASEDOCID"] = "21JF62402401090059";
			tmmsm67.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm67.Query("PURCHASEDOCID");
			tmmsm67.Print();
			if (epex.SetValue(0, tmmsm67) < 0)
			{
				sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
	     
			if (epex.SetValue("DEAL_FLAG", 0, v_deal_div) < 0)
			{

				sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			 
			if (epex.SetValue("S_DATETIME", 0,tmmsm67["S_DATETIME"].ToString().Substring(0,8)) < 0)
			{

				sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("E_DATETIME", 0, tmmsm67["E_DATETIME"].ToString().Substring(0, 8)) < 0)
			{

				sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

		
		
			//电文发送
			if (epex.SendTele() < 0)
			{
				strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else
			{
				Log::Trace("", __FUNCTION__, "发送电文成功");
			}

			/* 释放 */
			/*tmmsm65["STATUS"] = v_deal_div;
			tmmsm65.Update("STATUS","PURCHASEDOCID");*/
			epex.Uninitialize();
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


