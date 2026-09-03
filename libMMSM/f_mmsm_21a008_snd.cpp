/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一给物流-厂内转运用车计划
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21a008_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString v_deal_div("");
	EPEX epex;

	/* 实体类定义 */
	CModel tmmsm69("TMMSM69");
	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", "", "", "[{aaaa}]", bcls_rec->Tables[0].Rows.get_Count());
		if (bcls_rec->Tables[0].Columns.Contains("DEAL_FLAG"))
		{
			v_deal_div = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString().Trim();
		}

		if ("" == tmmsm69["PLAN_NO"].ToString())
		{
			strcpy(s.msg, "传入计划号为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tmmsm69.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm69.Query("PLAN_NO");

		tcNO = "21A008";
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		// 后补充，CG_ORDER_NO GX_PLAN_NO MISSION_NO 数据来源计量电文
		if (epex.SetValue("ZCHO_CNZYCJH", 0, tmmsm69) < 0)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (epex.SetValue("ZCHO_CNZYCJH", "DEAL_FLAG", 0, v_deal_div) < 0 ||
			epex.SetValue("ZCHO_CNZYCJH", "TRANS_TYPE", 0, tmmsm69["BUSY_TYPE"].ToString()) < 0 ||
			epex.SetValue("ZCHO_CNZYCJH", "YCDW", 0, tmmsm69["CAR_USE_UNIT_CODE"].ToString()) < 0 ||
			epex.SetValue("ZCHO_CNZYCJH", "RECEIVE_UNIT_CODE", 0, tmmsm69["RECV_DEPT_CODE"].ToString()) < 0 ||
			epex.SetValue("ZCHO_CNZYCJH", "GOODS_CODE", 0, tmmsm69["MAT_CODE"].ToString()) < 0 ||
			epex.SetValue("ZCHO_CNZYCJH", "UNLOAD_CODE", 0, tmmsm69["UNLOAD_POINT_CODE"].ToString()) < 0 ||
			epex.SetValue("ZCHO_CNZYCJH", "WAGON_NUM", 0, tmmsm69["CAR_NUM"].ToDecimal()) < 0 ||
			epex.SetValue("ZCHO_CNZYCJH", "QUANTITY", 0, tmmsm69["PLAN_WT"].ToDecimal()) < 0 ||
			epex.SetValue("ZCHO_CNZYCJH", "PLAN_START_TIME", 0, tmmsm69["S_DATETIME"].ToString()) < 0 ||
			epex.SetValue("ZCHO_CNZYCJH", "PLAN_END_TIME", 0, tmmsm69["E_DATETIME"].ToString()) < 0
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
