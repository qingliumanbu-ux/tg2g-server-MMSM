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
BM2F_ENTERACE(mmsm81f3_ins)

int f_mmsm81f3_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CDecimal i_bill = 0;
	CDecimal unit_w = 0;
	CString s_remark = " ";
	CString s_factory = " ";
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
	CModel tmmsm81("TMMSM81");
	CModel tmmsm89("TMMSM89");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

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

		mat_wt = tmmsm85["STOCK_WT"].ToDecimal();
		cs_buckle_wt = tmmsm85["BUCKLE_WT"].ToDecimal();
		cs_back_code_5 = tmmsm85["BACK_CODE_5"].ToString();

		if (cs_back_code_5 == "0")
		{
			tmmsm85["BACK_CODE_5"] = " ";
		}

		cs_unload_point_code = tmmsm85["UNLOAD_POINT_CODE"].ToString();

		if (cs_unload_point_code.Trim() == "")
		{
			strcpy(s.msg, "卸点代码不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		bunker_no = tmmsm85["BUNKER_NO"].ToString();

		if (cs_back_code_5 == "1")
		{
			if (bunker_no.Trim() == "")
			{
				strcpy(s.msg, "料仓号不能为空!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}


		tmmsm85.Print();
		tmmsm81["WEIGH_NO"] = tmmsm85["WEIGH_NO"].ToString();
		tmmsm81.Query("WEIGH_NO");
		if (tmmsm81["BUNKER_NO"].ToString().Trim() != "")
		{
			strcpy(s.msg, "请查询数据，已收货数据不能再次收货！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm81["RECEIVING_STATUS"].ToString().Trim() == "D")
		{
			strcpy(s.msg, "请查询数据，选择计量单已作废不允许收货！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm81["BACK_CODE_2"].ToString().Trim() == "1")
		{
			strcpy(s.msg, "扣重或扣带皮超过毛重-20吨不允许收货！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm81["RECEIVING_STATUS"].ToString().Trim() == "J" || tmmsm81["RECEIVING_STATUS"].ToString().Trim() == "8" || tmmsm81["RECEIVING_STATUS"].ToString().Trim() == "9")
		{
			strcpy(s.msg, "请查询数据，该计量单已收货不能再次收货！");
			throw CApplicationException(-1, s.msg, log.Location);
		}



		if (cs_buckle_wt>0)
		{
			tmmsm81["BUCKLE_WT"] = cs_buckle_wt;
		}
		if (cs_back_code_5.Trim() != "")
		{
			tmmsm81["BACK_CODE_5"] = cs_back_code_5;
		}
		if (cs_unload_point_code.Trim() != "")
		{
			tmmsm81["UNLOAD_POINT_CODE"] = cs_unload_point_code;
		}
		if (cs_back_code_5 == "T" || cs_back_code_5 == "2")
		{
			tmmsm81["RECEIVING_STATUS"] = "J";
		}
		if (cs_back_code_5 == "1")
		{
			tmmsm81["RECEIVING_STATUS"] = "8";
		}
		if (cs_back_code_5 == "0" || cs_back_code_5 == " ")
		{
			tmmsm81["RECEIVING_STATUS"] = "9";
		}

		// DEDUCT_WGT 扣袋皮 NET_WT 净重 STOCK_WT 在库重量 SECOND_NET_WT 二次扣重 TARE_WT 皮重 GROSS_WT 毛重
		/*stock_wt = tmmsm81["STOCK_WT"].ToDecimal();*/
		//deduct_wgt = tmmsm81["DEDUCT_WGT"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("DEDUCT_WGT"))
		{
			deduct_wgt = bcls_rec->Tables[0].Rows[0]["DEDUCT_WGT"].ToDecimal();
		}
		if (bcls_rec->Tables[0].Columns.Contains("I_BILLTYPE"))
		{
			i_bill = bcls_rec->Tables[0].Rows[0]["I_BILLTYPE"].ToDecimal();
		}
		if (bcls_rec->Tables[0].Columns.Contains("REMARK"))
		{
			s_remark = bcls_rec->Tables[0].Rows[0]["REMARK"].ToString();
		}

		if (bcls_rec->Tables[0].Columns.Contains("MANUFAC_NAME"))
		{
			s_factory = bcls_rec->Tables[0].Rows[0]["MANUFAC_NAME"].ToString();
		}
		sqlstr = "SELECT CODE FROM TWMSMZD02 WHERE REC_CREATE_TIME =(SELECT MAX(REC_CREATE_TIME) REC_CREATE_TIME FROM TWMSMZD02 t WHERE CODE_CLASS = 'LCSHDZ')";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			unit_w = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();
		if (i_bill != 0 && unit_w != 0 && deduct_wgt == 0)
		{
			
			deduct_wgt = i_bill*unit_w;
		}
		Log::Info("", __FUNCTION__, "deduct_wgt=[{0}]", deduct_wgt);
		//second_net_wt = tmmsm81["SECOND_NET_WT"].ToDecimal();
		//2025-1-20 直供料 有净重入库时 不会再回皮 这里减去用户录的袋皮，下面三个字段保持一致
		if (deduct_wgt != 0)
		{
			if (tmmsm81["NET_WT"].ToDecimal() - deduct_wgt >= 0)
			{
				tmmsm81["NET_WT"] = tmmsm81["NET_WT"].ToDecimal() - deduct_wgt;
			}
			if (tmmsm81["STOCK_WT"].ToDecimal() - deduct_wgt >= 0)
			{
				tmmsm81["STOCK_WT"] = tmmsm81["STOCK_WT"].ToDecimal() - deduct_wgt;
			}
			if (tmmsm81["SECOND_NET_WT"].ToDecimal() - deduct_wgt >= 0)
			{
				tmmsm81["SECOND_NET_WT"] = tmmsm81["SECOND_NET_WT"].ToDecimal() - deduct_wgt;
			}
		}
		tmmsm81["I_BILLTYPE"] = i_bill;
		if (s_remark.GetLength()==0)
		{
			tmmsm81["REMARK"] =" ";
		}
		else
		{
			tmmsm81["REMARK"] = s_remark;
		}
		if (s_factory.GetLength() == 0)
		{
			tmmsm81["MANUFAC_NAME"] = " ";
		}
		else
		{
			tmmsm81["MANUFAC_NAME"] = s_factory;
		}
		tmmsm81["MAT_RCV_TIME"] = datetime;
		tmmsm81["DEDUCT_WGT"] = deduct_wgt;
		tmmsm81["BUNKER_NO"] = bunker_no;
		f_epep_get_shift_group("SMCP", tmmsm81["MAT_RCV_TIME"].ToString(), prod_shift_no, prod_shift_group, conn);
		tmmsm81["PROD_SHIFT_NO"] = prod_shift_no;
		tmmsm81["PROD_SHIFT_GROUP"] = prod_shift_group;
		Log::Info("", __FUNCTION__, "WEIGH_NO111 =[{0}],STOCK_WT =[{1}]", tmmsm81["WEIGH_NO"].ToString(), tmmsm85["STOCK_WT"].ToString());

		if (cs_back_code_5 == "T" || cs_back_code_5 == "2")
		{
			tmmsm81.Update("RECEIVING_STATUS,DEDUCT_WGT,MAT_RCV_TIME,PROD_SHIFT_NO,PROD_SHIFT_GROUP,BACK_CODE_5,UNLOAD_POINT_CODE,BUNKER_NO,NET_WT,STOCK_WT,I_BILLTYPE,REMARK,MANUFAC_NAME,SECOND_NET_WT", "WEIGH_NO");
			tmmsm89.CopyFrom(tmmsm81);
			tmmsm89["EVENT_CODE"] = "RETURN";
			tmmsm89["EVENT_DESC"] = "退货/无法卸货";
			tmmsm89["EVENT_NAME"] = "原料退货/无法卸货";
			tmmsm89["REC_CREATOR"] = s.userid;
			tmmsm89["REC_CREATE_TIME"] = datetime;
			tmmsm89["DEDUCT_WGT"] = deduct_wgt;
			tmmsm89["I_BILLTYPE"] = i_bill;
			if (s_remark.GetLength() == 0)
			{
				tmmsm89["REMARK"] = " ";
			}
			else
			{
				tmmsm89["REMARK"] = s_remark;
			}
			if (s_factory.GetLength() == 0)
			{
				tmmsm89["MANUFAC_NAME"] = " ";
			}
			else
			{
				tmmsm89["MANUFAC_NAME"] = s_factory;
			}
			bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
			bcls_rec_tmmsm89_log.Tables[0].Rows[0].Merge(tmmsm89);
			doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{
			tmmsm81.Update("RECEIVING_STATUS,DEDUCT_WGT,MAT_RCV_TIME,PROD_SHIFT_NO,PROD_SHIFT_GROUP,BACK_CODE_5,UNLOAD_POINT_CODE,BUNKER_NO,NET_WT,STOCK_WT,I_BILLTYPE,REMARK,MANUFAC_NAME,SECOND_NET_WT", "WEIGH_NO");
			tmmsm85.CopyFrom(tmmsm81);
			tmmsm60["BUNKER_NO"] = bunker_no;
			Log::Info("", __FUNCTION__, "BUNKER_NO111 =[{0}]", tmmsm60["BUNKER_NO"].ToString());
			tmmsm60.Query("BUNKER_NO");
			tmmsm60["STOCK_WT"] = tmmsm60["STOCK_WT"].ToDecimal() + tmmsm81["STOCK_WT"].ToDecimal();
			Log::Info("", __FUNCTION__, "STOCK_WT111 =[{0}]", tmmsm60["STOCK_WT"].ToDecimal());
			if (tmmsm60["UPPER_LIMIT_VALUE"].ToDecimal() != 0)
			{
				tmmsm60["RATE"] = tmmsm60["STOCK_WT"].ToDecimal() / tmmsm60["UPPER_LIMIT_VALUE"].ToDecimal();
			}
			Log::Info("", __FUNCTION__, "RATE =[{0}]", tmmsm60["RATE"].ToString());
			tmmsm60.Print();
			tmmsm60.Update("RATE,STOCK_WT", "BUNKER_NO");
			//
			tmmsm85["REC_CREATE_TIME"] = datetime;
			tmmsm85["TIME_INSTOCK"] = datetime;
			tmmsm85["REC_CREATOR"] = s.userid;
			//获取流水号
			sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO			= @tmmsm81.BUNKER_NO";
			cmd_inq.Parameters.Set("tmmsm81.BUNKER_NO", bunker_no);
			//分页获取
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm85["SEQ_NO"] = cmd_inq.GetDecimal(1) + 1;
			}
			cmd_inq.Close();

			tmmsm81["SEQ_NOW"] = tmmsm85["SEQ_NO"];
			/*tmmsm81["RAW_WEIGHT"] = tmmsm85["STOCK_WT"];*/

			tmmsm81.Update("SEQ_NOW", "WEIGH_NO");


			tmmsm85.TrimOrBlank();
			if (tmmsm85["BUNKER_NO"].ToString().Trim() == "")
			{
				tmmsm85["BUNKER_NO"] = bunker_no;
			}
			tmmsm85["MAT_CODE_LOT_NO"] = mat_code_lot_no;
			tmmsm85.Insert();
			tmmsm89.CopyFrom(tmmsm85);

			tmmsm89["EVENT_CODE"] = "IN";
			tmmsm89["EVENT_DESC"] = "一般进厂";
			tmmsm89["EVENT_NAME"] = "原料接收入库";
			tmmsm89["REC_CREATOR"] = s.userid;
			tmmsm89["REC_CREATE_TIME"] = datetime;
			tmmsm89["DEDUCT_WGT"] = deduct_wgt;
			tmmsm89["I_BILLTYPE"] = i_bill;
			if (s_remark.GetLength() == 0)
			{
				tmmsm89["REMARK"] = " ";
			}
			else
			{
				tmmsm89["REMARK"] = s_remark;
			}
			if (s_factory.GetLength() == 0)
			{
				tmmsm89["MANUFAC_NAME"] = " ";
			}
			else
			{
				tmmsm89["MANUFAC_NAME"] = s_factory;
			}
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
		bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "TMMSM81";
		bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_KEY"] = "WEIGH_NO";
		bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_DATA"] = tmmsm81["WEIGH_NO"];

		//AUART 类型  配送或调拨-C 直供-B
		if (tmmsm81["AUART"].ToString() == "B")
		{
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21A001";
			doFlag = f_mmsm_21a001_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21a001_snd失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//SYSTEM_ID_MAT 物料来源系统   资源-C 铁区-B
			tmmsm50["MAT_CODE"] = tmmsm81["MAT_CODE"];
			tmmsm50.Query("MAT_CODE");
			if (tmmsm50["SYSTEM_ID_MAT"].ToString() == "B")
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
		}


		//自循环废钢收料
		/*if (tmmsm50["MAT_TYPE"].ToString() == "2")
		{
		bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C006";
		bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_NO"] = tmmsm81["WEIGH_NO"];
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