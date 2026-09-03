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
int f_mmsm_21a018_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
		//获取输入参数
		//blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		//if (blkNum < 0)
		//{
		//	strcpy(s.msg, "传入数据块 MMLCSND 不存在。");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		//if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
		//	tcNO = bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"].ToString().Trim();
		//if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME"))
		//	tableName = bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"].ToString().Trim();
		//if (bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_KEY"))
		//	primaryKey = bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_KEY"].ToString().Trim();
		//if (bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_DATA"))
		//	primaryData = bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_DATA"].ToString().Trim();
		//if (bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
		//	dealFlag = bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		//if ("" == tableName)
		//{
		//	strcpy(s.msg, "传入表名为空。");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		//if ("" == primaryKey)
		//{
		//	strcpy(s.msg, "传入主键为空。");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		//Log::Trace("", __FUNCTION__, "===tableName= [{0}]", tableName);
		//CModel tmodel(tableName);

		///* 查询主数据 */
		//tmodel[primaryKey] = primaryData;
		//tmodel.Query(primaryKey);
		//tmodel.TrimOrBlank();

		tcNO = "21A018";
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		// 后补充，CG_ORDER_NO GX_PLAN_NO MISSION_NO 数据来源计量电文
		if (epex.SetValue("ZCHO_TYXHQR", "DEAL_FLAG", 0, "I") < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "ORDER_NO", 0, "4") < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "WAGONNO", 0, "TL0001") < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "METERAGE_NO", 0, "JL2401240104") < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "STOCK_OFFTRAIN", 0, " ") < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "CONFM_TIMEW", 0, datetime) < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "PLAN_UNLOAD_END_TIME", 0, datetime) < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "SHIFTID", 0, "A") < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "SHIFTNO", 0, "1") < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "STATION_TRACK", 0, "6240") < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "STATION_TRACK_CNAME", 0, " ") < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "IN_TIME", 0, datetime) < 0 ||
			epex.SetValue("ZCHO_TYXHQR", "BACK1", 0, " ") < 0 
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
