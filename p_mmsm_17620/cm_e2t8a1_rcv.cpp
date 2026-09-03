/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-11-28
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 炉次加料实绩表
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
int f_mmsm_t8e2yc_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t8e2yx_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_mmsm2a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
int f_t8f003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源
int f_t82306_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量投料
int f_mmsm2ahj_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//料仓
int f_mmsm009d_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//L4
int f_mmsm_gyins2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//原料函数
//f_mmsm009a_snd
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE_TELE(cm_e2t8a1_rcv)

int f_cm_e2t8a1_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString v_proc_div;
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inq_test(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);
	CDbCommand cmd_inq_1(conn);
	CDbCommand cmd_inq_60(conn);
	CString dev_code = "";
	CString v_resume_seq_no = " ";
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm50("TMMSM50");
	CString heat_no = " ";
	CString proc_div = " ";
	CDecimal count = 1;
	CString sm_plan_no2 = " ";
	//EIClass tmmsm2a_back;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString seq ("");

	//调用发送电文 PROC_COUNT,L2_PROC_NO,PROD_SEQ_NO
	blkNum = bcls_rec->Tables.IndexOf("T823");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("T823");
	}
	 
	if (!bcls_rec->Tables["T823"].Columns.Contains("DEAL_FLAG"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "DEAL_FLAG");
	}

	if (!bcls_rec->Tables["T823"].Columns.Contains("PROC_NO"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "PROC_NO");
	}
	if (!bcls_rec->Tables["T823"].Columns.Contains("PROC_COUNT"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "PROC_COUNT");
	}

	if (!bcls_rec->Tables["T823"].Columns.Contains("HEAT_NO"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "HEAT_NO");
	}

	if (!bcls_rec->Tables["T823"].Columns.Contains("L2_PROC_NO"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "L2_PROC_NO");
	}
	if (!bcls_rec->Tables["T823"].Columns.Contains("PROD_SEQ_NO"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "PROD_SEQ_NO");
	}

	//调用发送电文
	blkNum = bcls_rec->Tables.IndexOf("TMMSM2A");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("TMMSM2A");
	}

	if (!bcls_rec->Tables["TMMSM2A"].Columns.Contains("TC_BACKLOG"))
	{
		bcls_rec->Tables["TMMSM2A"].Columns.Add(DT_STRING, "TC_BACKLOG");
	}

	if (!bcls_rec->Tables["TMMSM2A"].Columns.Contains("PROC_DIV"))
	{
		bcls_rec->Tables["TMMSM2A"].Columns.Add(DT_STRING, "PROC_DIV");
	}

	if (!bcls_rec->Tables["TMMSM2A"].Columns.Contains("L2_PROC_NO"))
	{
		bcls_rec->Tables["TMMSM2A"].Columns.Add(DT_STRING, "L2_PROC_NO");
	}

	if (!bcls_rec->Tables["TMMSM2A"].Columns.Contains("HEAT_NO"))
	{
		bcls_rec->Tables["TMMSM2A"].Columns.Add(DT_STRING, "HEAT_NO");
	}
	if (!bcls_rec->Tables["TMMSM2A"].Columns.Contains("MAT_CODE"))
	{
		bcls_rec->Tables["TMMSM2A"].Columns.Add(DT_STRING, "MAT_CODE");
	}
	if (!bcls_rec->Tables["TMMSM2A"].Columns.Contains("PROD_SEQ_NO"))
	{
		bcls_rec->Tables["TMMSM2A"].Columns.Add(DT_STRING, "PROD_SEQ_NO");
	}
	if (!bcls_rec->Tables["TMMSM2A"].Columns.Contains("DEV_CODE"))
	{
		bcls_rec->Tables["TMMSM2A"].Columns.Add(DT_STRING, "DEV_CODE");
	}
	if (!bcls_rec->Tables["TMMSM2A"].Columns.Contains("STATION_ID"))
	{
		bcls_rec->Tables["TMMSM2A"].Columns.Add(DT_STRING, "STATION_ID");
	}
	if (!bcls_rec->Tables["TMMSM2A"].Columns.Contains("PROC_COUNT"))
	{
		bcls_rec->Tables["TMMSM2A"].Columns.Add(DT_STRING, "PROC_COUNT");
	}

	//调用发送电文
	blkNum = bcls_rec->Tables.IndexOf("T8F");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("T8F");
	}

	if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_DIV"))
	{
		bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_DIV");
	}

	if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_NO"))
	{
		bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_NO");
	}

	if (!bcls_rec->Tables["T8F"].Columns.Contains("HEAT_NO"))
	{
		bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "HEAT_NO");
	}
	if (!bcls_rec->Tables["T8F"].Columns.Contains("PROD_SEQ_NO"))
	{
		bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROD_SEQ_NO");
	}
	if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_COUNT"))
	{
		bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_COUNT");
	}

	//调用L4发送电文
	blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MMSMSND");
	}

	if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
	{
		bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
	}

	if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
	{
		bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
	}

	if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("HEAT_NO"))
	{
		bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "HEAT_NO");
	}
	if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_NO"))
	{
		bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_NO");
	}
	if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
	{
		bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "L2_PROC_NO");
	}
	//PROD_SEQ_NO
	if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROD_SEQ_NO"))
	{
		bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROD_SEQ_NO");
	}
	if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
	{
		bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "L2_PROC_NO");
	}
	if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("MAT_CODE"))
	{
		bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_CODE");
	}

	EIClass tmmsm2a_back;
	if (blkNum < 0)
	{
		tmmsm2a_back.Tables.Add();
	}
	tmmsm2a_back.Tables[0].Columns.Add(tmmsm2a);
	try
	{
			///工号
			dev_code = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["AGGREGATE_NAME"].ToString();
			//INT_MES_CHARGE.ID
			tmmsm2a["ID_2A"] = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["ID"].ToString();
			Log::Trace("", "dev_code", "dev_code = {0}", dev_code);
			tmmsm2a["STATION_ID"] = dev_code.Substring(0, 1);
			sm_plan_no2 = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["ORDER_NUMBER"].ToString();

			//同工位处理次数
			tmmsm2a["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["TREATMENT_COUNTER"].ToString();
			tmmsm2a["PRACT_COLL_MODE"] = "0";
			//熔炼号
			if (tmmsm2a["STATION_ID"].ToString() != "Z" && tmmsm2a["STATION_ID"].ToString() != "E"){
				tmmsm2a["HEAT_NO"] = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["HEAT_NUMBER"].ToString();
			}
			else{
				tmmsm2a["HEAT_NO"] = " ";
			}
			tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["HEAT_NUMBER"].ToString();

			//根据计划号获取熔炼号和制造命令号
			cmd_inq_code.SetCommandText(" SELECT HEAT_NO, PONO, SM_PLAN_NO FROM "
				" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
				" UNION "
				" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no2 + "'");
			cmd_inq_code.ExecuteReader();
			if (cmd_inq_code.Read())
			{
				tmmsm2a["HEAT_NO"] = cmd_inq_code.GetString(1);
				tmmsm2a["PONO"] = cmd_inq_code.GetString(2);
				tmmsm2a["SM_PLAN_NO"] = cmd_inq_code.GetString(3);
			}
			cmd_inq_code.Close();
			if (tmmsm2a["HEAT_NO"].ToString() == " " && tmmsm2a["STATION_ID"].ToString() != "Z" && tmmsm2a["STATION_ID"].ToString() != "E"){
				tmmsm2a["HEAT_NO"] = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["HEAT_NUMBER"].ToString();
			}
			Log::Trace("", "dev_code", "HEAT_NO = {0}sm_plan_no =[{1}]", tmmsm2a["HEAT_NO"].ToString(), sm_plan_no2);

			cmd_inq.SetCommandText(" SELECT T1.PROC_NO,T2.STATION_ID,T2.STATION_NO FROM (  "
				" SELECT PROC_NO, DEV_CODE, DECODE(PRE_SOLUTION_FLAG, '1', '0', '1') C_DIV FROM TPSSM12 WHERE SM_PLAN_NOL2 = '" + sm_plan_no2 + "' AND DEV_CODE LIKE '" + tmmsm2a["STATION_ID"].ToString() + "%' and TREATMENT_COUNTER ='" + tmmsm2a["SAME_PROC_NUM"].ToString() + "' ) T1 LEFT JOIN TPSSMD1 T2  "
				" on t1.DEV_CODE = t2.DEV_CODE ");
			cmd_inq.ExecuteReader();
			if (tmmsm2a["LAYERNO"].ToString() == "2"){
				while (cmd_inq.Read())
				{
					if (tmmsm2a["LAYERNO"].ToString() == "2"){

						return 0;
					}
					tmmsm2a["PROC_NO"] = cmd_inq.GetString(1);
					tmmsm2a["STATION_ID"] = cmd_inq.GetString(2);
					tmmsm2a["STATION_NO"] = cmd_inq.GetString(3);
				}
			}
			else{
				if (cmd_inq.Read())
				{
					tmmsm2a["PROC_NO"] = cmd_inq.GetString(1);
					tmmsm2a["STATION_ID"] = cmd_inq.GetString(2);
					tmmsm2a["STATION_NO"] = cmd_inq.GetString(3);
				}
			}
			cmd_inq.Close();
			Log::Trace("", "STATION_NO", "STATION_NO = {0}]", tmmsm2a["STATION_NO"].ToString());
			if (tmmsm2a["STATION_NO"].ToString() != " "){
			}
			else
			{
				CString station_no = " ";
				station_no = dev_code.Substring(1,1);
				Log::Trace("", "STATION_NO", "STATION_NO = {0}]", tmmsm2a["STATION_NO"].ToString());
				tmmsm2a["STATION_NO"] = station_no;
				Log::Trace("", "STATION_NO", "STATION_NO = {0}]", tmmsm2a["STATION_NO"].ToString());
			}

			if (tmmsm2a["PROC_NO"].ToString() != " "&&tmmsm2a["PROC_NO"].ToString() != ""){
				
			}
			else{
				tmmsm2a["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString();
			}
			if (tmmsm2a["STATION_ID"].ToString() == "F"){
				tmmsm2a["PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString();
			}

			tmmsm2a["DEV_CODE"] = dev_code;

			//因二级未传时间，故取电文当前处理时间
			tmmsm2a["DEVO_TIME"] = dateNow;
			tmmsm2a["PROD_DATE"] = dateNow.Substring(0, 8);

			tmmsm2a["REC_CREATE_TIME"] = dateNow;
			tmmsm2a["REC_CREATOR"] = s.userid;
			tmmsm2a["HANDWORK_MARK"] = "0";

			//L2计划号
			tmmsm2a["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["ORDER_NUMBER"].ToString();//分包号
			//分包号 
			tmmsm2a["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["SPLIT_INDICATION"].ToString();
			//加料类型 INT_MES_CHARGE.CHARGE_TYPE
			tmmsm2a["CHARGE_TYPE"] = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["CHARGE_TYPE"].ToString();
			for (int i = 0; i < bcls_rec->Tables["INT_MES_CHARGE_DET"].Rows.get_Count(); i++)
			{
				doFlag = f_mm0011("TMMSM2A_SEQ", 8, v_resume_seq_no, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", "dateNow", "dateNow = {0}", dateNow);
				Log::Trace("", "v_resume_seq_no", "v_resume_seq_no = {0}", v_resume_seq_no);
				seq = dateNow + v_resume_seq_no;
				tmmsm2a["PROD_SEQ_NO"] = seq;
				Log::Trace("", "RESUME_SEQ_NO", "RESUME_SEQ_NO = {0}", seq);
				Log::Trace("", "RESUME_SEQ_NO", "RESUME_SEQ_NO = {0}", tmmsm2a["PROD_SEQ_NO"].ToString());
				Log::Info("", __FUNCTION__, "like=[{0}]", bcls_rec->Tables["INT_MES_CHARGE_DET"].Rows.get_Count());
				//层号 INT_MES_CHARGE_DET.CNT
				tmmsm2a["LAYERNO"] = bcls_rec->Tables["INT_MES_CHARGE_DET"].Rows[i]["CNT"].ToString();
				//中类
				tmmsm2a["MATERIAL_CODE"] = bcls_rec->Tables["INT_MES_CHARGE_DET"].Rows[i]["MATERIAL_CODE"].ToString();
				//料仓号
				CString batch_number = bcls_rec->Tables["INT_MES_CHARGE_DET"].Rows[i]["BATCH_NUMBER"].ToString();
				tmmsm2a["BATCH_NUMBER"] = bcls_rec->Tables["INT_MES_CHARGE_DET"].Rows[i]["BATCH_NUMBER"].ToString();
				//重量 INT_MES_CHARGE_DET.WEIGHT

				tmmsm2a["DEVO_WT"] = bcls_rec->Tables["INT_MES_CHARGE_DET"].Rows[i]["WEIGHT"].ToDecimal();

				tmmsm2a["MAT_AMOUNT1"] = tmmsm2a["DEVO_WT"].ToDecimal();
				Log::Trace("", "MAT_AMOUNT1", "MAT_AMOUNT1 = {0}", tmmsm2a["MAT_AMOUNT1"].ToString());
				//料位号 INT_MES_CHARGE_DET.BIN_NUMBER
				CString stk_no = bcls_rec->Tables["INT_MES_CHARGE_DET"].Rows[i]["BIN_NUMBER"].ToString();
				tmmsm2a["BIN_NUMBER"] = bcls_rec->Tables["INT_MES_CHARGE_DET"].Rows[i]["BIN_NUMBER"].ToString();
				cmd_inq_1.SetCommandText(" SELECT MAT_CODE,MAT_NAME FROM tmmsm50 WHERE LOT_NO =@BATCH_NUMBER AND MAT_CODE_L2 =@MATERIAL_CODE ");
				cmd_inq_1.Parameters.Clear();
				cmd_inq_1.Parameters.Set("BATCH_NUMBER", batch_number);
				cmd_inq_1.Parameters.Set("MATERIAL_CODE", tmmsm2a["MATERIAL_CODE"].ToString());
				cmd_inq_1.ExecuteReader();
				if (cmd_inq_1.Read())
				{
					tmmsm2a["MAT_CODE"] = cmd_inq_1.GetString(1);
					tmmsm2a["MAT_NAME"] = cmd_inq_1.GetString(2);

				}
				cmd_inq_1.Close();
				cmd_inq_60.SetCommandText(" SELECT BUNKER_NO FROM tmmsm60 WHERE STK_NO=@STK_NO ");
				cmd_inq_60.Parameters.Clear();
				cmd_inq_60.Parameters.Set("STK_NO", stk_no);
				cmd_inq_60.ExecuteReader();
				if (cmd_inq_60.Read())
				{
					tmmsm2a["STK_NO"] = cmd_inq_60.GetString(1);
					Log::Trace("", "STK_NO", "STK_NO = {0}", tmmsm2a["STK_NO"].ToString());

				}
				cmd_inq_60.Close();
				if (tmmsm2a["PROC_NO"].ToString() != " " || tmmsm2a["PROC_NO"].ToString() != ""){
					if (tmmsm2a["SM_PLAN_NOL2"].ToString() == "11111111"){//11111111
						cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2a where  L2_PROC_NO ='" + tmmsm2a["L2_PROC_NO"].ToString().Trim() + "'  ");
					}
					else{
						cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2a where  SM_PLAN_NOL2 ='" + tmmsm2a["SM_PLAN_NOL2"].ToString().Trim() + "' ");
					}
				}
				else{
					if (tmmsm2a["SM_PLAN_NOL2"].ToString() == "11111111"){
						cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2a where  L2_PROC_NO ='" + tmmsm2a["L2_PROC_NO"].ToString().Trim() + "'  ");
					}
					else{
						cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2a where  SM_PLAN_NOL2 ='" + tmmsm2a["SM_PLAN_NOL2"].ToString().Trim() + "' ");
					}
				}
				tmmsm2a["PROC_COUNT"] = cmd_inq.ExecuteScalar();

				Log::Trace("", "", "PROC_COUNT = {0}", tmmsm2a["PROC_COUNT"].ToString());

				if (tmmsm2a.QueryCount("PROD_SEQ_NO,L2_PROC_NO,PROC_COUNT"))
				{
					tmmsm2a.Update("PROD_SEQ_NO,L2_PROC_NO,PROC_COUNT");
					proc_div = "U";
				}
				else
				{
					tmmsm2a.Insert();
					proc_div = "I";
				}

				bcls_rec->Tables["T8F"].Rows.Add();
				bcls_rec->Tables["T8F"].Rows[i]["PROC_DIV"] = proc_div;
				bcls_rec->Tables["T8F"].Rows[i]["PROC_NO"] = tmmsm2a["PROC_NO"];
				bcls_rec->Tables["T8F"].Rows[i]["HEAT_NO"] = tmmsm2a["HEAT_NO"];
				bcls_rec->Tables["T8F"].Rows[i]["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"];
				bcls_rec->Tables["T8F"].Rows[i]["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
				Log::Trace("", "", "proc_div = {0}", proc_div);
				Log::Trace("", "", "HEAT_NO = {0}", tmmsm2a["HEAT_NO"].ToString());
				Log::Trace("", "", "PROD_SEQ_NO = {0}", tmmsm2a["PROD_SEQ_NO"].ToString());
				//doFlag = f_t8f003_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", "MAT_AMOUNT1", "1111 = {1}");

				//T8E2Y6 电炉加料数据（电文未申请）
				//晓丽姐 反馈 T8E2Y6是 老系统新增电文，不需要发
				//if (tmmsm2a["STATION_ID"].ToString() == "E")
				//{
				//	blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
				//	if (blkNum < 0)
				//	{
				//		bcls_rec->Tables.Add("MMLCSND");
				//	}
				//	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
				//	{
				//		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
				//	}
				//	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_NO"))
				//	{
				//		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROC_NO");
				//	}
				//	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_COUNT"))
				//	{
				//		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROC_COUNT");
				//	}
				//	if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("HEAT_NO"))
				//	{
				//		bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "HEAT_NO");
				//	}
				//	bcls_rec->Tables["MMLCSND"].Rows.Add();
				//	bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y6";
				//	bcls_rec->Tables["MMLCSND"].Rows[0]["HEAT_NO"] = tmmsm2a["HEAT_NO"];
				//	bcls_rec->Tables["MMLCSND"].Rows[0]["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
				//	Log::Trace("", "", "PROC_COUNT = {0}", tmmsm2a["PROC_COUNT"].ToString());
				//	doFlag = f_mmsm_t8e2yc_snd(bcls_rec, bcls_ret, conn);
				//	if (doFlag < 0)
				//	{
				//		Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t8e2yc_snd失败-------");
				//		//throw CApplicationException(-1, s.msg, log.Location);
				//	}
				//}
				
				//PROC_COUNT,L2_PROC_NO,PROD_SEQ_NO
				bcls_rec->Tables["T823"].Rows.Add();
				bcls_rec->Tables["T823"].Rows[i]["DEAL_FLAG"] = "I";
				bcls_rec->Tables["T823"].Rows[i]["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
				bcls_rec->Tables["T823"].Rows[i]["HEAT_NO"] = tmmsm2a["HEAT_NO"];
				bcls_rec->Tables["T823"].Rows[i]["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];
				bcls_rec->Tables["T823"].Rows[i]["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"];
				//doFlag = f_t82306_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				bcls_rec->Tables["MMSMSND"].Rows.Add();
				bcls_rec->Tables["MMSMSND"].Rows[i]["TC_BACKLOG"] = "MMSM2A";
				bcls_rec->Tables["MMSMSND"].Rows[i]["PROC_DIV"] = "U";
				bcls_rec->Tables["MMSMSND"].Rows[i]["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString();
				bcls_rec->Tables["MMSMSND"].Rows[i]["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString();
				bcls_rec->Tables["MMSMSND"].Rows[i]["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
				//PROD_SEQ_NO
				bcls_rec->Tables["MMSMSND"].Rows[i]["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString();
				Log::Trace("", "", "tmmsm2a.MAT_CODE = {0}", bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_CODE"].ToString());
				//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				
				bcls_rec->Tables["TMMSM2A"].Rows.Add();
				bcls_rec->Tables["TMMSM2A"].Rows[i]["TC_BACKLOG"] = "MMSM2A";
				bcls_rec->Tables["TMMSM2A"].Rows[i]["PROC_DIV"] = proc_div;
				bcls_rec->Tables["TMMSM2A"].Rows[i]["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString();
				bcls_rec->Tables["TMMSM2A"].Rows[i]["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString();
				bcls_rec->Tables["TMMSM2A"].Rows[i]["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
				bcls_rec->Tables["TMMSM2A"].Rows[i]["PROD_SEQ_NO"] = tmmsm2a["PROD_SEQ_NO"].ToString();
				bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"] = tmmsm2a["DEV_CODE"].ToString();
				//PROC_COUNT
				bcls_rec->Tables["TMMSM2A"].Rows[i]["STATION_ID"] = tmmsm2a["STATION_ID"].ToString();
				bcls_rec->Tables["TMMSM2A"].Rows[i]["PROC_COUNT"] = tmmsm2a["PROC_COUNT"].ToString();
				Log::Trace("", "", "tmmsm2a.MAT_CODE = {0}", bcls_rec->Tables["TMMSM2A"].Rows[i]["MAT_CODE"].ToString());

				
				//tmmsm2a.MergeTo(tmmsm2a_back.Tables[0], false);
				tmmsm2a_back.Tables[0].Rows.Add();
				tmmsm2a_back.Tables[0].Rows[i].Merge(tmmsm2a);				
			}

			doFlag = f_mmsm009d_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			doFlag = f_mmsm2ahj_proc(&tmmsm2a_back, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			// 20260212 优化：根据HEAT_NUMBER查询mmsmgy06表中所有匹配的heat_no，循环调用f_mmsm_gyins2
			CDbCommand cmd_inq_gy06(conn);
			CString strHeatNumber = bcls_rec->Tables["INT_MES_CHARGE"].Rows[0]["HEAT_NUMBER"].ToString().Trim();
			cmd_inq_gy06.SetCommandText(" SELECT distinct HEAT_NO FROM tmmsmgy06 WHERE L2_PROC_NO = @HEAT_NO ");
			cmd_inq_gy06.Parameters.Clear();
			cmd_inq_gy06.Parameters.Set("HEAT_NO", strHeatNumber);
			cmd_inq_gy06.ExecuteReader();
			while (cmd_inq_gy06.Read())
			{
				CString strGy06HeatNo = cmd_inq_gy06.GetString(1).Trim();

				if (!strGy06HeatNo.IsEmpty() && strGy06HeatNo != " " && strGy06HeatNo != "")
				{
					
					EIClass mmsmgy06;
					mmsmgy06.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
					mmsmgy06.Tables[0].Rows.Clear();
					mmsmgy06.Tables[0].Rows.Add();
					mmsmgy06.Tables[0].Rows[0]["HEAT_NO"] = strGy06HeatNo;

					doFlag = f_mmsm_gyins2(&mmsmgy06, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					Log::Info("", __FUNCTION__, "调用f_mmsm_gyins2成功，HEAT_NO={0}", strGy06HeatNo);
				}
			}
			cmd_inq_gy06.Close();
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

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


