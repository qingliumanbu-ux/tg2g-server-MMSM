/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-03-11
Description:鱼雷罐过程跟踪信息接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(cm_edt801_rcv)

int f_cm_edt801_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 		//系统日志类定义

	int doFlag = 0;					//返回值 

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//当前时间

	CModel tmmsm11c("TMMSM11C");	//鱼雷罐过程跟踪表实体类

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm11c.Reset();									//重置tmmsm11c
			tmmsm11c.MergeFrom(bcls_rec->Tables[0].Rows[i]);	//将i行的数据填充到tmmsm11c中

			if (tmmsm11c.Query("ITEM_ID, TIME_STAMP"))
			{
				tmmsm11c["REC_REVISOR"] = "EDT801";
				tmmsm11c["REC_REVISE_TIME"] = datetime;
				tmmsm11c["ITEM_VALUE"] = bcls_rec->Tables[0].Rows[i]["ITEM_VALUE"].ToString();
				tmmsm11c.Update("REC_REVISOR, REC_REVISE_TIME, ITEM_VALUE", "ITEM_ID, TIME_STAMP");
			}
			else{
			
				//记录新增信息
				tmmsm11c["REC_CREATOR"] = "EDT801";
				tmmsm11c["REC_CREATE_TIME"] = datetime;
				tmmsm11c["COMPANY_CODE"] = "TG";
				tmmsm11c["COMPANY_NAME"] = "太钢";

				tmmsm11c.TrimOrBlank();								//设置各字段默认值
				tmmsm11c.Insert();									//新增
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