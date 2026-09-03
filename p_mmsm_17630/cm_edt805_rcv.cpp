/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-03-11
Description:倒罐倾倒过程接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(cm_edt805_rcv)

int f_cm_edt805_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 		//系统日志类定义

	int doFlag = 0;					//返回值 

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//当前时间

	CModel tmmsm12("TMMSM12");		//炼钢倒罐实绩表 
	CModel tmmsm12a("TMMSM12A");	//炼钢鱼雷罐倒铁实绩表
	CModel tmmsm11("TMMSM11");		//炼钢铁水信息表
	CModel tmmsm16("TMMSM16");		//倒罐倾倒过程表

	CString tmmsm12UpdateFeilds = "";	//tmmsm12表更新字段
	CString sql_str = "";
	CString TPD_NO = "";				//倒罐处理号
	CString BATCH_NO = "";				//批次号
	CString IRON_NO = "";				//铁次号
	CString TRE_TPC_NO = "";			//罐次号
	CString TPC_NO = "";				//罐号
	CDecimal MOLTIRON_WT = 0;			//倒罐重量
	CDbCommand cmd_inq(conn);

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm16.Reset();
			tmmsm16.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm16.Insert();

			TPD_NO = bcls_rec->Tables[0].Rows[i]["HEATNUMBER"].ToString();									//倒罐处理号
			BATCH_NO = bcls_rec->Tables[0].Rows[i]["TORPEDO_BATCH_NUMBER"].ToString();						//批次号
			TPC_NO = bcls_rec->Tables[0].Rows[i]["TORPEDO_NUMBER"].ToString();
			MOLTIRON_WT = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["TAPPING_HM_WEIGHT"].ToString());		//倒罐重量

			//通过批次号获取铁水分配信息
			tmmsm11["BATCH_NO"] = BATCH_NO;
			Log::Trace("", __FUNCTION__, "批次号：{0}", BATCH_NO);
			if (tmmsm11.Query("BATCH_NO"))
			{
				tmmsm11.Print();
				IRON_NO = tmmsm11["TAPNO"];
				TRE_TPC_NO = tmmsm11["TPC_ID"];
			}
			else{
				IRON_NO = BATCH_NO;
				TRE_TPC_NO = BATCH_NO;
			}
			tmmsm12["TPD_NO"] = TPD_NO;
			sql_str = "SELECT *  FROM  TMMSM12 WHERE TPD_NO = @tpd_no  order by START_TIME desc ";
			cmd_inq.SetCommandText(sql_str);
			cmd_inq.Parameters.Set("tpd_no", tmmsm12["TPD_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm12);
				
				//更新信息
				tmmsm12["REC_REVISOR"] = "EDT805";
				tmmsm12["REC_REVISE_TIME"] = datetime;
				tmmsm12UpdateFeilds += "REC_REVISOR, REC_REVISE_TIME, ";

				//第一次倒罐量为0
				if (CDecimal::Parse(tmmsm12["MOLTIRON_WT1"].ToString()) == 0){
					//tmmsm12表更新字段
					tmmsm12["IRON_NO1"] = IRON_NO;
					tmmsm12["TPC_YL_NO1"] = TPC_NO;
					tmmsm12["MOLTIRON_WT1"] = MOLTIRON_WT;
					tmmsm12UpdateFeilds += "IRON_NO1, TPC_YL_NO1, MOLTIRON_WT1, ";
				}
				//第二次倒罐量为0
				else if (CDecimal::Parse(tmmsm12["MOLTIRON_WT2"].ToString()) == 0){
					//tmmsm12表更新字段
					tmmsm12["IRON_NO2"] = IRON_NO;
					tmmsm12["TPC_YL_NO2"] = TPC_NO;
					tmmsm12["MOLTIRON_WT2"] = MOLTIRON_WT;
					tmmsm12UpdateFeilds += "IRON_NO2, TPC_YL_NO2, MOLTIRON_WT2, ";
				}
				//第三次倒罐量为0
				else if (CDecimal::Parse(tmmsm12["MOLTIRON_WT3"].ToString()) == 0){
					//tmmsm12表更新字段
					tmmsm12["IRON_NO3"] = IRON_NO;
					tmmsm12["TPC_YL_NO3"] = TPC_NO;
					tmmsm12["MOLTIRON_WT3"] = MOLTIRON_WT;
					tmmsm12UpdateFeilds += "IRON_NO3, TPC_YL_NO3, MOLTIRON_WT3, ";
				}
				//第四次倒罐量为0
				else if (CDecimal::Parse(tmmsm12["MOLTIRON_WT4"].ToString()) == 0){
					//tmmsm12表更新字段
					tmmsm12["IRON_NO4"] = IRON_NO;
					tmmsm12["TPC_YL_NO4"] = TPC_NO;
					tmmsm12["MOLTIRON_WT4"] = MOLTIRON_WT;
					tmmsm12UpdateFeilds += "IRON_NO4, TPC_YL_NO4, MOLTIRON_WT4, ";
				}

				//tmmsm12表更新
				tmmsm12UpdateFeilds = tmmsm12UpdateFeilds.Substring(0, tmmsm12UpdateFeilds.GetLength() - 2);
				tmmsm12.Update(tmmsm12UpdateFeilds, "TPD_NO,TIME_STAMPS");
			}
			else
			{
				//-------------- tmmsm12倒罐实绩表新增 -------------------------
				tmmsm12["REC_CREATOR"] = "EDT805";
				tmmsm12["REC_CREATE_TIME"] = datetime;
				tmmsm12["COMPANY_CODE"] = "TG";
				tmmsm12["COMPANY_NAME"] = "太钢";
				tmmsm12["PRACT_COLL_MODE"] = "1";
				tmmsm12["FACTORY_DIV"] = "S2N";
				tmmsm12["IRON_NO1"] = IRON_NO;
				tmmsm12["TPC_YL_NO1"] = TPC_NO;
				tmmsm12["MOLTIRON_WT1"] = MOLTIRON_WT;
				tmmsm12.Insert();
				//-------------- tmmsm12倒罐实绩表新增 -------------------------
			}
			cmd_inq.Close();
			//--------------------- 鱼雷罐倒铁实绩新增 -----------------
			tmmsm12a["REC_CREATOR"] = "EDT805";
			tmmsm12a["REC_CREATE_TIME"] = datetime;
			tmmsm12a["COMPANY_CODE"] = "TG";
			tmmsm12a["COMPANY_NAME"] = "太钢";
			tmmsm12a["IRON_NO"] = IRON_NO;
			tmmsm12a["TPC_YL_NO"] = TPC_NO;
			tmmsm12a["MOLTIRON_WT"] = MOLTIRON_WT;
			tmmsm12a["IRON_LADLE_NO"] = tmmsm12["IRON_LADLE_NO"];
			tmmsm12a["PROC_COUNT"] = 1;
			tmmsm12a["TRE_TPC_NO"] = TRE_TPC_NO;
			tmmsm12a["TPD_NO"] = TPD_NO;
			tmmsm12a.Insert();
			//--------------------- 鱼雷罐倒铁实绩新增 -----------------
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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

	//返回-1时事务将回滚，返回为0是事务将提交	
	return doFlag;
}