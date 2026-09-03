/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:		李晓明
Version:	1.0
Date:		2023-11-13 17:13:56
Description: 鱼雷罐实绩修改(测试)
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmxm01_upd)

int f_mmsmxm01_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CModel tmmsmts01("TMMSMTS01");
	CModel tmmsmts01_old("TMMSMTS01");
	CString updateFields = "REC_REVISOR, REC_REVISE_TIME";

	try{
		tmmsmts01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsmts01.TrimOrBlank();
		tmmsmts01_old["TRE_TPC_NO"] = tmmsmts01["TRE_TPC_NO"];
		if (tmmsmts01_old.Query()){

			tmmsmts01_old["REC_REVISOR"] = s.userid;
			tmmsmts01_old["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsmts01_old["TAP_IRON_START_TIME"] = tmmsmts01["TAP_IRON_START_TIME"];
			tmmsmts01_old["TAP_IRON_END_TIME"] = tmmsmts01["TAP_IRON_END_TIME"];
			tmmsmts01_old["IRON_TEMP"] = tmmsmts01["IRON_TEMP"];
			tmmsmts01_old["RECV_IRON_START_TIME"] = tmmsmts01["RECV_IRON_START_TIME"];
			tmmsmts01_old["RECV_IRON_END_TIME"] = tmmsmts01["RECV_IRON_END_TIME"];
			tmmsmts01_old["GROSS_WT"] = tmmsmts01["GROSS_WT"];
			tmmsmts01_old["TARE_WT"] = tmmsmts01["TARE_WT"];
			tmmsmts01_old["NET_WT"] = tmmsmts01["NET_WT"];
			tmmsmts01_old["IRON_TEMP2"] = tmmsmts01["IRON_TEMP2"];
			tmmsmts01_old["TPC_WT"] = tmmsmts01["TPC_WT"];
			tmmsmts01_old["ACCOUNT_WT"] = tmmsmts01["ACCOUNT_WT"];
			tmmsmts01_old.Update("REC_REVISOR, REC_REVISE_TIME, TAP_IRON_START_TIME, TAP_IRON_END_TIME, IRON_TEMP, RECV_IRON_START_TIME, RECV_IRON_END_TIME,"
				"GROSS_WT, TARE_WT, NET_WT, IRON_TEMP2, TPC_WT, ACCOUNT_WT", "TRE_TPC_NO");
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