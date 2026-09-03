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
///天车重量-计算好的净重量数据
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



BM2F_ENTERACE_TELE(cm_ect803_rcv)

int f_cm_ect803_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm31("TMMSM31");
	CModel tmmsm31c("TMMSM31C");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		tmmsm31c["ID"] = bcls_rec->Tables[0].Rows[0]["id"].ToDecimal();
		tmmsm31c["STRAND_NO"] = bcls_rec->Tables[0].Rows[0]["strand_no"].ToString().Trim();
		tmmsm31c["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["heat_name"].ToString().Trim();
		tmmsm31c["STEEL_NET_WT_CRANE"] = bcls_rec->Tables[0].Rows[0]["steel_net_weight"].ToDecimal();
		tmmsm31c["PLAN_CLOSE_TIME"] = bcls_rec->Tables[0].Rows[0]["close_time"].ToString().Trim();
		tmmsm31c["TIME_STAMPS"] = bcls_rec->Tables[0].Rows[0]["timestamp"].ToString().Trim();

		//若查到则更新，否则新增
		if (tmmsm31c.QueryCount("ID,STRAND_NO,HEAT_NO"))
		{
			tmmsm31c.Update("STEEL_NET_WT_CRANE,PLAN_CLOSE_TIME,TIME_STAMPS", "ID,STRAND_NO,HEAT_NO");
		}
		else
		{
			tmmsm31c.Insert();
		}

		tmmsm31["HEAT_NO"] = tmmsm31c["HEAT_NO"];
		if (tmmsm31.QueryCount("HEAT_NO"))
		{
			tmmsm31.Update("STEEL_NET_WT_CRANE","HEAT_NO");
		}


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


