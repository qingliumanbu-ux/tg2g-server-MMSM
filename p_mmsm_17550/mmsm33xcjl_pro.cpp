/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wsl
Version:    1.0
Date:       2024-01-06
Description: 炼钢校秤记录维护
**************************************************/
//框架头文件
#include "stdafx.h"

//业务头文件

BM2F_ENTERACE(mmsm33xcjl_pro)

int f_mmsm33xcjl_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_c_div = "";//碳锈区分
	CDecimal v_mat_wt = 0;//砝码重量
	CString next_month = "";//下个月1号
	CString now_month_lastday = "";//这个月最后一天 即 当月的总天数
	CDecimal for_count = 0;//循环次数
	/* 业务变量 */
	CModel tmmsm33xcjl("TMMSM33XCJL");
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString C_APPLYBY1_now = "";
	CString PROC_DIV = "";
	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//传入参数接收
			tmmsm33xcjl.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			PROC_DIV = bcls_rec->Tables[0].Rows[i]["PROC_DIV"].ToString().Trim();

			Log::Trace("", __FUNCTION__, "PROC_DIV   =[{0}]", PROC_DIV);

			//修改
			if (PROC_DIV == "U")
			{
				tmmsm33xcjl.Print();
				tmmsm33xcjl.Update("*", "PROD_DATE,PROD_SHIFT_NO");
			}

			//更新砝码重量
			if (PROC_DIV == "FAMA")
			{
				sqlstr = "SELECT CODE FROM TWMSMZD02 WHERE CODE_CLASS ='MMXCJLFM' AND CODE_DESC_1_CONTENT  = '" + bcls_rec->Tables[0].Rows[i]["C_DIV"].ToString().Trim() + "'";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					v_mat_wt = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				v_c_div = bcls_rec->Tables[0].Rows[i]["C_DIV"].ToString().Trim();

				//不光要更新砝码重量，也要将现有数据中的砝码重量修改
				if (v_mat_wt != bcls_rec->Tables[0].Rows[i]["MAT_WT"].ToDecimal())
				{
					sqlstr = "UPDATE TWMSMZD02 SET CODE = '" + bcls_rec->Tables[0].Rows[i]["MAT_WT"].ToString().Trim() + "'"
						" WHERE CODE_CLASS ='MMXCJLFM' AND CODE_DESC_1_CONTENT = '" + v_c_div + "'";
					Log::Trace("", __FUNCTION__, "更新sqlstr   =[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

					//夜班数据判断更新
					tmmsm33xcjl["PROD_DATE"] = dateNow.Substring(0,8);
					tmmsm33xcjl["PROD_SHIFT_NO"] = "1";
					tmmsm33xcjl.Query("PROD_DATE,PROD_SHIFT_NO");
					if (tmmsm33xcjl["PROOFREAD_FIRST_WT_0"].ToDecimal() == 0)
					{
						if (v_c_div == "不锈钢")
						{
							tmmsm33xcjl["COUNTERWEIGH_S"] = bcls_rec->Tables[0].Rows[i]["MAT_WT"].ToDecimal();
							tmmsm33xcjl.Update("COUNTERWEIGH_S", "PROD_DATE,PROD_SHIFT_NO");
						}
						else if (v_c_div == "碳钢")
						{
							tmmsm33xcjl["COUNTERWEIGH_C"] = bcls_rec->Tables[0].Rows[i]["MAT_WT"].ToDecimal();
							tmmsm33xcjl.Update("COUNTERWEIGH_C", "PROD_DATE,PROD_SHIFT_NO");
						}
					}

					//当天的白班数据判断更新
					tmmsm33xcjl["PROD_DATE"] = dateNow.Substring(0, 8);
					tmmsm33xcjl["PROD_SHIFT_NO"] = "2";
					tmmsm33xcjl.Query("PROD_DATE,PROD_SHIFT_NO");
					if (tmmsm33xcjl["PROOFREAD_FIRST_WT_0"].ToDecimal() == 0)
					{
						if (v_c_div == "不锈钢")
						{
							tmmsm33xcjl["COUNTERWEIGH_S"] = bcls_rec->Tables[0].Rows[i]["MAT_WT"].ToDecimal();
							tmmsm33xcjl.Update("COUNTERWEIGH_S", "PROD_DATE,PROD_SHIFT_NO");
						}
						else if (v_c_div == "碳钢")
						{
							tmmsm33xcjl["COUNTERWEIGH_C"] = bcls_rec->Tables[0].Rows[i]["MAT_WT"].ToDecimal();
							tmmsm33xcjl.Update("COUNTERWEIGH_C", "PROD_DATE,PROD_SHIFT_NO");
						}
					}


					
					if (v_c_div == "不锈钢")
					{
						sqlstr = "UPDATE TMMSM33XCJL COUNTERWEIGH_S = '" + bcls_rec->Tables[0].Rows[i]["MAT_WT"].ToString().Trim() + "'  where PROD_DATE > '" + dateNow.Substring(0, 8) + "'";
						Log::Trace("", __FUNCTION__, "更新sqlstr   =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}
					else if (v_c_div == "碳钢")
					{
						sqlstr = "UPDATE TMMSM33XCJL COUNTERWEIGH_C = '" + bcls_rec->Tables[0].Rows[i]["MAT_WT"].ToString().Trim() + "'  where PROD_DATE > '" + dateNow.Substring(0, 8) + "'";
						Log::Trace("", __FUNCTION__, "更新sqlstr   =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();
					}
						
				}

				
			}
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


