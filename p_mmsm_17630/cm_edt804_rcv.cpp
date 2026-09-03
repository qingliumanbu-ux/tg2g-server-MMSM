/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-03-11
Description:铁水罐实绩接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(cm_edt804_rcv)

int f_cm_edt804_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 		//系统日志类定义

	int doFlag = 0;					//返回值 

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//当前时间

	CModel tmmsm12("TMMSM12");		//炼钢倒罐实绩表

	CString TPD_NO = "";			//处理号
	CString ACTION_TYPE = "";		//操作类型

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm12.Reset();

			TPD_NO = bcls_rec->Tables[0].Rows[i]["HEAT_NUMBER"];						//处理号
			tmmsm12["TPD_NO"] = TPD_NO;
			tmmsm12["REC_CREATOR"] = "EDT804";							//记录创建责任人
			tmmsm12["REC_CREATE_TIME"] = datetime;						//记录创建时间
			tmmsm12["COMPANY_CODE"] = "TG";								//公司代码
			tmmsm12["COMPANY_NAME"] = "太钢";							//公司名称
			tmmsm12["PRACT_COLL_MODE"] = "1";							//实绩接收方式，0:录入；1:接收
			tmmsm12["FACTORY_DIV"] = "S2N";								//工厂分区

			tmmsm12["IRON_TEMP"] = bcls_rec->Tables[0].Rows[i]["HM_TEMP"];						//铁水温度
			tmmsm12["MOLTIRON_WT"] = bcls_rec->Tables[0].Rows[i]["HM_WEIGHT"];					//铁水重量
			tmmsm12["MAT_DESTION"] = bcls_rec->Tables[0].Rows[i]["DESTINATION"];				//铁水包去向
			tmmsm12["LADLE_ARRIVE_TIME"] = bcls_rec->Tables[0].Rows[i]["LADLE_ARRIVAL_TIME"];	//铁水包到达时间
			tmmsm12["LADLE_LEAVE_TIME"] = bcls_rec->Tables[0].Rows[i]["LADLE_LEAVE_TIME"];		//铁水包离开时间
			tmmsm12["START_TIME"] = bcls_rec->Tables[0].Rows[i]["TAPPING_BEGIN_TIME"];			//开始倒罐时间
			tmmsm12["END_TIME"] = bcls_rec->Tables[0].Rows[i]["TAPPING_END_TIME"];				//结束倒罐时间
			tmmsm12["EMPTY_LADLE_WT"] = bcls_rec->Tables[0].Rows[i]["LADLE_WEIGHT"];			//空包重量
			tmmsm12["COUNT"] = bcls_rec->Tables[0].Rows[i]["TAPPING_TIMES"];					//倒铁次数
			tmmsm12["MEAS_TEMP_TIME"] = bcls_rec->Tables[0].Rows[i]["TEMP_TIME"];				//测温时间
			tmmsm12["SAMPLE_NO_1"] = bcls_rec->Tables[0].Rows[i]["SAMPLE_ID"];					//样号
			tmmsm12["TPD_DURATION"] = bcls_rec->Tables[0].Rows[i]["TAPPING_TIME"];				//倒罐持续时间
			tmmsm12["OPERCODE"] = bcls_rec->Tables[0].Rows[i]["OPERATORNAME"];					//操作工
			//班组
			if (bcls_rec->Tables[0].Rows[i]["SHIFT_NO"].ToString().Trim() = "甲班")
			{
				tmmsm12["PROD_SHIFT_GROUP"] = "A";
			}
			else if (bcls_rec->Tables[0].Rows[i]["SHIFT_NO"].ToString().Trim() = "乙班")
			{
				tmmsm12["PROD_SHIFT_GROUP"] = "B";
			}
			else if (bcls_rec->Tables[0].Rows[i]["SHIFT_NO"].ToString().Trim() = "丙班")
			{
				tmmsm12["PROD_SHIFT_GROUP"] = "C";
			}
			else if (bcls_rec->Tables[0].Rows[i]["SHIFT_NO"].ToString().Trim() = "丁班")
			{
				tmmsm12["PROD_SHIFT_GROUP"] = "D";
			}
			//班次
			if (bcls_rec->Tables[0].Rows[i]["SHIFT_NO"].ToString().Trim() = "夜")
			{
				tmmsm12["PROD_SHIFT_NO"] = "1";
			}
			else if (bcls_rec->Tables[0].Rows[i]["SHIFT_NO"].ToString().Trim() = "早")
			{
				tmmsm12["PROD_SHIFT_NO"] = "2";
			}
			else if (bcls_rec->Tables[0].Rows[i]["SHIFT_NO"].ToString().Trim() = "中")
			{
				tmmsm12["PROD_SHIFT_NO"] = "3";
			}
			tmmsm12["C_VALUE"] = bcls_rec->Tables[0].Rows[i]["C"];								//C
			tmmsm12["SI_VALUE"] = bcls_rec->Tables[0].Rows[i]["SI"];							//SI
			tmmsm12["MN_VALUE"] = bcls_rec->Tables[0].Rows[i]["MN"];							//MN
			tmmsm12["P_VALUE"] = bcls_rec->Tables[0].Rows[i]["P"];								//P
			tmmsm12["S_VALUE"] = bcls_rec->Tables[0].Rows[i]["S"];								//S
			tmmsm12["TI_VALUE"] = bcls_rec->Tables[0].Rows[i]["TI"];							//TI
			tmmsm12["V_VALUE"] = bcls_rec->Tables[0].Rows[i]["V"];								//V
			tmmsm12["SPLIT_INDICATION"] = bcls_rec->Tables[0].Rows[i]["SPLIT_INDICATION"];		//分包号
			tmmsm12["HANDLE_COUNT"] = bcls_rec->Tables[0].Rows[i]["TREATMENT_COUNTER"];			//处理次数
			tmmsm12["TIME_STAMPS"] = bcls_rec->Tables[0].Rows[i]["TIME_STAMP"];					//时间戳
			tmmsm12["CR_VALUE"] = bcls_rec->Tables[0].Rows[i]["CR"];							//CR
			tmmsm12["NI_VALUE"] = bcls_rec->Tables[0].Rows[i]["NI"];							//NI
			tmmsm12["IRON_LADLE_NO"] = bcls_rec->Tables[0].Rows[i]["LADLE_NUMBER"];				//铁水包号
			tmmsm12["STATION_NO"] = bcls_rec->Tables[0].Rows[i]["AGGREGATE_NAME"];				//工位号

			tmmsm12.TrimOrBlank();
			tmmsm12.Insert();
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