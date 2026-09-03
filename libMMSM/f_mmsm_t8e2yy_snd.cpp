/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:
一给原料L2-各工序料仓物料信息
T8E2Y1
T8E2Y3
T8E2Y5
T8E2Y8
T8E2Y9
T8E2YA
T8E2YF
T8E2YI
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_t8e2yy_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString bunkerNo = " ";
	CString seqNo = " ";
	CString matCode = " ";
	EPEX epex;

	/* 实体类定义 */
	CModel tmmsm50("TMMSM50");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm85("TMMSM85");
	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMLCSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
			tcNO = bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME"))
			tableName = bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_NO"))
			tmmsm85["BUNKER_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["BUNKER_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
			tmmsm85["LOT_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("QUALITY_BATCH_NO"))
			tmmsm85["QUALITY_BATCH_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["QUALITY_BATCH_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
			tmmsm85["MAT_CODE"] = bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"].ToString().Trim();
		if ("" == tableName)
		{
			strcpy(s.msg, "选择的料仓类型有误！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if ("" == tmmsm85["BUNKER_NO"].ToString())
		{
			strcpy(s.msg, "传入料仓号为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "===tcNO= [{0}]", tcNO);
		Log::Trace("", __FUNCTION__, "===tableName= [{0}]", tableName);

		/* 查询主数据 */
		tmmsm50["MAT_CODE"] = tmmsm85["MAT_CODE"];
		tmmsm50.Query("MAT_CODE");
		tmmsm60["BUNKER_NO"] = tmmsm85["BUNKER_NO"];
		tmmsm60.Query("BUNKER_NO");
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "MAT_CODE= [{0}]; BUNKER_NO= [{1}]", tmmsm50["MAT_CODE"].ToString(), tmmsm85["BUNKER_NO"].ToString());
		if (epex.SetValue(tableName, "LOCATION", 0, tmmsm60["STK_NO"].ToDecimal()) < 0 ||
			epex.SetValue(tableName, "MATERIAL_CODE", 0, tmmsm50["MAT_CODE_L2"].ToString()) < 0 ||
			epex.SetValue(tableName, "BATCH_NUMBER", 0, tmmsm50["LOT_NO"].ToString()) < 0 ||
			epex.SetValue(tableName, "QUALITY_BATCH", 0, tmmsm85["QUALITY_BATCH_NO"].ToString()) < 0)
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
