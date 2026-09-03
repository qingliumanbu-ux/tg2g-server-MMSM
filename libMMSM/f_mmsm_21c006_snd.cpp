/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		SONGWEI
Version:    1.0
Date:		2023-12-15
Description:一资源系统-自循环废钢收料实绩
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_21c006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tcNO = " ";
	CString heatNo = " ";
	CString procNo = " ";
	CString lotNo = " ";
	CString stkNo = " ";
	CString mat_code = " ";
	CString ship_name = "";
	EPEX epex;
	CString datetime = " ";
	CString SeqNo1 = " ";
	CString deal_flag = "";

	CDecimal tare_wt = 0;
	CDecimal gross_wt = 0;
	CDecimal stock_wt = 0;

	/* 实体类定义 */
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm21("TMMSM21");
	CModel tmmsm81("TMMSM81");
	CModel tmmsm81_s("TMMSM81_S");
	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_85inq(conn);
	try
	{
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMLCSND 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
			tcNO = bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_NO"))
			tmmsm81["WEIGH_NO"] = bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_NO"].ToString().Trim();
		if (bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
			deal_flag = bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		
		
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//CString  dh = "S" + datetime + EPGetNextSeq("SQ_JLYLID", conn);
		sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_ZXH1.NEXTVAL),4,0) FROM DUAL ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			SeqNo1 = cmd_inq.GetString(1).Trim();

		}
		cmd_inq.Close();
		CString dh = "S2S" + datetime.Substring(0, 8) + SeqNo1;

		Log::Trace("", __FUNCTION__, "=WEIGH_NO= [{0}]", tmmsm81["WEIGH_NO"].ToString());

		tmmsm81_s["WEIGH_NO"] = tmmsm81["WEIGH_NO"];
		tmmsm81.Query("WEIGH_NO");

		//2025-1-24 补丁 更新实绩流水号，资源以 实绩流水号进行 更新
		if (deal_flag == "I")
		{
			if (1 == tmmsm81.QueryCount("WEIGH_NO"))
			{
				tmmsm81["COMPANY_CODE"] = dh;
				tmmsm81.Update("COMPANY_CODE", "WEIGH_NO");
			}
			if (1 == tmmsm81_s.QueryCount("WEIGH_NO"))
			{
				tmmsm81_s["COMPANY_CODE"] = dh;
				tmmsm81_s.Update("COMPANY_CODE", "WEIGH_NO");
			}
		}
		if (0 == tmmsm81.QueryCount("WEIGH_NO"))
		{
			
			tmmsm81_s.Query("WEIGH_NO");
			tmmsm81.CopyFrom(tmmsm81_s);
			strcpy(s.msg, "传入磅单号为空。");
			tmmsm81_s.Print();
			//throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (deal_flag=="I")
		{
			tare_wt = tmmsm81["TARE_WT"].ToDecimal();
			gross_wt = tmmsm81["GROSS_WT"].ToDecimal();
			stock_wt = tmmsm81["STOCK_WT"].ToDecimal();
		}
		else
		{
			if (bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
				mat_code = bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"].ToString().Trim();
			if (bcls_rec->Tables["MMLCSND"].Columns.Contains("TARE_WT"))
				tare_wt = bcls_rec->Tables["MMLCSND"].Rows[0]["TARE_WT"].ToDecimal();
			if (bcls_rec->Tables["MMLCSND"].Columns.Contains("GROSS_WT"))
				gross_wt = bcls_rec->Tables["MMLCSND"].Rows[0]["GROSS_WT"].ToDecimal();
			if (bcls_rec->Tables["MMLCSND"].Columns.Contains("STOCK_WT"))
				stock_wt = bcls_rec->Tables["MMLCSND"].Rows[0]["STOCK_WT"].ToDecimal();
			if (bcls_rec->Tables["MMLCSND"].Columns.Contains("SHIP_NAME"))
				ship_name = bcls_rec->Tables["MMLCSND"].Rows[0]["SHIP_NAME"].ToString().Trim();
			tmmsm81["MAT_CODE"] = mat_code;
			tmmsm81["SHIP_NAME"] = ship_name;
		}
		tmmsm81["WT_DATE_TIME"] = tmmsm81["TARE_TIME"];
		if (tmmsm81["WT_DATE_TIME"].ToString().GetLength()!=14) tmmsm81["WT_DATE_TIME"] = nowTime;
		if (tmmsm81["WORK_DATE"].ToString().GetLength() != 8) tmmsm81["WORK_DATE"] = nowTime.SubstringNE(0,8);
		tmmsm81["DST_STOCK_CODE"] = "6241";
		tmmsm81["SRC_STOCK_CODE"] = "6241";
		if (epex.Initialize(tcNO) < 0)
		{
			sprintf(s.msg, "电文初始化失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		{

			sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue(0, tmmsm81) < 0)
		{
			sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (epex.SetValue("TARE_WT", 0, tare_wt / 1000) < 0 ||
			epex.SetValue("GROSS_WT", 0, gross_wt / 1000) < 0 ||
			epex.SetValue("STOCK_WT", 0, stock_wt / 1000) < 0 ||
			epex.SetValue("SETTLEMENT_WT", 0, stock_wt / 1000) < 0 ||
			epex.SetValue("BACK1", 0, "EGF2"))
		{
			sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (epex.SetValue("DATA_ID", 0, tmmsm81["COMPANY_CODE"].ToString()) < 0)
		{

			sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
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
