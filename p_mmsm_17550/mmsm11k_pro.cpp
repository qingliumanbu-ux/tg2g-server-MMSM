/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     Simon Li
Version:    1.0
Date:       2024-03-18
Description: 中位硅铁水罐查询
**************************************************/

//框架头文件
#include "stdafx.h"

//业务头文件


BM2F_ENTERACE(mmsm11k_pro)

int f_mmsm11k_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 		//服务调用日志输出

	int doFlag = 0;					//服务调用返回值

	/* 分页信息定义 */
	int	TotalRecordCount = 0;

	CModel tmmsm11h1("TMMSM11H1");	//中位硅1表
	int js_flag = 1;
	CDbCommand cmd_inq(conn);			//数据库操作对象定义
	CString ay_type = "";
	/* sql语句变量定义 */
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString js_mode = "";
	// 传入重量和
	CDecimal cd_cr_wt_1 = 0, cd_cr_wt_2 = 0;
	CDecimal cd_cr_cf_1 = 0, cd_cr_cf_2 = 0;
	// 最小成分和
	CDecimal cd_cf_sum = 0;
	CDecimal cd_cr_wt_all_1 = 0, cd_cr_wt_all_2 = 0;
	CDecimal cd_hm_weight_1 = 0, cd_hm_weight_2 = 0;
	// 
	CDecimal cd_js_wt = 0, cd_js_wt_max = 0, cd_js_wt_min = 0;
	CDecimal cd_js_cf = 0, cd_js_cf_max = 0, cd_js_cf_min = 0;
	CString cs_torpedo_number1 = "", cs_torpedo_number2 = "";
	CString cs_pos1 = "", cs_pos2 = "";
	// 传出结果，并且传入计算的计算量
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "MODE_NO");
	bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "ELEM_SI");
	bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "HM_WEIGHT");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "TORPEDO_NUMBER1");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "FLAG1");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "TORPEDO_NUMBER2");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "FLAG2");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "REMARK");
	bcls_ret->Tables[0].Rows.Add();
	try
	{
		bcls_ret->Tables[0].Rows[0]["FLAG1"] = "0";
		bcls_ret->Tables[0].Rows[0]["FLAG2"] = "0";
		if (bcls_rec->Tables[0].Rows.get_Count() > 0)
		{

			cd_cr_wt_1 = bcls_rec->Tables[0].Rows[0]["ASSIGNEDVOLUME"].ToDecimal();
			cd_cr_cf_1 = bcls_rec->Tables[0].Rows[0]["ELEM_SI"].ToDecimal();
			cs_torpedo_number1 = bcls_rec->Tables[0].Rows[0]["TORPEDO_NUMBER"].ToString();
			cd_hm_weight_1 = bcls_rec->Tables[0].Rows[0]["HM_WEIGHT"].ToDecimal();
			cs_pos1 = bcls_rec->Tables[0].Rows[0]["POS_DIR_CODE"].ToString();
			if (bcls_rec->Tables[0].Rows.get_Count() == 2)
			{
				cd_cr_wt_2 = bcls_rec->Tables[0].Rows[1]["ASSIGNEDVOLUME"].ToDecimal();
				cd_cr_cf_2 = bcls_rec->Tables[0].Rows[1]["ELEM_SI"].ToDecimal();
				cs_torpedo_number2 = bcls_rec->Tables[0].Rows[1]["TORPEDO_NUMBER"].ToString();
				cd_hm_weight_2 = bcls_rec->Tables[0].Rows[1]["HM_WEIGHT"].ToDecimal();
				cs_pos2 = bcls_rec->Tables[0].Rows[1]["POS_DIR_CODE"].ToString();
			}
		}
		Log::Info("", __FUNCTION__, "cd_hm_weight_2  =[{0}]", cd_hm_weight_2);
		if (bcls_rec->Tables[1].Rows.get_Count() > 0)
		{
			js_mode = bcls_rec->Tables[1].Rows[0]["MODE_NO"].ToString().Trim();
		}
		Log::Info("", __FUNCTION__, "js_mode  =[{0}]", js_mode);
		if (js_mode.Trim() == "")// 160t 设定值，上下限3t    设定鱼雷剩下的预测量：  脱P 
		{
			sprintf(s.msg, "请选择模式，否则无法计算！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (js_mode == "1")// 160t 设定值，上下限3t    设定鱼雷剩下的预测量：  脱P 
		{
			sqlstr = "SELECT STOCK_WT ,HEAT_WT_MAX ,HEAT_WT_MIN ,VALUE_SI ,  SAP_ERP_S1025_1_H, SAP_ERP_S1025_1_L,MODE_NO,AC_WT   FROM  TMMSM11J WHERE MODE_NO ='1'   ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cd_js_wt = cmd_inq.GetDecimal(1);
				cd_js_wt_max = cd_js_wt + cmd_inq.GetDecimal(2);
				cd_js_wt_min = cd_js_wt - cmd_inq.GetDecimal(3);
				cd_js_cf = cmd_inq.GetDecimal(4);
				cd_js_cf_max = cd_js_cf + cmd_inq.GetDecimal(6);
				cd_js_cf_min = cd_js_cf + cmd_inq.GetDecimal(5);
				if (cd_cr_wt_1 == 0)
				{
					cd_cr_wt_1 = cmd_inq.GetDecimal(8) - cd_hm_weight_1;
				}
				if (cd_cr_wt_2 == 0)
				{
					cd_cr_wt_2 = cmd_inq.GetDecimal(8) - cd_hm_weight_2;
				}
			}
		}
		if (js_mode == "2")   // 220t  设定值，上下限3t    设定鱼雷剩下的预测量： 脱碳
		{
			sqlstr = "SELECT STOCK_WT ,HEAT_WT_MAX ,HEAT_WT_MIN ,VALUE_SI , SAP_ERP_S1025_1_H, SAP_ERP_S1025_1_L, MODE_NO,AC_WT     FROM  TMMSM11J WHERE MODE_NO ='2'   ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cd_js_wt = cmd_inq.GetDecimal(1);
				cd_js_wt_max = cd_js_wt + cmd_inq.GetDecimal(2);
				cd_js_wt_min = cd_js_wt - cmd_inq.GetDecimal(3);
				cd_js_cf = cmd_inq.GetDecimal(4);
				cd_js_cf_max = cd_js_cf + cmd_inq.GetDecimal(6);
				cd_js_cf_min = cd_js_cf + cmd_inq.GetDecimal(5);
				if (cd_cr_wt_1 == 0)
				{
					cd_cr_wt_1 = cmd_inq.GetDecimal(8) - cd_hm_weight_1;
				}
				if (cd_cr_wt_2 == 0)
				{
					cd_cr_wt_2 = cmd_inq.GetDecimal(8) - cd_hm_weight_2;
				}
			}
		}
		bcls_ret->Tables[0].Rows[0]["MODE_NO"] = js_mode;
		Log::Info("", __FUNCTION__, "cd_cr_wt_1  =[{0}]", cd_cr_wt_1);
		Log::Info("", __FUNCTION__, "cd_cr_wt_2  =[{0}]", cd_cr_wt_2);
		Log::Info("", __FUNCTION__, "cd_js_wt  =[{0}]", cd_js_wt);
		// 计算合适目标成分合重量
		if (cd_cr_cf_1 == 0 && cd_cr_cf_2 == 0 )
		{
			bcls_ret->Tables[0].Rows[0]["REMARK"] = "无法计算结果";
		}

		// 用少的先算，有满足返回
		if (((cd_js_wt - cd_cr_wt_1) <= cd_cr_wt_2) && ((cd_js_wt - cd_cr_wt_1) >=0) )
		{
			bcls_ret->Tables[0].Rows[0]["TORPEDO_NUMBER2"] = cs_torpedo_number2;
			bcls_ret->Tables[0].Rows[0]["TORPEDO_NUMBER1"] = cs_torpedo_number1;
			bcls_ret->Tables[0].Rows[0]["FLAG1"] = "2";
			if (cd_js_wt - cd_cr_wt_1 == cd_cr_wt_2)
			{
				bcls_ret->Tables[0].Rows[0]["FLAG2"] = "2";
			}
			else
			{
				bcls_ret->Tables[0].Rows[0]["FLAG2"] = "1";
			}
			// 此时满足条件
			if (cd_js_cf_min < (cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt - cd_cr_wt_1)*cd_cr_cf_2) / cd_js_wt < cd_js_cf_max)
			{
				bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt - (cd_cr_wt_1))*cd_cr_cf_2) / cd_js_wt;
				bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt;
				js_flag = 0;
				bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos1 + "#" + cs_torpedo_number1 + "已清空，" + cs_pos2 + "#" + cs_torpedo_number2 + "消耗了" + (cd_js_wt - cd_cr_wt_1).Round(1).ToString() + "t";
				js_flag = 0;
			}
			else if (cd_js_cf_min < (cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt_max - cd_cr_wt_1)*cd_cr_cf_2) / cd_js_wt_max < cd_js_cf_max)
			{
				bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt_max - (cd_cr_wt_1))*cd_cr_cf_2) / cd_js_wt_max;
				bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt_max;
				bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos1 + "#" + cs_torpedo_number1 + "已清空，" + cs_pos2 + "#" + cs_torpedo_number2 + "消耗了" + (cd_js_wt_max - cd_cr_wt_1).Round(1).ToString() + "t";
				js_flag = 0;
			}
			else if (cd_js_cf_min < (cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt_min - cd_cr_wt_1)*cd_cr_cf_2) / cd_js_wt_min < cd_js_cf_max)
			{
				bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt_min - cd_cr_wt_1)*cd_cr_cf_2) / cd_js_wt_min;
				bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt_min;
				bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos1 + "#" + cs_torpedo_number1 + "已清空，" + cs_pos2 + "#" + cs_torpedo_number2 + "消耗了" + (cd_js_wt_min - cd_cr_wt_1).Round(1).ToString() + "t";
				js_flag = 0;
			}
			else
			{
				if ((cd_js_cf_min - ((cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt_max - cd_cr_wt_1)*cd_cr_cf_2) / cd_js_wt_max) <= ((cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt_min - (cd_cr_wt_1))*cd_cr_cf_2) / cd_js_wt_min) - cd_js_cf_max))
				{
					bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt_max - cd_cr_wt_1)*cd_cr_cf_2) / cd_js_wt_max;
					bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt_max;
					bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos1 + "#" + cs_torpedo_number1 + "已清空，" + cs_pos2 + "#" + cs_torpedo_number2 + "消耗了" + (cd_js_wt_max - cd_cr_wt_1).Round(1).ToString() + "t";
					js_flag = 0;
				}
				else
				{
					bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_1*cd_cr_cf_1 + (cd_js_wt_min - cd_cr_wt_1)*cd_cr_cf_2) / cd_js_wt_min;
					bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt_min;
					bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos1 + "#" + cs_torpedo_number1 + "已清空，" + cs_pos2 + "#" + cs_torpedo_number2 + "消耗了" + (cd_js_wt_min - cd_cr_wt_1).Round(1).ToString() + "t";
					js_flag = 0;
				}
			}
		}
		if (((cd_js_wt - cd_cr_wt_2) <= cd_cr_wt_1) && ((cd_js_wt - cd_cr_wt_2) >=0)&& js_flag)
		{
			// 此时满足条件
			bcls_ret->Tables[0].Rows[0]["TORPEDO_NUMBER1"] = cs_torpedo_number1;
			bcls_ret->Tables[0].Rows[0]["TORPEDO_NUMBER2"] = cs_torpedo_number2;
			bcls_ret->Tables[0].Rows[0]["FLAG2"] = "2";
			if (cd_js_wt - cd_cr_wt_2 == cd_cr_wt_1)
			{
				bcls_ret->Tables[0].Rows[0]["FLAG1"] = "2";
			}
			else
			{
				bcls_ret->Tables[0].Rows[0]["FLAG1"] = "1";
			}
			if (cd_js_cf_min < (cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt < cd_js_cf_max)
			{
				bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt;
				bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt;
				bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos2 + "#" + cs_torpedo_number2 + "已清空，" + cs_pos1 + "#" + cs_torpedo_number1 + "消耗了" + (cd_js_wt - cd_cr_wt_2).Round(1).ToString() + "t";
				js_flag = 0;
			}
			else if (cd_js_cf_min <  (cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt_max - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt_max < cd_js_cf_max)
			{
				bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt_max - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt_max;
				bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt_max;
				bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos2 + "#" + cs_torpedo_number2 + "已清空，" + cs_pos1 + "#" + cs_torpedo_number1 + "消耗了" + (cd_js_wt_max - cd_cr_wt_2).Round(1).ToString() + "t";
				js_flag = 0;
			}
			else if (cd_js_cf_min < (cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt_min - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt_min < cd_js_cf_max)
			{
				bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt_min - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt_min;
				bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt_min;
				bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos2 + "#" + cs_torpedo_number2 + "已清空，" + cs_pos1 + "#" + cs_torpedo_number1 + "消耗了" + (cd_js_wt_min - cd_cr_wt_2).Round(1).ToString() + "t";
				js_flag = 0;
			}
			else
			{
				if ((cd_js_cf_min - ((cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt_max - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt_max)) <= (((cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt_min - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt_min) - cd_js_cf_max))
				{
					bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt_max - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt_max;
					bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt_max;
					bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos2 + "#" + cs_torpedo_number2 + "已清空，" + cs_pos1 + "#" + cs_torpedo_number1 + "消耗了" + (cd_js_wt_max - cd_cr_wt_2).Round(1).ToString() + "t";
					js_flag = 0;
				}
				else
				{
					bcls_ret->Tables[0].Rows[0]["ELEM_SI"] = (cd_cr_wt_2*cd_cr_cf_2 + (cd_js_wt_min - cd_cr_wt_2)*cd_cr_cf_1) / cd_js_wt_min / cd_js_wt_min;
					bcls_ret->Tables[0].Rows[0]["HM_WEIGHT"] = cd_js_wt_min;
					bcls_ret->Tables[0].Rows[0]["REMARK"] = cs_pos2 + "#" + cs_torpedo_number2 + "已清空，" + cs_pos1 + "#" + cs_torpedo_number1 + "消耗了" + (cd_js_wt_min - cd_cr_wt_2).Round(1).ToString() + "t";
					js_flag = 0;
				}
			}
		}
		//均无 计算结果
		if (js_flag == 1)
		{
			bcls_ret->Tables[0].Rows[0]["REMARK"] = "无法计算结果";
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
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
	cmd_inq.Close();

	return doFlag;		//返回-1时事务将回滚，返回为0是事务将提交
}