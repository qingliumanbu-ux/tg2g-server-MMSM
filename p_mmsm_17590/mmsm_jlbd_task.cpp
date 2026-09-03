/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   herui
Version:    1.0
Date:     2024-1-23
Description: 一钢计量实绩接收
**************************************************/

#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm_jlbd_task)
BM2_FUNCTION_EXPORT
int f_mmsm_jlbd_task(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*系统日志类定义*/
	CTracer log(__FUNCTION__);

	// 程序内部变量
	int doFlag = 0;
	int	blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString cs_mmjl_seq_no = "";

	// 实体类定义
	CModel tmmsm81_rcv("TMMSM81_RCV");//计量单电文履历表
	CModel tmmsm81("TMMSM81");//原料计量单表
	CModel tmmsm85("TMMSM85");//原料库存表
	CModel tmmsm85s("TMMSM85");
	CModel tmmsm60("TMMSM60");//原料库存表
	CModel tmmsm81s("TMMSM81");//原料计量单表
	CModel twmsm61("TWMSM61");//交废计量单表

	// 数据库SQL操作字符串
	CString sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_ins(conn);
	CDbCommand cmd_upd(conn);

	try
	{
		// 获得当前时间
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", __FUNCTION__, "ZCHO_JLBD_RFC");
		bcls_ret->Tables.Add("ZCHO_JLBD_RFC");
		cmd_inq.SetCommandText("select * from tmmsm81_rcv t where t.call_flag = '0' order by REC_CREATE_TIME ");
		cmd_inq.ExecuteQuery(bcls_ret->Tables["ZCHO_JLBD_RFC"]);
		for (int i = 0; i < bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows.get_Count(); i++)
		{

			// 获得当前时间
			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

			tmmsm81_rcv["REC_CREATE_TIME"] =
				bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["REC_CREATE_TIME"].ToString();
			tmmsm81_rcv["WEIGH_NO"] = tmmsm81["WEIGH_NO"] =
				bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["WEIGH_NO"].ToString();

			Log::Trace("", __FUNCTION__, "REC_CREATE_TIME = [{0}]", tmmsm81_rcv["REC_CREATE_TIME"].ToString());
			Log::Trace("", __FUNCTION__, "WEIGH_NO = [{0}]", tmmsm81_rcv["WEIGH_NO"].ToString());

			tmmsm81["WEIGH_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["WEIGH_NO"].ToString();//磅单号

			//tmmsm81_rcv["WEIGH_NO"] = "1234567890";
			tmmsm81["TRUST_ID"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TRUST_ID"].ToString();//计量委托号
			tmmsm81["SHIP_NAME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SHIP_NAME"];//车号
			tmmsm81["MAT_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_CODE"];//物料代码
			tmmsm81["MAT_NAME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_NAME"];//物料名称

			Log::Trace("", __FUNCTION__, "TRUST_ID = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TRUST_ID"].ToString());
			Log::Trace("", __FUNCTION__, "SHIP_NAME = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SHIP_NAME"].ToString());
			Log::Trace("", __FUNCTION__, "MAT_CODE = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_CODE"].ToString());
			Log::Trace("", __FUNCTION__, "MAT_NAME = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_NAME"].ToString());
			//重量单位Kg
			tmmsm81["MEASURE_UNIT"] = "KG";
			tmmsm81["GROSS_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["GROSS_WT"].ToDecimal();//毛重
			tmmsm81["TARE_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TARE_WT"].ToDecimal();//皮重
			CDecimal net_wgt = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["NET_WT"].ToDecimal();//净重
			tmmsm81["NET_WT"] = net_wgt;
			if (net_wgt > 0)
			{
				tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
			}
			/*else
			{
				tmmsm81["STOCK_WT"] = tmmsm81["GROSS_WT"];
			}*/
			Log::Trace("", __FUNCTION__, "GROSS_WT = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["GROSS_WT"].ToString());
			Log::Trace("", __FUNCTION__, "TARE_WT = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TARE_WT"].ToString());
			Log::Trace("", __FUNCTION__, "NET_WT = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["NET_WT"].ToString());
			Log::Trace("", __FUNCTION__, "WEIGH_NO = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["WEIGH_NO"].ToString());

			tmmsm81["GROSS_TIME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["GROSS_TIME"].ToString();//毛重时间
			tmmsm81["TARE_TIME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TARE_TIME"].ToString();//过皮时间
			tmmsm81["BUCKLE_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUCKLE_WT"].ToDecimal();//扣重
			tmmsm81["BUCKLE_REMARK"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUCKLE_REMARK"].ToString();//扣重说明
			tmmsm81["SECOND_NET_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SECOND_NET_WT"].ToDecimal();//二次净重

			tmmsm81["PROJECT_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["PROJECT_NO"].ToString();//行项目号
			tmmsm81["VOUCHER_ID"] = tmmsm81["VOUCHER_CODE"] =
				bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["VOUCHER_CODE"].ToString();//凭证号

			tmmsm81["AUART"] =
				bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["AUART"].ToString();
			tmmsm81["BUSI_TYPE"] =
				bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUSI_TYPE"].ToString();

			Log::Trace("", __FUNCTION__, "凭证号 = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["VOUCHER_CODE"].ToString());

			tmmsm81["LOT_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["LOT_NO"].ToString();//批次号
			tmmsm81["MISSING_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MISSING_NO"].ToString();//实绩号
			Log::Trace("", __FUNCTION__, "实绩号 = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MISSING_NO"].ToString());
			tmmsm81["SRC_STOCK_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SRC_STOCK_CODE"].ToString();//发货单位代码
			tmmsm81["RECV_DEPT_NAME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["RECV_DEPT_NAME"].ToString();//收货单位
			tmmsm81["RECV_DEPT_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["RECV_DEPT_CODE"].ToString();//收货单位代码
			tmmsm81["LADE_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["LADE_CODE"].ToString();//装点代码
			tmmsm81["UNLOAD_POINT_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["UNLOAD_POINT_CODE"].ToString();//卸点代码
			Log::Trace("", __FUNCTION__, "卸点代码 = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["UNLOAD_POINT_CODE"].ToString());
			tmmsm81.TrimOrBlank();

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
			}
			else
			{
				//新建或更新计量表
				if (tmmsm81.QueryCount("WEIGH_NO") == 0)
				{
					tmmsm81s["WEIGH_NO"] = tmmsm81["WEIGH_NO"];

					//新建
					tmmsm81["REC_CREATE_TIME"] = tmmsm81["RECEIVE_DATA_TIME"] = datetime;

					tmmsm81["RECEIVING_STATUS"] = "0";
					tmmsm81.Insert();
				}
				else
				{
					tmmsm81s.Query("WEIGH_NO");
					if (tmmsm81s["FORM_EDIT_FLAG"].ToString() == "1")
					{
						strcpy(s.msg, "计量信息已上料不能进行扣重！");
						//throw CApplicationException(-1, s.msg, log.Location);
						
					}
					else{

						if (tmmsm81s["SECOND_NET_WT"].ToDecimal() > 0)
						{
							if (tmmsm81["NET_WT"].ToDecimal() - tmmsm81s["SECOND_NET_WT"].ToDecimal()>0)
							{
								tmmsm81["RAW_WEIGHT"] = tmmsm81["NET_WT"].ToDecimal() - tmmsm81s["SECOND_NET_WT"].ToDecimal();

								//更新(卸货前不更新毛重、扣重，只更新皮重、净重；卸货后只更新皮重然后入原料库)
								tmmsm81["REC_REVISE_TIME"] = tmmsm81["RECEIVE_DATA_TIME"] = datetime;
								tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
								CString str = "REC_REVISE_TIME,RECEIVE_DATA_TIME,TARE_WT,NET_WT,TARE_TIME,RAW_WEIGHT,SECOND_NET_WT,STOCK_WT,GROSS_WT";
								tmmsm81.Update(str, "WEIGH_NO");

							}
							else
							{
								strcpy(s.msg, "净重小于扣皮带重量无法扣重！");
								//throw CApplicationException(-1, s.msg, log.Location);

							}
						}
						else
						{
							tmmsm81["REC_REVISE_TIME"] = tmmsm81["RECEIVE_DATA_TIME"] = datetime;
							tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
							CString str = "REC_REVISE_TIME,RECEIVE_DATA_TIME,TARE_WT,NET_WT,TARE_TIME,SECOND_NET_WT,STOCK_WT,GROSS_WT";
							tmmsm81.Update(str, "WEIGH_NO");
						}

						if (tmmsm81s["BUNKER_NO"].ToString().Trim() != "")
						{
							tmmsm85["BUNKER_NO"] = tmmsm81s["BUNKER_NO"];
							tmmsm85["MAT_CODE"] = tmmsm81s["MAT_CODE"];
							tmmsm85["WEIGH_NO"] = tmmsm81s["WEIGH_NO"];
							tmmsm85["TARE_WT"] = tmmsm81["TARE_WT"];
							tmmsm85["NET_WT"] = tmmsm81["NET_WT"];
							tmmsm85["GROSS_WT"] = tmmsm81["GROSS_WT"];
							tmmsm85["STOCK_WT"] = tmmsm81["STOCK_WT"];

							if (tmmsm81s["SEQ_NOW"].ToDecimal()>0)
							{
								tmmsm85["SEQ_NO"] = tmmsm81s["SEQ_NOW"];
								tmmsm85s.CopyFrom(tmmsm85);
								tmmsm85.Update("TARE_WT,NET_WT,GROSS_WT,STOCK_WT", "SEQ_NO,BUNKER_NO,MAT_CODE,WEIGH_NO");
								tmmsm85s.Query("SEQ_NO,BUNKER_NO,MAT_CODE,WEIGH_NO");

							}
							else
							{
								tmmsm85s.CopyFrom(tmmsm85);

								if (tmmsm85s.QueryCount("BUNKER_NO,MAT_CODE,WEIGH_NO") == 1)
								{
									tmmsm85s.Query("BUNKER_NO,MAT_CODE,WEIGH_NO");
									tmmsm85.Update("TARE_WT,NET_WT,GROSS_WT,STOCK_WT", "BUNKER_NO,MAT_CODE,WEIGH_NO");
								}
								else
								{
									strcpy(s.msg, "库存数据异常请查证！");
									/*throw CApplicationException(-1, s.msg, log.Location);*/

								}
							}
							tmmsm60["BUNKER_NO"] = tmmsm81s["BUNKER_NO"];
							tmmsm60.Query("BUNKER_NO");
							if (tmmsm60["STOCK_WT"].ToDecimal()>(tmmsm85s["STOCK_WT"].ToDecimal() - tmmsm81["STOCK_WT"].ToDecimal()))
							{
								tmmsm60["STOCK_WT"] = tmmsm60["STOCK_WT"].ToDecimal() - tmmsm85s["STOCK_WT"].ToDecimal() + tmmsm81["STOCK_WT"].ToDecimal();
								tmmsm60.Update("STOCK_WT", "BUNKER_NO");
							}
							else
							{
								strcpy(s.msg, "库存量不够扣重！");
								//throw CApplicationException(-1, s.msg, log.Location);

							}

						}

					}

					

				}
			}
			Log::Trace("", __FUNCTION__, "REC_CREATE_TIME", tmmsm81_rcv["REC_CREATE_TIME"].ToString());
			Log::Trace("", __FUNCTION__, "REC_CREATE_TIME", tmmsm81_rcv["REC_CREATE_TIME"].ToString());
			Log::Trace("", __FUNCTION__, "REC_CREATE_TIME", tmmsm81_rcv["REC_CREATE_TIME"].ToString());

			tmmsm81_rcv["CALL_FLAG"] = "1";

			Log::Trace("", __FUNCTION__, "CALL_FLAG", tmmsm81_rcv["CALL_FLAG"].ToString());

			tmmsm81_rcv.Update("CALL_FLAG", "REC_CREATE_TIME,WEIGH_NO");
		}

		//Log::Trace("", __FUNCTION__, "ZCHO_JLBD_RFC");

		//bcls_ret->Tables.Add("ZCHO_JLBD_RFC");
		//cmd_inq.SetCommandText("select * from tmmsm81_rcv t where t.call_flag = '0'");
		//cmd_inq.ExecuteQuery(bcls_ret->Tables["ZCHO_JLBD_RFC"]);
		//for (int i = 0; i < bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows.get_Count(); i++)
		//{

		//	// 获得当前时间
		//	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//	tmmsm81_rcv["REC_CREATE_TIME"] =
		//		bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["REC_CREATE_TIME"].ToString();
		//	tmmsm81_rcv["WEIGH_NO"] = tmmsm81["WEIGH_NO"] =
		//		bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["WEIGH_NO"].ToString();

		//	Log::Trace("", __FUNCTION__, "REC_CREATE_TIME = [{0}]", tmmsm81_rcv["REC_CREATE_TIME"].ToString());
		//	Log::Trace("", __FUNCTION__, "WEIGH_NO = [{0}]", tmmsm81_rcv["WEIGH_NO"].ToString());

		//	tmmsm81["WEIGH_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["WEIGH_NO"].ToString();//磅单号

		//	//tmmsm81_rcv["WEIGH_NO"] = "1234567890";
		//	tmmsm81["TRUST_ID"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TRUST_ID"].ToString();//计量委托号
		//	tmmsm81["SHIP_NAME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SHIP_NAME"];//车号
		//	tmmsm81["MAT_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_CODE"];//物料代码
		//	tmmsm81["MAT_NAME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_NAME"];//物料名称

		//	Log::Trace("", __FUNCTION__, "TRUST_ID = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TRUST_ID"].ToString());
		//	Log::Trace("", __FUNCTION__, "SHIP_NAME = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SHIP_NAME"].ToString());
		//	Log::Trace("", __FUNCTION__, "MAT_CODE = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_CODE"].ToString());
		//	Log::Trace("", __FUNCTION__, "MAT_NAME = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MAT_NAME"].ToString());
		//	//重量单位Kg
		//	tmmsm81["MEASURE_UNIT"] = "KG";
		//	tmmsm81["GROSS_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["GROSS_WT"].ToDecimal();//毛重
		//	tmmsm81["TARE_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TARE_WT"].ToDecimal();//皮重
		//	CDecimal net_wgt = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["NET_WT"].ToDecimal();//净重
		//	tmmsm81["NET_WT"] = net_wgt;
		//	if (net_wgt > 0)
		//	{
		//		tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
		//	}
		//	else
		//	{
		//		tmmsm81["STOCK_WT"] = tmmsm81["GROSS_WT"];
		//	}
		//	Log::Trace("", __FUNCTION__, "GROSS_WT = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["GROSS_WT"].ToString());
		//	Log::Trace("", __FUNCTION__, "TARE_WT = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TARE_WT"].ToString());
		//	Log::Trace("", __FUNCTION__, "NET_WT = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["NET_WT"].ToString());
		//	Log::Trace("", __FUNCTION__, "WEIGH_NO = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["WEIGH_NO"].ToString());

		//	tmmsm81["GROSS_TIME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["GROSS_TIME"].ToString();//毛重时间
		//	tmmsm81["TARE_TIME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TARE_TIME"].ToString();//过皮时间
		//	tmmsm81["BUCKLE_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUCKLE_WT"].ToDecimal();//扣重
		//	tmmsm81["BUCKLE_REMARK"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUCKLE_REMARK"].ToString();//扣重说明
		//	tmmsm81["SECOND_NET_WT"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SECOND_NET_WT"].ToDecimal();//二次净重

		//	tmmsm81["PROJECT_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["PROJECT_NO"].ToString();//行项目号
		//	tmmsm81["VOUCHER_ID"] = tmmsm81["VOUCHER_CODE"] =
		//		bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["VOUCHER_CODE"].ToString();//凭证号

		//	tmmsm81["AUART"] =
		//		bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["AUART"].ToString();
		//	tmmsm81["BUSI_TYPE"] =
		//		bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["BUSI_TYPE"].ToString();

		//	Log::Trace("", __FUNCTION__, "凭证号 = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["VOUCHER_CODE"].ToString());

		//	tmmsm81["LOT_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["LOT_NO"].ToString();//批次号
		//	tmmsm81["MISSING_NO"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MISSING_NO"].ToString();//实绩号
		//	Log::Trace("", __FUNCTION__, "实绩号 = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["MISSING_NO"].ToString());
		//	tmmsm81["SRC_STOCK_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["SRC_STOCK_CODE"].ToString();//发货单位代码
		//	tmmsm81["RECV_DEPT_NAME"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["RECV_DEPT_NAME"].ToString();//收货单位
		//	tmmsm81["RECV_DEPT_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["RECV_DEPT_CODE"].ToString();//收货单位代码
		//	tmmsm81["LADE_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["LADE_CODE"].ToString();//装点代码
		//	tmmsm81["UNLOAD_POINT_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["UNLOAD_POINT_CODE"].ToString();//卸点代码
		//	Log::Trace("", __FUNCTION__, "卸点代码 = [{0}]", bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["UNLOAD_POINT_CODE"].ToString());
		//	tmmsm81.TrimOrBlank();

		//	tmmsm81["TRNP_MODE_CODE"] = bcls_ret->Tables["ZCHO_JLBD_RFC"].Rows[i]["TRNP_MODE_CODE"].ToString();//运输方式

		//	//判断是原料计量单还是交废钢计量单
		//	//VOUCHER_CODE
		//	if (tmmsm81["VOUCHER_CODE"].ToString().Substring(0, 4) == "21JF")
		//	{
		//		//select t.BACK11,t.MAT_WT,t.ARCHIVE_FLAG,t.PLAN_NO,t.PRACTICE_NO from TWMSM61 t
		//		twmsm61["BACK11"] = tmmsm81["GROSS_WT"]; //毛重
		//		twmsm61["MAT_WT"] = tmmsm81["NET_WT"]; //净重
		//		twmsm61["ARCHIVE_FLAG"] = "1"; //废钢标志
		//		twmsm61["PLAN_NO"] = tmmsm81["VOUCHER_CODE"]; //计划号/凭证号
		//		twmsm61["PRACTICE_NO"] = tmmsm81["MISSING_NO"]; //实绩号
		//		CString strUpdate = "BACK11,MAT_WT";
		//		CString strCondition = "ARCHIVE_FLAG,PLAN_NO,PRACTICE_NO";
		//		twmsm61.Update(strUpdate, strCondition);
		//	}
		//	else
		//	{
		//		//新建或更新计量表
		//		if (tmmsm81.QueryCount("WEIGH_NO") == 0)
		//		{
		//			//新建
		//			tmmsm81["REC_CREATE_TIME"] = tmmsm81["RECEIVE_DATA_TIME"] = datetime;
		//			tmmsm81.Insert();
		//		}
		//		else
		//		{
		//			//更新(卸货前不更新毛重、扣重，只更新皮重、净重；卸货后只更新皮重然后入原料库)
		//			tmmsm81["REC_REVISE_TIME"] = tmmsm81["RECEIVE_DATA_TIME"] = datetime;
		//			CString str = "REC_REVISE_TIME,RECEIVE_DATA_TIME,TARE_WT,NET_WT,TARE_TIME,SECOND_NET_WT";
		//			tmmsm81.Update(str, "WEIGH_NO");
		//		}
		//	}
		//	Log::Trace("", __FUNCTION__, "REC_CREATE_TIME", tmmsm81_rcv["REC_CREATE_TIME"].ToString());
		//	Log::Trace("", __FUNCTION__, "REC_CREATE_TIME", tmmsm81_rcv["REC_CREATE_TIME"].ToString());
		//	Log::Trace("", __FUNCTION__, "REC_CREATE_TIME", tmmsm81_rcv["REC_CREATE_TIME"].ToString());

		//	tmmsm81_rcv["CALL_FLAG"] = "1";

		//	Log::Trace("", __FUNCTION__, "CALL_FLAG", tmmsm81_rcv["CALL_FLAG"].ToString());

		//	tmmsm81_rcv.Update("CALL_FLAG", "REC_CREATE_TIME,WEIGH_NO");
		//}
	}
	catch (CDbException& ex)
	{
		tmmsm81_rcv["CALL_FLAG"] = "2";
		tmmsm81_rcv.Update("CALL_FLAG", "REC_CREATE_TIME,WEIGH_NO");

		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (const CApplicationException& ex)
	{
		tmmsm81_rcv["CALL_FLAG"] = "3";
		tmmsm81_rcv.Update("CALL_FLAG", "REC_CREATE_TIME,WEIGH_NO");

		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		tmmsm81_rcv["CALL_FLAG"] = "4";
		tmmsm81_rcv.Update("CALL_FLAG", "REC_CREATE_TIME,WEIGH_NO");

		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
