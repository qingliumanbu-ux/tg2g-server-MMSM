/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   herui
Version:    1.0
Date:     2024-1-23
Description: 一钢计量实绩接收
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT
int f_mmsm_updlc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21c006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm81_ins_h(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*系统日志类定义*/
	CTracer log(__FUNCTION__);

	// 程序内部变量
	int doFlag = 0;
	int	blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString cs_mmjl_seq_no = "";
	CDecimal buckle_wt = 0;
	CDecimal stock_wt_dif = 0;

	// 实体类定义
	CModel tmmsm81_rcv("TMMSM81_RCV");//计量单电文履历表
	CModel tmmsm81("TMMSM81");//原料计量单表
	CModel tmmsm85("TMMSM85");//原料库存表
	CModel tmmsm85s("TMMSM85");
	CModel tmmsm60("TMMSM60");//原料库存表
	CModel tmmsm81s("TMMSM81");//原料计量单表
	CModel twmsm61("TWMSM61");//交废计量单表
	CModel tmmsm89("TMMSM89");
	CDecimal second_net_wt = 0;

	// 数据库SQL操作字符串
	CString sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_ins(conn);
	CDbCommand cmd_upd(conn);

	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	bcls_rec_tmmsm89_log.Tables[0].Rows.Add();

	EIClass bcls_rec_updlc;
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
	bcls_rec_updlc.Tables[0].Rows.Add();

	try
	{
		// 获得当前时间
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		bcls_ret->Tables.Add("ZCHO_JLBD_RFC");
		cmd_inq.SetCommandText("select * from tmmsm81_rcv t where t.call_flag = '0' order by REC_CREATE_TIME ");
		cmd_inq.ExecuteQuery(bcls_ret->Tables["ZCHO_JLBD_RFC"]);
		for (int i = 0; i < bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows.get_Count(); i++)
		{ 
			// 获得当前时间
			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

			tmmsm81.Reset();
			tmmsm81s.Reset();
			tmmsm85.Reset();
			tmmsm81_rcv["REC_CREATE_TIME"] =bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["REC_CREATE_TIME"].ToString();
			tmmsm81_rcv["WEIGH_NO"] = tmmsm81["WEIGH_NO"] =	bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["WEIGH_NO"].ToString();
			tmmsm81["WEIGH_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["WEIGH_NO"].ToString();//磅单号
			tmmsm81["TRUST_ID"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TRUST_ID"].ToString();//计量委托号
			tmmsm81["SHIP_NAME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SHIP_NAME"];//车号
			tmmsm81["MAT_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_CODE"];//物料代码
			tmmsm81["MAT_NAME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_NAME"];//物料名称
		 //重量单位Kg
			tmmsm81["MEASURE_UNIT"] = "KG";
			tmmsm81["GROSS_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["GROSS_WT"].ToDecimal();//毛重
			tmmsm81["TARE_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TARE_WT"].ToDecimal();//皮重
			CDecimal net_wgt = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["NET_WT"].ToDecimal();//净重		
			
			tmmsm81["RAW_WEIGHT"] = tmmsm81["GROSS_WT"].ToDecimal() - tmmsm81["TARE_WT"].ToDecimal();//净重
			tmmsm81["BUCKLE_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUCKLE_WT"].ToDecimal();//扣重
			buckle_wt = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUCKLE_WT"].ToDecimal();//扣重
			tmmsm81["DEDUCT_WGT"] = buckle_wt;

			tmmsm81["GROSS_TIME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["GROSS_TIME"].ToString();//毛重时间
			tmmsm81["TARE_TIME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TARE_TIME"].ToString();//过皮时间
			
			
			tmmsm81["BUCKLE_REMARK"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUCKLE_REMARK"].ToString();//扣重说明
			tmmsm81["SECOND_NET_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SECOND_NET_WT"].ToDecimal();//二次净重
			second_net_wt = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SECOND_NET_WT"].ToDecimal();//二次净重

			tmmsm81["NET_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["NET_WT"].ToDecimal();//净重
			

			if (second_net_wt>0)
			{
				tmmsm81["NET_WT"] = second_net_wt;// 二次净重为净重 净重为进厂净重

			}

			tmmsm81["PROJECT_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["PROJECT_NO"].ToString();//行项目号
			tmmsm81["VOUCHER_ID"] = tmmsm81["VOUCHER_CODE"] =bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["VOUCHER_CODE"].ToString();//凭证号

			tmmsm81["AUART"] =bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["AUART"].ToString();
			tmmsm81["BUSI_TYPE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUSI_TYPE"].ToString();

			tmmsm81["LOT_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["LOT_NO"].ToString();//批次号
			tmmsm81["MISSING_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MISSING_NO"].ToString();//实绩号
			Log::Trace("", __FUNCTION__, "实绩号 = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MISSING_NO"].ToString());
			tmmsm81["SRC_STOCK_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SRC_STOCK_CODE"].ToString();//发货单位代码
			tmmsm81["RECV_DEPT_NAME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["RECV_DEPT_NAME"].ToString();//收货单位
			tmmsm81["RECV_DEPT_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["RECV_DEPT_CODE"].ToString();//收货单位代码
			tmmsm81["LADE_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["LADE_CODE"].ToString();//装点代码
			tmmsm81["UNLOAD_POINT_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["UNLOAD_POINT_CODE"].ToString();//卸点代码
			Log::Trace("", __FUNCTION__, "卸点代码 = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["UNLOAD_POINT_CODE"].ToString());
			

			tmmsm81["TRNP_MODE_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TRNP_MODE_CODE"].ToString();//运输方式
			
			//判断是原料计量单还是交废钢计量单
			//VOUCHER_CODE
			if (tmmsm81["VOUCHER_CODE"].ToString().Substring(0, 4) == "21JF")
			{
				//select t.BACK11,t.MAT_WT,t.ARCHIVE_FLAG,t.PLAN_NO,t.PRACTICE_NO from TWMSM61 t
				twmsm61["BACK11"] = tmmsm81["GROSS_WT"]; //毛重
				twmsm61["MAT_WT"] = tmmsm81["NET_WT"]; //净重
				twmsm61["ARCHIVE_FLAG"] = "1"; //废钢标志
				twmsm61["PLAN_NO"] = tmmsm81["VOUCHER_CODE"]; //计划号/凭证号
				twmsm61["PRACTICE_NO"] = tmmsm81["MISSING_NO"]; //实绩号
				CString strUpdate = "BACK11,MAT_WT";
				CString strCondition = "ARCHIVE_FLAG,PLAN_NO,PRACTICE_NO";
				twmsm61.Update(strUpdate, strCondition);

				sqlstr = " update WL_LOAD_RECORD_DET set MAT_ACT_WT = " + (twmsm61["MAT_WT"].ToDecimal()/1000).ToString() + " where SJ_NO = '" + twmsm61["PRACTICE_NO"].ToString() + "' ";
				Log::Trace("", "", "sqlstr", sqlstr);
				cmd_upd.SetCommandText(sqlstr);
				cmd_upd.ExecuteNonQuery();
				cmd_upd.Close();
			}
			else
			{
				if (tmmsm81["VOUCHER_CODE"].ToString().Substring(0, 4) == "21XH")
				{
					tmmsm81["AUART"] = "E";
					tmmsm81["BUSI_TYPE"] = "自循环废钢";
				}
				//新建或更新计量表
				if (tmmsm81.QueryCount("WEIGH_NO") == 0)
				{
					//新建
					tmmsm81["REC_CREATE_TIME"] = datetime;
					tmmsm81["RECEIVE_DATA_TIME"] = datetime;

					if (tmmsm81["NET_WT"].ToDecimal() >0)
					{
						tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
					}
					
					tmmsm81["RECEIVING_STATUS"] = "0";
					tmmsm81["FORM_EDIT_FLAG"] = "0";
					tmmsm81.TrimOrBlank();
					tmmsm81.Insert();
				}
				else
				{
					tmmsm81s["WEIGH_NO"] = tmmsm81["WEIGH_NO"];
					tmmsm81s.Query("WEIGH_NO");

					if (tmmsm81s["FORM_EDIT_FLAG"].ToString() == "1")
					{
						tmmsm81["BACK_CODE_6"] = "已上料,只更新计量单信息,未更新库存信息;";
					}
					if (tmmsm81["BUCKLE_WT"].ToDecimal()< 0 || tmmsm81["BUCKLE_WT"].ToDecimal()> tmmsm81["GROSS_WT"].ToDecimal()-2000)
					{
						tmmsm81["BACK_CODE_6"] = tmmsm81["BACK_CODE_6"].ToString() + "扣重超过毛重减20吨;";
						tmmsm81["BACK_CODE_2"] = "1";
						tmmsm81.Update("BACK_CODE_6,BACK_CODE_2,TRNP_MODE_CODE", "WEIGH_NO");
					}
					else
					{
						tmmsm81["REC_REVISE_TIME"] = datetime;
						//2024.7.30 初亮：接收数据时间是第一次数据接收时间
						tmmsm81["RECEIVE_DATA_TIME"] = datetime;
						tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
						stock_wt_dif = tmmsm81["NET_WT"].ToDecimal() - tmmsm81s["NET_WT"].ToDecimal();
						CString str = "REC_REVISE_TIME,DEDUCT_WGT,TRNP_MODE_CODE,TARE_WT,NET_WT,TARE_TIME,RAW_WEIGHT,SECOND_NET_WT,STOCK_WT,GROSS_WT,BACK_CODE_6";
						tmmsm81.Update(str, "WEIGH_NO");
					}

					

					//如果未上料则更新库存
					if (tmmsm81s["FORM_EDIT_FLAG"].ToString() != "1" && stock_wt_dif != 0 && tmmsm81s["RECEIVING_STATUS"].ToString() != "0" )
					{  
						//并且二次扣不能超过0到2吨
						if (buckle_wt >= 0 && buckle_wt< tmmsm81["GROSS_WT"].ToDecimal() - 2000)
						{
							if (tmmsm81s["BUNKER_NO"].ToString().Trim() != "")
							{
								tmmsm85["BUNKER_NO"] = tmmsm81s["BUNKER_NO"];
								tmmsm85["MAT_CODE"] = tmmsm81s["MAT_CODE"];
								tmmsm85["WEIGH_NO"] = tmmsm81s["WEIGH_NO"];
								tmmsm85["TARE_WT"] = tmmsm81["TARE_WT"];
								tmmsm85["GROSS_WT"] = tmmsm81["GROSS_WT"];
								tmmsm85["NET_WT"] = tmmsm81["NET_WT"];
								tmmsm85["STOCK_WT"] = tmmsm81["NET_WT"];  								

								if (tmmsm81s["SEQ_NOW"].ToDecimal()>0)
								{
									tmmsm85["SEQ_NO"] = tmmsm81s["SEQ_NOW"];									
									tmmsm85.Update("TARE_WT,NET_WT,GROSS_WT,STOCK_WT", "SEQ_NO,BUNKER_NO,MAT_CODE,WEIGH_NO");
									tmmsm85s.CopyFrom(tmmsm85);
									tmmsm85s.Query("SEQ_NO,BUNKER_NO,MAT_CODE,WEIGH_NO");

									tmmsm89.CopyFrom(tmmsm85s);
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

								}
								else
								{
									tmmsm85s.CopyFrom(tmmsm85);
									if (tmmsm85s.QueryCount("BUNKER_NO,MAT_CODE,WEIGH_NO") == 1)
									{
										tmmsm85s.Query("BUNKER_NO,MAT_CODE,WEIGH_NO");
										tmmsm85.Update("TARE_WT,NET_WT,GROSS_WT,STOCK_WT", "BUNKER_NO,MAT_CODE,WEIGH_NO");

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
										
									}
									else
									{
										strcpy(s.msg, "库存数据异常请查证！");
										/*throw CApplicationException(-1, s.msg, log.Location);*/
										//break;
									}
								}
								bcls_rec_updlc.Tables[0].Rows[0]["BUNKER_NO"] = tmmsm85s["BUNKER_NO"].ToString();
								doFlag = f_mmsm_updlc(&bcls_rec_updlc, bcls_ret, conn);
								if (doFlag < 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}

							}
						}
						
					}

					//自循环废钢收货，回皮后给资源发 自循环废钢收料实绩-21C006
					if (tmmsm81["VOUCHER_CODE"].ToString().SubstringNE(0, 4) == "21XH")
					{
						//609自循环物料装车，更新毛重、净重
						twmsm61["BACK11"] = tmmsm81["GROSS_WT"]; //毛重
						twmsm61["MAT_WT"] = tmmsm81["NET_WT"]; //净重
						twmsm61["ARCHIVE_FLAG"] = "2"; //废钢标志
						twmsm61["PLAN_NO"] = tmmsm81["VOUCHER_CODE"]; //计划号/凭证号
						twmsm61["PRACTICE_NO"] = tmmsm81["MISSING_NO"]; //实绩号
						CString strUpdate = "BACK11,MAT_WT";
						CString strCondition = "ARCHIVE_FLAG,PLAN_NO,PRACTICE_NO";
						twmsm61.Update(strUpdate, strCondition);

						//自循环废钢收料
						blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
						if (blkNum < 0)
						{
							bcls_rec->Tables.Add("MMLCSND");
						}
						if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
						{
							bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
						}
						if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_NO"))
						{
							bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WEIGH_NO");
						}
						if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
						{
							bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
						}
						bcls_rec->Tables["MMLCSND"].Rows.Add();
						bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C006";
						bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_NO"] = tmmsm81["WEIGH_NO"];
						bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "I";
						doFlag = f_mmsm_21c006_snd(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
						Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c006_snd失败-------");
						throw CApplicationException(-1, s.msg, log.Location);
						}


					}
					
				}
			}
			
			tmmsm81_rcv["CALL_FLAG"] = "1";
			tmmsm81_rcv.Update("CALL_FLAG", "REC_CREATE_TIME,WEIGH_NO");
		}

		
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (const CApplicationException& ex)
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
