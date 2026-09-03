/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 测温数据
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

int f_mmsm2b_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE_TELE(cm_e2t8t1_rcv)

int f_cm_e2t8t1_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);
	CDbCommand cmd_inq_count(conn);
	CDbCommand cmd_inq_test(conn);
	CModel tmmsm2b("TMMSM2B");
	CString heat_no = "";
	EIClass tmmsm2b_back;
	CString dev_code = "";
	CString sm_plan_no = " ";
	CDecimal count = 1;

	//加入函数的表

	tmmsm2b_back.Tables.Add();
	if (!tmmsm2b_back.Tables[0].Columns.Contains("PROC_DIV"))
	{
		tmmsm2b_back.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
	}
	if (!tmmsm2b_back.Tables[0].Columns.Contains("AREA_ID"))
	{
		tmmsm2b_back.Tables[0].Columns.Add(DT_STRING, "AREA_ID");
	}
	//tmmsm2b_back.Tables[0].Rows.Add();
	tmmsm2b_back.Tables[0].Columns.Add(tmmsm2b);
	//tmmsm2b_back.Tables[0].Rows.Add();

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		for (size_t i = 0; i < bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows.get_Count(); i++)
		{
			///工号
			dev_code = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[i]["AGGREGATE_NAME"].ToString();

			sm_plan_no = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[0]["ORDER_NUMBER"].ToString();

			//同工位处理次数
			tmmsm2b["PROC_COUNT"] = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[0]["TREATMENT_COUNTER"].ToString();
			tmmsm2b["STATION_ID"] = dev_code.Substring(0, 1);
			//熔炼号
			if (tmmsm2b["STATION_ID"].ToString() != "Z" && tmmsm2b["STATION_ID"].ToString() != "E"){
				tmmsm2b["HEAT_NO"] = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[0]["HEAT_NUMBER"].ToString();
			}
			else{
				tmmsm2b["HEAT_NO"] = " ";
			}
			//计划号

			tmmsm2b["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[0]["ORDER_NUMBER"].ToString();
			//二级炉号作为处理号存储到L2_PROC_NO字段中
			tmmsm2b["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[0]["HEAT_NUMBER"].ToString();
			Log::Info("", __FUNCTION__, "L2_PROC_NO=[{0}]", tmmsm2b["L2_PROC_NO"].ToString());
			Log::Info("", __FUNCTION__, "SM_PLAN_NO=[{0}]", tmmsm2b["SM_PLAN_NOL2"].ToString());
			Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no);
			//根据计划号获取熔炼号和制造命令号
			//根据计划号获取熔炼号和制造命令号
			cmd_inq_code.SetCommandText(" SELECT  PONO, SM_PLAN_NO FROM "
				" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
				" UNION "
				" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no + "'");
			cmd_inq_code.ExecuteReader();
			if (cmd_inq_code.Read())
			{
				tmmsm2b["PONO"] = cmd_inq_code.GetString(1);
				tmmsm2b["SM_PLAN_NO"] = cmd_inq_code.GetString(2);
			}
			cmd_inq_code.Close();
			//查询熔炼号
			Log::Info("", __FUNCTION__, "HEAT_NO=[{0}]", tmmsm2b["HEAT_NO"].ToString());
			sqlstr = " select PROC_NO from( "
				" SELECT PROC_NO, DEV_CODE, HEAT_NO FROM TPSSM12 "
				" union "
				" SELECT PROC_NO, DEV_CODE, HEAT_NO FROM TPSSM42 "
				" )  WHERE HEAT_NO ='" + tmmsm2b["HEAT_NO"].ToString() + "' and DEV_CODE = '" + dev_code + "' ";

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm2b["PROC_NO"] = cmd_inq.GetString(1);
			}
			Log::Info("", __FUNCTION__, "cmd_inq   =[{0}]", sqlstr);
			cmd_inq.Close();
			Log::Info("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm2b["PROC_NO"].ToString());
			if (tmmsm2b["PROC_NO"].ToString() != " " || tmmsm2b["PROC_NO"].ToString() != ""){
				Log::Info("", __FUNCTION__, "1=[{0}]", "1");
				if (tmmsm2b["SM_PLAN_NOL2"].ToString() == "11111111"){//11111111
					cmd_inq_count.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2b where  L2_PROC_NO ='" + tmmsm2b["L2_PROC_NO"].ToString().Trim() + "'  ");
				}
				else{
					if (tmmsm2b["HEAT_NO"].ToString() == " "){
						cmd_inq_count.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2b where  SM_PLAN_NOL2 ='" + tmmsm2b["SM_PLAN_NOL2"].ToString().Trim() + "' ");
					}
					else{
						cmd_inq_count.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2b where  HEAT_NO ='" + tmmsm2b["HEAT_NO"].ToString().Trim() + "' ");
					}
				}
			}
			else{
				Log::Info("", __FUNCTION__, "2=[{0}]", "2");
				if (tmmsm2b["SM_PLAN_NOL2"].ToString() == "11111111"){
					cmd_inq_count.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2b where  L2_PROC_NO ='" + tmmsm2b["L2_PROC_NO"].ToString().Trim() + "'  ");
				}
				else{
					Log::Info("", __FUNCTION__, "3=[{0}]", "3");
					cmd_inq_count.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2b where  SM_PLAN_NOL2 ='" + tmmsm2b["SM_PLAN_NOL2"].ToString().Trim() + "' ");
				}
			}
			tmmsm2b["PROC_COUNT"] = cmd_inq_count.ExecuteScalar();
			Log::Info("", __FUNCTION__, "PROC_COUNT=[{0}]", tmmsm2b["PROC_COUNT"].ToString());
			cmd_inq_count.Close();


			tmmsm2b["DEV_CODE"] = dev_code;
			tmmsm2b["PRACT_COLL_MODE"] = "0";
			//分包号
			tmmsm2b["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[i]["SPLIT_INDICATION"].ToString();
			//温度
			tmmsm2b["STEEL_TEMP"] = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[i]["TEMP"].ToString();
			//碳
			tmmsm2b["C_AIM"] = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[i]["C"].ToString();
			//氧
			tmmsm2b["O_AIM"] = bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[i]["OXYGEN"].ToString();
			tmmsm2b["REC_CREATOR"] = s.userid;
			tmmsm2b["REC_CREATE_TIME"] = datetime;
			/*if (tmmsm2b["HEAT_NO"].ToString() != " " || tmmsm2b["HEAT_NO"].ToString() != ""){
				cmd_inq_count.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2b where  HEAT_NO ='" + tmmsm2b["HEAT_NO"].ToString().Trim() + "' ");
			}
			else{
				cmd_inq_count.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2b where  HEAT_NO ='" + tmmsm2b["HEAT_NO"].ToString().Trim() + "'");
			}
			tmmsm2b["PROC_COUNT"] = cmd_inq_count.ExecuteScalar();*/
			Log::Info("", __FUNCTION__, "PROC_COUNT=[{0}]", tmmsm2b["PROC_COUNT"].ToString());
			cmd_inq_count.Close();
			tmmsm2b.MergeTo(tmmsm2b_back.Tables[0], false);
			if (tmmsm2b.QueryCount("PROC_NO,HEAT_NO,PROC_COUNT"))
			{
				Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "U");
				tmmsm2b_back.Tables[0].Rows[0]["PROC_DIV"] = "U";
				doFlag = f_mmsm2b_proc(&tmmsm2b_back, bcls_ret, conn);

				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				//tmmsm2b.Update("PROC_NO");
			}
			else
			{
				Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "I");
				tmmsm2b_back.Tables[0].Rows[0]["PROC_DIV"] = "I";
				doFlag = f_mmsm2b_proc(&tmmsm2b_back, bcls_ret, conn);

				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/*tmmsm2b.TrimOrBlank();
				tmmsm2b.Insert();*/
			}
			/*tmmsm2b.TrimOrBlank();
			tmmsm2b.Insert();*/
		}
		//tmmsm2b.MergeFrom(bcls_rec->Tables["INT_MES_TEMP_MEAS"].Rows[i]);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
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

	return doFlag;
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


