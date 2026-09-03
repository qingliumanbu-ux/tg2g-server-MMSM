/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-13 17:13:56
Description: 装车计划查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmts01_upd)

int f_mmsmts01_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CModel tmmsm88_0("TMMSM88_0");

	try{

		//--------------------------------
		//获取传入参数
		tmmsm88_0.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm88_0["REC_REVISOR"] = s.userid;
		tmmsm88_0["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tmmsm88_0.Print();
		tmmsm88_0.Update("REC_REVISOR, REC_REVISE_TIME, MAT_LEN, MAT_WIDTH, MAT_THICK, MAT_WT, LOAD_CODE_FACTORY, LOAD_CODE_AREA, LOAD_CODE, UNLOAD_CODE_FACTORY, UNLOAD_CODE_AREA, UNLOAD_CODE ", "PLAN_NO, MAT_NO");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "\r\n" + ex.GetMsg();
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