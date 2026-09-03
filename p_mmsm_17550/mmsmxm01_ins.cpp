/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:		李晓明
Version:	1.0
Date:		2023-11-13 17:13:56
Description: 鱼雷罐实绩新增(测试)
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmxm01_ins)

int f_mmsmxm01_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CModel tmmsmts01("TMMSMTS01");

	try{
		tmmsmts01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsmts01.TrimOrBlank();

		tmmsmts01["REC_CREATOR"] = s.userid;
		tmmsmts01["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tmmsmts01.Insert();
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