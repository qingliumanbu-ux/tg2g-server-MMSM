/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
//by20260806 ：新增废钢扣杂量，涉及库存扣减，原库存使用毛重扣减，新增上料的量使用（毛重-扣杂量），抛二级使用毛重，抛资源三个量一起
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t8e2yy_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm831_upd2)

int f_mmsm831_upd2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int   blkNum;
	int doFlag = 0;
	CString sqlstr = "";
	CString sql = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString s_bunker_no = "";
	CString s_bunker_no_original = "";
	CString bunker_no1 = "";
	CString quality_batch_no = "";
	int		TotalRecordCount = 0;
	int i_idx = 0;
	int n_idx = 0;
	int d_idx = 0;
	int x_idx = 0;
	CDecimal cd_stock_wt = 0;
	CDecimal bunker_deduct_wt = 0;
	CDecimal bunker_deduct_rate = 0;
	CDecimal d_stock_wt = 0;
	CDecimal cd_seq_no = 0;
	CDecimal nd_seq_no = 0;
	CDecimal seq_no = 0;
	CString d_seq_no = "";
	CString mat_code = "";
	CString mat_code1 = "";
	CString weigh_no = "";
	CString weigh_no1 = "";
	//系统的分页类信息。
	CPageInfo pageInfo;
	CModel tmmsm60("TMMSM60");
	CModel tmmsm60_O("TMMSM60");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm85_O("TMMSM85");
	CModel tmmsm85_Z("TMMSM85");
	CModel tmmsm85_Z1("TMMSM85");
	CModel tmmsm89("TMMSM89");
	CModel tmmsm81("TMMSM81");
	CModel tmmsm81s("TMMSM81_S");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	EIClass EITable;
	try
	{
		tmmsm85["BUNKER_NO"] = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString();
		tmmsm85_O["BUNKER_NO"] = bcls_rec->Tables[0].Rows[0]["BUNKER_NO1"].ToString();
		
		cd_stock_wt = bcls_rec->Tables[0].Rows[0]["STOCK_WT"].ToDecimal();
		//by20260806
		if (bcls_rec->Tables[0].Columns.Contains("BUNKER_DEDUCT_WT"))
		{		
		bunker_deduct_wt = bcls_rec->Tables[0].Rows[0]["BUNKER_DEDUCT_WT"].ToDecimal();
		}
		if (cd_stock_wt!=0)
		{
			bunker_deduct_rate = (bunker_deduct_wt / cd_stock_wt).Round(2);
		}
		if (cd_stock_wt<0)
		{
			sprintf(s.msg, "输入重量必须大于0");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm85["BUNKER_NO"].ToString() == tmmsm85_O["BUNKER_NO"].ToString())
		{
			sprintf(s.msg, "相同料仓不能上料");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		// 先更新60表数据
		tmmsm60["BUNKER_NO"] = tmmsm85["BUNKER_NO"];
		tmmsm60_O["BUNKER_NO"] = tmmsm85_O["BUNKER_NO"];
		tmmsm60.Query("BUNKER_NO");
		tmmsm60_O.Query("BUNKER_NO");

		if (tmmsm60["BACK_C3"].ToString() == "1")
		{
			sprintf(s.msg, "炉前状态不能上料");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm60_O["STOCK_WT"].ToDecimal() < cd_stock_wt)
		{
			sprintf(s.msg, "输入的重量已超过低位料仓总量");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm60_O["STOCK_WT"].ToDecimal() == 0)
		{
			sprintf(s.msg, "料仓重量为0不能上料");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm85["BUNKER_NO"].ToString() == tmmsm85_O["BUNKER_NO"].ToString())
		{
			sprintf(s.msg, "相同料仓不能上料");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//by20260806
		if (bunker_deduct_wt != 0 && tmmsm60_O["MAT_CODE"].ToString().SubstringNE(0, 1) != "F")
		{
			sprintf(s.msg, "非废钢料不能扣杂。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = "  SELECT * FROM TMMSM85 WHERE bunker_no = @bunker_no ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("bunker_no", tmmsm85["BUNKER_NO"]);
		cmd_inq.ExecuteQuery(EITable.Tables[0]);
		cmd_inq.Close();
		if (EITable.Tables[0].Rows.get_Count() >60)
		{
			sprintf(s.msg, "同一料槽上料不能超过60条记录");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85     WHERE 1=1   ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cd_seq_no = cmd_inq.GetDecimal(1) + 1;
		}
		cmd_inq.Close();

		sql = " SELECT NVL(min(SEQ_NO),0) SEQ_NO ,SUM(STOCK_WT) STOCK_WT FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO = @BUNKER_NO  and QUALITY_BATCH_NO <> ' ' AND STOCK_WT <> 0 ";
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm85_O["BUNKER_NO"].ToString());
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			nd_seq_no = cmd_inq.GetDecimal(1);
			if (cd_stock_wt>cmd_inq.GetDecimal(2))
			{
				sprintf(s.msg, "选择的低位料仓合格量低于输入重量不允许上料");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (nd_seq_no != 0)
			{
				d_idx = 1;
			}
		}
		cmd_inq.Close();

		sqlstr = " SELECT BUNKER_NO FROM TMMSM60 t WHERE CHECK_OUT_FLAG ='1' and BUNKER_NO = @BUNKER_NO   ";

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm85_O["BUNKER_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			if (bcls_rec->Tables.get_Count() != 2)
			{
				sprintf(s.msg, "特殊料仓需要先勾选低位料仓物料信息");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (bcls_rec->Tables[1].Rows.get_Count()>0)
			{
				for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
				{
					d_seq_no = d_seq_no + "," + "'" + bcls_rec->Tables[1].Rows[i]["SEQ_NO"].ToString() + "'";
					weigh_no = weigh_no + "," + "'" + bcls_rec->Tables[1].Rows[i]["WEIGH_NO"].ToString() + "'";
					mat_code = mat_code + "," + "'" + bcls_rec->Tables[1].Rows[i]["MAT_CODE"].ToString() + "'";
					quality_batch_no = bcls_rec->Tables[1].Rows[0]["QUALITY_BATCH_NO"].ToString();
					mat_code1 = bcls_rec->Tables[1].Rows[0]["MAT_CODE"].ToString();
					weigh_no1 = bcls_rec->Tables[1].Rows[0]["WEIGH_NO"].ToString();
					d_stock_wt = bcls_rec->Tables[1].Rows[i]["STOCK_WT"].ToDecimal() + d_stock_wt;
					if (quality_batch_no != bcls_rec->Tables[1].Rows[i]["QUALITY_BATCH_NO"].ToString())
					{
						sprintf(s.msg, "特殊料仓多选物料信息必须质检批号一致");
						throw CApplicationException(-1, s.msg, log.Location);
					}

					if ("" == bcls_rec->Tables[1].Rows[i]["QUALITY_BATCH_NO"].ToString().Trim())
					{
						sprintf(s.msg, "质检批号不能为空");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (mat_code1 != bcls_rec->Tables[1].Rows[i]["MAT_CODE"].ToString())
					{
						sprintf(s.msg, "特殊料仓多选物料信息必须物料代码一致");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (cd_stock_wt>d_stock_wt)
				{
					sprintf(s.msg, "输入重量大于选中的物料重量");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (d_seq_no.GetLength()>0)
				{
					d_seq_no = d_seq_no.Substring(1, d_seq_no.GetLength() - 1);
				}

				if (weigh_no.GetLength()>0)
				{
					weigh_no = weigh_no.Substring(1, weigh_no.GetLength() - 1);
					Log::Trace(" ", __FUNCTION__, "weigh_no =[{0}]", weigh_no);
				}

				if (mat_code.GetLength()>0)
				{
					mat_code = mat_code.Substring(1, mat_code.GetLength() - 1);
					Log::Trace(" ", __FUNCTION__, "weigh_no =[{0}]", mat_code);
				}
			}
			else
			{
				sprintf(s.msg, "特殊料仓需要先勾选低位料仓物料信息");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			seq_no = bcls_rec->Tables[1].Rows[0]["seq_no"].ToDecimal();//序号
			Log::Trace(" ", __FUNCTION__, "seq_no =[{0}]", seq_no);

			CString SeqNo1 = "";
			CString serial_number = "";

			//tmmsm85["SERIAL_NUMBER"] = serial_number;

			// 更新85表数据
			sql = "  SELECT * FROM  TMMSM85     WHERE 1=1 AND STOCK_WT <>0  and QUALITY_BATCH_NO <> ' '  AND BUNKER_NO = @BUNKER_NO and SEQ_NO in ( " + d_seq_no + " )   and MAT_CODE in ( " + mat_code + " ) and  WEIGH_NO in ( " + weigh_no + " )  ORDER BY SEQ_NO ASC ";
			cmd_inq1.Parameters.Set("BUNKER_NO", tmmsm85_O["BUNKER_NO"].ToString());			
			cmd_inq1.SetCommandText(sql);
			int  i = 0;
			cmd_inq1.ExecuteReader();
			while (cmd_inq1.Read())
			{
				cmd_inq1.Fetch(tmmsm85_Z);
				tmmsm85_Z1.CopyFrom(tmmsm85_Z);


				tmmsm81["WEIGH_NO"] = tmmsm85_Z1["WEIGH_NO"];
				tmmsm81s["WEIGH_NO"] = tmmsm85_Z1["WEIGH_NO"];
				tmmsm81["RECEIVING_STATUS"] = "K";
				tmmsm81s["RECEIVING_STATUS"] = "K";
				tmmsm81.Query("WEIGH_NO");
				if (tmmsm81["FORM_EDIT_FLAG"].ToString().Trim() != "1")
				{
					tmmsm81["FORM_EDIT_FLAG"] = "1";
					tmmsm81.Update("RECEIVING_STATUS,FORM_EDIT_FLAG", "WEIGH_NO");
				}
				tmmsm81s.Query("WEIGH_NO");
				if (tmmsm81s["FORM_EDIT_FLAG"].ToString().Trim() != "1")
				{
					tmmsm81s["FORM_EDIT_FLAG"] = "1";
					tmmsm81s.Update("RECEIVING_STATUS,FORM_EDIT_FLAG", "WEIGH_NO");
				}

				sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_LS.NEXTVAL),18 ) FROM DUAL ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					SeqNo1 = cmd_inq.GetString(1).Trim();

				}
				cmd_inq.Close();

				serial_number = "EG" + SeqNo1;
				
				if (tmmsm85_Z["STOCK_WT"] <= cd_stock_wt)
				{
					tmmsm85_Z.Delete("BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");

					cd_stock_wt = cd_stock_wt - tmmsm85_Z["STOCK_WT"];

					//新增毛重和扣重					
					if (cd_stock_wt == 0)
					{
						tmmsm85_Z1["BUNKER_DEDUCT_WT"] = bunker_deduct_wt;
						bunker_deduct_wt = 0;
					}
					else
					{
						tmmsm85_Z1["BUNKER_DEDUCT_WT"] = (tmmsm85_Z["STOCK_WT"] * bunker_deduct_rate).Round(0);
						bunker_deduct_wt = bunker_deduct_wt - (tmmsm85_Z["STOCK_WT"] * bunker_deduct_rate).Round(0);
					}
					tmmsm85_Z1["STOCK_WT"] = tmmsm85_Z["STOCK_WT"].ToDecimal() - tmmsm85_Z1["BUNKER_DEDUCT_WT"].ToDecimal();

					tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
					tmmsm85_Z1["BUNKER_NO"] = tmmsm60["BUNKER_NO"];					
					tmmsm85_Z1["BUNKER_TYPE"] = "AODBOX";
					tmmsm85_Z1["BUNKER_NAME"] = "AOD料槽";
					tmmsm85_Z1["BUNKER_NO_ORIGINAL"] = tmmsm85_O["BUNKER_NO"];
					tmmsm85_Z1["TIME_1"] = datetime;
					tmmsm85_Z1["SERIAL_NUMBER"] = serial_number;
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["TIME_INSTOCK"] = datetime;
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
					tmmsm89.CopyFrom(tmmsm85_Z1);
					tmmsm89["EVENT_CODE"] = "MOVE";
					tmmsm89["EVENT_DESC"] = "移库";
					tmmsm89["EVENT_NAME"] = "料槽料篮上料";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60_O["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = "AODBOX";
					tmmsm89["BUNKER_NAME_ORIGINAL"] = "AOD料槽";
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
					tmmsm85_Z1.Insert();
					tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
					tmmsm60["SERIAL_NUMBER"] = serial_number;
					tmmsm60["LASTACTDATE"] = datetime;
					tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
					tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
					tmmsm60.Update("SERIAL_NUMBER,LOT_NO,LASTACTDATE,QUALITY_BATCH_NO", "BUNKER_NO");

					if (cd_stock_wt == 0)
					{
						break;
					}
				}
				else
				{
					sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_LS.NEXTVAL),18 ) FROM DUAL ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						SeqNo1 = cmd_inq.GetString(1).Trim();

					}
					cmd_inq.Close();

					serial_number = "EG" + SeqNo1;
					tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"] - cd_stock_wt;
					tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
					tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;	
					tmmsm85_Z1["BUNKER_DEDUCT_WT"] = bunker_deduct_wt;
					tmmsm85_Z1["STOCK_WT"] = cd_stock_wt - tmmsm85_Z1["BUNKER_DEDUCT_WT"].ToDecimal();
					tmmsm85_Z1["BUNKER_NO"] = tmmsm60["BUNKER_NO"];
					tmmsm85_Z1["BUNKER_TYPE"] = "AODBOX";
					tmmsm85_Z1["BUNKER_NAME"] = "AOD料槽";
					tmmsm85_Z1["BUNKER_NO_ORIGINAL"] = tmmsm85_O["BUNKER_NO"];
					tmmsm85_Z1["TIME_1"] = datetime;
					tmmsm85_Z1["SERIAL_NUMBER"] = serial_number;
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["TIME_INSTOCK"] = datetime;
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
					tmmsm89.CopyFrom(tmmsm85_Z1);
					tmmsm89["EVENT_CODE"] = "MOVE";
					tmmsm89["EVENT_DESC"] = "移库";
					tmmsm89["EVENT_NAME"] = "料槽料篮上料";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60_O["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = "AODBOX";
					tmmsm89["BUNKER_NAME_ORIGINAL"] = "AOD料槽";
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
					tmmsm85_Z1.Insert();

					tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
					tmmsm60["SERIAL_NUMBER"] = serial_number;
					tmmsm60["LASTACTDATE"] = datetime;
					tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
					tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
					tmmsm60.Update("SERIAL_NUMBER,LOT_NO,LASTACTDATE,QUALITY_BATCH_NO", "BUNKER_NO");
					break;
				}


				i++;
			}
			cmd_inq1.Close();



			sqlstr = " update tmmsm60 set back_c1='1',stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = @bunker_no)"
				" where BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", tmmsm60["BUNKER_NO"].ToString());
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新库存
			sqlstr = " update tmmsm60 set stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = @bunker_no)"
				" where BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", tmmsm60_O["BUNKER_NO"].ToString());
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}
		else
		{
			x_idx = 1;
		}
		cmd_inq.Close();

		if (d_idx != 1)
		{
			sprintf(s.msg, "料仓信息缺失或没用成分数据");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (x_idx == 1)
		{
			d_idx = 0;
			CString SeqNo1 = "";
			CString serial_number = "";
			// 更新85表数据
			sql = "  SELECT * FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO			= @BUNKER_NO  and QUALITY_BATCH_NO <> ' ' AND STOCK_WT <> 0  ORDER BY SEQ_NO,TIME_INSTOCK ASC ";
			cmd_inq1.Parameters.Set("BUNKER_NO", tmmsm85_O["BUNKER_NO"].ToString());			
			cmd_inq1.SetCommandText(sql);
			int  i = 0;
			cmd_inq1.ExecuteReader();
			while (cmd_inq1.Read())
			{
				cmd_inq1.Fetch(tmmsm85_Z);
				tmmsm85_Z1.CopyFrom(tmmsm85_Z);


				tmmsm81["WEIGH_NO"] = tmmsm85_Z1["WEIGH_NO"];
				tmmsm81s["WEIGH_NO"] = tmmsm85_Z1["WEIGH_NO"];
				tmmsm81["RECEIVING_STATUS"] = "K";
				tmmsm81s["RECEIVING_STATUS"] = "K";
				tmmsm81.Query("WEIGH_NO");
				if (tmmsm81["FORM_EDIT_FLAG"].ToString().Trim() != "1")
				{
					tmmsm81["FORM_EDIT_FLAG"] = "1";
					tmmsm81.Update("RECEIVING_STATUS,FORM_EDIT_FLAG", "WEIGH_NO");
				}
				tmmsm81s.Query("WEIGH_NO");
				if (tmmsm81s["FORM_EDIT_FLAG"].ToString().Trim() != "1")
				{
					tmmsm81s["FORM_EDIT_FLAG"] = "1";
					tmmsm81s.Update("RECEIVING_STATUS,FORM_EDIT_FLAG", "WEIGH_NO");
				}

				sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_LS.NEXTVAL),18 ) FROM DUAL ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					SeqNo1 = cmd_inq.GetString(1).Trim();

				}
				cmd_inq.Close();

				serial_number = "EG" + SeqNo1;
				Log::Trace(" ", __FUNCTION__, "serial_number =[{0}]", serial_number);

				if (tmmsm85_Z["STOCK_WT"] <= cd_stock_wt)
				{					
					tmmsm85_Z.Delete("BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
					cd_stock_wt = cd_stock_wt - tmmsm85_Z["STOCK_WT"];
					//新增毛重和扣重
					if (cd_stock_wt == 0)
					{
						tmmsm85_Z1["BUNKER_DEDUCT_WT"] = bunker_deduct_wt;
						bunker_deduct_wt = 0;
					}
					else
					{
						tmmsm85_Z1["BUNKER_DEDUCT_WT"] = (tmmsm85_Z["STOCK_WT"] * bunker_deduct_rate).Round(0);
						bunker_deduct_wt = bunker_deduct_wt - (tmmsm85_Z["STOCK_WT"] * bunker_deduct_rate).Round(0);
					}
					tmmsm85_Z1["STOCK_WT"] = tmmsm85_Z["STOCK_WT"].ToDecimal() - tmmsm85_Z1["BUNKER_DEDUCT_WT"].ToDecimal();

					tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
					tmmsm85_Z1["BUNKER_NO"] = tmmsm60["BUNKER_NO"];
					tmmsm85_Z1["BUNKER_TYPE"] = "AODBOX";
					tmmsm85_Z1["BUNKER_NAME"] = "AOD料槽";
					tmmsm85_Z1["BUNKER_NO_ORIGINAL"] = tmmsm85_O["BUNKER_NO"];
					tmmsm85_Z1["TIME_1"] = datetime;
					tmmsm85_Z1["SERIAL_NUMBER"] = serial_number;
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["TIME_INSTOCK"] = datetime;
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
					tmmsm89.CopyFrom(tmmsm85_Z1);
					tmmsm89["EVENT_CODE"] = "MOVE";
					tmmsm89["EVENT_DESC"] = "移库";
					tmmsm89["EVENT_NAME"] = "料槽料篮上料";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60_O["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = "AODBOX";
					tmmsm89["BUNKER_NAME_ORIGINAL"] = "AOD料槽";
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
					tmmsm85_Z1.Insert();
					tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
					tmmsm60["SERIAL_NUMBER"] = serial_number;
					tmmsm60["LASTACTDATE"] = datetime;
					tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
					tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
					tmmsm60.Update("SERIAL_NUMBER,LOT_NO,LASTACTDATE,QUALITY_BATCH_NO", "BUNKER_NO");

					if (cd_stock_wt == 0)
					{
						break;
					}
				}
				else
				{					
					sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_LS.NEXTVAL),18 ) FROM DUAL ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						SeqNo1 = cmd_inq.GetString(1).Trim();

					}
					cmd_inq.Close();

					serial_number = "EG" + SeqNo1;
					tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"] - cd_stock_wt;
					tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
					tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
					tmmsm85_Z1["BUNKER_DEDUCT_WT"] = bunker_deduct_wt;
					tmmsm85_Z1["STOCK_WT"] = cd_stock_wt - tmmsm85_Z1["BUNKER_DEDUCT_WT"].ToDecimal();

					tmmsm85_Z1["BUNKER_NO"] = tmmsm60["BUNKER_NO"];
					tmmsm85_Z1["BUNKER_TYPE"] = "AODBOX";
					tmmsm85_Z1["BUNKER_NAME"] = "AOD料槽";
					tmmsm85_Z1["BUNKER_NO_ORIGINAL"] = tmmsm85_O["BUNKER_NO"];
					tmmsm85_Z1["TIME_1"] = datetime;
					tmmsm85_Z1["SERIAL_NUMBER"] = serial_number;
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["TIME_INSTOCK"] = datetime;
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
					tmmsm89.CopyFrom(tmmsm85_Z1);
					tmmsm89["EVENT_CODE"] = "MOVE";
					tmmsm89["EVENT_DESC"] = "移库";
					tmmsm89["EVENT_NAME"] = "料槽料篮上料";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60_O["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = "AODBOX";
					tmmsm89["BUNKER_NAME_ORIGINAL"] = "AOD料槽";
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
					tmmsm85_Z1.Insert();

					tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
					tmmsm60["SERIAL_NUMBER"] = serial_number;
					tmmsm60["LASTACTDATE"] = datetime;
					tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
					tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
					tmmsm60.Update("SERIAL_NUMBER,LOT_NO,LASTACTDATE,QUALITY_BATCH_NO", "BUNKER_NO");
					break;
				}


				i++;
			}
			cmd_inq1.Close();



			sqlstr = " update tmmsm60 set back_c1='1',stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = @bunker_no)"
				" where BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", tmmsm60["BUNKER_NO"].ToString());
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新库存
			sqlstr = " update tmmsm60 set stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = @bunker_no)"
				" where BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", tmmsm60_O["BUNKER_NO"].ToString());
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}		

		if (bcls_rec_tmmsm89_log.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

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
