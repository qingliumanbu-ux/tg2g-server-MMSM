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

BM2F_ENTERACE_TELE(cm_edt810_rcv)

int f_cm_edt810_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 		//系统日志类定义

	int doFlag = 0;					//返回值 

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//当前时间

	CModel tmmsm12d("TMMSM12F");	//倒罐操作履历表实体类

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
				tmmsm12d["HMH_ID"] = bcls_rec->Tables[0].Rows[i]["ID"];
				tmmsm12d["PLAN_ID"] = bcls_rec->Tables[0].Rows[i]["PLAN_ID"];
				tmmsm12d["IRON_LADLE_ID"] = bcls_rec->Tables[0].Rows[i]["LADLE_NUMBER"];
				tmmsm12d["AGGREGATE_NAME"] = bcls_rec->Tables[0].Rows[i]["AGGREGATE_NAME"];
				tmmsm12d["HM_TEMP"] = bcls_rec->Tables[0].Rows[i]["HM_TEMP"];
				tmmsm12d["MOLTIRON_WT"] = bcls_rec->Tables[0].Rows[i]["HM_WEIGHT"];
				tmmsm12d["DEST_CODE"] = bcls_rec->Tables[0].Rows[i]["DESTINATION"];
				tmmsm12d["LADLE_ARRIVAL_TIME"] = bcls_rec->Tables[0].Rows[i]["LADLE_ARRIVAL_TIME"];
				tmmsm12d["LADLE_LEAVE_TIME"] = bcls_rec->Tables[0].Rows[i]["LADLE_LEAVE_TIME"];
				tmmsm12d["TAPPING_BEGIN_TIME"] = bcls_rec->Tables[0].Rows[i]["TAPPING_BEGIN_TIME"];
				tmmsm12d["TAPPING_END_TIME"] = bcls_rec->Tables[0].Rows[i]["TAPPING_END_TIME"];
				tmmsm12d["LADLE_WEIGHT1"] = bcls_rec->Tables[0].Rows[i]["LADLE_WEIGHT"];
				tmmsm12d["TAPPING_TIMES"] = bcls_rec->Tables[0].Rows[i]["TAPPING_TIMES"];
				tmmsm12d["TEMP_TIME"] = bcls_rec->Tables[0].Rows[i]["TEMP_TIME"];
				tmmsm12d["TAPPING_TIME"] = bcls_rec->Tables[0].Rows[i]["TAPPING_TIME"];
				tmmsm12d["SPLIT_INDICATION"] = bcls_rec->Tables[0].Rows[i]["SPLIT_INDICATION"];
				tmmsm12d["TREATMENT_COUNTER1"] = bcls_rec->Tables[0].Rows[i]["TREATMENT_COUNTER"];
				tmmsm12d["NUMBER_OF_SPLITS"] = bcls_rec->Tables[0].Rows[i]["NUMBER_OF_SPLITS"];
				tmmsm12d["HEAT_NUMBER"] = bcls_rec->Tables[0].Rows[i]["HEAT_NUMBER"];
				tmmsm12d["MAT_ID"] = bcls_rec->Tables[0].Rows[i]["MAT_ID"];
				tmmsm12d["DESCRIPTION"] = bcls_rec->Tables[0].Rows[i]["DESCRIPTION"];
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