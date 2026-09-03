/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2023-11-13
Version:1.0
Description: 接收计量信息
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"

// service入口
BM2F_ENTERACE_TELE(cm_b02103_rcv)
int f_mmsm_updlc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_cm_b02103_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString busiType = "";
	CString divFlag = "";
	CModel tmmsm81("TMMSM81");
	CModel tmmsm81_1("TMMSM81");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm89("TMMSM89");
	CModel tmmsm85("TMMSM85");

	CDecimal stock_wt_dif = 0;
	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	bcls_rec_tmmsm89_log.Tables[0].Rows.Add();

	EIClass bcls_rec_updlc;
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
	bcls_rec_updlc.Tables[0].Rows.Add();


	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		divFlag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();
		
		busiType = bcls_rec->Tables[0].Rows[0]["LOGISTICS_TYPE"].ToString();
		if ("" == divFlag.Trim())
		{
			strcpy(s.msg, "处理标记不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm81.MergeFrom(bcls_rec->Tables[0].Rows[0]); 
		tmmsm81["BUSI_TYPE"] = bcls_rec->Tables[0].Rows[0]["LOGISTICS_TYPE"].ToString();
		tmmsm81["BUCKLE_WT"] = bcls_rec->Tables[0].Rows[0]["DEDUCTION_WT"].ToDecimal() * 1000;  //扣重
		tmmsm81["RAW_WEIGHT"] = tmmsm81["GROSS_WT"].ToDecimal() - tmmsm81["TARE_WT"].ToDecimal();//净重
		tmmsm81["MAT_NAME"] = bcls_rec->Tables[0].Rows[0]["MAT_CNAME"].ToString();
		

		if ("配送" == busiType)
		{
			tmmsm81["AUART"] = "C";
		}
		else if ("直供" == busiType)
		{
			tmmsm81["AUART"] = "B";
		}
		else if ("调拨" == busiType)
		{
			tmmsm81["AUART"] = "A";
		}
		tmmsm81["RECEIVE_DATA_TIME"] = datetime;
		tmmsm81["VOUCHER_ID"] = tmmsm81["PLAN_NO"];
		//质检批标记：无   质检批编辑状态：可编辑
		tmmsm81["COMBINE_YN"] = "0";
		tmmsm81["SEND_FLAG"] = "0";
		tmmsm81["FORM_EDIT_FLAG"] = "0"; 		
		tmmsm81["FACTORY_DIV"] = "LG1";
		
		if ("" == tmmsm81["MAT_CODE"].ToString().Trim())
		{
			strcpy(s.msg, "物料代码不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == tmmsm81["WORK_DATE"].ToString().Trim())
		{
			tmmsm81["WORK_DATE"] = datetime.Substring(0, 8);
		}
		tmmsm50["MAT_CODE"] = tmmsm81["MAT_CODE"];
		tmmsm50.Query("MAT_CODE");
		tmmsm81["MAT_TYPE"] = tmmsm50["MAT_TYPE"];

		if (tmmsm81.QueryCount("WEIGH_NO")==0)
		{
			tmmsm81["NET_WT"] = tmmsm81["NET_WT"].ToDecimal() * 1000;
			tmmsm81["GROSS_WT"] = tmmsm81["GROSS_WT"].ToDecimal() * 1000;
			tmmsm81["TARE_WT"] = tmmsm81["TARE_WT"].ToDecimal() * 1000;
			tmmsm81["RAW_WEIGHT"] = tmmsm81["GROSS_WT"].ToDecimal() - tmmsm81["TARE_WT"].ToDecimal();//净重		
			tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
			tmmsm81["SECOND_NET_WT"] = tmmsm81["NET_WT"];
			if (bcls_rec->Tables[0].Rows[0]["SETTLEMENT_WT"].ToDecimal() != 0)
			{
				tmmsm81["NET_WT"] = bcls_rec->Tables[0].Rows[0]["SETTLEMENT_WT"].ToDecimal() * 1000;
				tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
				tmmsm81["SECOND_NET_WT"] = tmmsm81["NET_WT"];
			}
			tmmsm81["RECEIVING_STATUS"] = "0";
			tmmsm81["REC_CREATE_TIME"] = datetime;
			tmmsm81["REC_CREATOR"] = s.userid;
			tmmsm81.TrimOrBlank();
			tmmsm81.Insert();
			
		}
		else
		{
			tmmsm81_1["WEIGH_NO"] = tmmsm81["WEIGH_NO"].ToString();
			tmmsm81_1.Query("WEIGH_NO");

			tmmsm81["NET_WT"] = tmmsm81_1["GROSS_WT"].ToDecimal() - tmmsm81_1["TARE_WT"].ToDecimal() - tmmsm81["BUCKLE_WT"].ToDecimal();
			tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
			tmmsm81["SECOND_NET_WT"] = tmmsm81["NET_WT"];

			//如果已经上料，则做下标记说明 			
			if (tmmsm81_1["FORM_EDIT_FLAG"].ToString() == "1")
			{
				tmmsm81["BACK_CODE_6"] = "已上料,只更新计量单信息,未更新库存信息;";
			}
			if (tmmsm81["BUCKLE_WT"].ToDecimal()< 0 || tmmsm81["BUCKLE_WT"].ToDecimal()>tmmsm81["GROSS_WT"].ToDecimal()*1000 - 2000)
			{
				tmmsm81["BACK_CODE_6"] = tmmsm81["BACK_CODE_6"].ToString() + "扣重超过毛重减20吨;";
				tmmsm81["BACK_CODE_2"] = "1";
				tmmsm81.Update("BACK_CODE_6,BACK_CODE_2,BUCKLE_WT", "WEIGH_NO");
			}
			else
			{
				tmmsm81["REC_REVISE_TIME"] = datetime;
				tmmsm81["RECEIVE_DATA_TIME"] = datetime;

				stock_wt_dif = tmmsm81["NET_WT"].ToDecimal() - tmmsm81_1["NET_WT"].ToDecimal();
				tmmsm81["BACK_CODE_6"] = " ";
				tmmsm81["BACK_CODE_2"] = " ";
				tmmsm81.Update("BACK_CODE_6,BACK_CODE_2,REC_REVISE_TIME,RECEIVE_DATA_TIME,BUCKLE_WT,NET_WT,STOCK_WT,SECOND_NET_WT", "WEIGH_NO");
			}

			//判断如果未上料且已经收货的料仓不为空
			if (tmmsm81_1["FORM_EDIT_FLAG"].ToString() != "1" && tmmsm81_1["BUNKER_NO"].ToString().Trim() != "" && stock_wt_dif != 0 && tmmsm81["BUCKLE_WT"].ToDecimal() >= 0 && tmmsm81["BUCKLE_WT"].ToDecimal()<tmmsm81["GROSS_WT"].ToDecimal()*1000 - 2000 && tmmsm81_1["RECEIVING_STATUS"].ToString() != "0")
			{
				sqlstr = " select * from tmmsm85"
					" where 1=1"
					" and mat_code =@mat_code"
					" and weigh_no =@weigh_no"
					" and bunker_no =@bunker_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_code", tmmsm81_1["MAT_CODE"].ToString());
				cmd_inq.Parameters.Set("weigh_no", tmmsm81_1["WEIGH_NO"].ToString());
				cmd_inq.Parameters.Set("bunker_no", tmmsm81_1["BUNKER_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm85.Reset();
					cmd_inq.Fetch(tmmsm85);
					tmmsm85["TARE_WT"] = tmmsm81["TARE_WT"].ToDecimal();
					tmmsm85["GROSS_WT"] = tmmsm81["GROSS_WT"].ToDecimal();
					tmmsm85["BUCKLE_WT"] = tmmsm81["BUCKLE_WT"].ToDecimal();
					tmmsm85["NET_WT"] = tmmsm81["NET_WT"].ToDecimal();
					tmmsm85["STOCK_WT"] = tmmsm85["STOCK_WT"].ToDecimal() + stock_wt_dif;
					tmmsm85.Update("STOCK_WT,TARE_WT,GROSS_WT,BUCKLE_WT,NET_WT", "SEQ_NO,BUNKER_NO,MAT_CODE,WEIGH_NO");

					tmmsm89.CopyFrom(tmmsm85);
					tmmsm89["STOCK_WT"] = stock_wt_dif;
					tmmsm89["EVENT_CODE"] = "IN";
					tmmsm89["EVENT_DESC"] = "一般进厂";
					tmmsm89["EVENT_NAME"] = "原料接收重量修正";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					
					bcls_rec_tmmsm89_log.Tables[0].Rows[0].Merge(tmmsm89);
					doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					bcls_rec_updlc.Tables[0].Rows[0]["BUNKER_NO"] = tmmsm85["BUNKER_NO"].ToString();
					doFlag = f_mmsm_updlc(&bcls_rec_updlc, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				cmd_inq.Close();
			}
		}

		
		
		


		//if (tmmsm81["BUCKLE_WT"].ToDecimal() > 0)
		//{
		//	/*tmmsm81["STOCK_WT"] = tmmsm81["STOCK_WT"].ToDecimal() - deduct_wgt; BUCKLE_2WT
		//	tmmsm81["NET_WT"] = tmmsm81["NET_WT"].ToDecimal() - deduct_wgt;*/

		//	// 判断是否第二次扣重 如果是第二次扣重 比较两次扣重大小 如果第二次扣重比第一次大 计算差值 如果第二次比第一次小不处理 
		//	if (tmmsm81_1["BUCKLE_WT"].ToDecimal() > 0)
		//	{
		//		if ( tmmsm81_1["BUCKLE_WT"].ToDecimal() > tmmsm81["BUCKLE_WT"].ToDecimal())
		//		{

		//		}
		//		else
		//		{
		//			if (tmmsm81_1["STOCK_WT"].ToDecimal()>0)
		//			{
		//				tmmsm81["STOCK_WT"] = tmmsm81_1["STOCK_WT"] - (tmmsm81["BUCKLE_WT"].ToDecimal() - tmmsm81_1["BUCKLE_WT"].ToDecimal());
		//			}
		//			if (tmmsm81_1["NET_WT"].ToDecimal()>0)
		//			{
		//				tmmsm81["NET_WT"] = tmmsm81_1["NET_WT"] - (tmmsm81["BUCKLE_WT"].ToDecimal() - tmmsm81_1["BUCKLE_WT"].ToDecimal());
		//			}
		//		}
		//		
		//	}
		//	else
		//	{
		//		if (tmmsm81_1["STOCK_WT"].ToDecimal() > 0)
		//		{
		//			if (tmmsm81_1["STOCK_WT"].ToDecimal() > tmmsm81["BUCKLE_WT"].ToDecimal())
		//			{
		//				tmmsm81["STOCK_WT"] = tmmsm81_1["STOCK_WT"].ToDecimal() - tmmsm81["BUCKLE_WT"].ToDecimal();
		//			}

		//		}
		//		if (tmmsm81_1["NET_WT"].ToDecimal() > 0)
		//		{
		//			if (tmmsm81_1["NET_WT"].ToDecimal() > tmmsm81["BUCKLE_WT"].ToDecimal())
		//			{
		//				tmmsm81["NET_WT"] = tmmsm81_1["NET_WT"].ToDecimal() - tmmsm81["BUCKLE_WT"].ToDecimal();
		//			}
		//		}
		//	}
		//	tmmsm81.Update("BUCKLE_2WT,STOCK_WT,NET_WT", "VOUCHER_ID");
		//}
		//else
		//{
		//	
		//	tmmsm81.Update("BUCKLE_2WT", "VOUCHER_ID");
		//}
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
