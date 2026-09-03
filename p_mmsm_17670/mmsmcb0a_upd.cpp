/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   zl
Version:    1.0
Date:     2024
Description: 更新去年到今年的物料标准价格
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
BM2_FUNCTION_IMPORT
int f_mmsm_zxh02(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsmcb0a_upd)

int f_mmsmcb0a_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMdd");
	CString stat_date = CDateTime::Now().ToString("yyyyMM");
	

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("DATE_C"))
		{
			stat_date = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0, 6);
		} 		

		EIClass bcls_rec_xh;
		bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "STAT_DATE");
		bcls_rec_xh.Tables[0].Rows.Add();
		bcls_rec_xh.Tables[0].Rows[0]["STAT_DATE"] = stat_date;
		
		doFlag = f_mmsm_zxh02(&bcls_rec_xh, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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
