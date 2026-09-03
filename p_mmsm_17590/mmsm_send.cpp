/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   黄华
Version:    1.0
Date:     2014-04-19 17:13:56
Description: 调用发送电文函数服务
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

int f_cm_pam1j1_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_cm_pam1j2_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm_send)

int f_mmsm_send(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;


	try
	{
		if (bcls_rec->Tables[0].Rows[0]["SEND_TAG"].ToString().Trim() == "FGLY")
		{
			doFlag = f_cm_pam1j1_snd(bcls_rec, bcls_ret, conn);
		}
		if (bcls_rec->Tables[0].Rows[0]["SEND_TAG"].ToString().Trim() == "FGHS")
		{
			doFlag = f_cm_pam1j2_snd(bcls_rec, bcls_ret, conn);
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
