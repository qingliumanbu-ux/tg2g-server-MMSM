/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   xxx
Version:    1.0
Date:     2023-09-22
Description: 料仓
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsm81_ins)

int f_mmsm81_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString bunker_no = "";
	int TotalRecordCount = 0;
	CString  now = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CModel tmmsm81("TMMSM81");

	try
	{
		
		Log::Info("", __FUNCTION__, "sql =[{0}]", sqlstr);
		if (bcls_rec->Tables.get_Count() > 0)
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				tmmsm81.Reset();
				tmmsm81.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tmmsm81.TrimOrBlank();
				/* 删除事件信息 */
				tmmsm81["FORM_EDIT_FLAG"] = "0";
				tmmsm81.Insert();

			}
		}
		cmd_inq.Close();
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
