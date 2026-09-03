/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面维护
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include <set>
/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */
int f_mmsm_t83313_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
// service入口
BM2F_ENTERACE(mmsmrqtl_pro)

int f_mmsmrqtl_pro(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int blkNum = 0;
	int doFlag = 0;
	
	CString v_proc_div = "";
	CString v_prod_desc = "";
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString s_formname = "";
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString v_table_name = "TMMSMWQ";//表名称。
	CModel tmmsmwq("TMMSMWQ");
	CModel tmmsmwq_ll("TMMSMWQ_LL");
	

	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	set<CString> lot_no_list = {};
	try
	{
		s_formname = s.formname;
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

		CModel tmmsmyl(v_table_name);
		Log::Info("", __FUNCTION__, "v_table_name =[{0}]", v_table_name);
		Log::Trace("", __FUNCTION__, "v_proc_div =[{0}]", v_proc_div);
		if (v_proc_div == "T")
		{
			int i_count = 0;
			int count0 = 0;
			int j = 0;
			int k = 0;
			v_prod_desc = "提交加权记录";
			std::set<CString> uniqueLotSet;  // 自动去重的集合
			// 遍历前端传过来的所有批次号
			for (int i = 0; i < bcls_rec->Tables["EDIT"].Rows.get_Count(); i++)
			{
				CString lot_no = bcls_rec->Tables["EDIT"].Rows[i]["LOT_NO"].ToString();
				if (!lot_no.IsEmpty())  // 排除空批次号
				{
					uniqueLotSet.insert(lot_no);
				}
			}
			Log::Trace("", "", "wcm=[{0}]", s.userid);
			for (auto& lot_no : uniqueLotSet)
			{
				sqlstr = " SELECT count(*) as I_COUNT FROM TMMSMWQ  WHERE LOT_NO='" + lot_no + "' AND C_STATE='2' ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					count0 = cmd_inq.GetInt16(1);
				}
				cmd_inq.Close();

				Log::Trace("", "", "i_count=[{0}]", count0);
				if (count0 == 0)
				{

					sqlstr = " SELECT count(*) as I_COUNT FROM TMMSMWQ  WHERE LOT_NO='" + lot_no + "' AND RES_TYPE IN ('3','5') ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						i_count = cmd_inq.GetInt16(1);
					}
					cmd_inq.Close();

					Log::Trace("", "", "i_count=[{0}]", i_count);
				
					if (i_count > 0)
					{
						j++;
						sqlstr = " SELECT * FROM TMMSMWQ  WHERE LOT_NO='" + lot_no + "' AND RES_TYPE IN ('2','3','1','4','5') ";
						Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_sql.SetCommandText(sqlstr);
						cmd_sql.ExecuteReader();

						while (cmd_sql.Read())
						{
							tmmsmwq.Reset();
							cmd_sql.Fetch(tmmsmwq);

							tmmsmwq["C_STATE"] = "2";
							tmmsmwq["REC_REVISOR"] = s.userid;
							tmmsmwq["REC_REVISE_TIME"] = nowTime;
							tmmsmwq["SUB_PER"] = s.userid;
							tmmsmwq["SUB_TIME"] = nowTime;

							tmmsmwq.Update("C_STATE,REC_REVISOR,REC_REVISE_TIME,SUB_PER,SUB_TIME", "PROC_COUNT,PROD_DATE");
						}
						cmd_sql.Close();
					}
				}
				else
				{
					k++;
				}
			}
			if (j==0)
			{
				if (k > 0)
				{
					sprintf(s.msg, "此批次号已提交，不能重复提交！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				else
				{
					sprintf(s.msg, "没有批次号可提交！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
			}
			
		}
		else
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				tmmsmyl.Reset();
				tmmsmyl.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				Log::Trace("", __FUNCTION__, "PROD_DATE =[{0}]", tmmsmyl["PROD_DATE"].ToString());

				CString xh_seq = "";
				sqlstr = " SELECT '6240'||TO_CHAR(SYSDATE, 'YYYYMMDDHH24MI') || LPAD(TO_CHAR(MMSM_RQXH.nextval), 4, '0') AS SEQ_FULL_NO FROM DUAL";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					xh_seq = cmd_inq.GetString(1);
				}
				cmd_inq.Close();

				Log::Trace("", "", "xh_seq=[{0}]", xh_seq);

				tmmsmyl["SERIAL_NO"] = xh_seq;

				if (v_proc_div == "I")
				{
					if (tmmsmyl["RES_TYPE"].ToString() == "4")
					{
						sqlstr = " select count(1) from TMMSMWQ where LOT_NO='" + tmmsmyl["LOT_NO"].ToString() + "' and C_STATE!='3' AND RES_TYPE!='1' ";
						Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						CDecimal v_ws = cmd_inq.ExecuteScalar();
						cmd_inq.Close();
						if (v_ws != 0)
						{
							sprintf(s.msg, "批次号[%s]有未审核的记录，不可以进行仲裁。", (const char*)tmmsmyl["LOT_NO"]);
							throw CApplicationException(-1, s.msg, log.Location);
						}

					}
					if (tmmsmyl["RES_TYPE"].ToString() == "2")//熔清记录默认参与计算
					{
						tmmsmyl["COM_FLAG"] = "1";
						sqlstr = " select count(1) from TMMSMWQ where LOT_NO='" + tmmsmyl["LOT_NO"].ToString() + "' and C_STATE IN ('2','3') AND RES_TYPE='2' ";
						Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						CDecimal v_ws = cmd_inq.ExecuteScalar();
						cmd_inq.Close();
						if (v_ws != 0)
						{
							sprintf(s.msg, "批次号[%s]有已提交的熔清记录，不可以进行新增。", (const char*)tmmsmyl["LOT_NO"]);
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					v_prod_desc = "新增";
					tmmsmyl["C_STATE"] = "1";
					if (bcls_rec->Tables[0].Rows[0]["EAF_HEAT_NO"].ToString().SubstringNE(0, 1) == "E")
					{
						cmd_sql.SetCommandText(" select to_number(nvl(ACTRESULT,'0')),EMPTY_LADLE_WEIGHT,PROD_DATE from tmmsm20 where PROC_NO='" + tmmsmyl["EAF_HEAT_NO"].ToString() + "' ");
						cmd_sql.ExecuteReader();
						if (cmd_sql.Read())
						{
							tmmsmyl["ACTRESULT"] = cmd_sql.GetDecimal(1);
							tmmsmyl["EMPTY_LADLE_WEIGHT"] = cmd_sql.GetDecimal(2);
							tmmsmyl["PRODUCE_DATE"] = cmd_sql.GetString(3);
						}
						cmd_sql.Close();
					}
					if (bcls_rec->Tables[0].Rows[0]["EAF_HEAT_NO"].ToString().SubstringNE(0, 1) == "F")
					{
						cmd_sql.SetCommandText(" select to_number(nvl(ACTRESULT,'0')),EMPTY_LADLE_WEIGHT,PROD_DATE from tmmsm19 where PROC_NO='" + tmmsmyl["EAF_HEAT_NO"].ToString() + "' ");
						cmd_sql.ExecuteReader();
						if (cmd_sql.Read())
						{
							tmmsmyl["ACTRESULT"] = cmd_sql.GetDecimal(1);
							tmmsmyl["EMPTY_LADLE_WEIGHT"] = cmd_sql.GetDecimal(2);
							tmmsmyl["PRODUCE_DATE"] = cmd_sql.GetString(3);
						}
						cmd_sql.Close();
					}
					Log::Trace("", __FUNCTION__, "ddd =[{0}]", tmmsmyl["PILE_NO"].ToString());
					Log::Trace("", __FUNCTION__, "fff =[{0}]", tmmsmyl["PROC_TYPE"].ToString());
					tmmsmyl["REC_CREATOR"] = s.userid;
					tmmsmyl["REC_CREATE_TIME"] = nowTime;
					tmmsmyl["REC_CREATOR"] = s.userid;
					tmmsmyl["PROD_DATE"] = nowTime.SubstringNE(0, 8);
					cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from " + v_table_name + " where  PROD_DATE = '" + tmmsmyl["PROD_DATE"].ToString().Trim() + "' ");
					tmmsmyl["PROC_COUNT"] = cmd_inq.ExecuteScalar();
					cmd_inq.Close();
					tmmsmyl.TrimOrBlank();
					tmmsmyl.Insert();
				}
				else if (v_proc_div == "U")
				{
					if (tmmsmyl["C_STATE"].ToString() != "1")
					{
						sprintf(s.msg, "只有已编制状态的记录可以修改。");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					v_prod_desc = "修改";
					//取原记录的创建时间和创建人
					sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
						"		FROM 	" + v_table_name + " "
						"   WHERE  PROD_DATE	= @PROD_DATE  AND  PROC_COUNT	= @PROC_COUNT ";

					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.Parameters.Clear();
					cmd_sql.Parameters.Set("PROD_DATE", tmmsmyl["PROD_DATE"].ToString());
					cmd_sql.Parameters.Set("PROC_COUNT", tmmsmyl["PROC_COUNT"].ToDecimal());
					cmd_sql.ExecuteReader();

					if (cmd_sql.Read())
					{
						tmmsmyl["REC_CREATE_TIME"] = cmd_sql.GetString(1);
						tmmsmyl["REC_CREATOR"] = cmd_sql.GetString(2);
					}
					cmd_sql.Close();

					tmmsmyl["REC_REVISE_TIME"] = nowTime;
					tmmsmyl["REC_REVISOR"] = s.userid;

					tmmsmyl.Delete("PROC_COUNT,PROD_DATE");
					tmmsmyl.TrimOrBlank();
					tmmsmyl.Insert();
				}
				else if (v_proc_div == "D")
				{
					if (tmmsmyl["C_STATE"].ToString() != "1")
					{
						sprintf(s.msg, "只有已编制状态的记录可以删除。");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					v_prod_desc = "删除";
					tmmsmyl.TrimOrBlank();
					tmmsmyl.Delete("PROC_COUNT,PROD_DATE");
				}

				else if (v_proc_div == "IZ")
				{
					if (tmmsmyl["COM_FLAG"] = "1")
					{
						sqlstr = " select count(1) from TMMSMWQ where LOT_NO='" + tmmsmyl["LOT_NO"].ToString() + "' and C_STATE IN ('2','3') AND RES_TYPE IN ('4','5') ";
						Log::Trace("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
						cmd_inq.SetCommandText(sqlstr);
						CDecimal v_ws = cmd_inq.ExecuteScalar();
						cmd_inq.Close();
						if (v_ws != 0)
						{
							sprintf(s.msg, "批次号[%s]的有提交或审核的仲裁记录，不可以进行仲裁。", (const char*)tmmsmyl["LOT_NO"]);
							throw CApplicationException(-1, s.msg, log.Location);
						}

					}



					v_prod_desc = "试样仲裁";
					tmmsmyl["C_STATE"] = "1";


					tmmsmyl["REC_CREATOR"] = s.userid;
					tmmsmyl["REC_CREATE_TIME"] = nowTime;
					tmmsmyl["REC_CREATOR"] = s.userid;
					tmmsmyl["PROD_DATE"] = nowTime.SubstringNE(0, 8);
					cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from " + v_table_name + " where  PROD_DATE = '" + tmmsmyl["PROD_DATE"].ToString().Trim() + "' ");
					tmmsmyl["PROC_COUNT"] = cmd_inq.ExecuteScalar();
					cmd_inq.Close();
					tmmsmyl.TrimOrBlank();
					tmmsmyl.Insert();
				}
				else if (v_proc_div == "RA")
				{
					v_prod_desc = "熔清加权";
					tmmsmyl["C_STATE"] = "1";

					tmmsmyl["REC_CREATE_TIME"] = nowTime;
					tmmsmyl["REC_CREATOR"] = s.userid;
					tmmsmyl["SUB_TIME"] = nowTime;
					tmmsmyl["SUB_PER"] = s.userid;
					tmmsmyl["PROD_DATE"] = nowTime.SubstringNE(0, 8);
					cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from " + v_table_name + " where  PROD_DATE = '" + tmmsmyl["PROD_DATE"].ToString().Trim() + "' ");
					tmmsmyl["PROC_COUNT"] = cmd_inq.ExecuteScalar();
					cmd_inq.Close();
					tmmsmyl.TrimOrBlank();
					tmmsmyl.Insert();


				}
				else if (v_proc_div == "ZA")
				{
					v_prod_desc = "仲裁加权";
					tmmsmyl["C_STATE"] = "1";

					tmmsmyl["REC_CREATE_TIME"] = nowTime;
					tmmsmyl["REC_CREATOR"] = s.userid;
					tmmsmyl["SUB_TIME"] = nowTime;
					tmmsmyl["SUB_PER"] = s.userid;
					tmmsmyl["PROD_DATE"] = nowTime.SubstringNE(0, 8);
					cmd_inq.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from " + v_table_name + " where  PROD_DATE = '" + tmmsmyl["PROD_DATE"].ToString().Trim() + "' ");
					tmmsmyl["PROC_COUNT"] = cmd_inq.ExecuteScalar();
					cmd_inq.Close();
					tmmsmyl.TrimOrBlank();
					tmmsmyl.Insert();


				}
				else if (v_proc_div == "SH")
				{
					blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("MMLCSND");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROD_DATE"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROD_DATE");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "LOT_NO");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_COUNT"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROC_COUNT");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}
					if (lot_no_list.find(tmmsmyl["LOT_NO"].ToString()) == lot_no_list.end())
					{
						lot_no_list.insert(tmmsmyl["LOT_NO"].ToString());
						v_prod_desc = "审核发送";
						tmmsmwq["LOT_NO"] = tmmsmyl["LOT_NO"];
						tmmsmwq["C_STATE"] = "2";
						if (tmmsmwq.QueryCount("LOT_NO,C_STATE") == 0)
						{
							sprintf(s.msg, "批次号[%s]没有已提交，可供审核的信息，请刷新。", (const char*)tmmsmyl["LOT_NO"]);
							throw CApplicationException(-1, s.msg, log.Location);
						}
						sqlstr = " select * from tmmsmwq where 1=1 and C_STATE='2' and lot_no='" + tmmsmwq["LOT_NO"].ToString() + "'  ";
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.ExecuteReader();
						while (cmd_inq.Read())
						{
							cmd_inq.Fetch(tmmsmwq);
							bcls_rec->Tables["MMLCSND"].Rows.Clear();
							bcls_rec->Tables["MMLCSND"].Rows.Add();
							bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T83313";
							bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "I";
							bcls_rec->Tables["MMLCSND"].Rows[0]["PROD_DATE"] = tmmsmwq["PROD_DATE"];
							bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"] = tmmsmwq["LOT_NO"];
							bcls_rec->Tables["MMLCSND"].Rows[0]["PROC_COUNT"] = tmmsmwq["PROC_COUNT"];
							doFlag = f_mmsm_t83313_snd(bcls_rec, bcls_ret, conn);
							if (doFlag < 0)
							{
								Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t83313_snd失败-------");
								throw CApplicationException(-1, s.msg, log.Location);
							}

							tmmsmwq["C_STATE"] = "3";
							tmmsmwq["REC_REVISOR"] = s.userid;
							tmmsmwq["REC_REVISE_TIME"] = nowTime;
							tmmsmwq["CHECK_MAKE"] = s.userid;
							tmmsmwq["CHECK_TIME"] = nowTime;
							tmmsmwq.Update("C_STATE,REC_REVISOR,REC_REVISE_TIME,CHECK_TIME,CHECK_MAKE", "PROC_COUNT,PROD_DATE");

						}
					}


				}
				else if (v_proc_div == "CH")
				{
					blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
					if (blkNum < 0)
					{
						bcls_rec->Tables.Add("MMLCSND");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROD_DATE"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROD_DATE");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "LOT_NO");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_COUNT"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROC_COUNT");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}

					v_prod_desc = "撤回发送";
					tmmsmwq["LOT_NO"] = tmmsmyl["LOT_NO"];
					tmmsmwq["C_STATE"] = "3";

					CString sql_temp = "";
					if (tmmsmyl["RES_TYPE"].ToString() == "4" || tmmsmyl["RES_TYPE"].ToString() == "5")
					{
						sqlstr_count = " select count(1) from tmmsmwq where 1=1 and C_STATE='3' and lot_no='" + tmmsmwq["LOT_NO"].ToString() + "' and RES_TYPE in ('4','5') ";
						sql_temp = " and RES_TYPE in ('4','5') ";
					}
					if (tmmsmyl["RES_TYPE"].ToString() == "2" || tmmsmyl["RES_TYPE"].ToString() == "3")
					{
						sqlstr_count = " select count(1) from tmmsmwq where 1=1 and C_STATE='3' and lot_no='" + tmmsmwq["LOT_NO"].ToString() + "' and RES_TYPE in ('1','2','3') ";
						sql_temp = " and RES_TYPE in ('1','2','3') ";
					}
					cmd_sql.SetCommandText(sqlstr_count);
					CDecimal v_count = cmd_sql.ExecuteScalar();
					if (v_count == 0)
					{
						sprintf(s.msg, "批次号[%s]没有已审核，可供撤回的信息，请刷新。", (const char*)tmmsmyl["LOT_NO"]);
						throw CApplicationException(-1, s.msg, log.Location);
					}

					sqlstr = " select * from tmmsmwq where 1=1 and C_STATE='3' and lot_no='" + tmmsmwq["LOT_NO"].ToString() + "'  " + sql_temp;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					while (cmd_inq.Read())
					{
						cmd_inq.Fetch(tmmsmwq);
						bcls_rec->Tables["MMLCSND"].Rows.Clear();
						bcls_rec->Tables["MMLCSND"].Rows.Add();
						bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T83313";
						bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "D";
						bcls_rec->Tables["MMLCSND"].Rows[0]["PROD_DATE"] = tmmsmwq["PROD_DATE"];
						bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"] = tmmsmwq["LOT_NO"];
						bcls_rec->Tables["MMLCSND"].Rows[0]["PROC_COUNT"] = tmmsmwq["PROC_COUNT"];
						doFlag = f_mmsm_t83313_snd(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t83313_snd失败-------");
							throw CApplicationException(-1, s.msg, log.Location);
						}

						tmmsmwq["C_STATE"] = "2";
						tmmsmwq["REC_REVISOR"] = s.userid;
						tmmsmwq["REC_REVISE_TIME"] = nowTime;
						tmmsmwq.Update("C_STATE,REC_REVISOR,REC_REVISE_TIME", "PROC_COUNT,PROD_DATE");


					}

				}
				else if (v_proc_div == "BH")
				{
					v_prod_desc = "驳回提交";
					tmmsmwq["LOT_NO"] = tmmsmyl["LOT_NO"];
					tmmsmwq["C_STATE"] = "2";
					CString sql_temp = "";
					if (tmmsmyl["RES_TYPE"].ToString() == "4" || tmmsmyl["RES_TYPE"].ToString() == "5")
					{
						sqlstr_count = " select count(1) from tmmsmwq where 1=1 and C_STATE='2' and lot_no='" + tmmsmwq["LOT_NO"].ToString() + "' and RES_TYPE in ('4','5') ";
						sql_temp = " and RES_TYPE in ('4','5') ";
					}
					if (tmmsmyl["RES_TYPE"].ToString() == "2" || tmmsmyl["RES_TYPE"].ToString() == "3")
					{
						sqlstr_count = " select count(1) from tmmsmwq where 1=1 and C_STATE='2' and lot_no='" + tmmsmwq["LOT_NO"].ToString() + "' and RES_TYPE in ('1','2','3') ";
						sql_temp = " and RES_TYPE in ('1','2','3') ";
					}
					cmd_sql.SetCommandText(sqlstr_count);
					CDecimal v_count = cmd_sql.ExecuteScalar();
					if (v_count == 0)
					{
						sprintf(s.msg, "批次号[%s]没有已提交，可供驳回的信息，请刷新。", (const char*)tmmsmyl["LOT_NO"]);
						throw CApplicationException(-1, s.msg, log.Location);
					}


					sqlstr = " select * from tmmsmwq where 1=1 and C_STATE='2' and lot_no='" + tmmsmwq["LOT_NO"].ToString() + "' " + sql_temp;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					while (cmd_inq.Read())
					{
						cmd_inq.Fetch(tmmsmwq);

						tmmsmwq["C_STATE"] = "1";
						tmmsmwq["REC_REVISOR"] = s.userid;
						tmmsmwq["REC_REVISE_TIME"] = nowTime;
						tmmsmwq.Update("C_STATE,REC_REVISOR,REC_REVISE_TIME", "PROC_COUNT,PROD_DATE");

						if (tmmsmwq["RES_TYPE"].ToString() == "3"
							|| tmmsmwq["RES_TYPE"].ToString() == "5")
						{
							tmmsmwq.Delete("PROC_COUNT,PROD_DATE");
						}
					}
				}


			}
		}
		
				tmmsmyl.Query("PROC_COUNT,PROD_DATE");
				tmmsmwq_ll.CopyFrom(tmmsmyl);
				tmmsmwq_ll["EVENT_ID"] = v_proc_div;
				tmmsmwq_ll["EVENT_DESC"] = v_prod_desc;
				tmmsmwq_ll["EVENT_MAKER"] = s.userid;
				tmmsmwq_ll["EVENT_TIME"] = nowTime;
				tmmsmwq_ll["SUB_PER"] = s.userid;
				tmmsmwq_ll["SUB_TIME"] = nowTime;
				tmmsmwq_ll.Insert();
				
			
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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

}
