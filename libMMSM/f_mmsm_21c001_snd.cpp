/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      563160
Version:     1.0
Date:        2023-12-12
Description: 要料信息申请
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_mmsm_21c001_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel tmmsm65("TMMSM65");
	CModel tmmsm50("TMMSM50");
	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//Log::Trace("", "", "", "[{0}]", bcls_rec->Tables["NEWMM_TABLE"].Rows.get_Count());
		if (bcls_rec->Tables[0].Columns.Contains("DEAL_FLAG"))
			v_deal_div = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString().Trim();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			/* 判断是否存在指定块 */
            tmmsm65.Reset();
			tmmsm65.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm50["MAT_CODE"] = tmmsm65["MAT_CODE"];
			tmmsm50.Query("MAT_CODE");
			if ("C" == tmmsm50["SYSTEM_ID_MAT"].ToString())
			{
				if (epex.Initialize("21C001") < 0)
				{
					CString ls = epex.GetMsg();
					strcpy(s.msg, "初始化电文21c001失败，原因[" + ls + "]。");

					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				Log::Trace(" ", __FUNCTION__, "1111 =[{0}]", 1111);
				if (epex.SetValue(0, tmmsm65) < 0)
				{
					sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (epex.SetValue("DEAL_FLAG", 0, v_deal_div) < 0)
				{

					sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//电文发送
				if (epex.SendTele() < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
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
			else
			{
				//电文初始化
				if (epex.Initialize("21B001") < 0)
				{
					CString ls = epex.GetMsg();
					strcpy(s.msg, "初始化电文21B001失败，原因[" + ls + "]。");

					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				tmmsm65.Reset();
				tmmsm65.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				
				if (epex.SetValue("21B001", "DEAL_FLAG", 0, v_deal_div) < 0 ||
					epex.SetValue("21B001", "APPLY_NO", 0, tmmsm65["PURCHASEDOCID"].ToString()) < 0 ||
					epex.SetValue("21B001", "CAR_USE_UNIT_CODE", 0, tmmsm65["CAR_USE_UNIT_CODE"].ToString()) < 0 ||
					epex.SetValue("21B001", "SEND_UNIT_CODE", 0, tmmsm65["DG_UNIT_CODE"].ToString()) < 0 ||
					epex.SetValue("21B001", "RECEIVE_UNIT_CODE", 0, tmmsm65["RECV_DEPT_CODE"].ToString()) < 0 ||
					epex.SetValue("21B001", "MAT_CODE", 0, tmmsm65["MAT_CODE"].ToString()) < 0 ||
					epex.SetValue("21B001", "LOAD_POS_CODE", 0, tmmsm65["LOAD_CODE"].ToString()) < 0 ||
					epex.SetValue("21B001", "UNLOAD_POS_CODE", 0, tmmsm65["UNLOAD_POINT_CODE"].ToString()) < 0 ||
					epex.SetValue("21B001", "PLAN_WT", 0, tmmsm65["PLAN_WT"].ToDecimal()) < 0 ||
					epex.SetValue("21B001", "WORK_DATE_FR", 0, tmmsm65["S_DATETIME"].ToString()) < 0 ||
					epex.SetValue("21B001", "WORK_DATE_TO", 0, tmmsm65["E_DATETIME"].ToString()) < 0 ||
					epex.SetValue("21B001", "WEIGH_TYPE", 0, tmmsm65["MEASURE_MODE"].ToString()) < 0 ||
					epex.SetValue("21B001", "CAR_TYPE", 0, tmmsm65["TRUCK_MODEL"].ToString()) < 0 ||
					epex.SetValue("21B001", "CAR_NUM", 0, tmmsm65["CAR_NUM"].ToDecimal()) < 0 ||
					epex.SetValue("21B001", "APPLY_BY", 0, tmmsm65["APPLY_BY"].ToString()) < 0 ||
					epex.SetValue("21B001", "APPLY_TIME", 0, tmmsm65["APTIME"].ToString()) < 0 ||
					epex.SetValue("21B001", "BACK1", 0, tmmsm65["COST_CENTER"].ToString()) < 0 ||
					epex.SetValue("21B001", "BACK2", 0, tmmsm65["BACK2"].ToString()) < 0 ||	
					epex.SetValue("21B001", "BACK3", 0, tmmsm65["BACK3"].ToString()) < 0 ||
					epex.SetValue("21B001", "BACK4", 0, tmmsm65["BACK4"].ToString()) < 0
					)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
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
				Log::Trace(" ", __FUNCTION__, "1111 =[{0}]", 4444);
				/* 释放 */
				/*tmmsm65["STATUS"] = v_deal_div;
				tmmsm65.Update("STATUS","PURCHASEDOCID");*/
				epex.Uninitialize();
			}
			
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


