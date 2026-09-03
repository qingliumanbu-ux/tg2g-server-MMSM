/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   mfj
Version:    1.0
Date:     2023-09-19
Description: 原辅料退货订单计划生成与下发
**************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm533bvf3_pro)
//-EP_SYSTEM_HEAD_END                                                  
int f_mmsm533bvf3_pro(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{

	EPEX epex;
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ret = 0;
	/* 业务变量 */

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");;
	CString v_table_type = "";
	CString work_date = "";
	CString tc_no = "";//电文号
	CString cc_msg_id = "";//序号


	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	CModel tmmsm54("TMMSM54");

	try
	{

		//获取传入参数
		tmmsm54.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		
		tmmsm54["PLAN_MAKE_TIME"] = datetime;//计划变成时刻
		tmmsm54["PLAN_WT"] = bcls_rec->Tables[0].Rows[0]["NET_WT"].ToDecimal();//计划重量取净重？
		//tmmsm54["FROM_FACTORY"] = "";//FROM_厂别  
		//tmmsm54["TO_FACTORY"] = "";//TO_厂别  

		tmmsm54["PLAN_STATUS"] = "1";//1为计划编成

		sqlstr = " VALUES LPAD(MM533_BV.nextval, 6, '0')  ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		if (cmd_inq.Read())
		{
			cc_msg_id = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		tmmsm54["PLAN_NO_Y"] = datetime + cc_msg_id;

		tmmsm54.Print();

		//当确认电文后，在这里写发送功能，并设置为true
		if (false)
		{
			//初始化电文
			if (epex.Initialize(tc_no) < 0)
			{
				strcpy(s.msg, "初始化电文[" + tc_no + "]失败，原因[" + epex.GetMsg() + "]。");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (epex.SetValue(0, tmmsm54) < 0)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//电文发送
			if (epex.SendTele() < 0)
			{
				strcpy(s.msg, "电文发送失败。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			/* 释放 */
			epex.Uninitialize();

			tmmsm54["PLAN_STATUS"] = "2";//2为计划下发
			tmmsm54["PLAN_SEND_TIME"] = datetime;//计划下发时刻
		}
		

		tmmsm54.Insert();

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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}




