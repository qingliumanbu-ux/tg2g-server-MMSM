/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2011-12-13
Version:1.0
Description: 接收二级转炉实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

#include "tmmsm21.h"
#include "tpssm11b.h"
#include "tmmsm14.h"

#include "tmmsm43i.h"

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 接收PES转炉实绩
/// <para>
/// 接收PES转炉实绩并处理
/// </para>
/// </summary>
/// <param name="tmmsm21">转炉实绩</param>
/// <param name="PROC_DIV">处理标记</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/

int f_mmsmb_area3_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsmb_area3_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_cm_gegsb2_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE_TELE(cm_1a1015_rcv)

int f_cm_1a1015_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString l_HTNO	                 = ""; // htno
	CString l_HM_ID	                 = ""; // 铁水处理号
	CString l_PLAN_PROD_ORDER_ID	 = ""; // 计划生产指令号
	CString l_STATION_ID	         = ""; // 设备号
	CString l_HM_LADLE_ID	         = ""; // 铁水包号
	CString l_HM_WEIGHT              = 0; // 铁水重量
	CString l_HM_TEMP                = 0; // 铁水温度
	CString l_FULL_JAR_BEGIN		 = ""; // 倒罐开始时间
	CString l_FULL_JAR_END	         = ""; // 倒罐结束时间
	CString l_ACT_GRADE_ID	         = ""; // 出钢记号
	CString l_ACT_STEEL_CODE	     = ""; // 钢种代码
	CString l_TPC_NO_1		         = "";  // 鱼雷罐号1
	CString l_BF_NO_1		         = ""; // 高炉炉次号1
	CString l_TPC_HM_WEIGHT_1	     = 0; // 倒出铁水重量1
	CString l_TPC_NO_2			     = "";  // 鱼雷罐号2
	CString l_BF_NO_2	             = ""; // 高炉炉次号2
	CString l_TPC_HM_WEIGHT_2	     = 0; // 倒出铁水重量2
	CString l_TPC_NO_3			     = "";  // 鱼雷罐号3
	CString l_BF_NO_3	             = ""; // 高炉炉次号3
	CString l_TPC_HM_WEIGHT_3	     = 0; // 倒出铁水重量3
	CString l_TPC_NO_4			     = "";  // 鱼雷罐号4
	CString l_BF_NO_4	             = ""; // 高炉炉次号4
	CString l_TPC_HM_WEIGHT_4	     = 0; // 倒出铁水重量4
	CString l_CONSUME1		         = 0; // 消耗1
	CString l_CONSUME2		         = 0; // 消耗2
	CString l_EMPTY_HM_LADLE_WEIGHT  = 0; // 铁水包空重
	CString l_OP_NO                  = ""; // 记录人
	CString l_OP_TIME                = ""; // 记录时间
	CString L_EQU_NO                 = "";   // 设备区域号

	l_ADD_PO_ST                      ="";
	l_ADD_PO_ET                      ="";
	l_REC_COUNT_1                    =0;
	l_REC_COUNT_2                    =0;
	l_REC_COUNT_3                    =0;
	l_REC_COUNT_4                    =0;
	l_GROUPID                        ="";// 班组
	l_SHIFTNO                        ="";// 班次
	l_num                            =0;
	l_HM_WEIGHT_1                    =0;
	l_LEAVE_WT                       =0;
	
	l_shift_no_1                     ="";
	l_shift_group_1                  ="";
	
	L_LOW_LIMIT_VALUE                ="";
	L_UP_LIMIT_VALUE                 ="";
	L_MADE_VALUE                     ="";
	L_HMWT                           ="";
	int l_COUNT;                     
	int l_HM_ID_COUNT;               
	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		l_HM_ID                = bcls_rec->Tables[0].Rows[0]["l_HM_ID"].ToString();
		//l_PLAN_PROD_ORDER_ID = bcls_rec->Tables[0].Rows[0]["l_HM_ID"].ToString();
		l_STATION_ID		   = bcls_rec->Tables[0].Rows[0]["l_STATION_ID"].ToString();
		l_EQU_NO			   = bcls_rec->Tables[0].Rows[0]["l_EQU_NO"].ToString();
		l_HM_LADLE_ID		   = bcls_rec->Tables[0].Rows[0]["l_HM_LADLE_ID"].ToString();
		l_HM_WEIGHT			   = bcls_rec->Tables[0].Rows[0]["l_HM_WEIGHT"];
		l_HM_TEMP			   = bcls_rec->Tables[0].Rows[0]["l_HM_TEMP"];
		l_FULL_JAR_BEGIN	   = bcls_rec->Tables[0].Rows[0]["l_FULL_JAR_BEGIN"].ToString();
		l_FULL_JAR_END		   = bcls_rec->Tables[0].Rows[0]["l_FULL_JAR_END"].ToString();
		l_ACT_GRADE_ID		   = bcls_rec->Tables[0].Rows[0]["l_ACT_GRADE_ID"].ToString();
		l_ACT_STEEL_CODE	   = bcls_rec->Tables[0].Rows[0]["l_ACT_STEEL_CODE"].ToString();
		l_TPC_NO_1			   = bcls_rec->Tables[0].Rows[0]["l_TPC_NO_1"].ToString();
		l_BF_NO_1			   = bcls_rec->Tables[0].Rows[0]["l_BF_NO_1"].ToString();
		l_TPC_HM_WEIGHT_1	   = bcls_rec->Tables[0].Rows[0]["l_TPC_HM_WEIGHT_1"];
		l_TPC_NO_2			   = bcls_rec->Tables[0].Rows[0]["l_TPC_NO_2"].ToString();
		l_BF_NO_2			   = bcls_rec->Tables[0].Rows[0]["l_BF_NO_2"].ToString();
		l_TPC_HM_WEIGHT_2	   = bcls_rec->Tables[0].Rows[0]["l_TPC_HM_WEIGHT_2"];
		l_TPC_NO_3			   = bcls_rec->Tables[0].Rows[0]["l_TPC_NO_3"].ToString();
		l_BF_NO_3			   = bcls_rec->Tables[0].Rows[0]["l_BF_NO_3"].ToString();
		l_TPC_HM_WEIGHT_3	   = bcls_rec->Tables[0].Rows[0]["l_TPC_HM_WEIGHT_3"];
		l_TPC_NO_4			   = bcls_rec->Tables[0].Rows[0]["l_TPC_NO_4"].ToString();
		l_BF_NO_4			   = bcls_rec->Tables[0].Rows[0]["l_BF_NO_4"].ToString();
		l_TPC_HM_WEIGHT_4	   = bcls_rec->Tables[0].Rows[0]["l_TPC_HM_WEIGHT_4"];
		l_CONSUME1			   = bcls_rec->Tables[0].Rows[0]["l_CONSUME1"];
		l_CONSUME2			   = bcls_rec->Tables[0].Rows[0]["l_CONSUME2"];
		l_EMPTY_HM_LADLE_WEIGHT= bcls_rec->Tables[0].Rows[0]["l_EMPTY_HM_LADLE_WEIGHT"];
		l_OP_NO				   = bcls_rec->Tables[0].Rows[0]["l_OP_NO"].ToString();
		l_OP_TIME			   = bcls_rec->Tables[0].Rows[0]["l_OP_TIME"].ToString();
		l_HM_WEIGHT_1		   = bcls_rec->Tables[0].Rows[0]["l_HM_WEIGHT_1"];
	

		sqlstr =
			" SELECT HTNO,PONO,STEEL_GRADE,MOLTIRON_TANK_NO "
			" FROM   VLO_TAP_PLAN_TOT "
			" WHERE  HM_ID =@t_hm_id " ;
		cmd_inq.SetCommandText(sqlstr);        
		cmd_inq.Parameters.Set("t_hm_id", l_HM_ID);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			l_HTNO = cmd_inq.GetString(1);
			l_PLAN_PROD_ORDER_ID = cmd_inq.GetString(2);
			l_ACT_GRADE_ID = cmd_inq.GetString(3);
			MOLTIRON_TANK_NO = cmd_inq.GetString(4);

			if (MOLTIRON_TANK_NO != l_HM_LADLE_ID)
			{
				l_HM_LADLE_ID = MOLTIRON_TANK_NO;
				Log::Trace("", __FUNCTION__, "实绩送入的包号与计划中的包号不一致");
			}
		}
		cmd_inq.Close();

		if (l_FULL_JAR_BEGIN.Trim() == "")
		{
			sqlstr =
				" SELECT MIN(EVENT_TIME) AS FULL_JAR_BEGIN   "
				" FROM LT_WI_EVENT"
				" WHERE HM_ID = @t_hm_id"
				" AND ZONE_NO = '1'"
				" AND EQU_NO = @t_equ_no"
				" AND EVENT_CODE = '2' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("t_hm_id", l_HM_ID);
			cmd_inq.Parameters.Set("t_equ_no", l_EQU_NO);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				l_FULL_JAR_BEGIN = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
		}
		
		for (int j = 1; j <= 4; j++)
		{
			CString TPC_NO = "";
			CString BF_NO = "";
			if (j == 1)
			{
				TPC_NO = l_TPC_NO_1;
				BF_NO = l_BF_NO_1;
			}
			else if (j == 2)
			{
				TPC_NO = l_TPC_NO_2;
				BF_NO = l_BF_NO_2;
			}
			else if (j == 3)
			{
				TPC_NO = l_TPC_NO_3;
				BF_NO = l_BF_NO_3;
			}
			else if (j == 4)
			{
				TPC_NO = l_TPC_NO_4;
				BF_NO = l_BF_NO_4;
			}
			sqlstr =
				" SELECT COUNT(1) REC_COUNT "
				" FROM  LM_MI_TPC "
				" WHERE  LM_MI_TPC.TPC_NO = @t_tpc_no"
				" AND  LM_MI_TPC.BF_NO = @t_bf_no"
				" AND  LM_MI_TPC.HM_ID = @t_hm_id; ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("t_tpc_no", TPC_NO);
			cmd_inq.Parameters.Set("t_bf_no", BF_NO);
			cmd_inq.Parameters.Set("t_hm_id", l_HM_ID);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if (j == 1)
				{
					l_REC_COUNT_1 = cmd_inq.GetInt32(1);
				}
				else if (j == 2)
				{
					l_REC_COUNT_2 = cmd_inq.GetInt32(1);
				}
				else if (j == 3)
				{
					l_REC_COUNT_3 = cmd_inq.GetInt32(1);
				}
				else if (j == 4)
				{
					l_REC_COUNT_4 = cmd_inq.GetInt32(1);
				}
				
			}
			cmd_inq.Close();
		}
		

		if (l_HM_ID_COUNT >0)
		{
			sprintf(s.msg, "[%s]已经存在。", (const char*)l_HM_ID);
			sprintf(s.sysmsg, "[%s]已经存在。", (const char*)l_HM_ID);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr =
			" SELECT COUNT(1)"
			" FROM LO_TAP_PLAN"
			" WHERE MOLTIRON_TANK_NO = @l_HM_LADLE_ID"
			" AND PONO = @l_PLAN_PROD_ORDER_ID"
			" AND CHARGE_STATUS = '2'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("l_HM_LADLE_ID", l_HM_LADLE_ID);
		cmd_inq.Parameters.Set("l_PLAN_PROD_ORDER_ID", l_PLAN_PROD_ORDER_ID);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_COUNT = cmd_inq.GetInt32(1);
		}
		cmd_inq.Close();

		
		if (l_HM_ID_COUNT == 0)
		{
			Log::Trace("", __FUNCTION__, "包号[{0}]对应的PONO[{1}]已经变更", (const char*)l_HM_LADLE_ID, (const char*)l_PLAN_PROD_ORDER_ID);
		}

		sqlstr =
			" SELECT PONO"
			" FROM LO_TAP_PLAN"
			" WHERE MOLTIRON_TANK_NO = @t_hm_ladle_id"
			" AND CHARGE_STATUS = '2'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("t_hm_ladle_id", l_HM_LADLE_ID);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_PLAN_PROD_ORDER_ID = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		
		sqlstr =
			" SELECT  STEEL_GRADE,PO_ET,HM_ID"
			" FROM  LO_TAP_PLAN"
			" WHERE  PONO = @t_pono"
			" AND  MOLTIRON_TANK_NO = @t_hm_ladle_id";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("t_pono", l_PLAN_PROD_ORDER_ID);
		cmd_inq.Parameters.Set("t_hm_ladle_id", l_HM_LADLE_ID);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			l_STEEL_GRADE = cmd_inq.GetString(1);		  
		    l_PO_ET = cmd_inq.GetString(2);
			l_FOUND_HM_ID = cmd_inq.GetString(3);
		}
		else
		{
			sprintf(s.msg, "[%s]不存在，检查计划!", (const char*)l_PLAN_PROD_ORDER_ID);
			sprintf(s.sysmsg, "[%s]不存在，检查计划!", (const char*)l_PLAN_PROD_ORDER_ID);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();


		

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
