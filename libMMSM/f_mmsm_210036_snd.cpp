/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   WANGSHULING
Version:    1.0
Date:     2023-12-26 11:17:56
Description: 【二炼北】锭坯精整、修磨实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

//电文发送头文件
#include "epex.h"

int f_mmsm_210036_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString tc_no = "210036";
	CString grindstone_type = "";//砂轮粒度
	CString grindstone_supplier = "";//砂轮厂家

	EPEX epex;

	CModel tmmsm34("TMMSM34");
	CModel tmmsm34_1("TMMSM34_1");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm33("TMMSM33");

	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	//2024-02-20
	CString deal_flag = "";//处理标记

	try{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("210036");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 210036 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		for (int i = 0; i < bcls_rec->Tables["210036"].Rows.get_Count(); i++){

			if (bcls_rec->Tables["210036"].Columns.Contains("DEAL_FLAG"))
				deal_flag = bcls_rec->Tables["210036"].Rows[i]["DEAL_FLAG"];


			Log::Trace("", "", "mend_flag={0}", bcls_rec->Tables["210036"].Rows.get_Count());
			if (epex.Initialize(tc_no) < 0){
				strcpy(s.msg, "电文初始化失败。");
				Log::Trace("", __FUNCTION__, "电文初始化失败[{0}]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			//bapiheader.msgtype
			//epex.SetValue("tmmsm3", "prod_date", 0, tmmsm01["PROD_TIME"].ToString().Substring(0, 8)) < 0
			if (epex.SetValue("bapiheader", "msgtype", 0, tc_no) < 0)
			{
				sprintf(s.msg, "电文压电文号失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//bapiheader.freeuse1-5  自由使用      34表中没有字段

			tmmsm34.Reset();
			tmmsm34.MergeFrom(bcls_rec->Tables["210036"].Rows[i]);

			/*
				日期：20240604 
				原因：如果34_1和34表都存在，这里会取34旧数据,所以判断一下，如果341里面有最新的，以341为准
			*/
			tmmsm34_1.Reset();
			tmmsm34_1.MergeFrom(bcls_rec->Tables["210036"].Rows[i]);
			if (!tmmsm34_1.Query())
			{
				tmmsm34.Query();
			}
			else
			{
				tmmsm34.CopyFrom(tmmsm34_1);
			}

			tmmsm01.Reset();
			tmmsm01.MergeFrom(bcls_rec->Tables["210036"].Rows[i]);
			tmmsm33.Reset();
			tmmsm33.MergeFrom(bcls_rec->Tables["210036"].Rows[i]);
			tmmsm33.Query();

			if (!tmmsm01.Query())
			{
				strcpy(s.sysmsg, "主档表数据已归档！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//砂轮粒度
			if (tmmsm34["GRINDING_WHEEL_OUTER_GRAININESS"].ToString().Trim() != "")
			{
				grindstone_type = tmmsm34["GRINDING_WHEEL_OUTER_GRAININESS"].ToString().Trim();
			}
			else
			{
				grindstone_type = tmmsm34["GRINDING_WHEEL_GRAININESS"].ToString().Trim();
			}

			//砂轮厂家
			if (tmmsm34["GRINDSTONE_SUPPLIER_OUT"].ToString().Trim() != "")
			{
				grindstone_supplier = tmmsm34["GRINDSTONE_SUPPLIER_OUT"].ToString().Trim();
			}
			else
			{
				grindstone_supplier = tmmsm34["GRINDSTONE_SUPPLIER_IN"].ToString().Trim();
			}
			

			if (epex.SetValue(0, tmmsm34) < 0)
			{
				sprintf(s.msg, "电文压值失败，原因[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			//tmmsm01["DEV_CODE"].ToString()=="C8"? tmmsm01["DEV_CODE"].ToString():
			if (epex.SetValue("tmmsm34", "deal_flag", 0, deal_flag) < 0            //处理标记
				|| epex.SetValue("tmmsm34","prod_date", 0, tmmsm34["START_TIME"].ToString().SubstringNE(0,8).Trim()==""?CDateTime::Now().ToString("yyyyMMdd"): tmmsm34["START_TIME"].ToString().SubstringNE(0, 8)) < 0		//生产日期
				|| epex.SetValue("tmmsm34", "dev_code", 0, "A" + tmmsm01["DEV_CODE"].ToString()) < 0		//工位
				|| epex.SetValue("tmmsm34","prod_group_no", 0, tmmsm34["PROD_SHIFT_GROUP"].ToString()) < 0		//班组
				|| epex.SetValue("tmmsm34", "prod_shift_no", 0, tmmsm34["PROD_SHIFT_NO"].ToString()) < 0		//班次
				|| epex.SetValue("tmmsm34","operator", 0, tmmsm34["REC_CREATOR"].ToString()) < 0	//操作者
				|| epex.SetValue("tmmsm34","treat_id", 0, tmmsm33["PROC_NO"].ToString()) < 0		//处理号
				|| epex.SetValue("tmmsm34", "st_no", 0, tmmsm34["ST_NO"].ToString()) < 0			//出钢记号
				|| epex.SetValue("tmmsm34", "mat_no", 0, tmmsm34["MAT_NO"].ToString()) < 0			//材料号
				|| epex.SetValue("tmmsm34", "sap_mat_no", 0, tmmsm34["BATCH"].ToString()) < 0		//批次号
				|| epex.SetValue("tmmsm34", "heat_no", 0, tmmsm34["HEAT_NO"].ToString()) < 0		//熔炼号
				|| epex.SetValue("tmmsm34", "plan_dest", 0, tmmsm01["GUIDE_DEST"].ToString()) < 0	//指导去向给  -计划去向？
				|| epex.SetValue("tmmsm34", "mat_act_wt", 0, tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal()) < 0	//材料实际重量
				|| epex.SetValue("tmmsm34", "mat_act_width", 0, tmmsm01["MAT_ACT_WIDTH"].ToDecimal()) < 0	//实际宽度
				|| epex.SetValue("tmmsm34", "mat_act_thick", 0, tmmsm01["MAT_ACT_THICK"].ToDecimal()) < 0	//实际厚度
				|| epex.SetValue("tmmsm34", "mat_act_len", 0, tmmsm01["MAT_ACT_LEN"].ToDecimal()) < 0		//实际长度
				|| epex.SetValue("tmmsm34", "surface_ok_flag", 0, tmmsm34["MEND_AFTER_QUALITY"].ToString()) < 0	//磨后质量判定是否合格  取值与磨后表面质量？需参考下L4的程序
				|| epex.SetValue("tmmsm34", "scrap_wt", 0, tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal()) < 0		//报废重量  用切废量赋值  后续需加对应逻辑
				|| epex.SetValue("tmmsm34", "mat_wt_before_grind", 0, tmmsm34["MEND_BEFORE_WEIGHT"].ToDecimal()) < 0		//磨前重量
				|| epex.SetValue("tmmsm34", "mat_width_b_grind", 0, tmmsm01["MAT_ACT_WIDTH"].ToDecimal()) < 0		//磨前宽度
				|| epex.SetValue("tmmsm34", "mat_thick_b_grind", 0, tmmsm01["MAT_ACT_THICK"].ToDecimal()) < 0		//磨前厚度
				|| epex.SetValue("tmmsm34", "mat_len_b_grind", 0, tmmsm34["MAT_ACT_LEN"].ToDecimal()) < 0			//磨前长度
				|| epex.SetValue("tmmsm34", "grinding_percent", 0, tmmsm34["MEND_CALCULATE_RATE"].ToString()) < 0	//修磨率
				|| epex.SetValue("tmmsm34", "grinding_start_time", 0, tmmsm34["GRINDING_START_TIME"].ToString()) < 0//开始修磨时刻
				|| epex.SetValue("tmmsm34", "grinding_end_time", 0, tmmsm34["GRINDING_END_TIME"].ToString()) < 0	//修磨结束时刻
				|| epex.SetValue("tmmsm34", "grinding_flag", 0, "1") < 0			//是否修磨  先给1，后续看L4程序
				|| epex.SetValue("tmmsm34", "grindstone_type", 0, grindstone_type) < 0		//砂轮粒度
				|| epex.SetValue("tmmsm34", "grindstone_supplier", 0, grindstone_supplier) < 0		//砂轮厂家
				|| epex.SetValue("tmmsm34", "grindstone_type_last", 0, grindstone_type) < 0		//最后一遍砂轮粒度
				|| epex.SetValue("tmmsm34", "grindstone_supplier2", 0, grindstone_supplier) < 0		//砂轮厂家2
				|| epex.SetValue("tmmsm34", "intrados_grind_mode", 0, tmmsm34["MEND_INNER_MODE"].ToString()) < 0		//内弧修磨方式
				|| epex.SetValue("tmmsm34", "extrados_grind_mode", 0, tmmsm34["MEND_OUTER_MODE"].ToString()) < 0		//外弧修磨方式
				|| epex.SetValue("tmmsm34", "remark", 0, tmmsm34["REMARK"].ToString()) < 0		//备注
				|| epex.SetValue("tmmsm34", "polish_grade", 0, tmmsm34["POLISH_GRADE"].ToString()) < 0		//修磨等级代码
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.SetValue("tmmsm34", "group_leader", 0, " ") < 0    //班长
				//|| epex.SetValue("tmmsm34", "operator", 0, " ") < 0    //操作者
				|| epex.SetValue("tmmsm34", "defect_desc", 0, " ") < 0  //缺陷描述
				|| epex.SetValue("tmmsm34", "surface_level", 0, " ") < 0  //铸坯预判级别
				|| epex.SetValue("tmmsm34", "grinding_level", 0, " ") < 0  //修磨后判定等级
				|| epex.SetValue("tmmsm34", "hot_cool_grind", 0, " ") < 0	//热修冷修区分
			
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//group_leader 班长 

			
			
			//SAP批次号   (CHAR(10)  (-----)) tmmsm34.sap_mat_no
			//计划流向 tmmsm34.plan_dest
			//磨后质量判定是否合格 tmmsm34.surface_ok_flag
			//defect_desc 缺陷描述
			//tmmsm34.scrap_wt  报废重量
			//铸坯预判级别 tmmsm34.surface_level
			//tmmsm34.grinding_level 修磨后判定等级
			//tmmsm34.mat_wt_before_grind 磨前重量
			//tmmsm34.mat_width_b_grind 磨前宽度
			//tmmsm34.mat_thick_b_grind 磨前厚度
			//tmmsm34.mat_len_b_grind 磨前长度
			//tmmsm34.grinding_percent 修磨率
		    //tmmsm34.hot_cool_grind  热修冷修区分
		
			//tmmsm34.grindstone_type 砂轮粒度
			//tmmsm34.grindstone_supplier 砂轮厂家
			//tmmsm34.grindstone_type_last 最后一遍砂轮粒度
			//tmmsm34.grindstone_supplier2 砂轮厂家2
			//tmmsm34.intrados_grind_mode
			//tmmsm34.extrados_grind_mode
			//tmmsm34.polish_grade 修磨等级代码


			if (epex.SendTele() < 0)
			{
				strcpy(s.msg, "电文发送失败。");
				Log::Trace("", __FUNCTION__, "电文发送失败[{0}]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			epex.Uninitialize();

			CString cs_tc_no = "T823SB";
			//电文初始化
			if (epex.Initialize(cs_tc_no) < 0)
			{
				strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
				Log::Trace("", __FUNCTION__, "电文初始化失败");
				s.flag = -1;
				doFlag = -1;
				return doFlag;
			}
			tmmsm34.Print();
			if (epex.SetValue(0, tmmsm34) < 0)
			{
				sprintf(s.msg, "发送电文失败，原因:[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0 //操作标记
				|| epex.SetValue("PROD_DATE", 0, tmmsm34["START_TIME"].ToString()) < 0  //生产日期
				|| epex.SetValue("COST_CENTER", 0, "EGBM") < 0     //成本中心
				|| epex.SetValue("HEAT_NO", 0, tmmsm34["HEAT_NO"].ToString()) < 0  //炉号
				|| epex.SetValue("ST_NO", 0, tmmsm34["ST_NO"].ToString()) < 0	//出钢记号----钢种描述
				|| epex.SetValue("CS06", 0, 0.0) < 0     //损失率
				|| epex.SetValue("JQ10", 0, tmmsm34["MEND_BEFORE_WEIGHT"].ToDecimal()*1000) < 0	 //磨前
				|| epex.SetValue("RG10", 0, tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal()*1000) < 0	 //磨后
				)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//电文发送
			if (epex.SendTele() < 0)
			{
				sprintf(s.msg, "发送电文失败.原因:[%s]", epex.GetMsg());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else
			{
				Log::Trace("", __FUNCTION__, "发送电文成功");
			}
			epex.Uninitialize();
		
		}

	
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}