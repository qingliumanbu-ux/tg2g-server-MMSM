/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一给资源-采购进厂卸货确认
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21c011_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString dealFlag = " ";
	CString tcNO = " ";
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
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_KEY"))
			primaryKey = bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_KEY"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_DATA"))
			primaryData = bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_DATA"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
			dealFlag = bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		if ("" == tableName)
		{
			strcpy(s.msg, "传入表名为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if ("" == primaryKey)
		{
			strcpy(s.msg, "传入主键为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "===tableName= [{0}]", tableName);
		CModel tmodel(tableName);

		/* 查询主数据 */
		tmodel[primaryKey] = primaryData;
		tmodel.Query(primaryKey);
		tmodel.TrimOrBlank();

		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("DEAL_FLAG", 0, dealFlag) < 0)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (epex.SetValue("C_FLAG_DIF", 0, "3") < 0)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (epex.SetValue(0, tmodel) < 0)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//2024-04-24,找初亮再次确认，凡是直供料，给物流/铁区/资源的卸货电文，重量和扣重的单位都是千克
		if (epex.SetValue("STOCK_WT", 0, tmodel["STOCK_WT"].ToDecimal() ) < 0 ||
			epex.SetValue("DEDUCT_WGT", 0, tmodel["DEDUCT_WGT"].ToDecimal()) < 0 
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
