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

/*<remark>=========================================================
/// <summary>
/// 能源请求消耗
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

BM2_FUNCTION_EXPORT
int f_t8f0s1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	EPEX epex;

	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tmmsmt801("TMMSMT801");

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString seq("");
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString no = " ";
	CString itemid = " ";
	CString clock = " ";

	try
	{
		for (size_t i = 0; i < bcls_rec->Tables["T8F0S1"].Rows.get_Count(); i++)
		{
			Log::Trace("", "Rows", "Rows = {0}", bcls_rec->Tables["T8F0S1"].Rows.get_Count());
			CString epex_number = "T8F0S1";
			if (epex.Initialize(epex_number) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//获取熔炼号
			no = bcls_rec->Tables["T8F0S1"].Rows[i]["NO"].ToString();
			itemid = bcls_rec->Tables["T8F0S1"].Rows[i]["ITEMID"].ToString();
			clock = bcls_rec->Tables["T8F0S1"].Rows[i]["CLOCK"].ToString();

			tmmsmt801.Reset();
			tmmsmt801["T8_NO"] = no;
			tmmsmt801["DATE_TIME"] = bcls_rec->Tables["T8F0S1"].Rows[i]["DATI_MSG_SENT"].ToString();
			Log::Trace("", "like", "like = {0}", __LINE__);
			tmmsmt801["SEND_FLAG"] = "1";
			tmmsmt801["DATI_MSG_SENT"] = bcls_rec->Tables["T8F0S1"].Rows[i]["DATI_MSG_SENT"].ToString();
			tmmsmt801["REC_CREATOR"] = s.userid;
			tmmsmt801["REC_CREATE_TIME"] = datetime;
			tmmsmt801.Insert();

			//序号
			if (epex.SetValue("no", 0, no) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//标签
			if (epex.SetValue("itemid", 0, itemid) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//日期
			if (epex.SetValue("clock", 0, clock) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}


			if (epex.SendTele() < 0)
			{
				Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			epex.Uninitialize();
			Log::Trace("", "seq_no", "seq_no = {0}", no);
			
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

	return doFlag;
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


