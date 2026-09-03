/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_mmsm_t8e2yy_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t8e2yb_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm831_updnbk)

int f_mmsm831_updnbk(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int   blkNum;
	CString sqlstr = "";
	CString sql = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString s_bunker_no = "";
	CString c_bunker_no = "";
	CString mat_code = " ";
	CString quality_batch_no = " ";
	CString s_bunker_no_original = "";
	int		TotalRecordCount = 0;
	int i_idx = 0;
	int n_idx = 0;
	int d_idx = 0;
	CDecimal cd_stock_wt = 0;
	CDecimal cd_seq_no = 0;
	CDecimal nd_seq_no = 0;
	//系统的分页类信息。
	CPageInfo pageInfo;
	CModel tmmsm60("TMMSM60");
	CModel tmmsm60_O("TMMSM60");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm85C("TMMSM85");
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
	try
	{
		//tmmsm85["BUNKER_NO"] = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString();

		tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		i_idx = bcls_rec->Tables[0].Rows.get_Count();
		//tmmsm85["BUNKER_NO"] = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString();

		tmmsm85_O["BUNKER_NO"] = bcls_rec->Tables[1].Rows[0]["BUNKER_NO"].ToString();

		Log::Trace(" ", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm85["BUNKER_NO"].ToString());
		Log::Trace(" ", __FUNCTION__, "tmmsm85_O =[{0}]", tmmsm85_O["BUNKER_NO"].ToString());
		if (bcls_rec->Tables[2].Columns.Contains("BUNKER_NO"))
		{
			c_bunker_no = bcls_rec->Tables[2].Rows[0]["BUNKER_NO"].ToString();
		}
		
		//20240428询问初亮后添加判断逻辑 上料时必须先选择料仓号
		if (c_bunker_no.Trim() == "all-全部镍板库-" || c_bunker_no.Trim() == "-" || c_bunker_no.Trim() == "all")
		{
			sprintf(s.msg, "请选择具体料仓号");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		cd_stock_wt = bcls_rec->Tables[2].Rows[0]["STOCK_WT"].ToDecimal();

		mat_code = bcls_rec->Tables[2].Rows[0]["MAT_CODE"].ToString();
		quality_batch_no = bcls_rec->Tables[2].Rows[0]["QUALITY_BATCH_NO"].ToString();

		
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

		tmmsm85C.CopyFrom(tmmsm85);
		if (tmmsm85C.QueryCount("BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO")==1)
		{
			tmmsm85C.Query("BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
			if (tmmsm85C["STOCK_WT"].ToDecimal() < cd_stock_wt)
			{
			sprintf(s.msg, "输入料仓大于料仓库存量");
			throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{
			sprintf(s.msg, "输入料仓大于料仓库存量,或该物料库存已不在");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		

		if (tmmsm85["BUNKER_NO"].ToString() == tmmsm85_O["BUNKER_NO"].ToString())
		{
			sprintf(s.msg, "相同料仓不能上料");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85     WHERE 1=1  ";
		//cmd_inq.Parameters.Set("BUNKER_NO", tmmsm85_O["BUNKER_NO"].ToString());
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cd_seq_no = cmd_inq.GetDecimal(1) + 1;
		}
		cmd_inq.Close();
		Log::Trace(" ", __FUNCTION__, "cd_seq_no =[{0}]", cd_seq_no); 

		sql = " SELECT NVL(min(SEQ_NO),0) SEQ_NO ,SUM(STOCK_WT) STOCK_WT FROM TMMSM85 WHERE 1=1 AND MAT_CODE = @MAT_CODE AND BUNKER_NO = @BUNKER_NO  and QUALITY_BATCH_NO <> ' ' AND STOCK_WT <> 0 ";
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm85["BUNKER_NO"].ToString());
		cmd_inq.Parameters.Set("MAT_CODE", mat_code);
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			nd_seq_no = cmd_inq.GetDecimal(1);
			if (cd_stock_wt>cmd_inq.GetDecimal(2))
			{
				sprintf(s.msg, "选择的料仓合格量低于输入重量不允许上料");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (nd_seq_no != 0)
			{
				d_idx = 1;
			}
		}
		cmd_inq.Close(); 


		if (1 == 1)
		{
			d_idx = 0;
			// 更新85表数据
			sql = "  SELECT * FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO = @BUNKER_NO  AND MAT_CODE = @MAT_CODE and QUALITY_BATCH_NO <> ' ' AND STOCK_WT <> 0  ";
			if (quality_batch_no.Trim()!="")
			{
				sql = sql + " and QUALITY_BATCH_NO=@QUALITY_BATCH_NO ";
			}

			if (i_idx==1)
			{
				sql = sql + " and SEQ_NO=@SEQ_NO  AND WEIGH_NO =@WEIGH_NO ";
				cmd_inq1.Parameters.Set("SEQ_NO", tmmsm85["SEQ_NO"].ToString());
				cmd_inq1.Parameters.Set("WEIGH_NO", tmmsm85["WEIGH_NO"].ToString());
			}

			sql = sql + " ORDER BY SEQ_NO ASC ";
			cmd_inq1.Parameters.Set("BUNKER_NO", tmmsm85["BUNKER_NO"].ToString());
			cmd_inq1.Parameters.Set("MAT_CODE", mat_code);
			cmd_inq1.Parameters.Set("QUALITY_BATCH_NO", quality_batch_no); 
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

				if (tmmsm85_Z["STOCK_WT"] <= cd_stock_wt)
				{
					Log::Trace(" ", __FUNCTION__, "1=[{0}]", 1);

					cd_stock_wt = cd_stock_wt - tmmsm85_Z["STOCK_WT"];
					tmmsm85_Z.Delete("BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
					tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
					//tmmsm85_Z1["BUNKER_NO"] = tmmsm60_O["STATION_NO"];
					tmmsm85_Z1["BUNKER_NO"] = tmmsm60_O["BUNKER_NO"];
					tmmsm85_Z1["BUNKER_TYPE"] = tmmsm60_O["BUNKER_TYPE"];
					tmmsm85_Z1["BUNKER_NAME"] = tmmsm60_O["BUNKER_NAME"];
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
					tmmsm89.CopyFrom(tmmsm85_Z1);
					tmmsm89["EVENT_CODE"] = "MOVE";
					tmmsm89["EVENT_DESC"] = "移库";
					tmmsm89["EVENT_NAME"] = "镍板库-平台";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["TIME_INSTOCK"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60["BUNKER_TYPE"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"];
					//tmmsm89["EVENT_DESC"] = "上料从" + tmmsm89["BUNKER_NO_ORIGINAL"].ToString() + "到" + tmmsm89["BUNKER_NO"].ToString();
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);

					tmmsm85_Z1.Insert();
					/*tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
					tmmsm60["LASTACTDATE"] = datetime;
					tmmsm60.Update("LASTACTDATE", "BUNKER_NO");*/

					tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
					tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
					tmmsm60["LASTACTDATE"] = datetime;
					tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
					tmmsm60.Update("LASTACTDATE,LOT_NO,QUALITY_BATCH_NO", "BUNKER_NO");

					if (cd_stock_wt == 0)
					{
						break;
					}
				}
				else
				{
					Log::Trace(" ", __FUNCTION__, "2=[{0}]", 1);
					tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"] - cd_stock_wt;
					tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
					tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
					tmmsm85_Z1["STOCK_WT"] = cd_stock_wt;

					Log::Trace(" ", __FUNCTION__, "BUNKER_NO=[{0}],MAT_CODE=[{1}],WEIGH_NO=[{2}],SEQ_NO=[{3}],STOCK_WT=[{4}]", 
						tmmsm85_Z1["BUNKER_NO"].ToString(), tmmsm85_Z1["BUNKER_NO"].ToString(), tmmsm85_Z1["MAT_CODE"].ToString(), tmmsm85_Z1["WEIGH_NO"].ToString(), tmmsm85_Z1["WEIGH_NO"].ToString());
					//tmmsm85_Z1["BUNKER_NO"] = tmmsm60_O["BUNKER_NO"];
					tmmsm85_Z1["BUNKER_NO"] = tmmsm60_O["BUNKER_NO"];
					tmmsm85_Z1["BUNKER_TYPE"] = tmmsm60_O["BUNKER_TYPE"];
					tmmsm85_Z1["BUNKER_NAME"] = tmmsm60_O["BUNKER_NAME"];
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
					tmmsm89.CopyFrom(tmmsm85_Z1);
					tmmsm89["EVENT_CODE"] = "MOVE";
					tmmsm89["EVENT_DESC"] = "移库";
					tmmsm89["EVENT_NAME"] = "镍板库-平台";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["TIME_INSTOCK"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60["BUNKER_TYPE"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = cd_stock_wt;
					//tmmsm89["EVENT_DESC"] = "上料从" + tmmsm89["BUNKER_NO_ORIGINAL"].ToString() + "到" + tmmsm89["BUNKER_NO"].ToString();
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
					Log::Trace(" ", __FUNCTION__, "2=[{0}]", 333);

					tmmsm85_Z1.Insert(); 					

					tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
					tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
					tmmsm60["LASTACTDATE"] = datetime;
					tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
					tmmsm60.Update("LASTACTDATE,LOT_NO,QUALITY_BATCH_NO", "BUNKER_NO");
					break;
				}
				i++;
			}
			cmd_inq1.Close();

			sqlstr = " update tmmsm60 set stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = @bunker_no)"
				" where BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", tmmsm85["BUNKER_NO"].ToString());
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新库存
			sqlstr = " update tmmsm60 set stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = @bunker_no)"
				" where BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", tmmsm85_O["BUNKER_NO"].ToString());
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}
		else
		{
			sprintf(s.msg, "料仓信息缺失或没用成分数据");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//给原料L2-发送料仓物料信息和成分信息
		Log::Trace(" ", __FUNCTION__, "BUNKER_NO=[{0}],SEQ_NO=[{1}],MAT_CODE=[{2}],BUNKER_TYPE=[{3}],QUALITY_BATCH_NO=[{4}]",
			tmmsm85_Z1["BUNKER_NO"].ToString(), tmmsm85_Z1["SEQ_NO"].ToString(), tmmsm85_Z1["MAT_CODE"].ToString(),
			tmmsm85_Z1["BUNKER_TYPE"].ToString(), tmmsm85_Z1["QUALITY_BATCH_NO"].ToString());
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMLCSND");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUNKER_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("QUALITY_BATCH_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "LOT_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STOCK_WT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STOCK_WT");
		}
		//给原料L2发成分信息添加字段
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ELM_NAME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "ELM_NAME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ELM_VALUE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "ELM_VALUE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STATION_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STATION_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ACTION"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "ACTION");
		}
		bcls_rec->Tables["MMLCSND"].Rows.Add();

		bcls_rec->Tables["MMLCSND"].Rows[0]["ACTION"] = "I";
		bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YB";
		bcls_rec->Tables["MMLCSND"].Rows[0]["STATION_NO"] = tmmsm60["BUNKER_TYPE"].ToString();
		bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = tmmsm85_Z1["MAT_CODE"];
		bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
		bcls_rec->Tables["MMLCSND"].Rows[0]["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
		doFlag = f_mmsm_t8e2yb_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t8e2yb_snd失败-------");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		// 20241224初亮要求成分电文发送和料仓电文晚一秒发送
		sleep(1);

		if (tmmsm60_O["BUNKER_NO"].ToString().SubstringNE(0, 1) == "A")
		{
			//AOD
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y1";
			bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_AOD_BIN";
		}
		else if (tmmsm60_O["BUNKER_NO"].ToString().SubstringNE(0, 1) == "B")
		{
			//转炉
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y3";
			bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_BOF_BIN";
		}
		else if (tmmsm60_O["BUNKER_NO"].ToString().SubstringNE(0, 1) == "E")
		{
			//电炉
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y5";
			bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_EAF_BIN";
		}
		//else if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) == "I")
		//{
		//	//中频炉
		//	bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y8";
		//	bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_IF_BIN";
		//}
		else if (tmmsm60_O["BUNKER_NO"].ToString().SubstringNE(0, 1) == "F")
		{
			//LF
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y9";
			bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_LF_BIN";
		}
		else if (tmmsm60_O["BUNKER_NO"].ToString().SubstringNE(0, 1) == "T") //待确认
		{
			//LTS
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YA";
			bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_LTS_BIN";
		}
		else if (tmmsm60_O["BUNKER_NO"].ToString().SubstringNE(0, 1) == "R")
		{
			//RH
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YF";
			bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_RH_BIN";
		}
		//else if (tmmsm60_O["BUNKER_NO"].ToString().SubstringNE(0, 1) == "V")
		//{
		//	//VOD
		//	bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YI";
		//	bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_VOD_BIN";
		//}
		bcls_rec->Tables["MMLCSND"].Rows[0]["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
		bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
		bcls_rec->Tables["MMLCSND"].Rows[0]["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
		bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = tmmsm85_Z1["MAT_CODE"];

		doFlag = f_mmsm_t8e2yy_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t8e2yy_snd失败-------");
			throw CApplicationException(-1, s.msg, log.Location);
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
