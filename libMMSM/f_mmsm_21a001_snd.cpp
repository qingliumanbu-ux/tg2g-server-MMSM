/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一给物流-汽运/火车采购进厂卸货确认
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21a001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
		Log::Trace("", __FUNCTION__, "===WEIGH_NO= [{0}]", tmodel["WEIGH_NO"].ToString());
		Log::Trace("", __FUNCTION__, "===BUCKLE_WT= [{0}]", tmodel["DEDUCT_WGT"].ToDecimal());
		Log::Trace("", __FUNCTION__, "===BUCKLE_WT0.0= [{0}]", tmodel["DEDUCT_WGT"].ToDecimal() / 1000);
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		// 后补充，CG_ORDER_NO GX_PLAN_NO MISSION_NO 数据来源计量电文
		//2024-4-24  查老系统近期电文履历-区分标记都为3，找初亮确认标记是磅单传过来的（查询磅单接收程序f_mmsm81_d021_handle_rcv 写死为3）
		if (epex.SetValue("ZCHO_JCXHQR", "DEAL_FLAG", 0, dealFlag) < 0||
			epex.SetValue("ZCHO_JCXHQR", "FLAG", 0, "3") < 0 ||
			epex.SetValue("ZCHO_JCXHQR", "CG_ORDER_NO", 0, tmodel["VOUCHER_ID"].ToString()) < 0 ||
			epex.SetValue("ZCHO_JCXHQR", "WEIGH_NO", 0, tmodel["WEIGH_NO"].ToString()) < 0 ||
			epex.SetValue("ZCHO_JCXHQR", "GX_PLAN_NO", 0, tmodel["PLAN_NO"].ToString()) < 0 ||
			epex.SetValue("ZCHO_JCXHQR", "MISSION_NO", 0, tmodel["MISSING_NO"].ToString()) < 0 ||
			epex.SetValue("ZCHO_JCXHQR", "TRUCK_NO", 0, tmodel["SHIP_NAME"].ToString()) < 0 ||
			epex.SetValue("ZCHO_JCXHQR", "FINISH_TIME", 0, tmodel["RECEIVE_DATA_TIME"].ToString()) < 0 ||
			epex.SetValue("ZCHO_JCXHQR", "UNLOAD_PART_FLAG", 0, tmodel["BACK_CODE_5"].ToString()) < 0 ||
			epex.SetValue("ZCHO_JCXHQR", "BACK1", 0, tmodel["TRUST_ID"].ToString()) < 0||
			epex.SetValue("ZCHO_JCXHQR", "BACK2", 0, tmodel["PROJECT_NO"].ToString()) < 0
			)
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//2024-04-24,找初亮再次确认，凡是直供料，给物流/铁区/资源的卸货电文，重量和扣重的单位都是千克，21A001给物流的重量为0
		if (epex.SetValue("ZCHO_JCXHQR1", "MATERIAL_CODE", 0, tmodel["MAT_CODE"].ToString()) < 0 ||
			epex.SetValue("ZCHO_JCXHQR1", "MATERIAL_NAME", 0, tmodel["MAT_NAME"].ToString()) < 0 ||
			epex.SetValue("ZCHO_JCXHQR1", "WEIGHT", 0, 0) < 0 ||
			epex.SetValue("ZCHO_JCXHQR1", "DEDUCT_WEIGHT", 0, tmodel["DEDUCT_WGT"].ToDecimal()) < 0)
			
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
