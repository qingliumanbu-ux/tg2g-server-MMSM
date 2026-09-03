/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-03-11
Description:过程操作记录接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(cm_edt808_rcv)

int f_cm_edt808_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 		//系统日志类定义

	int doFlag = 0;					//返回值 

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//当前时间

	CModel tmmsm11e("TMMSM11E");	//客户端操作记录表实体类

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++){
			tmmsm11e.Reset();									//重置tmmsm11e
			tmmsm11e.MergeFrom(bcls_rec->Tables[0].Rows[i]);	//将i行的数据填充到tmmsm11e中

			//记录新增信息
			tmmsm11e["REC_CREATOR"] = "EDT808";
			tmmsm11e["REC_CREATE_TIME"] = datetime;
			tmmsm11e["COMPANY_CODE"] = "TG";
			tmmsm11e["COMPANY_NAME"] = "太钢";
			tmmsm11e["SEQ_CODE"] = bcls_rec->Tables[0].Rows[i]["SEQ_NO"];
			tmmsm11e["ADDR_CODE"] = bcls_rec->Tables[0].Rows[i]["COMPUTER_ADDR"];
			tmmsm11e["ADDR_ENAME"] = bcls_rec->Tables[0].Rows[i]["COMPUTER_NAME"];
			tmmsm11e["REMARK"] = bcls_rec->Tables[0].Rows[i]["FORM_CAPTION"];
			tmmsm11e["REMARK_1"] = bcls_rec->Tables[0].Rows[i]["FORM_NAME"];
			tmmsm11e["REMARK_3"] = bcls_rec->Tables[0].Rows[i]["OP_TYPE"];
			tmmsm11e["REMARK_DESC"] = bcls_rec->Tables[0].Rows[i]["OP_DESC"];
			tmmsm11e["TIME_1"] = bcls_rec->Tables[0].Rows[i]["COMPUTER_TIME"];
			tmmsm11e["TIME_STAMPS"] = bcls_rec->Tables[0].Rows[i]["TIME_STAMP"];

			tmmsm11e.TrimOrBlank();								//设置各字段默认值
			tmmsm11e.Insert();									//新增
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