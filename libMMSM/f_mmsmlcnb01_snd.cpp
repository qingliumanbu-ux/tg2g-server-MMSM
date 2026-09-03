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
int f_mmsmlcnb01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString mat_code = "";
	CString mat_name = "";
	CString system_id_mat = "";
	CString in_factory_code = "";
	CString out_factory_code = "";
	CString adjust_dt = "";
	CDecimal adjust_wt = 0;
	CString rec_creator = "";
	CString adjust_reason = "";
	CString data_resource = "";
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
		
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
			mat_code = bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_NAME"))
			mat_name = bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_NAME"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("SYSTEM_ID_MAT"))
			system_id_mat = bcls_rec->Tables["MMLCSND"].Rows[0]["SYSTEM_ID_MAT"].ToString().Trim();  //电文区分C资源 B铁区
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("IN_FACTORY_CODE"))
			in_factory_code = bcls_rec->Tables["MMLCSND"].Rows[0]["IN_FACTORY_CODE"].ToString().Trim();//入库工厂或源库区代码
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("OUT_FACTORY_CODE"))
			out_factory_code = bcls_rec->Tables["MMLCSND"].Rows[0]["OUT_FACTORY_CODE"].ToString().Trim(); //出库工厂或目的库区代码
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("ADJUST_DT"))
			adjust_dt = bcls_rec->Tables["MMLCSND"].Rows[0]["ADJUST_DT"].ToString().Trim(); //转库日期或调整日期
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("ADJUST_WT"))
			adjust_wt = bcls_rec->Tables["MMLCSND"].Rows[0]["ADJUST_WT"].ToDecimal(); //调整量或移库量
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("REC_CREATOR")) 
			rec_creator = bcls_rec->Tables["MMLCSND"].Rows[0]["REC_CREATOR"].ToString().Trim(); //转库责任者
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("ADJUST_REASON"))
			adjust_reason = bcls_rec->Tables["MMLCSND"].Rows[0]["ADJUST_REASON"].ToString().Trim(); //转库原因(南转北 或者  北转南)
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("DATA_RESOURCE"))
			data_resource = bcls_rec->Tables["MMLCSND"].Rows[0]["DATA_RESOURCE"].ToString().Trim(); //物料分类(废钢   或者   合金)

		/*if ("" == tableName)
		{
			strcpy(s.msg, "传入表名为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if ("" == primaryKey)
		{
			strcpy(s.msg, "传入主键为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		Log::Trace("", __FUNCTION__, "===system_id_mat= [{0}]", system_id_mat);
		//CModel tmodel(tableName);

		if (system_id_mat == "" )
		{
			strcpy(s.msg, "选中物料未查询到来源系统。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (system_id_mat=="C")
		{
			tcNO = "21C012";
		}
		else if (system_id_mat == "B")
		{
			tcNO = "21B007";
		}
		else
		{
			strcpy(s.msg, "选中物料来源系统有误。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 查询主数据 */
		/*tmodel[primaryKey] = primaryData;
		tmodel.Query(primaryKey);
		tmodel.TrimOrBlank();*/
	/*	Log::Trace("", __FUNCTION__, "===WEIGH_NO= [{0}]", tmodel["WEIGH_NO"].ToString());
		Log::Trace("", __FUNCTION__, "===BUCKLE_WT= [{0}]", tmodel["DEDUCT_WGT"].ToDecimal());
		Log::Trace("", __FUNCTION__, "===BUCKLE_WT0.0= [{0}]", tmodel["DEDUCT_WGT"].ToDecimal() / 1000);*/
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (system_id_mat == "C")
		{
			if (epex.SetValue("DEAL_FLAG", 0, "I") < 0 ||
				epex.SetValue("REC_CREATOR", 0, rec_creator) < 0 ||
				epex.SetValue("ADJUST_REASON", 0, adjust_reason) < 0 ||
				epex.SetValue("WORK_DATE", 0, adjust_dt) < 0 ||
				epex.SetValue("MAT_CODE", 0, mat_code) < 0 ||
				epex.SetValue("DATA_RESOURCE", 0, data_resource) < 0 ||
				epex.SetValue("SRC_STOCK_CODE", 0, in_factory_code) < 0 ||
				epex.SetValue("DST_STOCK_CODE", 0, out_factory_code) < 0 ||
				epex.SetValue("ADJUST_WGT", 0, adjust_wt) < 0 ||
				epex.SetValue("BACK1", 0," ") < 0 ||
				epex.SetValue("BACK2", 0," ") < 0 ||
				epex.SetValue("BACK3", 0, " ") < 0 ||
				epex.SetValue("BACK4", 0, " ") < 0 ||
				epex.SetValue("BACK5", 0, " ") < 0 ||
				epex.SetValue("BACK6", 0, " ") < 0 ||
				epex.SetValue("BACK7", 0, " ") < 0 ||
				epex.SetValue("BACK8", 0, " ") < 0 ||
				epex.SetValue("BACK9", 0, " ") < 0 ||
				epex.SetValue("BACK10", 0, " ") < 0 ||
				epex.SetValue("BACK11", 0, " ") < 0 ||
				epex.SetValue("BACK12", 0, " ") < 0 ||
				epex.SetValue("BACK13", 0, " ") < 0 ||
				epex.SetValue("BACK14", 0, " ") < 0 ||
				epex.SetValue("BACK15", 0, " ") < 0 ||
				epex.SetValue("BACK16", 0, " ") < 0 ||
				epex.SetValue("BACK17", 0, " ") < 0 ||
				epex.SetValue("BACK18", 0, " ") < 0 ||
				epex.SetValue("BACK19", 0, " ") < 0 ||
				epex.SetValue("BACK20", 0, " ") < 0
				)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		if (system_id_mat == "B")
		{
			if (epex.SetValue("Default0", "MAT_CODE", 0, mat_code) < 0 ||
				epex.SetValue("Default0", "MAT_CNAME", 0, mat_name) < 0 ||
				epex.SetValue("Default0", "IN_FACTORY_CODE", 0, in_factory_code) < 0 ||
				epex.SetValue("Default0", "OUT_FACTORY_CODE", 0, out_factory_code) < 0 ||
				epex.SetValue("Default0", "ADJUST_DT", 0, adjust_dt) < 0 ||
				epex.SetValue("Default0", "ADJUST_WT", 0, adjust_wt) < 0 ||
				epex.SetValue("Default0", "BACK1", 0, " ") < 0 ||
				epex.SetValue("Default0", "BACK2", 0, " ") < 0 ||
				epex.SetValue("Default0", "BACK3", 0, " ") < 0 ||
				epex.SetValue("Default0", "BACK4", 0, " ") < 0 ||
				epex.SetValue("Default0", "BACK5", 0, " ") < 0
				)
			{
				sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
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
