/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-03-11
Description:鱼雷罐运转事件接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

//外部函数引用
int f_21b003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//发送铁区MES收料确认实绩
int f_21b004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//发送铁区MES收料确认实绩

BM2F_ENTERACE_TELE(cm_edt807_rcv)

int f_cm_edt807_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 		//系统日志类定义

	int doFlag = 0;					//返回值 

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");		//当前时间

	CModel tmmsm11("TMMSM11");		//炼钢铁水信息表

	CString DEAL_FLAG = "";			//操作标记
	CString C_BATCHID = "";			//高炉号、铁次号、罐次号、罐号集合
	CString BF_NO = "";				//高炉号
	CString IR_TAP_NO = "";			//铁次号
	CString TRE_TPC_NO = "";		//罐次号
	CString TPC_NO = "";			//鱼雷罐号
	CString tmmsm11UpdateFields = "";	//TMMSM11表更新字段
	CString cs_st_sample_no = "";
	CString sqlstr = "";

	CDbCommand cmd(conn);
	//收料确认电文格式定义
	EIClass temp;
	temp.Tables.Add();
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "DEAL_FLAG");			//操作标记
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "WEIGH_NO");			//磅单号
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "WORK_DATE");			//作业日期
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "CAR_NO");			//车号
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "MAT_CODE");			//物料代码
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "SRC_STOCK_CODE");	//源库区代码
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "LOAD_POS_CODE");		//装点代码			
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "DST_STOCK_CODE");	//目的库区代码
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "UNLOAD_POS_CODE");	//卸点代码
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "WEIGH_APP_NO");		//计量委托号
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "WEIGH_TIME");		//计量时刻
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "TARE_WT");			//皮重
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "GROSS_WT");			//毛重(炼钢加废钢量)
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "NET_WT");			//净重
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "SETTLEMENT_WT");		//结算重量
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "TCP_NO");			//铁次号
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "TPC_SEQ");			//罐次号
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "TPC_NO");			//鱼雷罐号
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "EMPTY_SIGN");		//空罐标志
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "BACK1");				//备用1(炼钢调拨单号)
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "BACK2");				//备用2
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "BACK3");				//备用3(铁水到达温度)
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "BACK4");				//备用4
	temp.Tables[0].Columns.Add(DsType::DT_STRING, "BACK5");				//备用5(铁水到达时间)

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++){
			DEAL_FLAG = bcls_rec->Tables[0].Rows[i]["DEAL_FLAG"].ToString();
			C_BATCHID = bcls_rec->Tables[0].Rows[i]["C_BATCHID"].ToString();
			tmmsm11["TICODE"] = bcls_rec->Tables[0].Rows[i]["C_DELIVERYID"];
			if (!tmmsm11.Query("TICODE"))
			{
				strcpy(s.msg, "TMMSM11表中调拨单号[" + tmmsm11["TICODE"].ToString() + "]的记录不存在！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			temp.Tables[0].Rows.Add();

			if (DEAL_FLAG == "4"){
				// 发送成分信息
				cs_st_sample_no = bcls_rec->Tables[0].Rows[i]["EMPTY_SIGN"].ToString().Trim() + bcls_rec->Tables[0].Rows[i]["TIME_STAMPS"].ToString().Trim();
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
				cmd.Parameters.Set("ST_SAMPLE_NO", cs_st_sample_no);
				cmd.ExecuteQuery(bcls_ret->Tables[0]);
				
				if (bcls_ret->Tables[0].Rows.get_Count() > 0){
					if (f_21b004_snd(bcls_rec, bcls_ret, conn)){
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			
			}
			else
			{
				int char1Index = C_BATCHID.Find("#", 0);
				int char2Index = C_BATCHID.Find("#", char1Index + 1);
				int char3Index = C_BATCHID.Find("Y", char2Index);
				Log::Trace("", __FUNCTION__, "index1 = {0}, index = {1}, index = {2}", char1Index, char2Index, char3Index);

				BF_NO = "BF"+C_BATCHID.Substring(0, char1Index);
				IR_TAP_NO = "BF" + C_BATCHID.Substring(char1Index + 1, char2Index - char1Index - 1);
				TRE_TPC_NO = C_BATCHID.Substring(char2Index + 1, char3Index - char2Index - 1);
				TPC_NO = C_BATCHID.Substring(char3Index, 3);
				Log::Trace("", __FUNCTION__, "BF_NO = {0}", BF_NO);
				Log::Trace("", __FUNCTION__, "IR_TAP_NO = {0}", IR_TAP_NO);
				Log::Trace("", __FUNCTION__, "TRE_TPC_NO = {0}", TRE_TPC_NO);
				Log::Trace("", __FUNCTION__, "TPC_NO = {0}", TPC_NO);

				//通过调拨单号获取炼钢铁水信息表
				

				//固定更新字段
				tmmsm11["REC_REVISOR"] = "EDT807";
				tmmsm11["REC_REVISE_TIME"] = datetime;
				tmmsm11["RECV_FLAG"] = DEAL_FLAG;
				tmmsm11["RECV_TIME"] = datetime;
				tmmsm11["RECV_BY"] = "EDT807";
				tmmsm11UpdateFields += "REC_REVISOR, REC_REVISE_TIME, RECV_FLAG, RECV_TIME, RECV_BY, ";

				//电文固定发送字段
				temp.Tables[0].Rows[i]["DEAL_FLAG"] = DEAL_FLAG;
				temp.Tables[0].Rows[i]["WORK_DATE"] = datetime.Substring(0, 8);
				temp.Tables[0].Rows[i]["TCP_NO"] = IR_TAP_NO;
				temp.Tables[0].Rows[i]["TPC_SEQ"] = TRE_TPC_NO;
				temp.Tables[0].Rows[i]["TPC_NO"] = TPC_NO;

				//预计倒空
				if (DEAL_FLAG == "1")
				{
					//预计倒空更新字段
					tmmsm11["EMPTY_TIME"] = bcls_rec->Tables[0].Rows[i]["WEIGH_TIME"];
					tmmsm11UpdateFields += "EMPTY_TIME, ";

					//预计倒空电文发送字段
					temp.Tables[0].Rows[i]["WEIGH_TIME"] = bcls_rec->Tables[0].Rows[i]["WEIGH_TIME"];
				}
				//倒空
				else if (DEAL_FLAG == "2"){
					//倒空更新字段
					tmmsm11["EMPTY_FLAG"] = "1";
					tmmsm11["EMPTY_TIME_ACT"] = bcls_rec->Tables[0].Rows[i]["WEIGH_TIME"];
					tmmsm11UpdateFields += "EMPTY_FLAG, EMPTY_TIME_ACT, ";

					//倒空电文发送字段
					temp.Tables[0].Rows[i]["WEIGH_TIME"] = bcls_rec->Tables[0].Rows[i]["WEIGH_TIME"];
					temp.Tables[0].Rows[i]["EMPTY_SIGN"] = bcls_rec->Tables[0].Rows[i]["EMPTY_SIGN"];
				}
				//返重
				else if (DEAL_FLAG == "3"){
					//返重更新字段
					tmmsm11["TPC_WT"] = bcls_rec->Tables[0].Rows[i]["NET_WT"];
					tmmsm11["ACCOUNT_WT"] = bcls_rec->Tables[0].Rows[i]["SETTLEMENT_WT"];
					//tmmsm11["FG_WT_LG"] = bcls_rec->Tables[0].Rows[i]["WEIGH_TIME"];
					tmmsm11UpdateFields += "TPC_WT, ACCOUNT_WT, ";

					//返重电文发送字段
					temp.Tables[0].Rows[i]["GROSS_WT"] = bcls_rec->Tables[0].Rows[i]["EMPTY_SIGN"];
					temp.Tables[0].Rows[i]["NET_WT"] = bcls_rec->Tables[0].Rows[i]["NET_WT"];
					temp.Tables[0].Rows[i]["SETTLEMENT_WT"] = bcls_rec->Tables[0].Rows[i]["SETTLEMENT_WT"];
					temp.Tables[0].Rows[i]["BACK1"] = bcls_rec->Tables[0].Rows[i]["C_DELIVERYID"];
				}
				//到达时间&温度
				else if (DEAL_FLAG == "5"){
					//到达时间&温度更新字段
					tmmsm11["IRON_TEMP"] = bcls_rec->Tables[0].Rows[i]["NET_WT"];
					tmmsm11["TIME_TORPEDO_IN"] = bcls_rec->Tables[0].Rows[i]["WEIGH_TIME"];
					tmmsm11UpdateFields += "IRON_TEMP, TIME_TORPEDO_IN, ";

					//到达时间&温度电文发送字段
					temp.Tables[0].Rows[i]["BACK3"] = bcls_rec->Tables[0].Rows[i]["NET_WT"];
					temp.Tables[0].Rows[i]["BACK5"] = bcls_rec->Tables[0].Rows[i]["WEIGH_TIME"];
				}

				//更新TMMSM11表
				tmmsm11UpdateFields = tmmsm11UpdateFields.Substring(0, tmmsm11UpdateFields.GetLength() - 2);
				Log::Trace("", __FUNCTION__, "UpdateFields = {0}", tmmsm11UpdateFields);
				tmmsm11.Update(tmmsm11UpdateFields, "TICODE");

				//铁水收料确认发送
				if (f_21b003_snd(&temp, bcls_ret, conn) != 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
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

	//返回-1时事务将回滚，返回为0是事务将提交	
	return doFlag;
}