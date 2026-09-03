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
BM2F_ENTERACE(mmsm533cvf3_pro)
//-EP_SYSTEM_HEAD_END                                                  
int f_mmsm533cvf3_pro(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	CModel tmmsm84("TMMSM84");
	CModel tmmsm54("TMMSM54");

	try
	{

		if (!bcls_rec->Tables.Contains("Grid1"))
		{
			strcpy(s.msg, "获取Grid1数据失败，请重新检查!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (!bcls_rec->Tables.Contains("Grid2"))
		{
			strcpy(s.msg, "获取Grid2数据失败，请重新检查!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm54.MergeFrom(bcls_rec->Tables["Grid1"].Rows[0]);
		tmmsm84.MergeFrom(bcls_rec->Tables["Grid2"].Rows[0]);

		tmmsm84["STATUS"] = '1';//1  为装车确认， V为车辆信息

		//材料信息
		tmmsm84["MAT_CODE"] = tmmsm84["MAT_CODE"].ToString();
		tmmsm84["MAT_NAME"] = tmmsm84["MAT_NAME"].ToString();
		tmmsm84["MAT_TYPE"] = tmmsm84["MAT_TYPE"].ToString();

		//车船
		tmmsm84["VEHICLE_NO"] = tmmsm84["VEHICLE_NO"].ToString();
		tmmsm84["WEIGH_NO"] = tmmsm84["WEIGH_NO"].ToString();

		tmmsm84.Insert();
		

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




