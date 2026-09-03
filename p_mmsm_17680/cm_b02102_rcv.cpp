/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-01-17
Description:铁水分配信息接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

int f_getSeqNextValue(CString SEQ_NAME, CString & SEQ_VALUE, CDbConnection * conn);
int f_t8ed01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE_TELE(cm_b02102_rcv)

int f_cm_b02102_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{

	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	CString time("");
	CString sqlstr("");
	
	/* 业务变量 */

	/* 数据库操作类定义 */
	CDbCommand cmd(conn);

	CModel tmmsm11("TMMSM11");

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//日期时间
	time = CDateTime::Now().ToString("HH:mm:ss");				//时间
	CString ticode = "";										//调拨单号
	CString stpc_seq = "";
	try
	{
		CString dealFlag = bcls_rec->Tables["B02102"].Rows[0]["DEAL_FLAG"].ToString();		//操作标志
		tmmsm11.MergeFrom(bcls_rec->Tables["B02102"].Rows[0]);
		//20250425bywcmTCP_ID加预留1实绩值
		tmmsm11["TPC_ID"] = bcls_rec->Tables["B02102"].Rows[0]["TPC_SEQ"].ToString()+bcls_rec->Tables["B02102"].Rows[0]["BACK1_VALUE"].ToString();

		if (dealFlag == "I")
		{
			if (!tmmsm11.Query("TPC_ID")){
				tmmsm11["REC_CREATOR"] = "B02102";
				tmmsm11["REC_CREATE_TIME"] = datetime;
				tmmsm11["COMPANY_CODE"] = "TG";
				tmmsm11["COMPANY_NAME"] = "TG";
				tmmsm11["PRACT_COLL_MODE"] = "1";

				if (f_getSeqNextValue("TI_CODE_SEQ", ticode, conn)){

				}
				stpc_seq = bcls_rec->Tables["B02102"].Rows[0]["TPC_SEQ"].ToString();
				tmmsm11["TICODE"] = ticode;
				tmmsm11["BF_ID"] = bcls_rec->Tables["B02102"].Rows[0]["BF_NO"].ToString();
				tmmsm11["TAPNO"] = bcls_rec->Tables["B02102"].Rows[0]["TCP_NO"].ToString();
				tmmsm11["IRON_NO"] = bcls_rec->Tables["B02102"].Rows[0]["TCP_NO"].ToString();
				tmmsm11["TPC_ID"] = bcls_rec->Tables["B02102"].Rows[0]["TPC_SEQ"].ToString()+bcls_rec->Tables["B02102"].Rows[0]["BACK1_VALUE"].ToString();
				tmmsm11["TPC_YL_NO"] = bcls_rec->Tables["B02102"].Rows[0]["TPC_NO"].ToString();
				tmmsm11["ELEM_C"] = bcls_rec->Tables["B02102"].Rows[0]["C_VALUE"].ToDecimal();
				tmmsm11["ELEM_SI"] = bcls_rec->Tables["B02102"].Rows[0]["SI_VALUE"].ToDecimal();
				tmmsm11["ELEM_MN"] = bcls_rec->Tables["B02102"].Rows[0]["MN_VALUE"].ToDecimal();
				tmmsm11["ELEM_P"] = bcls_rec->Tables["B02102"].Rows[0]["P_VALUE"].ToDecimal();
				tmmsm11["ELEM_S"] = bcls_rec->Tables["B02102"].Rows[0]["S_VALUE"].ToDecimal();
				tmmsm11["ADDSCRAP_WT"] = bcls_rec->Tables["B02102"].Rows[0]["BACK3_VALUE"].ToDecimal();
				tmmsm11["ALLOC_NUM"] = bcls_rec->Tables["B02102"].Rows[0]["BACK1_VALUE"].ToString();
				tmmsm11["IRON_TEMP_COM"] = bcls_rec->Tables["B02102"].Rows[0]["IRON_TEMP"].ToDecimal();
				tmmsm11["EMPTY_FLAG"] = "0";
				tmmsm11["IRON_TEMP"] = 0;
				tmmsm11["RECV_FLAG"] = "0";
				tmmsm11["ZL_SEND_FLAG"] = "0";

				tmmsm11.Insert();
				//来铁信息发送倒罐站
				//电文格式定义
				EIClass temp;
				temp.Tables.Add();
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "OP_FLAG");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_DELIVERYID");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_PRODUCTID");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_PRODUCTNAME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_BATCHID");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_BATCHUNIT");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "DELIVERY_THICKNESS");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "N_SENDAMOUNT");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_SENDUNIT");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_CLOSEGATETIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "N_SENDCOUNT");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_SENDCOUNTUNIT");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "SUM_WGT");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "D_BILLDATE");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "D_OPERATIONDATE");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_ACCEPTTIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_SALESCOMFIRMTIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "TIME_STAMP");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_SENDDEPT");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_ACCEPTDEPT");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_SENDSTOCK");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_ACCEPTSTOCK");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_ISFREEZE");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_STOCKSPEC");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_STATESIGN");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_PURVEYID");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "UPLOAD_RV_TIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "D_REQUIREDATE");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_OVERRULETIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_OUTSTOCKTIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_INSTOCKTIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_UPLOADTIME");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "I_YEAR");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "I_MONTH");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_ACCEPTUNIT");
				temp.Tables[0].Columns.Add(DsType::DT_STRING, "IS_301_RETURN");
				temp.Tables[0].Rows.Add();
				temp.Tables[0].Rows[0]["OP_FLAG"] = dealFlag;							//操作标志
				temp.Tables[0].Rows[0]["C_DELIVERYID"] = ticode;						//调拨单号
				temp.Tables[0].Rows[0]["C_PRODUCTID"] = "TS0000";						//物料代码
				temp.Tables[0].Rows[0]["C_PRODUCTNAME"] = "普通铁水";					//物料描述
				temp.Tables[0].Rows[0]["C_BATCHID"] = tmmsm11["BF_ID"].ToString().Substring(2, 1) + 
					"#" + tmmsm11["TAPNO"].ToString().Substring(2, 6) + 
					"#" + stpc_seq + tmmsm11["TPC_YL_NO"].ToString();		//批次号
				temp.Tables[0].Rows[0]["C_BATCHUNIT"] = tmmsm11["TPC_YL_NO"].ToString();		//罐号
				temp.Tables[0].Rows[0]["DELIVERY_THICKNESS"] = tmmsm11["ADDSCRAP_WT"];			//高炉加废钢量
				temp.Tables[0].Rows[0]["N_SENDAMOUNT"] = 0;										//发送重量
				temp.Tables[0].Rows[0]["C_SENDUNIT"] = "TON";									//发送单位
				temp.Tables[0].Rows[0]["C_CLOSEGATETIME"] = tmmsm11["TPC_ST_END_TIME"];			//关门时间
				//20250425bywcm预留实绩值赋值给发送次数,原来赋值为1
				temp.Tables[0].Rows[0]["N_SENDCOUNT"] = tmmsm11["ALLOC_NUM"];										//发送数量
				temp.Tables[0].Rows[0]["C_SENDCOUNTUNIT"] = "罐";								//发送数量单位
				temp.Tables[0].Rows[0]["SUM_WGT"] = 0;											//总重
				temp.Tables[0].Rows[0]["D_BILLDATE"] = datetime;
				temp.Tables[0].Rows[0]["D_OPERATIONDATE"] = datetime;
				temp.Tables[0].Rows[0]["T_ACCEPTTIME"] = datetime;
				temp.Tables[0].Rows[0]["TIME_STAMP"] = datetime;
				temp.Tables[0].Rows[0]["C_SENDDEPT"] = "6120";
				temp.Tables[0].Rows[0]["C_ACCEPTDEPT"] = "6240";
				temp.Tables[0].Rows[0]["C_SENDSTOCK"] = "6124";
				temp.Tables[0].Rows[0]["C_ACCEPTSTOCK"] = "6241";
				temp.Tables[0].Rows[0]["C_ISFREEZE"] = "FREE";
				temp.Tables[0].Rows[0]["C_STOCKSPEC"] = "FREE";
				temp.Tables[0].Rows[0]["C_STATESIGN"] = "1";
				temp.Tables[0].Rows[0]["C_PURVEYID"] = time;
				temp.Tables[0].Rows[0]["UPLOAD_RV_TIME"] = "19700101000000";
				temp.Tables[0].Rows[0]["D_REQUIREDATE"] = "19700101000000";
				temp.Tables[0].Rows[0]["T_OVERRULETIME"] = "19700101000000";
				temp.Tables[0].Rows[0]["T_OUTSTOCKTIME"] = "19700101000000";
				temp.Tables[0].Rows[0]["T_INSTOCKTIME"] = "19700101000000";
				temp.Tables[0].Rows[0]["T_UPLOADTIME"] = "19700101000000";
				temp.Tables[0].Rows[0]["C_ACCEPTUNIT"] = "TON";
				temp.Tables[0].Rows[0]["IS_301_RETURN"] = "N";

				//查询铁次关门时间
				sqlstr = "SELECT T.TAP_IRON_END_TIME FROM TMMSM11B T WHERE T.IRON_NO = @IRON_NO";
				cmd.SetCommandText(sqlstr);
				cmd.Parameters.Set("IRON_NO", tmmsm11["IRON_NO"]);
				cmd.ExecuteReader();

				if (cmd.Read())
				{
					temp.Tables[0].Rows[0]["T_SALESCOMFIRMTIME"] = cmd.GetString(1);	//铁次堵口时间
				}
				else{
					temp.Tables[0].Rows[0]["T_SALESCOMFIRMTIME"] = "19700101000000";
				}

				if (f_t8ed01_snd(&temp, bcls_ret, conn)){
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else{
				strcpy(s.msg, "罐次号[" + tmmsm11["TPC_ID"].ToString() + "]的记录已存在！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else if(dealFlag == "D"){
			sqlstr = "DELETE FROM TMMSM11 T WHERE T.TPC_ID = @TPC_ID";
			cmd.Close();
			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("TPC_ID", tmmsm11["TPC_ID"]);
			cmd.ExecuteNonQuery();

			//来铁信息发送倒罐站
			//电文格式定义
			EIClass temp;
			temp.Tables.Add();
			temp.Tables[0].Columns.Add(DsType::DT_STRING, "OP_FLAG");
			temp.Tables[0].Columns.Add(DsType::DT_STRING, "C_DELIVERYID");
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
			temp.Tables[0].Rows[0]["OP_FLAG"] = dealFlag;							//操作标志
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

			//根据罐次号获取调拨单号
			sqlstr = "SELECT T.TICODE FROM TMMSM11 T WHERE T.TPC_ID = @TPC_ID";
			cmd.Close();
			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("TPC_ID", tmmsm11["TPC_ID"]);
			cmd.ExecuteReader();

			if (cmd.Read())
			{
				temp.Tables[0].Rows[0]["C_DELIVERYID"] = cmd.GetString(1);	//铁次堵口时间

				if (f_t8ed01_snd(&temp, bcls_ret, conn)){
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
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

	return doFlag;
}