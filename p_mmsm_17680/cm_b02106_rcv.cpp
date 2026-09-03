/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2023-11-28
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 理论铁产量分摊明细接收
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

BM2F_ENTERACE_TELE(cm_b02106_rcv)

int f_cm_b02106_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	CModel tmmsm15("TMMSM15");
	CModel tmmsm15_old("TMMSM15");

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		for (int i = 0; i < bcls_rec->Tables["B02106"].Rows.get_Count(); i++)
		{
			tmmsm15.Reset();
			tmmsm15.MergeFrom(bcls_rec->Tables["B02106"].Rows[i]);
			tmmsm15.TrimOrBlank();
			tmmsm15_old["SEND_TIME"] = tmmsm15["SEND_TIME"];

			if (tmmsm15_old.Query("SEND_TIME")){
				tmmsm15_old.Delete();
			}

			tmmsm15["REC_CREATOR"] = s.userid;
			tmmsm15["REC_CREATE_TIME"] = datetime;
			tmmsm15.Insert();
			Log::Trace("", __FUNCTION__, "i = {0}", i);
		}

		Log::Trace("", __FUNCTION__, "执行完成！");

		/*sprintf(s.msg, "%d条记录新增成功！请重新查询！");*/  //这行只有报错的时候，用来抛错误异常信息的 在这里不需要
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

	cmd_inq.Close();

	return doFlag;
}