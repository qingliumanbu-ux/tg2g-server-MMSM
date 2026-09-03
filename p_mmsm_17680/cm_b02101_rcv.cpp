/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-01-17
Description:高炉出铁实绩接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

int f_t8ed01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE_TELE(cm_b02101_rcv)

int f_cm_b02101_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	CString sqlstr("");
	/* 业务变量 */

	/* 数据库操作类定义 */
	CDbCommand cmd(conn);

	CModel tmmsm11b("TMMSM11B");

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		CString dealFlag = bcls_rec->Tables["B02101"].Rows[0]["TAP_EVENT_TYPE"].ToString();		//事件区分
		Log::Trace("", __FUNCTION__, "dealFlag = {0}", dealFlag);
		tmmsm11b.MergeFrom(bcls_rec->Tables["B02101"].Rows[0]);
		tmmsm11b["BF_ID"] = bcls_rec->Tables["B02101"].Rows[0]["BF_NO"].ToString();
		tmmsm11b["IRON_NO"] = bcls_rec->Tables["B02101"].Rows[0]["TCP_NO"].ToString();
		tmmsm11b["IRON_MOUTH_NO"] = bcls_rec->Tables["B02101"].Rows[0]["TAP_NO"].ToString();
		tmmsm11b["TAP_IRON_START_TIME"] = bcls_rec->Tables["B02101"].Rows[0]["TAP_START_TIME"].ToString();
		tmmsm11b["TAP_IRON_END_TIME"] = bcls_rec->Tables["B02101"].Rows[0]["TAP_END_TIME"].ToString();
		tmmsm11b["SLAG_IRON_START_TIME"] = bcls_rec->Tables["B02101"].Rows[0]["SLAG_START_TIME"].ToString();
		tmmsm11b["SLAG_IRON_END_TIME"] = bcls_rec->Tables["B02101"].Rows[0]["SLAG_END_TIME"].ToString();
		tmmsm11b["IRON_TEMP"] = bcls_rec->Tables["B02101"].Rows[0]["TAP_TEMP"].ToString();
		tmmsm11b.Print();

		if (dealFlag == "I"){
			tmmsm11b["REC_CREATOR"] = "B02101";
			tmmsm11b["REC_CREATE_TIME"] = datetime;
			tmmsm11b["COMPANY_CODE"] = "TG";
			tmmsm11b["COMPANY_NAME"] = "TG";
			tmmsm11b.Insert();

			//查询铁次对应罐次信息
			sqlstr = "SELECT T.TICODE FROM TMMSM11 T WHERE T.IRON_NO = @IRON_NO";
			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("IRON_NO", tmmsm11b["IRON_NO"]);
			cmd.ExecuteReader();

			while (cmd.Read())
			{
				//来铁信息发送倒罐站
				//电文格式定义
				EIClass temp;
				temp.Tables.Add();
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "OP_FLAG");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_DELIVERYID");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_SALESCOMFIRMTIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_CLOSEGATETIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "D_BILLDATE");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "D_OPERATIONDATE");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_ACCEPTTIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "TIME_STAMP");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "UPLOAD_RV_TIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "D_REQUIREDATE");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_OVERRULETIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_OUTSTOCKTIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_INSTOCKTIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_UPLOADTIME");
				temp.Tables[0].Rows.Add();
				temp.Tables[0].Rows[0]["OP_FLAG"] = dealFlag;									//操作标志
				temp.Tables[0].Rows[0]["C_DELIVERYID"] = cmd.GetString(1);						//调拨单号

				Log::Trace("", __FUNCTION__, "C_DELIVERYID = {0}", cmd.GetString(1));
				temp.Tables[0].Rows[0]["T_SALESCOMFIRMTIME"] = tmmsm11b["TAP_IRON_END_TIME"];	//铁次堵口时间
				temp.Tables[0].Rows[0]["C_CLOSEGATETIME"] = datetime;
				temp.Tables[0].Rows[0]["D_BILLDATE"] = datetime;
				temp.Tables[0].Rows[0]["D_OPERATIONDATE"] = datetime;
				temp.Tables[0].Rows[0]["T_ACCEPTTIME"] = datetime;
				temp.Tables[0].Rows[0]["TIME_STAMP"] = datetime;
				temp.Tables[0].Rows[0]["UPLOAD_RV_TIME"] = "19700101000000";
				temp.Tables[0].Rows[0]["D_REQUIREDATE"] = "19700101000000";
				temp.Tables[0].Rows[0]["T_OVERRULETIME"] = "19700101000000";
				temp.Tables[0].Rows[0]["T_OUTSTOCKTIME"] = "19700101000000";
				temp.Tables[0].Rows[0]["T_INSTOCKTIME"] = "19700101000000";
				temp.Tables[0].Rows[0]["T_UPLOADTIME"] = "19700101000000";
				if (f_t8ed01_snd(&temp, bcls_ret, conn)){
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		else if (dealFlag == "U"){
			CModel tmmsm11b_old("TMMSM11B");
			tmmsm11b_old["IRON_NO"] = tmmsm11b["IRON_NO"];
			if (tmmsm11b_old.Query("IRON_NO"))
			{
				tmmsm11b_old["REC_REVISOR"] = "B02101";
				tmmsm11b_old["REC_REVISE_TIME"] = datetime;

				tmmsm11b_old["BF_ID"] = tmmsm11b["BF_ID"];
				tmmsm11b_old["IRON_MOUTH_NO"] = tmmsm11b["IRON_MOUTH_NO"];
				tmmsm11b_old["TAP_IRON_START_TIME"] = tmmsm11b["TAP_IRON_START_TIME"];
				tmmsm11b_old["TAP_IRON_END_TIME"] = tmmsm11b["TAP_IRON_END_TIME"];
				tmmsm11b_old["SLAG_IRON_START_TIME"] = tmmsm11b["SLAG_IRON_START_TIME"];
				tmmsm11b_old["SLAG_IRON_END_TIME"] = tmmsm11b["SLAG_IRON_END_TIME"];
				tmmsm11b_old["CAL_WEIGHT"] = tmmsm11b["CAL_WEIGHT"];
				tmmsm11b_old["ACT_WEIGHT"] = tmmsm11b["ACT_WEIGHT"];
				tmmsm11b_old["CAL_SLAG_WEIGHT"] = tmmsm11b["CAL_SLAG_WEIGHT"];
				tmmsm11b_old["IRON_TEMP"] = tmmsm11b["IRON_TEMP"];

				tmmsm11b_old.Update("REC_REVISOR, REC_REVISE_TIME, BF_ID, IRON_MOUTH_NO, TAP_IRON_START_TIME, TAP_IRON_END_TIME, SLAG_IRON_START_TIME, "
					"SLAG_IRON_END_TIME, CAL_WEIGHT, ACT_WEIGHT, CAL_SLAG_WEIGHT, IRON_TEMP", "IRON_NO");
			}
		}
		else if (dealFlag == "D")
		{
			tmmsm11b.Delete();
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

	cmd.Close();
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}