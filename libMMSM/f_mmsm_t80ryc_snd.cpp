/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      李振
Version:     1.0
Date:        2023-11-20
Description: 制造倒垛
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_mmsm_t80ryc_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);
	/* 实体类定义 */
	CModel twma0("TWMA0");

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("T80RYC");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 T80RYC 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cs_tc_no = "T80RYC";
		//电文初始化
		if (epex.Initialize(cs_tc_no) < 0)
		{
			strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
			s.flag = -1;
			doFlag = -1;
			return doFlag;
		}
		twma0.MergeFrom(bcls_rec->Tables["T80RYC"].Rows[0]);

		if (epex.SetValue(0, twma0) < 0)
		{
			sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
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

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


