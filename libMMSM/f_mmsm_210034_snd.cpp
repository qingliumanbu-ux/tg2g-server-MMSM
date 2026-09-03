/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		MFJ
Version:    1.0
Date:		2024-01-18
Description:二切实绩接收 发送L4电文
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_210034_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tcNO = " ";
	CString v_mat_no = " ";
	CString primaryKey = " ";
	CString primaryData = " ";
	CString cs_tc_no = "";//电文号
	CString deal_flag = "";//处理标记
	EPEX epex;

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm33("TMMSM33");
	int snd_count = 0;//循环次数

	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("210034");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 210034 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		


		for (int i = bcls_rec->Tables["210034"].Rows.get_Count() - 1; i >= 0; i--)
		{

			cs_tc_no = "210034";
			//电文初始化
			if (epex.Initialize(cs_tc_no) < 0)
			{
				strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
				Log::Trace("", __FUNCTION__, "电文初始化失败");
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}

			if (epex.SetValue("bapiheader", "msgtype", 0, cs_tc_no) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm01.Reset();//将数据清空
			tmmsm01.MergeFrom(bcls_rec->Tables["210034"].Rows[i]);//获取数据
			//tmmsm01.Query();//查询不到数据，不会覆盖掉结构体中原本数据

			//因已删除子坯，查询不到，故取传入数据
			if (bcls_rec->Tables["210034"].Rows[i]["DEAL_FLAG"].ToString() == "D")
			{
				tmmsm01.MergeFrom(bcls_rec->Tables["210034"].Rows[i]);//获取数据
			}

			if (bcls_rec->Tables["210034"].Columns.Contains("DEAL_FLAG"))
				deal_flag = bcls_rec->Tables["210034"].Rows[i]["DEAL_FLAG"];

			tmmsm33["MAT_NO"] = tmmsm01["IN_MAT_NO"];
			tmmsm33.Query();//从33表获取部分数据


			/*if (epex.SetValue(0, tmmsm01) < 0)
			{
			sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
			throw CApplicationException(-1, s.msg, s.svc_name);
			}*/


			Log::Trace("", __FUNCTION__, "PROD_TIME[{0}]  ", tmmsm01["PROD_TIME"].ToString());

			if (epex.SetValue("tmmsm35", "deal_flag", 0, deal_flag) < 0				//处理标记
				|| epex.SetValue("tmmsm35", "prod_date", 0, tmmsm01["PROD_TIME"].ToString().Substring(0, 8)) < 0
				|| epex.SetValue("tmmsm35", "dev_code", 0, tmmsm01["DEV_CODE"].ToString()) < 0  //设备代码
				|| epex.SetValue("tmmsm35", "prod_group_no", 0, tmmsm01["PROD_SHIFT_GROUP"].ToString()) < 0//生产班组号
				|| epex.SetValue("tmmsm35", "prod_shift_no", 0, tmmsm01["PROD_SHIFT_NO"].ToString()) < 0  //生产班次号
				|| epex.SetValue("tmmsm35", "st_no", 0, tmmsm01["ST_NO"].ToString()) < 0	//内部钢种
				|| epex.SetValue("tmmsm35", "prev_mat_no", 0, tmmsm01["IN_MAT_NO"].ToString()) < 0	 //前材料号
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (
				epex.SetValue("tmmsm35_1", "sap_mat_no", snd_count, tmmsm01["BATCH"].ToString()) < 0	 //批次号
				|| epex.SetValue("tmmsm35_1", "mat_no", snd_count, tmmsm01["MAT_NO"].ToString()) < 0  //材料号
				//|| epex.SetValue("tmmsm35", "cast_seq", snd_count, tmmsm01["CAST_NO"].ToString()) < 0    //浇次号   (CHAR(10)
				|| epex.SetValue("tmmsm35_1", "heat_no", snd_count, tmmsm01["HEAT_NO"].ToString()) < 0     //熔炼号
				//|| epex.SetValue("tmmsm35", "strand_no",snd_count, tmmsm01["STRAND_NO"].ToString()) < 0  //流号
				|| epex.SetValue("tmmsm35_1", "ingot_code", snd_count, tmmsm01["INGOT_CODE"].ToString()) < 0 //锭型代码
				|| epex.SetValue("tmmsm35_1", "mat_act_width", snd_count, tmmsm01["MAT_ACT_WIDTH"].ToString()) < 0  //材料实际宽度
				|| epex.SetValue("tmmsm35_1", "mat_act_thick", snd_count, tmmsm01["MAT_ACT_THICK"].ToString()) < 0  //材料实际厚度
				|| epex.SetValue("tmmsm35_1", "mat_aim_len", snd_count, tmmsm01["MAT_TARG_THICK"].ToString()) < 0   //材料计划长度
				|| epex.SetValue("tmmsm35_1", "mat_act_len", snd_count, tmmsm01["MAT_ACT_LEN"].ToString()) < 0      //材料实际长度
				|| epex.SetValue("tmmsm35_1", "mat_act_wt", snd_count, tmmsm01["MAT_ACT_WT"].ToString()) < 0		  //材料实际重量
				|| epex.SetValue("tmmsm35_1", "prod_time", snd_count, tmmsm01["PROD_TIME"].ToString()) < 0		  //生产时刻
				//|| epex.SetValue("tmmsm35", "adjust_width_mark", snd_count, tmmsm01["ADJUST_WIDTH_MARK"].ToString()) < 0 //调宽标记
				//|| epex.SetValue("tmmsm35", "slab_head_width", snd_count, tmmsm01["SLAB_HEAD_WIDTH"].ToString()) < 0  //板坯头部宽度
				//|| epex.SetValue("tmmsm35", "slab_tail_width", snd_count, tmmsm01["SLAB_TAIL_WIDTH"].ToString()) < 0   //板坯尾部宽度
				//|| epex.SetValue("tmmsm35", "hot_send_div", snd_count, tmmsm01["HOT_SEND_FLAG"].ToString()) < 0			//热送标记
				//|| epex.SetValue("tmmsm35", "hot_charge_flag",snd_count, tmmsm01["HOT_CHARGE_FLAG"].ToString()) < 0	//热装标记
				//|| epex.SetValue("tmmsm35", "slab_place_code",snd_count, tmmsm01["SLAB_PLACE_CODE"].ToString()) < 0	//板坯位置代码
				|| epex.SetValue("tmmsm35_1", "pono_slab_1", snd_count, tmmsm01["PONO_SLAB_1"].ToString()) < 0			//命令板坯号1
				|| epex.SetValue("tmmsm35_1", "pono_slab_2", snd_count, tmmsm01["PONO_SLAB_2"].ToString()) < 0			//命令板坯号2
				|| epex.SetValue("tmmsm35_1", "pono_slab_3", snd_count, tmmsm01["PONO_SLAB_3"].ToString()) < 0			//命令板坯号3
				|| epex.SetValue("tmmsm35_1", "pono_slab_4", snd_count, tmmsm01["PONO_SLAB_4"].ToString()) < 0			//命令板坯号4
				|| epex.SetValue("tmmsm35_1", "pono_slab_5", snd_count, tmmsm01["PONO_SLAB_5"].ToString()) < 0			//命令板坯号5
				|| epex.SetValue("tmmsm35_1", "pono_slab_6", snd_count, tmmsm01["PONO_SLAB_6"].ToString()) < 0			//命令板坯号6
				|| epex.SetValue("tmmsm35_1", "pono_slab_7", snd_count, tmmsm01["PONO_SLAB_7"].ToString()) < 0			//命令板坯号7
				|| epex.SetValue("tmmsm35_1", "pono_slab_8", snd_count, tmmsm01["PONO_SLAB_8"].ToString()) < 0			//命令板坯号8
				|| epex.SetValue("tmmsm35_1", "pono_slab_9", snd_count, tmmsm01["PONO_SLAB_9"].ToString()) < 0			//命令板坯号9
				|| epex.SetValue("tmmsm35_1", "pono_slab_10", snd_count, tmmsm01["PONO_SLAB_10"].ToString()) < 0      	//命令板坯号10
				//|| epex.SetValue("tmmsm35", "product_flag",snd_count, tmmsm01["PRODUCT_FLAG"].ToString()) < 0       	//成品标记
				//|| epex.SetValue("tmmsm35", "accounting_date",snd_count, tmmsm01["RECV_MAT_TIME"].ToString()) < 0		 //记账日期
				//|| epex.SetValue("tmmsm35", "pre_heat_no",snd_count, tmmsm01["PONO"].ToString()) < 0)					//预定炉次号 
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.SetValue("tmmsm35", "group_leader", 0, " ") < 0    //班长
				|| epex.SetValue("tmmsm35", "operator", 0, " ") < 0    //操作者
				|| epex.SetValue("tmmsm35", "treat_id", 0, tmmsm33["PROC_NO"].ToString()) < 0    //处理号
				|| epex.SetValue("tmmsm35_1", "surface_level", snd_count, " ") < 0  //铸坯预判级别
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//snd_count++;//循环一次，次数加1

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
