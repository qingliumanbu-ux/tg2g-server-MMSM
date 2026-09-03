/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:
一给原料L2-电炉加料数据
T8E2Y6
T8E2YC
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_t8e2yc_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm81("TMMSM81");
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
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("HEAT_NO"))
			tmmsm2a["HEAT_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_COUNT"))
			tmmsm2a["PROC_COUNT"] = bcls_rec->Tables["MMLCSND"].Rows[0]["PROC_COUNT"].ToString().Trim();

		
		if ("" == tmmsm2a["HEAT_NO"].ToString())
		{
			strcpy(s.msg, "传入熔炼号为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "===tcNO= [{0}]", tcNO);

		/* 查询主数据 */
		tmmsm2a.Query("HEAT_NO,PROC_COUNT");

		sqlstr = "  SELECT WEIGH_NO FROM  TMMSM81     WHERE  LOT_NO			= @tmmsm81.LOT_NO";
		cmd_inq.Parameters.Set("tmmsm81.LOT_NO", tmmsm2a["LOT_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm81["WEIGH_NO"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (epex.SetValue("INT_EAF_CHARGE_EXT", "AGGREGATE_NAME", 0, "I") < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "VAI_CHARGE_ID", 0, " ") < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "HEAT_NUMBER", 0, tmmsm2a["HEAT_NO"].ToString()) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "ORDER_NUMBER", 0, tmmsm2a["PONO"].ToString()) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "SPLIT_INDICATION", 0, tmmsm2a["SPLIT_INDICATION"].ToDecimal()) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "TREATMENT_COUNTER", 0, tmmsm2a["PROC_COUNT"].ToDecimal()) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "CHARGE_TYPE", 0, tmmsm2a["CHARGE_TYPE"].ToString()) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "CNT", 0, 0) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "MATID", 0, tmmsm2a["MAT_CODE"].ToString()) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "DESCRIPTION", 0, tmmsm2a["MAT_NAME"].ToString()) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "MEASUREID", 0, tmmsm81["WEIGH_NO"].ToString()) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "STKID", 0, tmmsm2a["STK_NO"].ToString()) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "STATUS", 0, 0) < 0 ||
			//epex.SetValue("INT_EAF_CHARGE_EXT", "SENDTIME", 0, tmmsm2a["DEVO_TIME"].ToString().SubstringNE(2,6)) < 0 ||
			epex.SetValue("INT_EAF_CHARGE_EXT", "WEIGHT", 0, tmmsm2a["DEVO_WT"].ToDecimal()) < 0
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
