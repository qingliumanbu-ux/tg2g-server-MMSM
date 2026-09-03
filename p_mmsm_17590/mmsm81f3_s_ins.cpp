/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */
int f_mmsm_21a001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21a010_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21c011_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21c006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm81f3_s_ins)

int f_mmsm81f3_s_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int   blkNum;
	int doFlag = 0;
	CString tcNO = "";
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString bunker_no = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString datetime = "";
	CDecimal mat_wt = 0;
	CDecimal cs_buckle_wt = 0;
	CDecimal stock_wt = 0;
	CDecimal deduct_wgt = 0;
	CDecimal second_net_wt = 0;
	CString cs_back_code_5 = "";
	CString cs_unload_point_code = "";
	CString mat_code_lot_no = "";
	CString SeqNo1 = "";
	CString serial_number = "";
	CString prod_shift_no = "";
	CString prod_shift_group = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;
	CModel tmmsm50("TMMSM50");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm81_s("TMMSM81_S");
	CModel tmmsm89("TMMSM89");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	tmmsm50.Reset();
	tmmsm85.Reset();
	tmmsm60.Reset();
	tmmsm81_s.Reset();
	tmmsm89.Reset();


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


		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables.Contains("MESSAGE"))
		{
			Log::Info("", __FUNCTION__, "MAT_CODE1 =[{0}]", bcls_rec->Tables["MESSAGE"].Rows.get_Count());
			tmmsm85.MergeFrom(bcls_rec->Tables["MESSAGE"].Rows[0]);

		}
		Log::Info("", __FUNCTION__, "MAT_CODE1 =[{0}]", bcls_rec->Tables.get_Count());
		Log::Info("", __FUNCTION__, "MAT_CODE1 =[{0}]", bcls_rec->Tables[0].Rows.get_Count());

		tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		mat_wt = tmmsm85["STOCK_WT"].ToDecimal();
		if (mat_wt == 0)
		{
			tmmsm85["STOCK_WT"] = tmmsm85["NET_WT"];
		}
		Log::Info("", __FUNCTION__, "STOCK_WT =[{0}]", tmmsm85["STOCK_WT"].ToDecimal());
		Log::Info("", __FUNCTION__, "mat_wt111 =[{0}]", tmmsm85["NET_WT"].ToDecimal());
		cs_buckle_wt = tmmsm85["BUCKLE_WT"].ToDecimal();
		cs_back_code_5 = tmmsm85["BACK_CODE_5"].ToString();

		if (cs_back_code_5 == "0")
		{
			tmmsm85["BACK_CODE_5"] = " ";
		}

		cs_unload_point_code = tmmsm85["UNLOAD_POINT_CODE"].ToString();
		bunker_no = tmmsm85["BUNKER_NO"].ToString();

		if (bunker_no.Trim() == "")
		{
			strcpy(s.msg, "料仓号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		else{

		}

		//tmmsm85.Print();
		tmmsm81_s["WEIGH_NO"] = tmmsm85["WEIGH_NO"].ToString();
		Log::Info("", __FUNCTION__, "WEIGH_NO =[{0}]", tmmsm81_s["WEIGH_NO"].ToString());

		tmmsm81_s.Query("WEIGH_NO");
		/*	tmmsm81.Print();
		tmmsm85.Print();*/


		tmmsm50["MAT_CODE"] = tmmsm85["MAT_CODE"];
		if (tmmsm50.QueryCount("MAT_CODE") == 1)
		{
			tmmsm50.Query("MAT_CODE");
			mat_code_lot_no = tmmsm50["MAT_CODE_L2"].ToString() + "@" + tmmsm50["LOT_NO"].ToString();
		}


		tmmsm81_s["MAT_RCV_TIME"] = datetime;
		tmmsm81_s["BUNKER_NO"] = bunker_no;
		tmmsm81_s["REC_REVISE_TIME"] = datetime;
		f_epep_get_shift_group("SMCP", tmmsm81_s["MAT_RCV_TIME"].ToString(), prod_shift_no, prod_shift_group, conn);
		tmmsm81_s["PROD_SHIFT_NO"] = prod_shift_no;
		tmmsm81_s["PROD_SHIFT_GROUP"] = prod_shift_group;
		Log::Info("", __FUNCTION__, "WEIGH_NO111 =[{0}],STOCK_WT =[{1}]", tmmsm81_s["WEIGH_NO"].ToString(), tmmsm81_s["STOCK_WT"].ToDecimal());
		Log::Info("", __FUNCTION__, "cs_back_code_5 =[{0}]", cs_back_code_5);

		{
			tmmsm81_s.Update("REC_REVISE_TIME,RECEIVING_STATUS,BUCKLE_WT,MAT_RCV_TIME,PROD_SHIFT_NO,PROD_SHIFT_GROUP,BACK_CODE_5,UNLOAD_POINT_CODE,BUNKER_NO,STOCK_WT,NET_WT", "WEIGH_NO");
			tmmsm85.CopyFrom(tmmsm81_s);
			Log::Info("", __FUNCTION__, "STOCK_WT =[{0}]", tmmsm85["STOCK_WT"].ToDecimal());
			Log::Info("", __FUNCTION__, "NET_WT =[{0}]", tmmsm85["NET_WT"].ToDecimal());

			tmmsm60["BUNKER_NO"] = bunker_no;
			Log::Info("", __FUNCTION__, "BUNKER_NO111 =[{0}]", tmmsm60["BUNKER_NO"].ToString());
			tmmsm60.Query("BUNKER_NO");
			tmmsm60["STOCK_WT"] = tmmsm60["STOCK_WT"].ToDecimal() + tmmsm81_s["STOCK_WT"].ToDecimal();
			Log::Info("", __FUNCTION__, "STOCK_WT111 =[{0}]", tmmsm60["STOCK_WT"].ToDecimal());
			if (tmmsm60["UPPER_LIMIT_VALUE"].ToDecimal() != 0)
			{
				tmmsm60["RATE"] = tmmsm60["STOCK_WT"].ToDecimal() / tmmsm60["UPPER_LIMIT_VALUE"].ToDecimal();
			}
			Log::Info("", __FUNCTION__, "RATE =[{0}]", tmmsm60["RATE"].ToString());
			//tmmsm60.Print();
			tmmsm60.Update("RATE,STOCK_WT", "BUNKER_NO");
			//
			tmmsm85["REC_CREATE_TIME"] = s.datetime;
			tmmsm85["TIME_INSTOCK"] = datetime;
			tmmsm85["REC_CREATOR"] = s.userid;
			//获取流水号
			sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO			= @tmmsm81_s.BUNKER_NO";
			cmd_inq.Parameters.Set("tmmsm81_s.BUNKER_NO", bunker_no);
			//分页获取
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm85["SEQ_NO"] = cmd_inq.GetDecimal(1) + 1;
			}
			cmd_inq.Close();

			tmmsm81_s["SEQ_NOW"] = tmmsm85["SEQ_NO"];
			tmmsm81_s["WEIGH_NO"] = tmmsm85["WEIGH_NO"].ToString();
			/*tmmsm81["RAW_WEIGHT"] = tmmsm85["STOCK_WT"];*/

			tmmsm81_s.Update("SEQ_NOW", "WEIGH_NO");

			tmmsm85.TrimOrBlank();
			if (tmmsm85["BUNKER_NO"].ToString().Trim() == "")
			{
				tmmsm85["BUNKER_NO"] = bunker_no;
			}

			/*sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_ZXH.NEXTVAL),4,0) FROM DUAL ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
			SeqNo1 = cmd_inq.GetString(1).Trim();

			}
			cmd_inq.Close();

			serial_number = "S2S" + datetime.Substring(0,8) + SeqNo1;*/

			tmmsm85["MAT_CODE_LOT_NO"] = mat_code_lot_no;
			tmmsm85["TIME_INSTOCK"] = datetime;
			tmmsm85.Insert();
			tmmsm89.CopyFrom(tmmsm85);
			//tmmsm89["EVENT_DESC"] = "原料接收入库";
			tmmsm89["EVENT_CODE"] = "IN";
			tmmsm89["EVENT_DESC"] = "一般进厂";
			tmmsm89["EVENT_NAME"] = "自循环废钢进厂";
			tmmsm89["REC_CREATOR"] = s.userid;
			tmmsm89["REC_CREATE_TIME"] = datetime;
			bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
			bcls_rec_tmmsm89_log.Tables[0].Rows[0].Merge(tmmsm89);
			doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


		}
		//给物流/资源/铁区-汽运/火车采购进厂卸货确认 
		//1、直供物料确认卸货时给物流系统发送电文21A001 - 汽运采购进厂卸货确认，同时按照物料编码区分给铁区系统发送电文21B005 - 卸车确认 或者 资源系统发送电文21C011 - 采购进厂卸货确认信息
		//2、配送物料及调拨物料确认卸货时，给物流系统发送电文21A010 - 汽运、铁路卸车确认，由物流系统给资源系统或者铁区系统转发卸车电文
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
		bcls_rec->Tables["MMLCSND"].Rows.Add();
		bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "I";
		bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "TMMSM81_S";
		bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_KEY"] = "WEIGH_NO";
		bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_DATA"] = tmmsm81_s["WEIGH_NO"];

		//AUART 类型  配送或调拨-C 直供-B
		/*if (tmmsm81_s["AUART"].ToString() == "B")
		{
		bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21A001";
		doFlag = f_mmsm_21a001_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21a001_snd失败-------");
		throw CApplicationException(-1, s.msg, log.Location);
		}
		}
		else
		{
		bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21A010";
		doFlag = f_mmsm_21a010_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21a010_snd失败-------");
		throw CApplicationException(-1, s.msg, log.Location);
		}
		}*/

		//SYSTEM_ID_MAT 物料来源系统   资源-C 铁区-B
		tmmsm50["MAT_CODE"] = tmmsm81_s["MAT_CODE"];
		tmmsm50.Query("MAT_CODE");
		/*if (tmmsm50["SYSTEM_ID_MAT"].ToString() == "B")
		{
		bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21B005";
		doFlag = f_mmsm_21b005_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21b005_snd失败-------");
		throw CApplicationException(-1, s.msg, log.Location);
		}
		}
		else
		{
		bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C011";
		doFlag = f_mmsm_21c011_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21C011_snd失败-------");
		throw CApplicationException(-1, s.msg, log.Location);
		}
		}*/
		//自循环废钢收料
		if (tmmsm50["MAT_TYPE"].ToString() == "2")
		{
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C006";
			bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_NO"] = tmmsm81_s["WEIGH_NO"];
			doFlag = f_mmsm_21c006_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c006_snd失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		/*if (bcls_rec_tmmsm89_log.Tables[0].Rows.get_Count() > 0)
		{
		doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
		if (doFlag < 0)
		{
		throw CApplicationException(-1, s.msg, log.Location);
		}
		}*/
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
