/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
///连铸电子记录
///铸坯温度
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



BM2F_ENTERACE_TELE(cm_ect804_rcv)

int f_cm_ect804_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm31_temp("TMMSM31_TEMP");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	EPEX epex;
	try
	{
		tmmsm31_temp["ID"] = bcls_rec->Tables[0].Rows[0]["id"].ToDecimal();
		tmmsm31_temp["CC_MACH_NO"] = bcls_rec->Tables[0].Rows[0]["agg"].ToString().Trim();
		tmmsm31_temp["PRINT_NO"] = bcls_rec->Tables[0].Rows[0]["slab_name"].ToString().Trim();
		tmmsm31_temp["SMELTING_TEMP"] = bcls_rec->Tables[0].Rows[0]["temp"].ToDecimal();
		tmmsm31_temp["MEAS_TEMP_TIME"] = bcls_rec->Tables[0].Rows[0]["mea_time"].ToString().Trim();
		tmmsm31_temp["REC_CREATE_TIME"] = datetime;

		//若查到则更新，否则新增
		if (tmmsm31_temp.QueryCount("ID,PRINT_NO"))
		{
			tmmsm31_temp.Update("CC_MACH_NO,SMELTING_TEMP,MEAS_TEMP_TIME", "ID,PRINT_NO");
		}
		else
		{
			tmmsm31_temp.Insert();
		}

		//转发2250 by songwei 2026-08-13

		//电文初始化
		if (epex.Initialize("T8P306") < 0)
		{
			CString ls = epex.GetMsg();
			strcpy(s.msg, "初始化电文T8P306失败，原因[" + ls + "]。");

			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
		if (epex.SetValue("id", 0, tmmsm31_temp["ID"].ToDecimal()) < 0 ||
			epex.SetValue("agg", 0, tmmsm31_temp["CC_MACH_NO"].ToString()) < 0 ||
			epex.SetValue("l1id", 0, bcls_rec->Tables[0].Rows[0]["l1id"].ToDecimal()) < 0 ||
			epex.SetValue("slab_name", 0, tmmsm31_temp["PRINT_NO"].ToString()) < 0 ||
			epex.SetValue("temp", 0, tmmsm31_temp["SMELTING_TEMP"].ToDecimal()) < 0 ||
			epex.SetValue("mea_time", 0, tmmsm31_temp["MEAS_TEMP_TIME"].ToString()) < 0 ||
			epex.SetValue("timestamp", 0, bcls_rec->Tables[0].Rows[0]["timestamp"].ToString().Trim()) < 0 ||
			epex.SetValue("read_flag", 0, bcls_rec->Tables[0].Rows[0]["read_flag"].ToString().Trim()) < 0 ||
			epex.SetValue("read_time", 0, bcls_rec->Tables[0].Rows[0]["read_time"].ToString().Trim()) < 0
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

		epex.Uninitialize();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
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

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


