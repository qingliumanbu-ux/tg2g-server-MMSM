/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一资源系统-废钢料篮计量信息
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21c004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString tcNO = " ";
	CString heatNo = " ";
	CString procNo = " ";
	CString lotNo = " ";
	CString stkNo = " ";
	CString dealFlag = " ";
	EPEX epex;
	CDecimal gross_wt = 0;
	CDecimal deduct_wt = 0;
	/* 实体类定义 */
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm21("TMMSM21");
	CModel tmmsm85("TMMSM85");
	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_85inq(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMLCSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tcNO = bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"];
		dealFlag = bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"];
		if ("" == bcls_rec->Tables["MMLCSND"].Rows[0]["BASKET_NO"].ToString().Trim())
		{
			strcpy(s.msg, "传入料篮号为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "BASKET_NO= [{0}]", bcls_rec->Tables["MMLCSND"].Rows[0]["BASKET_NO"].ToString());
		Log::Trace("", __FUNCTION__, "MAT_CODE [{0}]", bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"].ToString());

		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_GROSS_WT"))
		{
			gross_wt = 0;
		}
		else
		{
			gross_wt = bcls_rec->Tables["MMLCSND"].Rows[0]["BUNKER_GROSS_WT"].ToDecimal();
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_DEDUCT_WT"))
		{
			deduct_wt = 0;
		}
		else
		{
			deduct_wt = bcls_rec->Tables["MMLCSND"].Rows[0]["BUNKER_DEDUCT_WT"].ToDecimal();
		}
		if (epex.SetValue("DEAL_FLAG", 0, dealFlag) < 0 ||
			epex.SetValue("WORK_SEQ_NO", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["WORK_SEQ_NO"].ToString()) < 0 ||
			epex.SetValue("FACTORY_CODE", 0, "6240") < 0 ||
			epex.SetValue("DST_STOCK_CODE", 0, "6241") < 0 ||
			//epex.SetValue("WORK_DATE", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["WORK_DATE"].ToString().SubstringNE(0, 8)) < 0 ||
			epex.SetValue("WORK_DATE", 0, datetime.SubstringNE(0, 8)) < 0 ||
			epex.SetValue("BASKET_NO", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["BASKET_NO"].ToString()) < 0 ||
			epex.SetValue("MAT_CODE", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"].ToString()) < 0 ||
			epex.SetValue("MAT_CNAME", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CNAME"].ToString()) < 0 ||
			epex.SetValue("SRC_STOCK_CODE", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["SRC_STOCK_CODE"].ToString()) < 0 ||
			epex.SetValue("SRC_STOCK_PLACE", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["SRC_STOCK_PLACE"].ToString()) < 0 ||
			epex.SetValue("BUY_ORDER_NO", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["BUY_ORDER_NO"].ToString()) < 0 ||
			epex.SetValue("NET_WGT", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["NET_WGT"].ToDecimal()/1000) < 0 ||
			epex.SetValue("WEIGH_TIME", 0, datetime) < 0 ||
			epex.SetValue("BUNKER_GROSS_WT", 0, gross_wt/ 1000) <0 ||
			epex.SetValue("BUNKER_DEDUCT_WT", 0, deduct_wt/ 1000) <0
			//epex.SetValue("WEIGH_TIME", 0, bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_TIME"].ToString()) < 0
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
