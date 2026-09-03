/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-03-11
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 鱼雷罐实绩接收
/// </summary>
/// <returns></returns>
===========================================================</remark>*/
BM2F_ENTERACE_TELE(cm_edt802_rcv)

int f_cm_edt802_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 		//系统日志类定义

	int doFlag = 0;					//返回值 

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//当前时间

	CModel tmmsm11("TMMSM11");		//炼钢铁水信息表实体类

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm11.Reset();
			tmmsm11.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			tmmsm11["REC_CREATOR"] = s.userid;
			tmmsm11["REC_CREATE_TIME"] = datetime;
			tmmsm11["COMPANY_CODE"] = "TG";
			tmmsm11["COMPANY_NAME"] = "太钢";
			tmmsm11["BATCH_NO"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_BATCH_NUMBER"];
			tmmsm11["TPC_YL_NO"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_NUMBER"];
			tmmsm11["IRON_TEMP"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["HM_WEIGHT"].ToString());
			tmmsm11["NWEIGHT"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["HM_TEMP"].ToString());
			tmmsm11["TIME_TORPEDO_IN"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_ARRIVAL_TIME"];
			tmmsm11["TIME_TORPEDO_OUT"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_LEAVE_TIME"];
			tmmsm11["C_SAMPLEID"] = bcls_rec->Tables[0].Rows[i]["SAMPLE_ID"];
			tmmsm11["TPC_SOURCE"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_SOURCE"];
			tmmsm11["TICODE"] = bcls_rec->Tables[0].Rows[i]["PLAN_ID"];
			tmmsm11["C_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["C"].ToString());
			tmmsm11["SI_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["SI"].ToString());
			tmmsm11["MN_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["MN"].ToString());
			tmmsm11["P_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["P"].ToString());
			tmmsm11["S_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["S"].ToString());
			tmmsm11["TI_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["TI"].ToString());
			tmmsm11["V_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["V"].ToString());
			tmmsm11["HEAT_NO"] = bcls_rec->Tables[0].Rows[i]["HEAT_NUMBER"];
			tmmsm11["HM_WT_MANUAL"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["HM_WEIGHT_MANUAL"].ToString());
			tmmsm11["CR_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["CR"].ToString());
			tmmsm11["NI_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["NI"].ToString());
			tmmsm11["STATION_NO"] = bcls_rec->Tables[0].Rows[i]["POSITION_ID"];
			tmmsm11["C_CLOSEGATETIME"] = bcls_rec->Tables[0].Rows[i]["DEAD_WEIGHT_DATE"];
			tmmsm11["IS_TO_LIANTIE"] = bcls_rec->Tables[0].Rows[i]["IS_TO_LIANTIE"];
			tmmsm11["TIDCODE"] = bcls_rec->Tables[0].Rows[i]["C_DELIVERYID"];
			//tmmsm11.TrimOrBlank();
			tmmsm11.Update("*");
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

	//返回-1时事务将回滚，返回为0是事务将提交	
	return doFlag;
}