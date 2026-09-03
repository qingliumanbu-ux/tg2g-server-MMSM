/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

int f_mmsm_21c006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm81s_del_q)

int f_mmsm81s_del_q(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString SeqNo1 = "";
	CString mat_code = "";
	CString mat_name = "";
	int		TotalRecordCount = 0;

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal stock_wt = 0;
	
	CDecimal stock_wt_1 = 0;
	CDecimal stock_wt_c = 0;
	//系统的分页类信息。
	CPageInfo pageInfo;
	EPEX epex;
	CModel tmmsm81_s("TMMSM81_S");
	CModel tmmsm81_s1("TMMSM81_S");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm85_que("TMMSM85");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm89("TMMSM89");
	int   blkNum;

	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);

	CDbCommand cmd_inq(conn);

	blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MMLCSND");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TABLE_NAME");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_KEY"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PRIMARY_KEY");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_DATA"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PRIMARY_DATA");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_NO"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WEIGH_NO");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CODE");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SHIP_NAME"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SHIP_NAME");
	}

	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TARE_WT"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_DECIMAL, "TARE_WT");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("GROSS_WT"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_DECIMAL, "GROSS_WT");
	}
	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STOCK_WT"))
	{
		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_DECIMAL, "STOCK_WT");
	}
	/*if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_NO"))
	{
	bcls_rec->Tables["MMLCSND"].Columns.Add(DT_DECIMAL, "WEIGH_NO");
	}*/

	try
	{
		try{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
		//冲销重量
		stock_wt_1 = bcls_rec->Tables[0].Rows[0]["STOCK_WT_1"].ToDecimal();


		//--------------------------------
		//获取传入参数
		Log::Info("", __FUNCTION__, "stock_wt =[{0}]", mat_code);
		Log::Info("", __FUNCTION__, "stock_wt_1 =[{0}]", stock_wt_1);
		if (mat_code.Trim() == "")
		{
			sprintf(s.msg, "物料代码不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (0> stock_wt_1)
		{
			sprintf(s.msg, "冲销重量不能小于0");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_ZXH.NEXTVAL),4,0) FROM DUAL ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			SeqNo1 = cmd_inq.GetString(1).Trim();

		}
		cmd_inq.Close();
		CString weighno_c = "SC" + datetime + SeqNo1;

		sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_ZXH1.NEXTVAL),4,0) FROM DUAL ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			SeqNo1 = cmd_inq.GetString(1).Trim();

		}
		cmd_inq.Close();
		CString dh = "S2S" + datetime.Substring(0, 8) + SeqNo1;

		if (epex.Initialize("21C006") < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (epex.SetValue("DEAL_FLAG", 0, "D") < 0 ||
			epex.SetValue("DATA_ID", 0, dh) < 0 ||
			epex.SetValue("WORK_DATE", 0, datetime.Substring(0, 8)) < 0 ||
			epex.SetValue("DST_STOCK_CODE", 0, "6241") < 0 ||
			epex.SetValue("SRC_STOCK_CODE", 0, "6241") < 0 ||
			epex.SetValue("MAT_CODE", 0, mat_code) < 0 ||
			epex.SetValue("WEIGH_NO", 0, weighno_c) < 0 ||
			epex.SetValue("STOCK_WT", 0, stock_wt_1 / 1000) < 0 ||
			epex.SetValue("SETTLEMENT_WT", 0, stock_wt_1 / 1000) < 0 ||
			epex.SetValue("BACK1", 0, "EGF2"))
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

		tmmsm50["MAT_CODE"] = mat_code;
		tmmsm50.Query("MAT_CODE");
		tmmsm89["MAT_CODE"] = mat_code;
		tmmsm89["MAT_NAME"] = tmmsm50["MAT_NAME"];
		tmmsm89["WEIGH_NO"] = weighno_c;
		tmmsm89["EVENT_CODE"] = "IN";
		tmmsm89["EVENT_DESC"] = "一般进厂";
		tmmsm89["EVENT_NAME"] = "自循环废钢冲销";
		tmmsm89["REC_CREATOR"] = s.userid;
		tmmsm89["REC_CREATE_TIME"] = datetime;
		tmmsm89["RECEIVE_DATA_TIME"] = datetime;
		tmmsm89["STOCK_WT"] = -stock_wt_1;
		tmmsm89["BUNKER_NO"] = " ";
		bcls_rec_tmmsm89_log.Tables[0].Rows.Clear();
		bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
		bcls_rec_tmmsm89_log.Tables[0].Rows[0].Merge(tmmsm89);
		doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//给物流/资源/铁区-汽运/火车采购进厂卸货确认 
		//1、直供物料确认卸货时给物流系统发送电文21A001 - 汽运采购进厂卸货确认，同时按照物料编码区分给铁区系统发送电文21B005 - 卸车确认 或者 资源系统发送电文21C011 - 采购进厂卸货确认信息
		//2、配送物料及调拨物料确认卸货时，给物流系统发送电文21A010 - 汽运、铁路卸车确认，由物流系统给资源系统或者铁区系统转发卸车电文



	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
