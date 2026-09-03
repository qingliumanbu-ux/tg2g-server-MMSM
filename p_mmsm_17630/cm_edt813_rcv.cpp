/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-03-11
Description:倒罐操作履历接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(cm_edt813_rcv)

int f_cm_edt813_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 		//系统日志类定义

	int doFlag = 0;					//返回值 

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//当前时间

	CModel tmmsm12d("TMMSM12I");	//倒罐操作履历表实体类

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++){
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++){
				tmmsm12d.Reset();
				tmmsm12d.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				tmmsm12d["REC_CREATOR"] = s.userid;
				tmmsm12d["REC_CREATE_TIME"] = datetime;
				tmmsm12d["COMPANY_CODE"] = "TG";
				tmmsm12d["COMPANY_NAME"] = "太钢";
				tmmsm12d["WEIGHT_NO"] = bcls_rec->Tables[0].Rows[i]["C_DELIVERYID"];
				tmmsm12d["TOTAL_WT"] = bcls_rec->Tables[0].Rows[i]["SUM_WGT"];
				tmmsm12d["LOAD_WT"] = bcls_rec->Tables[0].Rows[i]["SUB_WGT"];
				tmmsm12d["IN_STOCK_WT"] = bcls_rec->Tables[0].Rows[i]["SEND_WGT"];
				tmmsm12d["UNIT"] = bcls_rec->Tables[0].Rows[i]["WGT_UNIT"];
				tmmsm12d["REASON_1"] = bcls_rec->Tables[0].Rows[i]["CHANGE_REASON"];
				tmmsm12d["TIME_STAMPS"] = bcls_rec->Tables[0].Rows[i]["TIME_STAMP"];
				tmmsm12d["REMARK"] = bcls_rec->Tables[0].Rows[i]["REMARK"];
				tmmsm12d.TrimOrBlank();
				tmmsm12d.Insert();
			}
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