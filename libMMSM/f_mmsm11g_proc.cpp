/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:		Simon Li
Version:    1.0
Date:		2024-03-18
Description: 过程跟踪条目增删改函数
***********************************************************************/

//框架头文件
#include "stdafx.h"

BM2_FUNCTION_EXPORT
int f_mmsm11g_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	CString v_proc_div = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tmmsm11g("TMMSM11G");

	try{
		//参数接收
		v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString();
		Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);

		//循环传入参数集合
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm11g.Reset();
			tmmsm11g.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm11g.TrimOrBlank();
			Log::Trace("", __FUNCTION__, "v_proc_div = {0}", v_proc_div);

			//新增
			if (v_proc_div == "I")
			{
				tmmsm11g["REC_CREATOR"] = s.userid;
				tmmsm11g["REC_CREATE_TIME"] = datetime;
				tmmsm11g["COMPANY_CODE"] = "TG";
				tmmsm11g["COMPANY_NAME"] = "太钢";
				tmmsm11g.Insert();
			}
			//修改
			if (v_proc_div == "U")
			{
				tmmsm11g["REC_REVISOR"] = s.userid;
				tmmsm11g["REC_REVISE_TIME"] = datetime;
				tmmsm11g.Update("REC_REVISOR, REC_REVISE_TIME, ITEM_DES", "ITEM_NAME");
			}
			//删除
			else if (v_proc_div == "D"){
				tmmsm11g.Delete();
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();
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