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
//int f_mmsm_21a001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm_21a010_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm_21c011_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm_21c006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm_21b005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_mmsm_t8t701_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsmlcnb01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm81f3_nb_ins)

int f_mmsm81f3_nb_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	Log::Info("", __FUNCTION__, "进来了");

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
	CString datetime1 = "";
	CDecimal mat_wt = 0;
	CDecimal cs_buckle_wt = 0;
	CDecimal stock_wt = 0;
	CDecimal deduct_wgt = 0;
	CDecimal second_net_wt = 0;
	CString cs_back_code_5 = "";
	CString cs_unload_point_code = "";
	CString noin_net_wt = "";
	CString mat_code_lot_no = " ";
	CString prod_shift_no = "";
	CString prod_shift_group = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;
	CModel tmmsm50("TMMSM50");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm60("TMMSM60");
	CModel tmmsmnb01("TMMSMNB01");
	CModel tmmsmnb01_1("TMMSMNB01");
	CModel tmmsm89("TMMSM89");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	datetime1 = CDateTime::Now().ToString("yyyyMMddHHmmss");

	datetime1 = datetime1.Substring(0,8);
	try
	{

		Log::Info("", __FUNCTION__, "11111111");

		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables.Contains("MESSAGE"))
		{
			Log::Info("", __FUNCTION__, "MAT_CODE1 =[{0}]", bcls_rec->Tables["MESSAGE"].Rows.get_Count());
			tmmsm85.MergeFrom(bcls_rec->Tables["MESSAGE"].Rows[0]);

		}

		if (bcls_rec->Tables[0].Columns.Contains("NOIN_NET_WT"))
		{
			noin_net_wt = bcls_rec->Tables[0].Rows[0]["NOIN_NET_WT"].ToString().Trim();
		}
		Log::Info("", __FUNCTION__, "NOIN_NET_WT =[{0}]", noin_net_wt);


		tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		tmmsm50["MAT_CODE"] = tmmsm85["MAT_CODE"];
		if (tmmsm50.QueryCount("MAT_CODE") == 1)
		{
			tmmsm50.Query("MAT_CODE");
			mat_code_lot_no = tmmsm50["MAT_CODE_L2"].ToString() + "@" + tmmsm50["LOT_NO"].ToString();
			if (tmmsm50["MAT_SIMPLE_ENAME"].ToString().Trim() == "" || tmmsm50["MAT_TYPE"].ToString().Trim() == "")
			{
				strcpy(s.msg, "物料英文名称或是物料类型不能为空，请到物料主数据画面MMSM50S2N 进行维护!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{
			strcpy(s.msg, "物料代码信息不能为空，请到物料主数据画面MMSM50S2N 进行维护!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm85["STOCK_WT"] = tmmsm85["NET_WT"];

		mat_wt = tmmsm85["STOCK_WT"].ToDecimal();
		cs_buckle_wt = tmmsm85["BUCKLE_WT"].ToDecimal();
		cs_back_code_5 = tmmsm85["BACK_CODE_5"].ToString();

		if (cs_back_code_5 == "0")
		{
			tmmsm85["BACK_CODE_5"] = " ";
		}

		cs_unload_point_code = tmmsm85["UNLOAD_POINT_CODE"].ToString();

		/*if (cs_unload_point_code.Trim() == "")
		{
			strcpy(s.msg, "卸点代码不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		bunker_no = tmmsm85["BUNKER_NO"].ToString();

		if (bunker_no.Trim() == "")
		{
			strcpy(s.msg, "料仓号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm85.Print();
		tmmsmnb01["TICODE"] = bcls_rec->Tables[0].Rows[0]["TICODE"].ToString().Trim();
		tmmsmnb01["MAT_CODE"] = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		tmmsmnb01_1["TICODE"] = bcls_rec->Tables[0].Rows[0]["TICODE"].ToString().Trim();
		tmmsmnb01_1["MAT_CODE"] = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		tmmsmnb01.Query("TICODE,MAT_CODE");
		tmmsmnb01_1.Query("TICODE,MAT_CODE");
		//暂时注释
		/*if (tmmsmnb01["BUNKER_NO"].ToString().Trim() != "")
		{
			strcpy(s.msg, "请查询数据，已收货数据不能再次收货！");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		/*if (tmmsmnb01["RECEIVING_STATUS"].ToString().Trim() == "D")
		{
			strcpy(s.msg, "请查询数据，选择计量单已作废不允许收货！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsmnb01["BACK_CODE_2"].ToString().Trim() == "1")
		{
			strcpy(s.msg, "扣重或扣带皮超过毛重-20吨不允许收货！");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/



		if (cs_buckle_wt>0)
		{
			tmmsmnb01["BUCKLE_WT"] = cs_buckle_wt;
		}
		/*if (cs_back_code_5.Trim() != "")
		{
			tmmsmnb01["BACK_CODE_5"] = cs_back_code_5;
		}
		if (cs_unload_point_code.Trim() != "")
		{
			tmmsmnb01["UNLOAD_POINT_CODE"] = cs_unload_point_code;
		}
		if (cs_back_code_5 == "T" || cs_back_code_5 == "2")
		{
			tmmsmnb01["RECEIVING_STATUS"] = "J";
		}
		if (cs_back_code_5 == "1")
		{
			tmmsmnb01["RECEIVING_STATUS"] = "8";
		}
		if (cs_back_code_5 == "0" || cs_back_code_5 == " ")
		{
			tmmsmnb01["RECEIVING_STATUS"] = "9";
		}*/

		// DEDUCT_WGT 扣袋皮 NET_WT 净重 STOCK_WT 在库重量 SECOND_NET_WT 二次扣重 TARE_WT 皮重 GROSS_WT 毛重
		/*stock_wt = tmmsmnb01["MAT_WT"].ToDecimal();*/
		//deduct_wgt = tmmsmnb01["DEDUCT_WGT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("DEDUCT_WGT"))
		{
			deduct_wgt = bcls_rec->Tables[0].Rows[0]["DEDUCT_WGT"].ToDecimal();
		}

		//second_net_wt = tmmsmnb01["SECOND_NET_WT"].ToDecimal();
		/*	if (deduct_wgt !=0 )
		{
		if (tmmsmnb01["NET_WT"].ToDecimal() - deduct_wgt >=0)
		{
		tmmsmnb01["NET_WT"] = tmmsmnb01["NET_WT"].ToDecimal() - deduct_wgt;
		}
		if (tmmsmnb01["MAT_WT"].ToDecimal() - deduct_wgt>=0)
		{
		tmmsmnb01["MAT_WT"] = tmmsmnb01["MAT_WT"].ToDecimal() - deduct_wgt;
		}
		}*/

		tmmsmnb01["STATUS"] = "1";
		tmmsmnb01["MAT_RCV_TIME"] = datetime;
		tmmsmnb01["AFFIRM_TIME"] = datetime;
		tmmsmnb01["BUNKER_NO"] = bunker_no;
		f_epep_get_shift_group("SMCP", tmmsmnb01["MAT_RCV_TIME"].ToString(), prod_shift_no, prod_shift_group, conn);
		/*tmmsmnb01["PROD_SHIFT_NO"] = prod_shift_no;
		tmmsmnb01["PROD_SHIFT_GROUP"] = prod_shift_group;*/
		Log::Info("", __FUNCTION__, "WEIGH_NO111 =[{0}],STOCK_WT =[{1}]", tmmsmnb01["WEIGH_NO"].ToString(), tmmsm85["STOCK_WT"].ToString());

		/*if (cs_back_code_5 == "T" || cs_back_code_5 == "2")
		{
			tmmsmnb01.Update("STATUS,AFFIRM_TIME,MAT_RCV_TIME,BUNKER_NO", "WEIGH_NO");
		}
		else*/
		{
			tmmsmnb01.Update("STATUS,AFFIRM_TIME,MAT_RCV_TIME,BUNKER_NO", "TICODE,MAT_CODE");
			tmmsm85.CopyFrom(tmmsmnb01);
			tmmsm60["BUNKER_NO"] = bunker_no;
			Log::Info("", __FUNCTION__, "BUNKER_NO111 =[{0}]", tmmsm60["BUNKER_NO"].ToString());
			tmmsm60.Query("BUNKER_NO");
			tmmsm60["STOCK_WT"] = tmmsm60["STOCK_WT"].ToDecimal() + tmmsmnb01["MAT_WT"].ToDecimal();
			Log::Info("", __FUNCTION__, "STOCK_WT111 =[{0}]", tmmsm60["STOCK_WT"].ToDecimal());
			if (tmmsm60["UPPER_LIMIT_VALUE"].ToDecimal() != 0)
			{
				tmmsm60["RATE"] = tmmsm60["STOCK_WT"].ToDecimal() / tmmsm60["UPPER_LIMIT_VALUE"].ToDecimal();
			}
			Log::Info("", __FUNCTION__, "RATE =[{0}]", tmmsm60["RATE"].ToString());
			tmmsm60.Print();
			tmmsm60.Update("RATE,STOCK_WT", "BUNKER_NO");
			//
			tmmsm85["REC_CREATE_TIME"] = s.datetime;
			tmmsm85["TIME_INSTOCK"] = datetime;
			tmmsm85["REC_CREATOR"] = s.userid;
			//获取流水号
			sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO			= @tmmsmnb01.BUNKER_NO";
			cmd_inq.Parameters.Set("tmmsmnb01.BUNKER_NO", bunker_no);
			//分页获取
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm85["SEQ_NO"] = cmd_inq.GetDecimal(1) + 1;
			}
			cmd_inq.Close();

			tmmsmnb01["SEQ_NOW"] = tmmsm85["SEQ_NO"];
			/*tmmsmnb01["RAW_WEIGHT"] = tmmsm85["STOCK_WT"];*/

			tmmsmnb01.Update("SEQ_NOW", "WEIGH_NO");


			tmmsm85.TrimOrBlank();
			if (tmmsm85["BUNKER_NO"].ToString().Trim() == "")
			{
				tmmsm85["BUNKER_NO"] = bunker_no;
			}
			tmmsm85["MAT_CODE_LOT_NO"] = mat_code_lot_no;
			tmmsm85.Insert();
			tmmsm89.CopyFrom(tmmsm85);

			tmmsm89["EVENT_CODE"] = "NB";
			tmmsm89["EVENT_DESC"] = "南北互调";
			tmmsm89["EVENT_NAME"] = "原料接收入库";
			tmmsm89["REC_CREATOR"] = s.userid;
			tmmsm89["REC_CREATE_TIME"] = datetime;
			//tmmsm89["DEDUCT_WGT"] = deduct_wgt;
			bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
			bcls_rec_tmmsm89_log.Tables[0].Rows[0].Merge(tmmsm89);
			/*doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}*/


		}

		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMLCSND");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_NAME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_NAME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SYSTEM_ID_MAT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SYSTEM_ID_MAT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("IN_FACTORY_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "IN_FACTORY_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("OUT_FACTORY_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "OUT_FACTORY_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ADJUST_DT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "ADJUST_DT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ADJUST_WT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_DECIMAL, "ADJUST_WT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("REC_CREATOR"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "REC_CREATOR");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ADJUST_REASON"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "ADJUST_REASON");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DATA_RESOURCE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DATA_RESOURCE");
		}

		bcls_rec->Tables["MMLCSND"].Rows.Add();
		bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = tmmsm50["MAT_CODE"];
		bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_NAME"] = tmmsm50["MAT_NAME"];
		bcls_rec->Tables["MMLCSND"].Rows[0]["SYSTEM_ID_MAT"] = tmmsm50["SYSTEM_ID_MAT"];
		bcls_rec->Tables["MMLCSND"].Rows[0]["IN_FACTORY_CODE"] = "6240";
		bcls_rec->Tables["MMLCSND"].Rows[0]["OUT_FACTORY_CODE"] = "6241";
		bcls_rec->Tables["MMLCSND"].Rows[0]["ADJUST_DT"] = datetime1;
		bcls_rec->Tables["MMLCSND"].Rows[0]["ADJUST_WT"] = mat_wt;
		bcls_rec->Tables["MMLCSND"].Rows[0]["REC_CREATOR"] = s.userid;
		bcls_rec->Tables["MMLCSND"].Rows[0]["ADJUST_REASON"] = "南转北";
		bcls_rec->Tables["MMLCSND"].Rows[0]["DATA_RESOURCE"] = tmmsmnb01["WEIGH_NO"];

		doFlag = f_mmsmlcnb01_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", __FUNCTION__, "-------调用f_mmsmlcnb01_snd失败-------");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		EIClass inBlock_1000;

		inBlock_1000.Tables[0].Clear();
		tmmsmnb01_1.MergeTo(inBlock_1000.Tables[0]);

		if (!inBlock_1000.Tables[0].Columns.Contains("DEAL_FLAG"))
		{
			inBlock_1000.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
			inBlock_1000.Tables[0].Rows[0]["DEAL_FLAG"] = "2";
		}

		doFlag = f_mmsm_t8t701_snd(&inBlock_1000, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t8t701_snd失败-------");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		////AUART 类型  配送或调拨-C 直供-B
		//if (tmmsmnb01["AUART"].ToString() == "B")
		//{
		//	bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21A001";
		//	doFlag = f_mmsm_21a001_snd(bcls_rec, bcls_ret, conn);
		//	if (doFlag < 0)
		//	{
		//		Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21a001_snd失败-------");
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//	//SYSTEM_ID_MAT 物料来源系统   资源-C 铁区-B
		//	tmmsm50["MAT_CODE"] = tmmsmnb01["MAT_CODE"];
		//	tmmsm50.Query("MAT_CODE");
		//	if (tmmsm50["SYSTEM_ID_MAT"].ToString() == "B")
		//	{
		//		bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21B005";
		//		doFlag = f_mmsm_21b005_snd(bcls_rec, bcls_ret, conn);
		//		if (doFlag < 0)
		//		{
		//			Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21b005_snd失败-------");
		//			throw CApplicationException(-1, s.msg, log.Location);
		//		}
		//	}
		//	else
		//	{
		//		bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C011";
		//		doFlag = f_mmsm_21c011_snd(bcls_rec, bcls_ret, conn);
		//		if (doFlag < 0)
		//		{
		//			Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21C011_snd失败-------");
		//			throw CApplicationException(-1, s.msg, log.Location);
		//		}
		//	}
		//}
		//else
		//{
		//	bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21A010";
		//	doFlag = f_mmsm_21a010_snd(bcls_rec, bcls_ret, conn);
		//	if (doFlag < 0)
		//	{
		//		Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21a010_snd失败-------");
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//}


		//自循环废钢收料
		/*if (tmmsm50["MAT_TYPE"].ToString() == "2")
		{
		bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C006";
		bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_NO"] = tmmsmnb01["WEIGH_NO"];
		doFlag = f_mmsm_21c006_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c006_snd失败-------");
		throw CApplicationException(-1, s.msg, log.Location);
		}
		}*/

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
