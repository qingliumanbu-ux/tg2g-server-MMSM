/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      李振
Version:     1.0
Date:        2023-11-20
Description: 制造入库
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_mmsmacsh_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);
	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm33("TMMSM33");
	CModel tpssm03("TPSSM03");
	//2024-02-05

	CString v_pono = "";
	CString cast_lot_no = "";
	CString deal_flag = "";//处理标记  N  D
	CString v_guide_dest = "";//指导去向
	CString v_guide_dest_snd = " ";//发送指导去向
	CDbCommand cmd_inq(conn);

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MMSMACSH");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSMACSH 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}



		for (int i = 0; i < bcls_rec->Tables["MMSMACSH"].Rows.get_Count(); i++)
		{
			cs_tc_no = "210033";
			//电文初始化
			if (epex.Initialize(cs_tc_no) < 0)
			{
				strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
				Log::Trace("", __FUNCTION__, "电文初始化失败");
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}

			tmmsm01.Reset();//将数据清空
			tmmsm01.MergeFrom(bcls_rec->Tables["MMSMACSH"].Rows[i]);//获取数据

			tmmsm33["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm33.Query();//从33表获取部分数据

			//2024-2-5  这里通过TMMSM01表（PONO）获取TPSSM03表里的CAST_LOT_NO

			if (tmmsm01["PONO_SLAB"].ToString().Trim() != "")
			{
				tpssm03["SLAB_NO"] = tmmsm01["PONO_SLAB"].ToString().Trim();
				tpssm03.Query("SLAB_NO");
				cast_lot_no = tpssm03["CAST_LOT_NO"];
			}

			deal_flag = bcls_rec->Tables["MMSMACSH"].Rows[i]["DEAL_FLAG"].ToString();

			if ("" == cast_lot_no.Trim())
			{
				v_pono = tmmsm01["PONO"];
				sqlstr = " select DISTINCT t.CAST_LOT_NO from TPSSM03 t WHERE 1 = 1 "
					" AND t.PONO =  @pono ";

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("pono", v_pono);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cast_lot_no = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
			}




			if (epex.SetValue(0, tmmsm01) < 0)
			{
				sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (epex.SetValue("bapiheader", "msgtype", 0, cs_tc_no) < 0
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			Log::Trace("", __FUNCTION__, "PROD_TIME[{0}]  ", tmmsm01["PROD_TIME"].ToString());
			if (tmmsm01["PROD_TIME"].ToString().Trim() == "")
			{
				if (epex.SetValue("tmmsm33", "prod_date", 0, " ") < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				if (epex.SetValue("tmmsm33", "prod_date", 0, tmmsm01["PROD_TIME"].ToString().Substring(0, 8)) < 0)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			//针对撤销收货时重量重置为0的情况
			if (tmmsm01["MAT_ACT_WT"].ToDecimal() == 0)
			{
				//没有收货时，先取称重量，再取理论量
				if (tmmsm01["MEASURE_WT"].ToDecimal() != 0)
				{
					if (epex.SetValue("tmmsm33", "mat_act_wt", 0, tmmsm01["MEASURE_WT"].ToString()) < 0) //材料实际重量
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else
				{
					if (epex.SetValue("tmmsm33", "mat_act_wt", 0, tmmsm01["MAT_WT"].ToString()) < 0) //材料实际重量
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				
			}
			else
			{
				if (epex.SetValue("tmmsm33", "mat_act_wt", 0, tmmsm01["MAT_ACT_WT"].ToString()) < 0) //材料实际重量
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			//2026.03.25 指导去向发送小代码对应文字
			if (tmmsm01["GUIDE_DEST"].ToString().Trim() != "")
			{
				v_guide_dest = tmmsm01["GUIDE_DEST"];
				sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TWMSMZD02 T WHERE 1=1 AND T.CODE_CLASS='WM02' AND T.CODE = @v_guide_dest ";

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("v_guide_dest", v_guide_dest);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					v_guide_dest_snd = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
				Log::Trace("", __FUNCTION__, "v_guide_dest_snd[{0}]  ", v_guide_dest_snd);
			}

			if (epex.SetValue("tmmsm33", "deal_flag", 0, deal_flag) < 0  //处理标记
				|| epex.SetValue("tmmsm33", "dev_code", 0, "A" + tmmsm01["DEV_CODE"].ToString()) < 0  //设备代码
				|| epex.SetValue("tmmsm33", "prod_group_no", 0, tmmsm01["PROD_SHIFT_GROUP"].ToString()) < 0//生产班组号
				|| epex.SetValue("tmmsm33", "prod_shift_no", 0, tmmsm01["PROD_SHIFT_NO"].ToString()) < 0  //生产班次号
				|| epex.SetValue("tmmsm33", "st_no", 0, tmmsm01["ST_NO"].ToString()) < 0	//内部钢种
				|| epex.SetValue("tmmsm33", "mat_no", 0, tmmsm01["MAT_NO"].ToString()) < 0  //材料号
				|| epex.SetValue("tmmsm33", "sap_mat_no", 0, tmmsm01["BATCH"].ToString()) < 0	 //批次号
				//|| epex.SetValue("tmmsm33", "cast_seq", 0, tmmsm01["CAST_NO"].ToString()) < 0    //浇次号   (CHAR(10)
				//2024-02-05
				|| epex.SetValue("tmmsm33", "cast_seq", 0, cast_lot_no) < 0    //浇次号   (CHAR(10)
				|| epex.SetValue("tmmsm33", "heat_seq_cast", 0, tmmsm01["CAST_DIV_NO"].ToString()) < 0    //连浇炉序号
				|| epex.SetValue("tmmsm33", "heat_no", 0, tmmsm01["HEAT_NO"].ToString()) < 0     //熔炼号
				|| epex.SetValue("tmmsm33", "strand_no", 0, tmmsm01["STRAND_NO"].ToString()) < 0  //流号
				|| epex.SetValue("tmmsm33", "ingot_code", 0, tmmsm01["INGOT_CODE"].ToString()) < 0 //锭型代码
				|| epex.SetValue("tmmsm33", "mat_act_width", 0, tmmsm01["MAT_ACT_WIDTH"].ToString()) < 0  //材料实际宽度
				|| epex.SetValue("tmmsm33", "mat_act_thick", 0, tmmsm01["MAT_ACT_THICK"].ToString()) < 0  //材料实际厚度
				|| epex.SetValue("tmmsm33", "mat_aim_len", 0, tmmsm33["MAT_TARG_LEN"].ToString()) < 0   //材料计划长度
				|| epex.SetValue("tmmsm33", "mat_act_len", 0, tmmsm01["MAT_ACT_LEN"].ToString()) < 0      //材料实际长度
				|| epex.SetValue("tmmsm33", "prod_time", 0, tmmsm01["PROD_TIME"].ToString()) < 0		  //生产时刻
				|| epex.SetValue("tmmsm33", "adjust_width_mark", 0, tmmsm01["ADJUST_WIDTH_MARK"].ToString()) < 0 //调宽标记
				|| epex.SetValue("tmmsm33", "slab_head_width", 0, tmmsm01["SLAB_HEAD_WIDTH"].ToString()) < 0  //板坯头部宽度
				|| epex.SetValue("tmmsm33", "slab_tail_width", 0, tmmsm01["SLAB_TAIL_WIDTH"].ToString()) < 0   //板坯尾部宽度
				|| epex.SetValue("tmmsm33", "hot_send_div", 0, tmmsm01["HOT_SEND_FLAG"].ToString()) < 0			//热送区分
				|| epex.SetValue("tmmsm33", "hot_charge_flag", 0, tmmsm01["HOT_CHARGE_FLAG"].ToString()) < 0	//热装标记
				|| epex.SetValue("tmmsm33", "slab_place_code", 0, tmmsm01["SLAB_PLACE_CODE"].ToString()) < 0	//板坯位置代码
				|| epex.SetValue("tmmsm33", "pono_slab_1", 0, tmmsm01["PONO_SLAB_1"].ToString()) < 0			//命令板坯号1
				|| epex.SetValue("tmmsm33", "pono_slab_2", 0, tmmsm01["PONO_SLAB_2"].ToString()) < 0			//命令板坯号2
				|| epex.SetValue("tmmsm33", "pono_slab_3", 0, tmmsm01["PONO_SLAB_3"].ToString()) < 0			//命令板坯号3
				|| epex.SetValue("tmmsm33", "pono_slab_4", 0, tmmsm01["PONO_SLAB_4"].ToString()) < 0			//命令板坯号4
				|| epex.SetValue("tmmsm33", "pono_slab_5", 0, tmmsm01["PONO_SLAB_5"].ToString()) < 0			//命令板坯号5
				|| epex.SetValue("tmmsm33", "pono_slab_6", 0, tmmsm01["PONO_SLAB_6"].ToString()) < 0			//命令板坯号6
				|| epex.SetValue("tmmsm33", "pono_slab_7", 0, tmmsm01["PONO_SLAB_7"].ToString()) < 0			//命令板坯号7
				|| epex.SetValue("tmmsm33", "pono_slab_8", 0, tmmsm01["PONO_SLAB_8"].ToString()) < 0			//命令板坯号8
				|| epex.SetValue("tmmsm33", "pono_slab_9", 0, tmmsm01["PONO_SLAB_9"].ToString()) < 0			//命令板坯号9
				//2026.03.19 借用命令板坯号10发送指导去向
				|| epex.SetValue("tmmsm33", "pono_slab_10", 0, v_guide_dest_snd) < 0      	//命令板坯号10
				|| epex.SetValue("tmmsm33", "product_flag", 0, tmmsm01["PRODUCT_FLAG"].ToString()) < 0       	//成品标记
				|| epex.SetValue("tmmsm33", "accounting_date", 0, tmmsm01["RECV_MAT_TIME"].ToString()) < 0		 //记账日期
				|| epex.SetValue("tmmsm33", "pre_heat_no", 0, tmmsm01["PONO"].ToString()) < 0)					//预定炉次号)       	
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.SetValue("tmmsm33", "group_leader", 0, " ") < 0    //班长
				|| epex.SetValue("tmmsm33", "operator", 0, " ") < 0    //操作者
				|| epex.SetValue("tmmsm33", "treat_id", 0, tmmsm33["PROC_NO"].ToString()) < 0    //处理号
				|| epex.SetValue("tmmsm33", "surface_level", 0, " ") < 0  //铸坯预判级别
				|| epex.SetValue("tmmsm33", "straighten_temp", 0, 0) < 0 //拉矫前温度
				|| epex.SetValue("tmmsm33", "mat_hot_quality", 0, "") < 0  //铸坯红坯质量
				|| epex.SetValue("tmmsm33", "qty", 0, 1) < 0			 //数量
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
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
			epex.Uninitialize();
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


