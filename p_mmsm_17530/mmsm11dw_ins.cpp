/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      郑强强
Version:     1.0
Date:        2023-01-12 13:44:44
Description: 单表通用保存-信融专用后台
**************************************************/

#include "stdafx.h"
#include "string.h"
#include "CUtils.h"

int f_getSeqNextValue(CString SEQ_NAME, CString & SEQ_VALUE, CDbConnection * conn);
int f_t8ed01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(mmsm11dw_ins)
int f_mmsm11dw_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString sqlstr = " ";
	CString table_name = " ";
	CString msgstr = "提示信息:";	//提示信息。
	int proc_sum = 0;				//操作总数
	CDbCommand cmd_sql(conn);
	CDataTable temp_table;
	CString cs_tc_no = "";
	CString cs_date = "";
	CString cs_date_1 = "";
	int index = 0;
	CString dealFlag = "";
	CModel tmmsm11b("TMMSM11B");
	CModel tmmsm11("TMMSM11");
	try
	{
		cs_tc_no = bcls_rec->Tables[0].Rows[0]["TC_NO"].ToString();
		cs_date = bcls_rec->Tables[0].Rows[0]["DATA"].ToString();
		Log::Info("", __FUNCTION__, "cs_date =[{0}]", cs_date);
		if (cs_tc_no=="B02101")
		{
			int i = 0;
			while (cs_date!="")
			{
				index = cs_date.Find("|");
				cs_date_1 = cs_date.Substring(0, index);
				if (i==13)
				{
					tmmsm11b["BF_ID"] = cs_date_1;
				}
				else if (i == 14)
				{
					dealFlag = cs_date_1;
				}
				else if (i == 15)
				{
					tmmsm11b["IRON_NO"] = cs_date_1;
				}
				else if (i == 16)
				{
					tmmsm11b["IRON_MOUTH_NO"] = cs_date_1;
				}
				else if (i == 17)
				{
					tmmsm11b["TAP_IRON_START_TIME"] = cs_date_1;
				}
				else if (i == 18)
				{
					tmmsm11b["TAP_IRON_END_TIME"] = cs_date_1;
				}
				else if (i == 19)
				{
					tmmsm11b["SLAG_IRON_START_TIME"] = cs_date_1;
				}
				else if (i == 20)
				{
					tmmsm11b["SLAG_IRON_END_TIME"] = cs_date_1;
				}
				else if (i == 21)
				{
					tmmsm11b["CAL_WEIGHT"] = cs_date_1;
				}
				else if (i == 22)
				{
					tmmsm11b["ACT_WEIGHT"] = cs_date_1;
				}
				else if (i == 23)
				{
					tmmsm11b["CAL_SLAG_WEIGHT"] = cs_date_1;
				}
				else if (i == 24)
				{
					tmmsm11b["IRON_TEMP"] = cs_date_1;
				}

				cs_date = cs_date.Substring(index+1, cs_date.GetLength()-index-1);
				i++;
			}
			if (dealFlag == "I"){
				tmmsm11b["REC_CREATOR"] = "B02101";
				tmmsm11b["REC_CREATE_TIME"] = datetime;
				tmmsm11b["COMPANY_CODE"] = "TG";
				tmmsm11b["COMPANY_NAME"] = "TG";
				tmmsm11b.TrimOrBlank();
				tmmsm11b.Insert();
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

		if (cs_tc_no == "B02102")
		{
			int i = 0;
			while (cs_date != "")
			{
				index = cs_date.Find("|");
				cs_date_1 = cs_date.Substring(0, index);
				if (i == 13)
				{
					dealFlag = cs_date_1;
				}
				else if (i == 14)
				{
					tmmsm11["BF_ID"] = cs_date_1;
				}
				else if (i == 15)
				{
					tmmsm11["TAPNO"] = cs_date_1;
					tmmsm11["IRON_NO"] = cs_date_1;
				}
				else if (i == 16)
				{
					tmmsm11["TPC_ID"] = cs_date_1;
				}
				else if (i == 17)
				{
					tmmsm11["TPC_YL_NO"] = cs_date_1;
				}
				else if (i == 18)
				{
					tmmsm11["LINE_NO"] = cs_date_1;
				}
				else if (i == 19)
				{
					tmmsm11["NET_WT_COMPUT"] = cs_date_1;
				}
				else if (i == 20)
				{
					tmmsm11["IRON_TEMP"] = cs_date_1;
				}
				else if (i == 21)
				{
					tmmsm11["TPC_ST_END_TIME"] = cs_date_1;
				}
				else if (i == 22)
				{
					tmmsm11["C_VALUE"] = cs_date_1;
				}
				else if (i == 23)
				{
					tmmsm11["SI_VALUE"] = cs_date_1;
				}
				else if (i == 24)
				{
					tmmsm11["MN_VALUE"] = cs_date_1;
				}
				else if (i == 25)
				{
					tmmsm11["P_VALUE"] = cs_date_1;
				}
				else if (i == 26)
				{
					tmmsm11["S_VALUE"] = cs_date_1;
				}
				else if (i == 27)
				{
					tmmsm11["ELEM_C"] = cs_date_1;
				}
				else if (i == 28)
				{
					tmmsm11["ELEM_SI"] = cs_date_1;
				}
				else if (i == 29)
				{
					tmmsm11["ELEM_MN"] = cs_date_1;
				}
				else if (i == 30)
				{
					tmmsm11["ELEM_P"] = cs_date_1;
				}
				else if (i == 31)
				{
					tmmsm11["ELEM_S"] = cs_date_1;
				}
				else if (i == 32)
				{
					tmmsm11["TI_VALUE"] = cs_date_1;
				}
				else if (i == 33)
				{
					tmmsm11["ALLOC_NUM"] = cs_date_1;
				}
				else if (i == 34)
				{
					
				}
				else if (i == 35)
				{
					tmmsm11["ADDSCRAP_WT"] = cs_date_1;
				}
				else if (i == 36)
				{
					
				}
				cs_date = cs_date.Substring(index + 1, cs_date.GetLength() - index - 1);
				i++;
			}
			tmmsm11.Print();
			if (dealFlag == "I")
			{
				if (!tmmsm11.QueryCount("TPC_ID")){
					tmmsm11["REC_CREATOR"] = "B02102";
					tmmsm11["REC_CREATE_TIME"] = datetime;
					tmmsm11["COMPANY_CODE"] = "TG";
					tmmsm11["COMPANY_NAME"] = "TG";
					tmmsm11["PRACT_COLL_MODE"] = "1";

					CString ticode = "";
					if (f_getSeqNextValue("TI_CODE_SEQ", ticode, conn)){

					}
					tmmsm11["TICODE"] = ticode;
					tmmsm11["EMPTY_FLAG"] = "0";
					tmmsm11["IRON_TEMP"] = 0;
					tmmsm11["RECV_FLAG"] = "0";

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
					temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_UPLOADTIME");
					temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_OUTSTOCKTIME");
					temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_INSTOCKTIME");
					temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_SALESCOMFIRMTIME");
					temp.Tables[0].Columns.Add(DsType::DT_STRING, "D_REQUIREDATE");
					temp.Tables[0].Columns.Add(DsType::DT_STRING, "TIME_STAMP");
					temp.Tables[0].Columns.Add(DsType::DT_STRING, "UPLOAD_RV_TIME");
					temp.Tables[0].Columns.Add(DsType::DT_STRING, "T_OVERRULETIME");
					temp.Tables[0].Rows.Add();
					temp.Tables[0].Rows[0]["OP_FLAG"] = dealFlag;							//操作标志
					temp.Tables[0].Rows[0]["C_DELIVERYID"] = ticode;						//调拨单号
					temp.Tables[0].Rows[0]["C_PRODUCTID"] = "TS0000";						//物料代码
					temp.Tables[0].Rows[0]["C_PRODUCTNAME"] = "普通铁水";					//物料描述
					temp.Tables[0].Rows[0]["C_BATCHID"] = tmmsm11["BF_ID"].ToString().Substring(2, 1) +
						"#" + tmmsm11["TAPNO"].ToString().Substring(2, 6) +
						"#" + tmmsm11["TPC_ID"].ToString() + tmmsm11["TPC_YL_NO"].ToString();		//批次号
					temp.Tables[0].Rows[0]["C_BATCHUNIT"] = tmmsm11["TPC_YL_NO"].ToString();		//罐号
					temp.Tables[0].Rows[0]["DELIVERY_THICKNESS"] = tmmsm11["ADDSCRAP_WT"];			//高炉加废钢量
					temp.Tables[0].Rows[0]["N_SENDAMOUNT"] = tmmsm11["NET_WT_COMPUT"];				//发送重量
					temp.Tables[0].Rows[0]["C_SENDUNIT"] = "TON";									//发送单位
					temp.Tables[0].Rows[0]["C_CLOSEGATETIME"] = tmmsm11["TPC_ST_END_TIME"];			//关门时间
					temp.Tables[0].Rows[0]["N_SENDCOUNT"] = 1;										//发送数量
					temp.Tables[0].Rows[0]["C_SENDCOUNTUNIT"] = "罐";								//发送数量单位
					temp.Tables[0].Rows[0]["SUM_WGT"] = tmmsm11["NET_WT_COMPUT"];					//总重
					temp.Tables[0].Rows[0]["D_BILLDATE"] = "00010101000000";
					temp.Tables[0].Rows[0]["D_OPERATIONDATE"] = "00010101000000";
					temp.Tables[0].Rows[0]["T_ACCEPTTIME"] = "00010101000000";
					temp.Tables[0].Rows[0]["T_UPLOADTIME"] = "00010101000000";
					temp.Tables[0].Rows[0]["T_OUTSTOCKTIME"] = "00010101000000";
					temp.Tables[0].Rows[0]["T_INSTOCKTIME"] = "00010101000000";
					temp.Tables[0].Rows[0]["T_SALESCOMFIRMTIME"] = "00010101000000";
					temp.Tables[0].Rows[0]["D_REQUIREDATE"] = "00010101000000";
					temp.Tables[0].Rows[0]["TIME_STAMP"] = datetime;
					temp.Tables[0].Rows[0]["UPLOAD_RV_TIME"] = "00010101000000";
					temp.Tables[0].Rows[0]["T_OVERRULETIME"] = "00010101000000";

					if (f_t8ed01_snd(&temp, bcls_ret, conn)){
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else{
					strcpy(s.msg, "罐次号[" + tmmsm11["TPC_ID"].ToString() + "]的记录已存在！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常 
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
		////Log::Warn("", __FUNCTION__, "CDbException: {0}", s.msg);
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
		////Log::Error("", __FUNCTION__, "CApplicationException: {0}", ex.GetMsg());
	}
	catch (CException& ex)
	{
		strncpy(s.sysmsg, (const char*)ex.GetMsg(), sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
		////Log::Fatal("", __FUNCTION__, "CException: {0}", ex.GetMsg());
	}
	return doFlag;
}


