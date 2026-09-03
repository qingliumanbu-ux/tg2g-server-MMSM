/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     Simon Li
Version:    1.0
Date:       2024-03-18
Description: 中位硅铁水罐查询
**************************************************/

//框架头文件
#include "stdafx.h"

//业务头文件


BM2F_ENTERACE(mmsm11j_upd)

int f_mmsm11j_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 		//服务调用日志输出

	int doFlag = 0;					//服务调用返回值

	/* 分页信息定义 */
	CPageInfo pageInfo;
	int	TotalRecordCount = 0;

	CModel tmmsm11j("TMMSM11J");	//中位硅1表

	CDbCommand cmd_inq(conn);			//数据库操作对象定义
	CString ay_type = "";
	/* sql语句变量定义 */
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";

	try
	{
		//sql语句赋初值
		tmmsm11j.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm11j.Print();
		tmmsm11j.Update("AC_WT,STOCK_WT,HEAT_WT_MAX,HEAT_WT_MIN,VALUE_SI,SAP_ERP_S1025_1_H,SAP_ERP_S1025_1_L","MODE_NO");
		tmmsm11j.Reset();
		tmmsm11j["MODE_NO"] = bcls_rec->Tables[0].Rows[0]["MODE_NO1"];
		tmmsm11j["STOCK_WT"] = bcls_rec->Tables[0].Rows[0]["STOCK_WT1"];
		tmmsm11j["HEAT_WT_MAX"] = bcls_rec->Tables[0].Rows[0]["HEAT_WT_MAX1"];
		tmmsm11j["HEAT_WT_MIN"] = bcls_rec->Tables[0].Rows[0]["HEAT_WT_MIN1"];
		tmmsm11j["VALUE_SI"] = bcls_rec->Tables[0].Rows[0]["VALUE_SI1"];
		tmmsm11j["SAP_ERP_S1025_1_H"] = bcls_rec->Tables[0].Rows[0]["SAP_ERP_S1025_1_H1"];
		tmmsm11j["SAP_ERP_S1025_1_L"] = bcls_rec->Tables[0].Rows[0]["SAP_ERP_S1025_1_L1"];
		tmmsm11j["AC_WT"] = bcls_rec->Tables[0].Rows[0]["AC_WT"];
		tmmsm11j.Print();
		tmmsm11j.Update("AC_WT,STOCK_WT,HEAT_WT_MAX,HEAT_WT_MIN,VALUE_SI,SAP_ERP_S1025_1_H,SAP_ERP_S1025_1_L", "MODE_NO");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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

	return doFlag;		//返回-1时事务将回滚，返回为0是事务将提交
}