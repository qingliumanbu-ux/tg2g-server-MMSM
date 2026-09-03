/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		MFJ
Version:    1.0
Date:		2024-01-18
Description:钢坯切废实绩 发送L4电文
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsm_210044_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString v_proc_div = "";//区分工序 MMSM34 修磨和切废
	CDecimal v_cutscrap_wt = 0;//切废量
	CString v_deal_flag = "";//处置标记
	EPEX epex;

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm39("TMMSM39");
	CModel tmmsm39_1("TMMSM39_1");

	// 数据库SQL操作字符串
	CString  sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获取输入参数
		blkNum = bcls_rec->Tables.IndexOf("210044");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 210044 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (bcls_rec->Tables["210044"].Columns.Contains("PROC_DIV"))
		{
			v_proc_div = bcls_rec->Tables["210044"].Rows[0]["PROC_DIV"].ToString().Trim();
		}
		if (bcls_rec->Tables["210044"].Columns.Contains("DEAL_FLAG"))
		{
			v_deal_flag = bcls_rec->Tables["210044"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		}

		for (int i = 0; i < bcls_rec->Tables["210044"].Rows.get_Count(); i++)
		{
			cs_tc_no = "210044";
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
			tmmsm01.MergeFrom(bcls_rec->Tables["210044"].Rows[i]);//获取数据
			tmmsm01.Query();

			tmmsm39.MergeFrom(bcls_rec->Tables["210044"].Rows[i]);//获取数据
			tmmsm39.Query();
			if (v_proc_div == "TMMSM39_1")
			{
				tmmsm39.Reset();
				tmmsm39_1.MergeFrom(bcls_rec->Tables["210044"].Rows[i]);
				tmmsm39.CopyFrom(tmmsm39_1);
			}
			
			//修磨
			if (v_proc_div == "MMSM34")
			{
				if (bcls_rec->Tables["210044"].Columns.Contains("CUT_SCRAP_WT"))//获取切废量
				{
					v_cutscrap_wt = bcls_rec->Tables["210044"].Rows[i]["CUT_SCRAP_WT"].ToDecimal();
				}
				if (tmmsm39["CUT_BEFORE_WT"].ToDecimal() != tmmsm01["MAT_ACT_WT"].ToDecimal())
				{
					v_cutscrap_wt = tmmsm01["MAT_ACT_WT"].ToDecimal() - tmmsm39["CUT_AFTER_WT"].ToDecimal();
				}
				tmmsm39.Print();

				if (epex.SetValue(0, tmmsm39) < 0)
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

				Log::Trace("", __FUNCTION__, "PROD_TIME[{0}]  ", tmmsm39["PROD_TIME"].ToString());

				if (epex.SetValue("tmmsm39", "mat_no", 0, tmmsm39["MAT_NO"].ToString()) < 0  //材料号
					|| epex.SetValue("tmmsm39", "mat_act_width", 0, tmmsm39["CUT_AFTER_WIDTH"].ToDecimal()) < 0  //材料实际宽度
					|| epex.SetValue("tmmsm39", "mat_act_thick", 0, tmmsm39["CUT_AFTER_THICK"].ToDecimal()) < 0  //材料实际厚度
					|| epex.SetValue("tmmsm39", "mat_act_len", 0, tmmsm39["CUT_AFTER_LEN"].ToDecimal()) < 0      //材料实际长度
					|| epex.SetValue("tmmsm39", "mat_act_wt", 0, tmmsm39["CUT_AFTER_WT"].ToDecimal()) < 0		  //材料实际重量
					|| epex.SetValue("tmmsm39", "recut_reason", 0,"磨屑") < 0	  //切割类型  待确认
					|| epex.SetValue("tmmsm39", "cut_scrap_wt", 0, v_cutscrap_wt) < 0
					|| epex.SetValue("tmmsm39", "deal_flag", 0, v_deal_flag) < 0   //处置标记
					)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				tmmsm39["MAT_NO"] = tmmsm01["MAT_NO"];
				tmmsm39.MergeFrom(bcls_rec->Tables["210044"].Rows[i]);
				//tmmsm39.Query();//从33表获取部分数据
				//tmmsm39.Print();
				if (tmmsm39["CUT_BEFORE_WT"].ToDecimal() != tmmsm01["MAT_ACT_WT"].ToDecimal())
				{
					tmmsm39["CUT_SCRAP_WT"] = tmmsm01["MAT_ACT_WT"].ToDecimal() - tmmsm39["CUT_AFTER_WT"].ToDecimal();
				}
				if (epex.SetValue(0, tmmsm39) < 0)
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

				Log::Trace("", __FUNCTION__, "PROD_TIME[{0}]  ", tmmsm39["PROD_TIME"].ToString());

				if (epex.SetValue("tmmsm39", "mat_no", 0, tmmsm39["MAT_NO"].ToString()) < 0  //材料号
					|| epex.SetValue("tmmsm39", "mat_act_width", 0, tmmsm39["CUT_AFTER_WIDTH"].ToDecimal()) < 0  //材料实际宽度
					|| epex.SetValue("tmmsm39", "mat_act_thick", 0, tmmsm39["CUT_AFTER_THICK"].ToDecimal()) < 0  //材料实际厚度
					|| epex.SetValue("tmmsm39", "mat_act_len", 0, tmmsm39["CUT_AFTER_LEN"].ToDecimal()) < 0      //材料实际长度
					|| epex.SetValue("tmmsm39", "mat_act_wt", 0, tmmsm39["CUT_AFTER_WT"].ToDecimal()) < 0		  //材料实际重量
					|| epex.SetValue("tmmsm39", "recut_reason", 0, " ") < 0	  //切割类型
					|| epex.SetValue("tmmsm39", "cut_scrap_wt", 0, tmmsm39["CUT_SCRAP_WT"].ToDecimal()) < 0
					|| epex.SetValue("tmmsm39", "deal_flag", 0, v_deal_flag) < 0   //处置标记
					)
				{
					sprintf(s.msg, epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//当为改切实绩，且为删除时，取01表重量
				if (v_deal_flag == "D" &&v_proc_div == "TMMSM39_1")
				{
						if( epex.SetValue("tmmsm39", "mat_act_wt", 0, tmmsm01["MAT_ACT_WT"].ToDecimal()) < 0		  //材料实际重量
							)
					{
						sprintf(s.msg, epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

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
