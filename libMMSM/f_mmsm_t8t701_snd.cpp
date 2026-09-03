/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      REN XI MING
Version:     1.0
Date:        2023-12-12
Description: 要料信息申请
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_mmsm_t8t701_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	//电文号
	CString v_deal_div("");
	//电文变量
	EPEX epex(&s, conn);
	/* 实体类定义 */
	CModel tmmsm65("TMMSMNB01");
	CModel tmmsm50("TMMSM50");
	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//Log::Trace("", "", "", "[{0}]", bcls_rec->Tables["NEWMM_TABLE"].Rows.get_Count());
		if (bcls_rec->Tables[0].Columns.Contains("DEAL_FLAG"))
			v_deal_div = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString().Trim();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++) {
			/* 判断是否存在指定块 */
			tmmsm65.Reset();
			tmmsm65.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			/*tmmsm50["MAT_CODE"] = tmmsm65["MAT_CODE"];
			tmmsm50.Query("MAT_CODE");*/
			//if ("C" == tmmsm50["SYSTEM_ID_MAT"].ToString())
			//{
			//	if (epex.Initialize("T8T701") < 0)
			//	{
			//		CString ls = epex.GetMsg();
			//		strcpy(s.msg, "初始化电文T8T701失败，原因[" + ls + "]。");

			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}
			//	Log::Trace(" ", __FUNCTION__, "1111 =[{0}]", 1111);
			//	if (epex.SetValue(0, tmmsm65) < 0)
			//	{
			//		sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}

			//	if (epex.SetValue("DEAL_FLAG", 0, v_deal_div) < 0)
			//	{

			//		sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, log.Location);
			//	}

			//	//电文发送
			//	if (epex.SendTele() < 0)
			//	{
			//		sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
			//		throw CApplicationException(-1, s.msg, s.svc_name);
			//	}
			//	else
			//	{
			//		Log::Trace("", __FUNCTION__, "发送电文成功");
			//	}

			//	/* 释放 */
			//	/*tmmsm65["STATUS"] = v_deal_div;
			//	tmmsm65.Update("STATUS","PURCHASEDOCID");*/
			//	epex.Uninitialize();
			//}
			//else
			{
				//电文初始化
				if (epex.Initialize("T8T701") < 0)
				{
					CString ls = epex.GetMsg();
					strcpy(s.msg, "初始化电文T8T701失败，原因[" + ls + "]。");

					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				tmmsm65.Reset();
				tmmsm65.MergeFrom(bcls_rec->Tables[0].Rows[i]);

				if (epex.SetValue("MES_MM_GM_RFC", "msgtype", 0, "T8T701") < 0	||
					epex.SetValue("MES_MM_GM_RFC", "freeuse1", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "freeuse2", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "freeuse3", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "freeuse4", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "freeuse5", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "bsend_key", 0, 0) < 0		||
					epex.SetValue("MES_MM_GM_RFC", "it_name", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "it_rout1", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "it_rout2", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "isend_key", 0, 0) < 0		||
					epex.SetValue("MES_MM_GM_RFC", "ot_name", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "ot_rout1", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "ot_rout2", 0, " ") < 0		||
					epex.SetValue("MES_MM_GM_RFC", "osend_key", 0, 0) < 0		||
					epex.SetValue("mes_mm_gm", "mes_mm_gm_key", 0, 0) < 0		||
					epex.SetValue("mes_mm_gm", "send_key", 0, 0) < 0			||
					epex.SetValue("mes_mm_gm", "c_deliveryid", 0, tmmsm65["TICODE"].ToString()) < 0 ||			// 调拨单号
					epex.SetValue("mes_mm_gm", "c_productid", 0, tmmsm65["MAT_CODE"].ToString()) < 0 ||			// 物料编码
					epex.SetValue("mes_mm_gm", "c_productname", 0, tmmsm65["MAT_NAME"].ToString()) < 0 ||		// 物料名称
					epex.SetValue("mes_mm_gm", "c_qulitytraceid", 0, " ") < 0 ||								// 炉号
					epex.SetValue("mes_mm_gm", "c_batchid", 0, tmmsm65["LOT_NO"].ToString()) < 0 ||				// 批次号
					epex.SetValue("mes_mm_gm", "c_batchunit", 0, " ") < 0 ||									// 件次号
					epex.SetValue("mes_mm_gm", "c_senddept", 0, tmmsm65["SOURCE_WERKS"].ToString()) < 0 ||		// 发送工厂
					epex.SetValue("mes_mm_gm", "c_acceptdept", 0, tmmsm65["FACTORY_CODE"].ToString()) < 0 ||	// 接受工厂
					epex.SetValue("mes_mm_gm", "c_sendstock", 0, tmmsm65["SRC_LOC_CODE"].ToString()) < 0 ||		// 发送库房
					epex.SetValue("mes_mm_gm", "c_acceptstock", 0, tmmsm65["DST_LOC_CODE"].ToString()) < 0 ||	// 接受库房
					epex.SetValue("mes_mm_gm", "delivery_thickness", 0, 0) < 0 ||								// 厚度
					epex.SetValue("mes_mm_gm", "delivery_width", 0, 0) < 0 ||									// 宽度
					epex.SetValue("mes_mm_gm", "steelgrade", 0, " ") < 0 ||										// 钢种
					epex.SetValue("mes_mm_gm", "n_sendamount", 0, tmmsm65["MAT_WT"].ToDecimal()) < 0 ||			// 发送重量
					epex.SetValue("mes_mm_gm", "c_sendunit", 0, " ") < 0 ||										// 发送单位	
					epex.SetValue("mes_mm_gm", "n_acceptamount", 0, tmmsm65["MAT_WT"].ToDecimal()) < 0 ||		// 接收重量
					epex.SetValue("mes_mm_gm", "c_acceptunit", 0, " ") < 0 ||									// 接收单位
					epex.SetValue("mes_mm_gm", "c_isfreeze", 0, " ") < 0 ||										// 库存类型-
					epex.SetValue("mes_mm_gm", "c_stockspec", 0, " ") < 0 ||									// 特殊库存标识
					epex.SetValue("mes_mm_gm", "c_statesign", 0, v_deal_div) < 0 ||								// 调拨状态（1-未确认，2-接收，3-驳回）
					epex.SetValue("mes_mm_gm", "d_billdate", 0, " ") < 0 ||										// 制单日期
					epex.SetValue("mes_mm_gm", "d_operationdate", 0, " ") < 0 ||							// 业务日期 datetime
					epex.SetValue("mes_mm_gm", "c_senduserid", 0, " ") < 0 ||									// 送料人
					epex.SetValue("mes_mm_gm", "c_acceptuserid", 0, " ") < 0 ||									// 收料人
					epex.SetValue("mes_mm_gm", "c_group", 0, " ") < 0 ||										// 班组
					epex.SetValue("mes_mm_gm", "i_stockmode", 0, " ") < 0 ||									// 库存管理方式
					epex.SetValue("mes_mm_gm", "c_trucknum", 0, tmmsm65["C_TRUCKNUM"].ToString()) < 0 ||		// 车号
					epex.SetValue("mes_mm_gm", "t_accepttime", 0, " ") < 0 ||									// 收货确认时间
					epex.SetValue("mes_mm_gm", "c_ishotsend", 0, " ") < 0 ||									// 是否红送
					epex.SetValue("mes_mm_gm", "c_infreezestocksign", 0, " ") < 0 ||							// 是否入冻结库标志
					epex.SetValue("mes_mm_gm", "c_documentnumber", 0, " ") < 0 ||								// 物料凭证编号
					epex.SetValue("mes_mm_gm", "c_sendcostcenter", 0, " ") < 0 ||								// 发料工厂成本中心
					epex.SetValue("mes_mm_gm", "r_plantcostcenter", 0, " ") < 0 ||								// 接收工厂成本中心
					epex.SetValue("mes_mm_gm", "c_outstocksign", 0, " ") < 0 ||									// 出库标志
					epex.SetValue("mes_mm_gm", "c_instocksign", 0, " ") < 0 ||									// 入库标志
					epex.SetValue("mes_mm_gm", "c_reservecol3", 0, " ") < 0 ||									// 二次质检单号
					epex.SetValue("mes_mm_gm", "c_movetype", 0, " ") < 0 ||										// 移动类型
					epex.SetValue("mes_mm_gm", "c_qualitygrade", 0, " ") < 0 ||									// 质量等级
					epex.SetValue("mes_mm_gm", "c_isachievefinish", 0, " ") < 0 ||								// 是否收料入库
					epex.SetValue("mes_mm_gm", "c_stockid", 0, " ") < 0 ||										// 库存制单主键
					epex.SetValue("mes_mm_gm", "c_wastetype", 0, " ") < 0 ||									// 废钢类型
					epex.SetValue("mes_mm_gm", "c_sheetid", 0, " ") < 0 ||										// 单据分组号
					epex.SetValue("mes_mm_gm", "c_batchgroupid", 0, " ") < 0 ||									// 批次分组号
					epex.SetValue("mes_mm_gm", "c_goupblock", 0, " ") < 0 ||									// 铁水样本编码
					epex.SetValue("mes_mm_gm", "s_selfuploadsign", 0, " ") < 0 ||								// 自动上传标志
					epex.SetValue("mes_mm_gm", "s_selfoutstocksign", 0, " ") < 0 ||								// 自动出库标志
					epex.SetValue("mes_mm_gm", "s_selfinstocksign", 0, " ") < 0 ||								// 自动入库标志
					epex.SetValue("mes_mm_gm", "c_closegatetime", 0, " ") < 0 ||								// 关门时间
					epex.SetValue("mes_mm_gm", "t_uploadtime", 0, " ") < 0 ||									// 上传时间
					epex.SetValue("mes_mm_gm", "t_outstocktime", 0, " ") < 0 ||									// 出库时间
					epex.SetValue("mes_mm_gm", "t_instocktime", 0, " ") < 0 ||									// 入库时间
					epex.SetValue("mes_mm_gm", "c_qulitytype", 0, " ") < 0 ||									// 质量成份
					epex.SetValue("mes_mm_gm", "c_salessign", 0, " ") < 0 ||									// 销售标志
					epex.SetValue("mes_mm_gm", "t_salescomfirmtime", 0, " ") < 0 ||								// 检配确认时间
					epex.SetValue("mes_mm_gm", "c_updatesign", 0, " ") < 0 ||									// 修改标志
					epex.SetValue("mes_mm_gm", "t_overruletime", 0, " ") < 0 ||									// 驳回时间
					epex.SetValue("mes_mm_gm", "n_sendcount", 0, 1) < 0 ||										// 发送数量
					epex.SetValue("mes_mm_gm", "c_sendcountunit", 0, " ") < 0 ||								// 发送数量计量单位
					epex.SetValue("mes_mm_gm", "n_acceptcount", 0, 0) < 0 ||									// 接收数量
					epex.SetValue("mes_mm_gm", "c_acceptcountunit", 0, " ") < 0 ||								// 接收数量计量单位
					epex.SetValue("mes_mm_gm", "c_orderid", 0, " ") < 0 ||										// 订单号
					epex.SetValue("mes_mm_gm", "c_purveyid", 0, " ") < 0 ||										// 供应商号
					epex.SetValue("mes_mm_gm", "c_sellid", 0, " ") < 0 ||										// 销售订单号
					epex.SetValue("mes_mm_gm", "c_sellitemid", 0, " ") < 0 ||									// 销售订单号行项目号
					epex.SetValue("mes_mm_gm", "c_uploadsign", 0, " ") < 0 ||									// 上传标志-
					epex.SetValue("mes_mm_gm", "i_year", 0, 2024) < 0 ||										// 年份
					epex.SetValue("mes_mm_gm", "i_month", 0, 10) < 0 ||											// 月份
					epex.SetValue("mes_mm_gm", "c_billuserid", 0, " ") < 0 ||									// 制单人
					epex.SetValue("mes_mm_gm", "d_requiredate", 0, " ") < 0 ||									// 需求日期
					epex.SetValue("mes_mm_gm", "n_applyamount", 0, 0) < 0 ||									// 申请重量[新炼钢接收重量]
					epex.SetValue("mes_mm_gm", "c_applyunit", 0, " ") < 0 ||									// 申请计量单位[新炼钢接收重量单位]
					epex.SetValue("mes_mm_gm", "c_ismeasure", 0, " ") < 0 ||									// 是否计量
					epex.SetValue("mes_mm_gm", "c_isinspection", 0, " ") < 0 ||									// 是否质检
					epex.SetValue("mes_mm_gm", "c_issurfaceinspection", 0, " ") < 0 ||							// 是否表面检查
					epex.SetValue("mes_mm_gm", "c_deliverytype", 0, " ") < 0 ||									// 调拨类型
					epex.SetValue("mes_mm_gm", "c_rowstate", 0, " ") < 0 ||										// 投料状态
					epex.SetValue("mes_mm_gm", "c_achieveid", 0, " ") < 0 ||									// 生产实绩号
					epex.SetValue("mes_mm_gm", "c_reservecol1", 0, " ") < 0 ||									// 新物料
					epex.SetValue("mes_mm_gm", "c_reservecol2", 0, " ") < 0 ||									// 一次质检单号
					epex.SetValue("mes_mm_gm", "i_reservecol3", 0, 0) < 0 ||									// 备用列
					epex.SetValue("mes_mm_gm", "i_reservecol4", 0, " ") < 0 ||									// 调拨类型（0-正常调拨，1-回退调拨）
					epex.SetValue("mes_mm_gm", "c_remark", 0, " ") < 0											// 备注
					)
				{
					sprintf(s.msg, "设置电文数据失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//电文发送
				if (epex.SendTele() < 0)
				{
					strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				else
				{
					Log::Trace("", __FUNCTION__, "发送电文成功");
				}
				Log::Trace(" ", __FUNCTION__, "1111 =[{0}]", 4444);
				/* 释放 */
				/*tmmsm65["STATUS"] = v_deal_div;
				tmmsm65.Update("STATUS","PURCHASEDOCID");*/
				epex.Uninitialize();
			}

		}



	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


