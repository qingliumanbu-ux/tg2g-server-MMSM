/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-11 14:13:28
Description: 集控大屏数据添加
**************************************************/

#include "stdafx.h"
#include "epex.h"

int f_mmsm009c_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//集控大屏写表
int f_t8f009_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//中频炉-能源
int f_t823s3_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//中频炉-智慧质量
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//L4
int f_t8f007_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//AOD能源发送
int f_t823s6_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量AOD
int f_t8f010_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源电炉
int f_t823s5_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量电炉
int f_t8f001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源-转炉
int f_t8f004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源实绩-转炉
int f_t82307_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量转炉操作
int f_t823s4_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量转炉实绩
int f_t8f005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源LE
int f_t823s7_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量LF
int f_t8f006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源RH
int f_t82308_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量的RH操作实绩
int f_t823s8_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量难度系数RH
int f_t8f008_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源VOD
int f_t823s9_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量VOD
int f_t8f011_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源连铸
int f_t823sa_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧-连铸
int f_t8f003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源-加料
int f_t82306_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量投料
int f_mmsm2ahj_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//料仓

BM2_FUNCTION_EXPORT
int f_mmsm009e_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int   blkNum;
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_sql2a(conn);
	CString datetime(" ");
	CString table_name = " ";
	CString sm_plan_nol2 = " ";
	CString heat_no = " ";
	CString l2_proc_no = " ";
	CString dev_code = " ";
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm2a_yl("TMMSM2A_YL");

	//CDateTime datetime = CDateTime::Now();

	//DateTime convertedDate = DateTime.Parse(dateString);

	try
	{
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

		//集控大屏
		blkNum = bcls_rec->Tables.IndexOf("JKDP");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("JKDP");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "PROC_NO");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "HEAT_NO");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["JKDP"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}
		if (!bcls_rec->Tables["JKDP"].Columns.Contains("ID_SJ"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "ID_SJ");
		}


		//能源
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
		//PROC_COUNT
		if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_COUNT"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_COUNT");
		}
		//L2_PROC_NO
		if (!bcls_rec->Tables["T8F"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}
		if (!bcls_rec->Tables["T8F"].Columns.Contains("SM_PLAN_NOL2"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
		}

		//智慧质量
		blkNum = bcls_rec->Tables.IndexOf("T823S");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("T823S");
		}

		if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
		{
			bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
		}

		if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
		}

		if (!bcls_rec->Tables["T823S"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}

		if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		//SM_PLAN_NOL2
		if (!bcls_rec->Tables["T823S"].Columns.Contains("SM_PLAN_NOL2"))
		{
			bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
		}

		//PROC_COUNT,L2_PROC_NO,PROD_SEQ_NO
		blkNum = bcls_rec->Tables.IndexOf("T823");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("T823");
		}

		if (!bcls_rec->Tables["T823"].Columns.Contains("DEAL_FLAG"))
		{
			bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "DEAL_FLAG");
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
		//PROC_NO
		if (!bcls_rec->Tables["T823"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "PROC_NO");
		}
		if (!bcls_rec->Tables["T823"].Columns.Contains("SM_PLAN_NOL2"))
		{
			bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
		}

		table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().Trim();
		sm_plan_nol2 = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NOL2"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "table_name=[{0}]", table_name);
		CModel tes(table_name);
		Log::Trace("", __FUNCTION__, "sm_plan_nol2=[{0}]", sm_plan_nol2);
		cmd_sql.SetCommandText(" SELECT L2_PROC_NO,HEAT_NO FROM "+table_name+" where SM_PLAN_NOL2='"+sm_plan_nol2+"' ");
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			l2_proc_no = cmd_sql.GetString(1);
			heat_no = cmd_sql.GetString(2);
		}
		else{
			Log::Trace("", __FUNCTION__, "查不到=[{0}]", l2_proc_no);
		}
		Log::Trace("", __FUNCTION__, "l2_proc_no=[{0}]", l2_proc_no);
		cmd_sql.Close();
		if (l2_proc_no != " "&&l2_proc_no != ""){
			if (table_name == "TMMSM19"){
				tes.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tes["L2_PROC_NO"] = l2_proc_no;
				Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tes["PROC_NO"].ToString());
				if (heat_no != " "){
					Log::Trace("", __FUNCTION__, "该实绩已经有炉号了,HEAT_NO=[{0}]", heat_no);
				}
				else
				{
					tes.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2,L2_PROC_NO");
					//给L4发送
					bcls_rec->Tables["MMSMSND"].Rows.Add();
					bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM19";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//集控大屏
					bcls_rec->Tables["JKDP"].Rows.Add();
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tes["PROC_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_AOD_PRO_SUMMARY";

					//doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//能源
					bcls_rec->Tables["T8F"].Rows.Add();
					bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["SM_PLAN_NOL2"] = tes["SM_PLAN_NOL2"].ToString();
					doFlag = f_t8f009_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//智慧质量
					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"].ToString();
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					doFlag = f_t823s3_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			if (table_name == "TMMSM27"){
				tes.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tes["L2_PROC_NO"] = l2_proc_no;
				Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tes["PROC_NO"].ToString());
				if (heat_no != " "){
					Log::Trace("", __FUNCTION__, "该实绩已经有炉号了,HEAT_NO=[{0}]", heat_no);
				}
				else{
					tes.Update("PROC_NO,PONO,SM_PLAN_NO", "SM_PLAN_NOL2,L2_PROC_NO");
					//L4
					bcls_rec->Tables["MMSMSND"].Rows.Add();
					bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM27";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//能源发送
					bcls_rec->Tables["T8F"].Rows.Add();
					bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_t8f007_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//智慧质量发送
					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_t823s6_snd(bcls_rec, bcls_ret, conn);
					//集控大屏
					bcls_rec->Tables["JKDP"].Rows.Add();
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tes["PROC_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_AOD_PRO_SUMMARY";

					//doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else if (table_name == "TMMSM20"){
				tes.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tes["L2_PROC_NO"] = l2_proc_no;
				Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tes["PROC_NO"].ToString());
				if (heat_no != " "){
					Log::Trace("", __FUNCTION__, "该实绩已经有炉号了,HEAT_NO=[{0}]", heat_no);
				}
				else{
					tes.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2,L2_PROC_NO");
					//L4
					bcls_rec->Tables["MMSMSND"].Rows.Add();
					bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM20";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//集控大屏
					bcls_rec->Tables["JKDP"].Rows.Add();
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tes["PROC_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_EAF_PRO_SUMMARY";
					//doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//能源
					bcls_rec->Tables["T8F"].Rows.Add();
					bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					doFlag = f_t8f010_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//智慧质量
					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					//doFlag = f_t823s5_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

			}
			else if (table_name == "TMMSM21")
			{
				tes.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tes["L2_PROC_NO"] = l2_proc_no;
				Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tes["PROC_NO"].ToString());
				if (heat_no != " "){
					Log::Trace("", __FUNCTION__, "该实绩已经有炉号了,HEAT_NO=[{0}]", heat_no);
				}
				else{
					tes.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2,L2_PROC_NO");
					//L4
					bcls_rec->Tables["MMSMSND"].Rows.Add();
					bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM21";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//集控大屏
					bcls_rec->Tables["JKDP"].Rows.Add();
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tes["PROC_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"].ToString();
					Log::Trace("", __FUNCTION__, "L2_PROC_NO=[{0}]", tes["L2_PROC_NO"].ToString());
					bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_BOF_PRO_SUMMARY";
					//doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//能源
					bcls_rec->Tables["T8F"].Rows.Add();
					bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					doFlag = f_t8f001_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//能源实绩
					bcls_rec->Tables["T8F"].Rows.Add();
					bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					doFlag = f_t8f004_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//智慧质量转炉实绩
					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["SM_PLAN_NOL2"] = tes["SM_PLAN_NOL2"];
					doFlag = f_t823s4_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//智慧质量转炉实绩
					bcls_rec->Tables["T823"].Rows.Add();
					bcls_rec->Tables["T823"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T823"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["T823"].Rows[0]["SM_PLAN_NOL2"] = tes["SM_PLAN_NOL2"];
					doFlag = f_t82307_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else if (table_name == "TMMSM24"){
				tes.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tes["L2_PROC_NO"] = l2_proc_no;
				Log::Trace("", __FUNCTION__, "PROC_NO1=[{0}]", tes["PROC_NO"].ToString());
				tes["SAME_PROC_NUM"] = bcls_rec->Tables[0].Rows[0]["TREATMENT_COUNTER"].ToString();
				if (heat_no != " "){
					Log::Trace("", __FUNCTION__, "该实绩已经有炉号了,HEAT_NO=[{0}]", heat_no);
				}
				else{
					tes.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2,L2_PROC_NO,SAME_PROC_NUM");
					Log::Trace("", __FUNCTION__, "HEAT_NO=[{0}]", tes["HEAT_NO"].ToString());
					Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tes["PROC_NO"].ToString());
					tes.Query("PROC_NO,HEAT_NO");
					tes.TrimOrBlank();
					Log::Trace("", __FUNCTION__, "C_DIV=[{0}]", tes["C_DIV"].ToString());
					//L4
					if (tes["C_DIV"].ToString() == "2")
					{
						bcls_rec->Tables["MMSMSND"].Rows.Add();
						bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM24B";
						bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
						bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
						bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
						bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
						Log::Trace("", __FUNCTION__, "LIKE=[{0}]", __LINE__);
						doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					else if (tes["C_DIV"].ToString() == "1"){
						bcls_rec->Tables["MMSMSND"].Rows.Add();
						Log::Trace("", __FUNCTION__, "LIKE=[{0}]", __LINE__);
						bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM24A";
						bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
						bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
						bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
						bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];

						doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					//集控大屏
					bcls_rec->Tables["JKDP"].Rows.Add();
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tes["PROC_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_LF_PRO_SUMMARY";
					Log::Trace("", __FUNCTION__, "LIKE=[{0}]", __LINE__);
					//doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//
					bcls_rec->Tables["T8F"].Rows.Add();
					bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					//PROC_NO
					bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					Log::Trace("", __FUNCTION__, "LIKE=[{0}]", __LINE__);
					doFlag = f_t8f005_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//
					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_t823s7_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else if (table_name == "TMMSM23"){
				tes.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tes["L2_PROC_NO"] = l2_proc_no;
				Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tes["PROC_NO"].ToString());
				tes.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2,L2_PROC_NO");
				//1
				bcls_rec->Tables["MMSMSND"].Rows.Add();
				bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM23";
				bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
				bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
				bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
				bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
				doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//2
				bcls_rec->Tables["JKDP"].Rows.Add();
				bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = "U";
				bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tes["PROC_NO"].ToString();
				bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"].ToString();
				bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_RH_PRO_SUMMARY";
				//doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//3
				bcls_rec->Tables["T8F"].Rows.Add();
				bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
				bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
				//L2_PROC_NO
				bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
				bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
				doFlag = f_t8f006_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//4
				bcls_rec->Tables["T823"].Rows.Add();
				bcls_rec->Tables["T823"].Rows[0]["DEAL_FLAG"] = "U";
				bcls_rec->Tables["T823"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
				bcls_rec->Tables["T823"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
				bcls_rec->Tables["T823"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
				doFlag = f_t82308_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//5
				bcls_rec->Tables["T823S"].Rows.Add();
				bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
				bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
				bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
				bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
				doFlag = f_t823s8_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (table_name == "TMMSM26"){
				tes.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tes["L2_PROC_NO"] = l2_proc_no;
				Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tes["PROC_NO"].ToString());
				if (heat_no != " "){
					Log::Trace("", __FUNCTION__, "该实绩已经有炉号了,HEAT_NO=[{0}]", heat_no);
				}
				else{
					tes.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2,L2_PROC_NO");
					//1
					bcls_rec->Tables["MMSMSND"].Rows.Add();
					bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM26";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//2
					bcls_rec->Tables["JKDP"].Rows.Add();
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tes["PROC_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_LTS_PROD_SUMMARY";
					//doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else if (table_name == "TMMSM25"){
				tes.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tes["L2_PROC_NO"] = l2_proc_no;
				Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tes["PROC_NO"].ToString());
				if (heat_no != " "){
					Log::Trace("", __FUNCTION__, "该实绩已经有炉号了,HEAT_NO=[{0}]", heat_no);
				}
				else{
					tes.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2,L2_PROC_NO");
					//1
					bcls_rec->Tables["MMSMSND"].Rows.Add();
					bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM25";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//2
					bcls_rec->Tables["JKDP"].Rows.Add();
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tes["PROC_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_VOD_PRO_SUMMARY";
					//doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//3
					bcls_rec->Tables["T8F"].Rows.Add();
					bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					doFlag = f_t8f008_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//4
					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_t823s9_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else if (table_name == "TMMSM31"){
				tes.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tes["L2_PROC_NO"] = l2_proc_no;
				Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tes["PROC_NO"].ToString());
				if (heat_no != " "){
					Log::Trace("", __FUNCTION__, "该实绩已经有炉号了,HEAT_NO=[{0}]", heat_no);
				}
				else{
					tes.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2,L2_PROC_NO");
					//1
					bcls_rec->Tables["MMSMSND"].Rows.Add();
					bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM31";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//2
					bcls_rec->Tables["T8F"].Rows.Add();
					bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					doFlag = f_t8f011_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//3
					bcls_rec->Tables["T823S"].Rows.Add();
					bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = "U";
					bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tes["PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tes["L2_PROC_NO"];
					bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"];
					doFlag = f_t823sa_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//4
					bcls_rec->Tables["JKDP"].Rows.Add();
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = "U";
					bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tes["PROC_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tes["HEAT_NO"].ToString();
					bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_CCM_PRO_SUMMARY";
					//doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			//检查加料是否有数据
			tmmsm2a["L2_PROC_NO"] = l2_proc_no;
			Log::Trace("", "", "L2_PROC_NO= {0}", tmmsm2a["L2_PROC_NO"].ToString());
			if (table_name == "TMMSM19"){
				dev_code = "Z";
			}
			else if (table_name == "TMMSM31"){
				dev_code = "C";
			}
			else if (table_name == "TMMSM27"){
				dev_code = "A";
			}
			else if (table_name == "TMMSM20"){
				dev_code = "E";
			}
			else if (table_name == "TMMSM21"){
				dev_code = "B";
			}
			else if (table_name == "TMMSM24"){
				dev_code = "F";
			}
			else if (table_name == "TMMSM23"){
				dev_code = "R";
			}
			else if (table_name == "TMMSM26"){
				dev_code = "S";
			}
			else if (table_name == "TMMSM26"){
				dev_code = "V";
			}
			Log::Trace("", "", "dev_code= {0}", dev_code);
			tmmsm2a["L2_PROC_NO"] = l2_proc_no;
			tmmsm2a["SM_PLAN_NOL2"] = sm_plan_nol2;
			if (tmmsm2a.QueryCount("SM_PLAN_NOL2") > 0){
				Log::Trace("", __FUNCTION__, "LIKE=[{0}]", __LINE__);
				tmmsm2a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tmmsm2a["PROC_NO"] = bcls_rec->Tables[0].Rows[0]["PROC_NO"].ToString();
				Log::Trace("", __FUNCTION__, "LIKE=[{0}]", __LINE__);
				cmd_sql2a.SetCommandText(" SELECT * from TMMSM2A where SM_PLAN_NOL2='" + sm_plan_nol2 + "' ");
				Log::Trace("", __FUNCTION__, "LIKE=[{0}]", __LINE__);
				cmd_sql2a.ExecuteReader();
				bcls_rec->Tables["T8F"].Rows.Add();
				bcls_rec->Tables["T823"].Rows.Add();
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "L2_PROC_NO");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("MAT_CODE"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_CODE");
				}
				bcls_rec->Tables["MMSMSND"].Rows.Add();
				while (cmd_sql2a.Read())
				{
					cmd_sql2a.Fetch(tmmsm2a);
					Log::Trace("", __FUNCTION__, "LIKE=[{0}]", __LINE__);
					tmmsm2a["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
					tmmsm2a["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"].ToString();
					tmmsm2a["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
					Log::Trace("", __FUNCTION__, "PROC_NO1=[{0}]", tmmsm2a["PROC_NO"].ToString());
					tmmsm2a.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2");
					tmmsm2a_yl["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString();
					tmmsm2a_yl["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"].ToString();
					tmmsm2a_yl["SM_PLAN_NO"] = tmmsm2a["SM_PLAN_NO"].ToString();
					tmmsm2a_yl["PONO"] = tmmsm2a["PONO"].ToString();
					tmmsm2a_yl.Update("PROC_NO,PONO,SM_PLAN_NO,HEAT_NO", "SM_PLAN_NOL2");
				}
				cmd_sql2a.Close();
			}
		}
		else{
			Log::Trace("", __FUNCTION__, "实绩还没有到=[{0}]", table_name);
		}
		
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		/*strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;*/
		return 0;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		return 0;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		return 0;
	}
	return doFlag;
}


