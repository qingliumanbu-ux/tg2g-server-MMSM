/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一配送物料及调拨物料确认卸货时，给物流系统发送电文21A010-汽运、铁路卸车确认
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21a010_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tcNO = " ";
	CString tableName = " ";
	CString primaryKey = " ";
	CString primaryData = " ";
	CString dealFlag = " ";
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
		tmodel.Print();

		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", __FUNCTION__, "===000= [{0}]", s.userid);
		Log::Trace("", __FUNCTION__, "===111= [{0}]", s.username);
		if (tmodel["UNLOAD_POINT_CODE"].ToString().Trim()!="")
		{

			// 后补充，CG_ORDER_NO GX_PLAN_NO MISSION_NO 数据来源计量电文
			// 20240601030000创新工作室初亮提出 发送该电文时 卸点库区代码UNLOAD_STOCK_CODE 直接给空
			if (epex.SetValue("ZCHO_XCSJ", "DEAL_FLAG", 0, dealFlag) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "YC_FLAG", 0, tmodel["BACK_CODE_5"].ToString()) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "TRANS_TYPE", 0, tmodel["TRNP_MODE_CODE"].ToString()) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "PLAN_NO", 0, tmodel["VOUCHER_ID"].ToString()) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "PRACTICE_NO", 0, tmodel["MISSING_NO"].ToString()) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "TRUCK_NO", 0, tmodel["SHIP_NAME"].ToString()) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "TRUCK_BOARD_NO", 0, " ") < 0 ||
				epex.SetValue("ZCHO_XCSJ", "WAGONNO", 0, " ") < 0 ||
				epex.SetValue("ZCHO_XCSJ", "UNLOAD_CODE_FACTORY", 0, tmodel["UNLOAD_POINT_CODE"].ToString().SubstringNE(0, 4)) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "UNLOAD_CODE_AREA", 0, tmodel["UNLOAD_POINT_CODE"].ToString().SubstringNE(0, 6)) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "UNLOAD_STOCK_CODE", 0, " ") < 0 ||
				epex.SetValue("ZCHO_XCSJ", "UNLOAD_CODE", 0, tmodel["UNLOAD_POINT_CODE"].ToString()) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "UNLOAD_END_TIME", 0, tmodel["RECEIVE_DATA_TIME"].ToString()) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "EMP_CODE", 0, s.userid) < 0 ||
				epex.SetValue("ZCHO_XCSJ", "EMP_NAME", 0, s.username) < 0
				)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		//2024-04-24,找初亮再次确认，凡是直供料，给物流/铁区/资源的卸货电文，重量和扣重的单位都是千克，21A010给物流的重量为0
		if (epex.SetValue("ZCHO_XCSJ1", "MATERIAL_CODE", 0, tmodel["MAT_CODE"].ToString()) < 0 ||
			epex.SetValue("ZCHO_XCSJ1", "WEIGHT", 0, 0) < 0 ||
			epex.SetValue("ZCHO_XCSJ1", "DEDUCT_WEIGHT", 0, tmodel["BUCKLE_WT"].ToDecimal()) < 0)

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
