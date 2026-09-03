/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-03-11
Description:鱼雷罐实绩接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

int f_mmsm_210050_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_21b004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE_TELE(cm_edt803_rcv)

int f_cm_edt803_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 		//系统日志类定义

	int doFlag = 0;					//返回值 

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//当前时间

	CModel tmmsm11("TMMSM11");		//炼钢铁水信息表实体类
	CModel tmmsm12("TMMSM12");		//炼钢倒罐实绩表 
	CModel tmmsm12a("TMMSM12A");	//炼钢鱼雷罐倒铁实绩表
	CModel tqmts24("TQMTS24");		//

	CString TPD_NO = "";				//倒罐处理号
	CString IRON_LADLE_NO = "";			//铁包号

	CString sqlstr = "";
	CDbCommand cmd(conn);	


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			TPD_NO = bcls_rec->Tables[0].Rows[i]["HEAT_NUMBER"].ToString();									//倒罐处理号

			tmmsm11.Reset();
			tmmsm11.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			tmmsm11["REC_REVISOR"] = "EDT803";
			tmmsm11["REC_REVISE_TIME"] = datetime;
			tmmsm11["BATCH_NO"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_BATCH_NUMBER"];
			tmmsm11["TPC_YL_NO"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_NUMBER"];
			tmmsm11["IRON_TEMP"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["HM_TEMP"].ToString());
			tmmsm11["NWEIGHT"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["HM_WEIGHT"].ToString());
			tmmsm11["TIME_TORPEDO_IN"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_ARRIVAL_TIME"];
			tmmsm11["TIME_TORPEDO_OUT"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_LEAVE_TIME"];
			tmmsm11["C_SAMPLEID"] = bcls_rec->Tables[0].Rows[i]["SAMPLE_ID"];
			tmmsm11["TPC_SOURCE"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_SOURCE"];
			tmmsm11["TIDCODE"] = bcls_rec->Tables[0].Rows[i]["PLAN_ID"];
			tmmsm11["C_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["C"].ToString());
			tmmsm11["SI_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["SI"].ToString());
			tmmsm11["MN_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["MN"].ToString());
			tmmsm11["P_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["P"].ToString());
			tmmsm11["S_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["S"].ToString());
			tmmsm11["TI_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["TI"].ToString());
			tmmsm11["V_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["V"].ToString());
			tmmsm11["HEAT_NO"] = bcls_rec->Tables[0].Rows[i]["HEAT_NUMBER"];
			tmmsm11["HM_WT_MANUAL"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["HM_WEIGHT_MANUAL"].ToString());
			tmmsm11["CR_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["CR"].ToString());
			tmmsm11["NI_VALUE"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["NI"].ToString());
			tmmsm11["STATION_NO"] = bcls_rec->Tables[0].Rows[i]["POSITION_ID"];
			tmmsm11["C_CLOSEGATETIME"] = bcls_rec->Tables[0].Rows[i]["DEAD_WEIGHT_DATE"];
			tmmsm11["IS_TO_LIANTIE"] = bcls_rec->Tables[0].Rows[i]["IS_TO_LIANTIE"];
			tmmsm11["TICODE"] = bcls_rec->Tables[0].Rows[i]["C_DELIVERYID"];
			tmmsm11["TPD_NO"] = bcls_rec->Tables[0].Rows[i]["HEAT_NUMBER"];
			tmmsm11.TrimOrBlank();

			tmmsm11.Update("REC_REVISOR, REC_REVISE_TIME, BATCH_NO, TPC_YL_NO, IRON_TEMP, NWEIGHT, "
				"TIME_TORPEDO_IN, TIME_TORPEDO_OUT, C_SAMPLEID, TPC_SOURCE, TIDCODE, C_VALUE, SI_VALUE, "
				"MN_VALUE, P_VALUE, S_VALUE, TI_VALUE, V_VALUE, HEAT_NO, HM_WT_MANUAL, CR_VALUE, NI_VALUE, "
				"STATION_NO, C_CLOSEGATETIME, IS_TO_LIANTIE, TPD_NO", "TICODE");

			if (tmmsm11.Query("TICODE"))
			{
				//发送铁水数据给制造管理系统
				EIClass temp;
				temp.Tables.Add("BAPIHEADER");
				temp.Tables["BAPIHEADER"].Columns.Add(DsType::DT_STRING, "MSGTYPE");
				temp.Tables["BAPIHEADER"].Rows.Add();
				temp.Tables["BAPIHEADER"].Rows[0]["MSGTYPE"] = "ZCHO_QM_TS_RFC";
				temp.Tables.Add("TQMTSTS");
				temp.Tables["TQMTSTS"].Columns.Add(DsType::DT_STRING, "ID");
				temp.Tables["TQMTSTS"].Columns.Add(DsType::DT_STRING, "JC_NO");
				temp.Tables["TQMTSTS"].Columns.Add(DsType::DT_STRING, "BATCH_NO");
				temp.Tables["TQMTSTS"].Columns.Add(DsType::DT_STRING, "SEND_WEIGHT");
				temp.Tables["TQMTSTS"].Columns.Add(DsType::DT_STRING, "CLOSE_TIME");
				temp.Tables["TQMTSTS"].Columns.Add(DsType::DT_STRING, "SI");
				temp.Tables["TQMTSTS"].Columns.Add(DsType::DT_STRING, "S");
				temp.Tables["TQMTSTS"].Columns.Add(DsType::DT_STRING, "TIMESTAMPS");
				temp.Tables["TQMTSTS"].Columns.Add(DsType::DT_STRING, "BACK1");
				temp.Tables["TQMTSTS"].Rows.Add();
				temp.Tables["TQMTSTS"].Rows[0]["ID"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_BATCH_NUMBER"];
				temp.Tables["TQMTSTS"].Rows[0]["JC_NO"] = bcls_rec->Tables[0].Rows[i]["TORPEDO_NUMBER"];
				temp.Tables["TQMTSTS"].Rows[0]["BATCH_NO"] = tmmsm11["BF_ID"].ToString().Substring(2, 1) + "#" + tmmsm11["TAPNO"].ToString().Substring(2, 6) + "#" + tmmsm11["TPC_ID"].ToString() + tmmsm11["TPC_YL_NO"].ToString();
				temp.Tables["TQMTSTS"].Rows[0]["SEND_WEIGHT"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["HM_WEIGHT"].ToString());
				temp.Tables["TQMTSTS"].Rows[0]["CLOSE_TIME"] = bcls_rec->Tables[0].Rows[i]["DEAD_WEIGHT_DATE"];
				temp.Tables["TQMTSTS"].Rows[0]["SI"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["SI"].ToString());
				temp.Tables["TQMTSTS"].Rows[0]["S"] = CDecimal::Parse(bcls_rec->Tables[0].Rows[i]["S"].ToString());
				temp.Tables["TQMTSTS"].Rows[0]["TIMESTAMPS"] = datetime;
				temp.Tables["TQMTSTS"].Rows[0]["BACK1"] = "6240";
				if (f_mmsm_210050_snd(&temp, bcls_ret, conn) != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				Log::Trace("", __FUNCTION__, "倒罐处理号：{0}", TPD_NO);
				cmd.Close();
				sqlstr = "SELECT T.IRON_LADLE_NO FROM TMMSM12 T WHERE T.TPD_NO = @TPD_NO";
				cmd.SetCommandText(sqlstr);
				cmd.Parameters.Set("TPD_NO", TPD_NO);
				cmd.ExecuteReader();

				if (cmd.Read())
				{
					IRON_LADLE_NO = cmd.GetString(1);
				}
				else{
					IRON_LADLE_NO = tmmsm11["BATCH_NO"];
				}
				Log::Trace("", __FUNCTION__, "铁包号：{0}", IRON_LADLE_NO);

				sqlstr = "UPDATE TMMSM12A SET IRON_NO = @IRON_NO, IRON_LADLE_NO = @IRON_LADLE_NO, TRE_TPC_NO = @TRE_TPC_NO "
					"WHERE IRON_NO = @tmmsm12a.IRON_NO AND TPD_NO = @tmmsm12a.TPD_NO";
				cmd.SetCommandText(sqlstr);
				cmd.Parameters.Set("IRON_NO", tmmsm11["TAPNO"]);
				cmd.Parameters.Set("IRON_LADLE_NO", IRON_LADLE_NO);
				cmd.Parameters.Set("TRE_TPC_NO", tmmsm11["TPC_ID"]);
				cmd.Parameters.Set("tmmsm12a.IRON_NO", tmmsm11["BATCH_NO"]);
				cmd.Parameters.Set("tmmsm12a.TPD_NO", TPD_NO);
				cmd.ExecuteNonQuery();

				
				//发送铁区铁水质量信息
				cmd.Close();
				sqlstr = "SELECT T.ST_SAMPLE_NO FROM TQMTS24 T WHERE T.HEAT_NO = @HEAT_NO";
				cmd.SetCommandText(sqlstr);
				cmd.Parameters.Set("HEAT_NO", tmmsm11["HEAT_NO"]);
				bcls_ret->Tables.Add();
				cmd.ExecuteQuery(bcls_ret->Tables[1]);

				for (int i = 0; i < bcls_ret->Tables[1].Rows.get_Count(); i++){
					CString ST_SAMPLE_NO = bcls_ret->Tables[1].Rows[i]["ST_SAMPLE_NO"].ToString();

					cmd.Close();
					sqlstr = "SELECT 'I' DEAL_FLAG,"
						"@TAPNO TCP_NO,"
						"@TPC_ID TPC_SEQ,"
						"@TPC_YL_NO TPC_NO,"
						"ST_SAMPLE_NO SAMPLE_NO,"
						"'TS0000' MAT_CODE,"
						"'普通铁水' MAT_CNAME,"
						"DEV_CODE SAMPLE_POS_CODE,"
						"SAMPLE_TAKEN_TIME SAMPLE_TIME,"
						"ANALYSE_TIME,"
						"7 ANALYSE_ITEM_NUM,"
						"DECODE(ANALYSE_ITEM_NAME,"
						"'C',"
						"'Y001',"
						"'Si',"
						"'Y002',"
						"'Mn',"
						"'Y003',"
						"'P',"
						"'Y004',"
						"'S',"
						"'Y005',"
						"'Ti',"
						"'Y006',"
						"'V',"
						"'Y028',"
						"ANALYSE_ITEM_NAME) ANALYSE_ITEM_CODE,"
						"ANALYSE_ITEM_NAME,"
						"'Y' ANALYSE_DATA_TYPE,"
						"ANALYSE_ITEM_VALUE "
						"FROM(SELECT T.ST_SAMPLE_NO,"
						"T.DEV_CODE,"
						"T.SAMPLE_TAKEN_TIME,"
						"T.ANALYSE_TIME,"
						"T.ELM_001 \"C\","
						"T.ELM_002 \"Si\","
						"T.ELM_003 \"Mn\","
						"T.ELM_004 \"P\","
						"T.ELM_005 \"S\","
						"T.ELM_013 \"Ti\","
						"T.ELM_012 \"V\" "
						"FROM TQMTS24 T "
						"WHERE T.ST_SAMPLE_NO = @ST_SAMPLE_NO) UNPIVOT(\"ANALYSE_ITEM_VALUE\" FOR \"ANALYSE_ITEM_NAME\" IN(\"C\","
						"\"Si\","
						"\"Mn\","
						"\"P\","
						"\"S\","
						"\"V\","
						"\"Ti\"))";
					cmd.SetCommandText(sqlstr);
					cmd.Parameters.Set("TAPNO", tmmsm11["TAPNO"]);
					cmd.Parameters.Set("TPC_ID", tmmsm11["TPC_ID"]);
					cmd.Parameters.Set("TPC_YL_NO", tmmsm11["TPC_YL_NO"]);
					cmd.Parameters.Set("ST_SAMPLE_NO", ST_SAMPLE_NO);
					cmd.ExecuteQuery(bcls_ret->Tables[0]);

				/*	if (bcls_ret->Tables[0].Rows.get_Count() > 0){
						if (f_21b004_snd(bcls_rec, bcls_ret, conn)){
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}*/
				}

				tmmsm11["ZL_SEND_FLAG"] = "1";
				tmmsm11.Update("ZL_SEND_FLAG","TICODE");
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

	//返回-1时事务将回滚，返回为0是事务将提交	
	return doFlag;
}