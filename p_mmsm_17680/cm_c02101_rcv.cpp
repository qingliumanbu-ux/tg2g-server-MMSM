/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:songwei
Date:2023-11-28
Version:1.0
Description: 接收资源系统
物料移动实绩**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


// service入口
BM2F_ENTERACE_TELE(cm_c02101_rcv)
int f_mmsm_updlc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_cm_c02101_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlStr = "";
	CString busiType = "";
	CString divFlag = "";
	CModel tmmsm81("TMMSM81");
	CModel tmmsm81_1("TMMSM81");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm89("TMMSM89");
	CModel tmmsm85("TMMSM85");
	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal stock_wt_dif = 0;
	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	bcls_rec_tmmsm89_log.Tables[0].Rows.Add();

	EIClass bcls_rec_updlc;
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
	bcls_rec_updlc.Tables[0].Rows.Add();

	try
	{
		divFlag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();
		busiType = bcls_rec->Tables[0].Rows[0]["BUSI_TYPE"].ToString();
		if ("" == divFlag.Trim())
		{
			strcpy(s.msg, "处理标记不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm81.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if ("配送" == busiType)  tmmsm81["AUART"] = "C";
		else if ("直供" == busiType)  tmmsm81["AUART"] = "B";
		else if ("采购直供" == busiType)  tmmsm81["AUART"] = "B";
		else if ("采购转运" == busiType)  tmmsm81["AUART"] = "B";
		else if ("调拨" == busiType)  tmmsm81["AUART"] = "A";
		else if ("生产性回收" == busiType) tmmsm81["AUART"] = "1";
		else if ("非生产性回收" == busiType)
		{
			if (tmmsm81["DST_STOCK_CODE"].ToString() == "6240" || tmmsm81["DST_STOCK_CODE"].ToString() == "6241")
			{
				tmmsm81["AUART"] = "C";
			}
			else
			{
				tmmsm81["AUART"] = "2";
			}

		}
		else if ("生产性回收直供" == busiType)
		{
			if (tmmsm81["DST_STOCK_CODE"].ToString() == "6240" || tmmsm81["DST_STOCK_CODE"].ToString() == "6241")
			{
				tmmsm81["AUART"] = "C";
			}
			else
			{
				tmmsm81["AUART"] = "3";
			}
		}
		else if ("生产性回收直供(加工)" == busiType)
		{
			if (tmmsm81["DST_STOCK_CODE"].ToString() == "6240" || tmmsm81["DST_STOCK_CODE"].ToString() == "6241")
			{
				tmmsm81["AUART"] = "C";
			}
			else
			{
				tmmsm81["AUART"] = "7";
			}
		}
		else if ("非生产性回收直供" == busiType)
		{
			if (tmmsm81["DST_STOCK_CODE"].ToString() == "6240" || tmmsm81["DST_STOCK_CODE"].ToString() == "6241")
			{
				tmmsm81["AUART"] = "C";
			}
			else
			{
				tmmsm81["AUART"] = "4";
			}
		}
		else if ("循环物资回收" == busiType) tmmsm81["AUART"] = "5";
		else if ("回收" == busiType) tmmsm81["AUART"] = "6";
		else
		{
			strcpy(s.msg, "业务类型有误!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm81["RECEIVE_DATA_TIME"] = datetime;
		tmmsm81["VOUCHER_ID"] = tmmsm81["PLAN_NO"];
		//质检批标记：无   质检批编辑状态：可编辑
		tmmsm81["COMBINE_YN"] = "0";
		tmmsm81["SEND_FLAG"] = "0";
		tmmsm81["FORM_EDIT_FLAG"] = "0";
		tmmsm81["FACTORY_DIV"] = "LG1";
		tmmsm81["BUCKLE_WT"] = tmmsm81["BUCKLE_WT"].ToDecimal() * 1000;  //扣重


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

		if (tmmsm81.QueryCount("WEIGH_NO") == 0)
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
			tmmsm81["TRNP_MODE_CODE"] = "3";//运输方式
			tmmsm81["REC_CREATE_TIME"] = datetime;
			tmmsm81["REC_CREATOR"] = s.userid;
			tmmsm81.TrimOrBlank();
			tmmsm81.Insert();
		}
		else
		{

			tmmsm81_1["WEIGH_NO"] = tmmsm81["WEIGH_NO"].ToString();
			tmmsm81_1.Query("WEIGH_NO");
			if (tmmsm81_1["AUART"].ToString().Trim() == "B")
			{
				tmmsm81_1["LOT_NO"] = bcls_rec->Tables[0].Rows[0]["LOT_NO"].ToString();
				tmmsm81_1.Update("LOT_NO", "WEIGH_NO");

				//更新上料的，更新消耗的，更新履历
				if (tmmsm81_1["LOT_NO"].ToString().Trim() != "")
				{

					sqlstr = " update tmmsm85 set lot_no = @lot_no"
						" where 1=1"
						" and WEIGH_NO = @weigh_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("weigh_no", tmmsm81_1["WEIGH_NO"].ToString());
					cmd_inq.Parameters.Set("lot_no", tmmsm81_1["LOT_NO"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					sqlstr = " update tmmsm89 set lot_no = @lot_no"
						" where 1=1"
						" and WEIGH_NO = @weigh_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("weigh_no", tmmsm81_1["WEIGH_NO"].ToString());
					cmd_inq.Parameters.Set("lot_no", tmmsm81_1["LOT_NO"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					sqlstr = " update tmmsm2a_yl set lot_no = @lot_no"
						" where 1=1"
						" and WEIGH_NO = @weigh_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("weigh_no", tmmsm81_1["WEIGH_NO"].ToString());
					cmd_inq.Parameters.Set("lot_no", tmmsm81_1["LOT_NO"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					sqlstr = " update tmmsmgy08 set lot_no = @lot_no"
						" where 1=1"
						" and WEIGH_NO = @weigh_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("weigh_no", tmmsm81_1["WEIGH_NO"].ToString());
					cmd_inq.Parameters.Set("lot_no", tmmsm81_1["LOT_NO"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					sqlstr = " update tmmsm56 set lot_no = @lot_no"
						" where 1=1"
						" and WEIGH_NO = @weigh_no"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("weigh_no", tmmsm81_1["WEIGH_NO"].ToString());
					cmd_inq.Parameters.Set("lot_no", tmmsm81_1["LOT_NO"].ToString());
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}


			}
			else
			{
				if (tmmsm81_1["BUCKLE_WT"].ToDecimal() != tmmsm81["BUCKLE_WT"].ToDecimal())
				{
					stock_wt_dif = tmmsm81["BUCKLE_WT"].ToDecimal() - tmmsm81_1["BUCKLE_WT"].ToDecimal();

					tmmsm81["NET_WT"] = tmmsm81_1["NET_WT"] - stock_wt_dif;
					tmmsm81["STOCK_WT"] = tmmsm81_1["STOCK_WT"] - stock_wt_dif;
					tmmsm81["SECOND_NET_WT"] = tmmsm81_1["SECOND_NET_WT"] - stock_wt_dif;


					//如果已经上料，则做下标记说明 			
					if (tmmsm81_1["FORM_EDIT_FLAG"].ToString() == "1")
					{
						tmmsm81["BACK_CODE_6"] = "已上料,只更新计量单信息,未更新库存信息;";
					}
					if (tmmsm81["BUCKLE_WT"].ToDecimal()< 0 || tmmsm81["BUCKLE_WT"].ToDecimal()> tmmsm81["GROSS_WT"].ToDecimal() * 1000 - 2000)
					{
						tmmsm81["BACK_CODE_6"] = tmmsm81["BACK_CODE_6"].ToString() + "扣重超过毛重减20吨;";
						tmmsm81["BACK_CODE_2"] = "1";
						tmmsm81.Update("BACK_CODE_6,BACK_CODE_2,BUCKLE_WT", "WEIGH_NO");
					}
					else
					{
						if (tmmsm81_1["LOT_NO"].ToString().Trim() != "")
						{
							tmmsm81["LOT_NO"] = tmmsm81_1["LOT_NO"].ToString();
						} 						
						tmmsm81["REC_REVISE_TIME"] = datetime;
						tmmsm81["RECEIVE_DATA_TIME"] = datetime;
						tmmsm81["BACK_CODE_6"] = " ";
						tmmsm81["BACK_CODE_2"] = " ";
						tmmsm81.Update("BACK_CODE_6,BACK_CODE_2,REC_REVISE_TIME,BUCKLE_WT,STOCK_WT,NET_WT,SECOND_NET_WT,LOT_NO", "WEIGH_NO");
					}



					//判断如果未上料且已经收货的料仓不为空
					if (tmmsm81_1["FORM_EDIT_FLAG"].ToString() != "1" && tmmsm81_1["BUNKER_NO"].ToString().Trim() != "" && stock_wt_dif != 0 && tmmsm81["BUCKLE_WT"].ToDecimal() < tmmsm81["GROSS_WT"].ToDecimal() * 1000 - 2000 && tmmsm81_1["RECEIVING_STATUS"].ToString() != "0")
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
							Log::Trace("", __FUNCTION__, "===TARE_WT= [{0}]", tmmsm81["TARE_WT"].ToDecimal());
							cmd_inq.Fetch(tmmsm85);
							tmmsm85["TARE_WT"] = tmmsm81["TARE_WT"].ToDecimal();
							Log::Trace("", __FUNCTION__, "===11TARE_WT= [{0}]", tmmsm81["TARE_WT"].ToDecimal());
							tmmsm85["GROSS_WT"] = tmmsm81["GROSS_WT"].ToDecimal();
							tmmsm85["BUCKLE_WT"] = tmmsm81["BUCKLE_WT"].ToDecimal();
							tmmsm85["NET_WT"] = tmmsm81["NET_WT"].ToDecimal() - stock_wt_dif;
							tmmsm85["LOT_NO"] = tmmsm81["LOT_NO"];
							tmmsm85["STOCK_WT"] = tmmsm85["STOCK_WT"].ToDecimal() - stock_wt_dif;
							tmmsm85.Update("STOCK_WT,BUCKLE_WT,NET_WT,LOT_NO", "SEQ_NO,BUNKER_NO,MAT_CODE,WEIGH_NO");


							tmmsm89.CopyFrom(tmmsm85);
							tmmsm89["STOCK_WT"] = 0-stock_wt_dif;
							tmmsm89["EVENT_CODE"] = "IN";
							tmmsm89["EVENT_DESC"] = "一般进厂";
							tmmsm89["EVENT_NAME"] = "扣重接收重量修正";
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

					//如果已上料则补充履历
					if (tmmsm81_1["FORM_EDIT_FLAG"].ToString() == "1")
					{
						//地位料仓是否还有，如果有则更新
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
							tmmsm85["LOT_NO"] = tmmsm81["LOT_NO"];
							tmmsm85["STOCK_WT"] = tmmsm85["STOCK_WT"].ToDecimal() - stock_wt_dif;
							tmmsm85["NET_WT"] = tmmsm85["NET_WT"].ToDecimal() - stock_wt_dif;
							tmmsm85["SECOND_NET_WT"] = tmmsm85["SECOND_NET_WT"].ToDecimal() - stock_wt_dif;
							if (tmmsm85["STOCK_WT"].ToDecimal() <=0)  // 不够扣则删除
							{
								tmmsm85.Delete("SEQ_NO,BUNKER_NO,MAT_CODE,WEIGH_NO");								
							}
							else
							{
								
								tmmsm85.Update("STOCK_WT,BUCKLE_WT,SECOND_NET_WT,NET_WT,LOT_NO", "SEQ_NO,BUNKER_NO,MAT_CODE,WEIGH_NO");
							}							
						}
						cmd_inq.Close();

						//拆入更新的值
						tmmsm89.CopyFrom(tmmsm81_1);
						tmmsm89["STOCK_WT"] = 0 - stock_wt_dif;
						tmmsm89["EVENT_CODE"] = "IN";
						tmmsm89["EVENT_DESC"] = "一般进厂";
						tmmsm89["EVENT_NAME"] = "扣重接收重量修正";
						tmmsm89["REC_CREATOR"] = s.userid;
						tmmsm89["REC_CREATE_TIME"] = datetime;

						bcls_rec_tmmsm89_log.Tables[0].Rows[0].Merge(tmmsm89);
						doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

						bcls_rec_updlc.Tables[0].Rows[0]["BUNKER_NO"] = tmmsm81_1["BUNKER_NO"].ToString();
						doFlag = f_mmsm_updlc(&bcls_rec_updlc, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

					}


				}
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
		CString str = sqlStr + "\r\n" + ex.GetMsg();
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
