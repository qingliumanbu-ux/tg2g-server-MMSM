/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一给物流-采购退货装货确认
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21b003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tcNO = " ";
	CString dealFlag = " ";
	CString tableName = " ";
	CString primaryKey = " ";
	CString primaryData = " ";
	EPEX epex;

	/* 实体类定义 */

	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tcNO = "21B003";
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		// 后补充，CG_ORDER_NO GX_PLAN_NO MISSION_NO 数据来源计量电文
		if (epex.SetValue("TSDK", "DEAL_FLAG", 0, "A") < 0 ||
			epex.SetValue("TSDK", "WEIGH_NO", 0, "4123121419512010") < 0 ||
			epex.SetValue("TSDK", "WORK_DATE", 0, datetime.SubstringNE(0,8)) < 0 ||
			epex.SetValue("TSDK", "CAR_NO", 0, "晋A0001") < 0 ||
			epex.SetValue("TSDK", "MAT_CODE", 0, "AT000385") < 0 ||
			epex.SetValue("TSDK", "SRC_STOCK_CODE", 0, "624002054 ") < 0 ||
			epex.SetValue("TSDK", "LOAD_POS_CODE", 0, "624002054") < 0 ||
			epex.SetValue("TSDK", "DST_STOCK_CODE", 0, "624002054") < 0 ||
			epex.SetValue("TSDK", "UNLOAD_POS_CODE", 0, "624002054") < 0 ||
			epex.SetValue("TSDK", "WEIGH_APP_NO", 0, "JLCN2312140210") < 0 ||
			epex.SetValue("TSDK", "WEIGH_TIME", 0, datetime) < 0 ||
			epex.SetValue("TSDK", "TARE_WT", 0, 11.2) < 0 ||
			epex.SetValue("TSDK", "GROSS_WT", 0, 99.3) < 0 ||
			epex.SetValue("TSDK", "NET_WT", 0, 88.1) < 0 ||
			epex.SetValue("TSDK", "SETTLEMENT_WT", 0, 88.1) < 0 ||
			epex.SetValue("TSDK", "TCP_NO", 0, " ") < 0 ||
			epex.SetValue("TSDK", "TPC_SEQ", 0, " ") < 0 ||
			epex.SetValue("TSDK", "TPC_NO", 0, " ") < 0 ||
			epex.SetValue("TSDK", "EMPTY_SIGN", 0, "1") < 0 ||
			epex.SetValue("TSDK", "BACK1", 0, " ") < 0 ||
			epex.SetValue("TSDK", "BACK2", 0, "测试") < 0 ||
			epex.SetValue("TSDK", "BACK3", 0, " ") < 0 ||
			epex.SetValue("TSDK", "BACK4", 0, "测试") < 0 ||
			epex.SetValue("TSDK", "BACK5", 0, " ") < 0 
			)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (epex.SendTele() < 0)
		{
			sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		// 释放
		epex.Uninitialize();

		/* ********* 程序处理结束 ********** */
		strcpy(s.msg, _RES("GCRSS0000002")/*处理成功。*/);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
