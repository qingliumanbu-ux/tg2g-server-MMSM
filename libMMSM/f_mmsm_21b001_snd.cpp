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
int f_mmsm_21b001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
		tcNO = "21B001";
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (epex.SetValue("21B001", "DEAL_FLAG", 0, "A") < 0 ||
			epex.SetValue("21B001", "APPLY_NO", 0, "21PS62402312147229") < 0 ||
			epex.SetValue("21B001", "CAR_USE_UNIT_CODE", 0, "624002048") < 0 ||
			epex.SetValue("21B001", "SEND_UNIT_CODE", 0, "624002048") < 0 ||
			epex.SetValue("21B001", "RECEIVE_UNIT_CODE", 0, "624002048") < 0 ||
			epex.SetValue("21B001", "MAT_CODE", 0, "AT000385 ") < 0 ||
			epex.SetValue("21B001", "LOAD_POS_CODE", 0, "624002048") < 0 ||
			epex.SetValue("21B001", "UNLOAD_POS_CODE", 0, "624002048") < 0 ||
			epex.SetValue("21B001", "PLAN_WT", 0, "100") < 0 ||
			epex.SetValue("21B001", "WORK_DATE_FR", 0, datetime.SubstringNE(0,8)) < 0 ||
			epex.SetValue("21B001", "WORK_DATE_TO", 0, datetime.SubstringNE(0, 8)) < 0 ||
			epex.SetValue("21B001", "WEIGH_TYPE", 0, "1") < 0 ||
			epex.SetValue("21B001", "CAR_TYPE", 0, "HJ001") < 0 ||
			epex.SetValue("21B001", "CAR_NUM", 0, 1) < 0 ||
			epex.SetValue("21B001", "APPLY_BY", 0, "测试") < 0 ||
			epex.SetValue("21B001", "APPLY_TIME", 0, datetime) < 0
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
