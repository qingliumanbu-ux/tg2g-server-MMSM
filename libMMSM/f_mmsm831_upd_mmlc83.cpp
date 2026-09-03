/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   renximing
Version:    1.0
Date:     2024-6-19
Description: 一钢计量实绩接收
**************************************************/

#include "stdafx.h"

int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t8e2yy_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t8e2yb_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm_updlc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm831_upd_mmlc83(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*系统日志类定义*/
	CTracer log(__FUNCTION__);

	// 程序内部变量
	int doFlag = 0;
	int n_flag = 0;
	int	blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString cs_mmjl_seq_no = "";

	CString c_orderid = "";
	CString c_x_item = "";
	CString SeqNo = "";
	CString weigh_no = "";

	// 实体类定义
	CModel tmmsm81_rcv("TMMSM81_RCV");//计量单表电文履历

	// 数据库SQL操作字符串
	CString sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_ins(conn);
	CDbCommand cmd_upd(conn);
	CString auart = " ";
	EIClass eitable;

	try
	{
		CTracer log(__FUNCTION__);

		/* ***** 自定义变量 ***** */
		int   blkNum;
		int doFlag = 0;
		CString sqlstr = "";
		CString sql = "";
		CString sqlstr_count = "";
		CString sqlstr_temp = "";
		CString s_bunker_no = "";
		CString mat_code = " ";
		CString s_bunker_no_original = "";
		CString bunker_no1 = "";
		CDecimal fl_wt = 0; //粉率
		
		CDecimal cd_stock_wt = 0;
		CDecimal cd_seq_no = 0;
		CDecimal nd_seq_no = 0;
		
		CModel tmmsm60("TMMSM60");
		CModel tmmsm60_O("TMMSM60");
		CModel tmmsm85("TMMSM85");
		CModel tmmsm85_O("TMMSM85");
		CModel tmmsm85_Z("TMMSM85");
		CModel tmmsm85_Z1("TMMSM85");
		CModel tmmsm89("TMMSM89");
		CModel tmmsm81("TMMSM81");
		CModel tmmsm81s("TMMSM81_S");
		CModel tmmsm83("TMMSM83");
		CModel tmmsm81V("TMMSM81V");
		CDbCommand cmd_inq(conn);
		CDbCommand cmd_inq1(conn);
		CDbCommand cmd_inq2(conn);
		CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		EIClass bcls_rec_tmmsm89_log;
		bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);

		//给原料L2-发送料仓物料信息
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMLCSND");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ACTION"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "ACTION");
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
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STATION_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STATION_NO");
		}
		bcls_rec->Tables["MMLCSND"].Rows.Add();
		
			tmmsm85["BUNKER_NO"] = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString();				// 高位料仓
			tmmsm85_O["BUNKER_NO"] = bcls_rec->Tables[0].Rows[0]["BUNKER_NO_ORIGINAL"].ToString();	// 低位料仓
			tmmsm83["SEQ_CODE"] = bcls_rec->Tables[0].Rows[0]["SEQ_CODE"].ToString();	// 序号
			tmmsm83.Query("SEQ_CODE");
			if (tmmsm83["FLAG1"].ToString() == "1")
			{				
				sprintf(s.msg, "已上料不能重复上料");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm83["FLAG1"] = "1";	
			tmmsm83["REC_REVISOR"] = s.userid;
			tmmsm83["REC_REVISE_TIME"] = datetime;
			
			tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			cd_stock_wt = bcls_rec->Tables[0].Rows[0]["REAL_WEIGHT"].ToDecimal();					// 皮带重量
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


			if (tmmsm60_O["STOCK_WT"].ToDecimal() < cd_stock_wt)
			{
				sprintf(s.msg, "输入的重量已超过低位料仓总量:" + tmmsm60_O["STOCK_WT"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm60["MAT_CODE"].ToString().Trim() != tmmsm60_O["MAT_CODE"].ToString().Trim())
			{
				sprintf(s.msg, "选择的物料代码和高位料仓物料代码不一致不允许上料");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm85["BUNKER_NO"].ToString() == tmmsm85_O["BUNKER_NO"].ToString())
			{
				sprintf(s.msg, "相同料仓不能上料");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			CDecimal count1 = 0;
			sql = " select count(1) from  TMMSM85 where 1=1 AND BUNKER_NO	= @BUNKER_NO and QUALITY_BATCH_NO <> ' ' ";
			cmd_inq1.SetCommandText(sql);
			cmd_inq1.Parameters.Set("BUNKER_NO", tmmsm85_O["BUNKER_NO"].ToString());
			cmd_inq1.ExecuteReader();
			if (cmd_inq1.Read())
			{
				count1 = cmd_inq.GetDecimal(1);
			}
			cmd_inq1.Close();

			if (count1 == 0 )
			{
				sprintf(s.msg, "为质检信息不能上料");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85     WHERE 1=1";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cd_seq_no = cmd_inq.GetDecimal(1) + 1;
			}
			cmd_inq.Close();

			if (tmmsm85_O["BUNKER_NO"].ToString() == "H01" || tmmsm85_O["BUNKER_NO"].ToString() == "H02")  //如果是石灰，则按石灰模式上料
			{
				s_bunker_no_original = tmmsm83["BELT"].ToString();
				// 查询粉率
				sqlstr = "  SELECT CODE FROM TWMSMZD02 WHERE REC_CREATE_TIME =(SELECT MAX(REC_CREATE_TIME) REC_CREATE_TIME FROM TWMSMZD02 t WHERE CODE_CLASS ='MMLC03') ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					fl_wt = cmd_inq.GetDecimal(1);
					fl_wt = fl_wt / 100;
				}
				else
				{
					fl_wt = 0.03;
				}
				cmd_inq.Close();  

				sql = "  SELECT LPAD(TO_CHAR(MMLC_DD.NEXTVAL),3 ,'0') FROM DUAL ";
				cmd_inq.SetCommandText(sql);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					SeqNo = cmd_inq.GetString(1).Trim();

				}
				cmd_inq.Close();
				weigh_no = datetime.Substring(2, 12) + SeqNo + "S";	

				
				// 更新85表数据
				sql = "  SELECT * FROM  TMMSM85 "
					" WHERE 1=1   AND BUNKER_NO	= @BUNKER_NO"
					" and QUALITY_BATCH_NO <> ' ' ORDER BY SEQ_NO ASC ";
				int  i = 0;
				cmd_inq1.SetCommandText(sql); 				
				cmd_inq1.Parameters.Set("BUNKER_NO", tmmsm85_O["BUNKER_NO"].ToString());
				cmd_inq1.ExecuteReader();
				while (cmd_inq1.Read())
				{
					cmd_inq1.Fetch(tmmsm85_Z);
					tmmsm85_Z1.CopyFrom(tmmsm85_Z);	

					//取采购订单信息  ：生效标记为1，开始时间比当前日期最大的值
					sqlstr = " select C_ORDERID"
						" from tmmsm81v"
						" where 1=1"
						" and C_STATE = '1'"
						" and MAT_CODE = @mat_code"
						" and DATE_START<=@datetime"
						" order by DATE_START desc"
						;
					cmd_inq2.SetCommandText(sqlstr);
					cmd_inq2.Parameters.Set("datetime", datetime);
					cmd_inq2.Parameters.Set("mat_code", tmmsm85_Z["MAT_CODE"].ToString());
					cmd_inq2.ExecuteReader();
					if (cmd_inq2.Read())
					{
						tmmsm85_Z1["C_ORDERID"] = cmd_inq2.GetString(1);
						tmmsm85_Z1["VOUCHER_ID"] = cmd_inq2.GetString(1);
					}
					cmd_inq2.Close();


					if (tmmsm85_Z["STOCK_WT"] <= cd_stock_wt)
					{ 
						cd_stock_wt = cd_stock_wt - tmmsm85_Z["STOCK_WT"];
						tmmsm85_Z.Delete("BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
						tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
						tmmsm85_Z1["BUNKER_NO"] = tmmsm60["BUNKER_NO"];
						tmmsm85_Z1["BUNKER_TYPE"] = tmmsm60["BUNKER_TYPE"];
						tmmsm85_Z1["BUNKER_NAME"] = tmmsm60["BUNKER_NAME"];
						

						tmmsm85_Z1["PLATE_NUMBER"] = s_bunker_no_original;
						tmmsm85_Z1["BUCKLE_WT"] = (tmmsm85_Z1["STOCK_WT"].ToDecimal() * fl_wt).Round(0);
						tmmsm85_Z1["STOCK_WT"] = tmmsm85_Z1["STOCK_WT"] - (tmmsm85_Z1["STOCK_WT"].ToDecimal() * fl_wt).Round(0);
						tmmsm85_Z1["WEIGH_NO"] = weigh_no;
						tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
						tmmsm85_Z1["TIME_INSTOCK"] = datetime;
						tmmsm85_Z1["REC_CREATOR"] = s.userid;
						tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
						tmmsm89.CopyFrom(tmmsm85_Z1);
						tmmsm89["EVENT_CODE"] = "LIMEIN";
						tmmsm89["EVENT_DESC"] = "石灰进厂";
						tmmsm89["EVENT_NAME"] = "自动上料";
						tmmsm89["REC_CREATOR"] = s.userid;
						tmmsm89["REC_CREATE_TIME"] = datetime;
						tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60_O["BUNKER_NO"];
						tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60_O["BUNKER_TYPE"];
						tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60_O["BUNKER_NAME"];
						bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
						bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);

						
						tmmsm85_Z1.Insert();
						tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
						tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
						tmmsm60["LASTACTDATE"] = datetime;
						tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
						tmmsm60.Update("LASTACTDATE,LOT_NO,QUALITY_BATCH_NO", "BUNKER_NO");
						tmmsm81V["C_ORDERID"] = tmmsm85_Z1["C_ORDERID"].ToString();
						tmmsm81V.Query("C_ORDERID");
						tmmsm81V["MAT_WT"] = tmmsm81V["MAT_WT"].ToDecimal() + tmmsm85_Z1["STOCK_WT"].ToDecimal();
						tmmsm81V.Update("MAT_WT", "C_ORDERID");
						if (cd_stock_wt == 0)
						{
							break;
						}
					}
					else
					{
						tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"] - cd_stock_wt;
						tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
						tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
						tmmsm85_Z1["STOCK_WT"] = cd_stock_wt;
						tmmsm85_Z1["BUNKER_NO"] = tmmsm60["BUNKER_NO"];
						tmmsm85_Z1["BUNKER_TYPE"] = tmmsm60["BUNKER_TYPE"];
						tmmsm85_Z1["BUNKER_NAME"] = tmmsm60["BUNKER_NAME"];
						tmmsm85_Z1["PLATE_NUMBER"] = s_bunker_no_original;
						tmmsm85_Z1["BUCKLE_WT"] = (tmmsm85_Z1["STOCK_WT"].ToDecimal() * fl_wt).Round(0);
						tmmsm85_Z1["STOCK_WT"] = tmmsm85_Z1["STOCK_WT"] - (tmmsm85_Z1["STOCK_WT"].ToDecimal() * fl_wt).Round(0);
						tmmsm85_Z1["WEIGH_NO"] = weigh_no;
						tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
						tmmsm85_Z1["TIME_INSTOCK"] = datetime;
						tmmsm85_Z1["REC_CREATOR"] = s.userid;
						tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
						tmmsm89.CopyFrom(tmmsm85_Z1);
						tmmsm89["EVENT_CODE"] = "LIMEIN";
						tmmsm89["EVENT_DESC"] = "石灰进厂";
						tmmsm89["EVENT_NAME"] = "自动上料";
						tmmsm89["REC_CREATOR"] = s.userid;
						tmmsm89["REC_CREATE_TIME"] = datetime;
						tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60_O["BUNKER_NO"];
						tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60_O["BUNKER_TYPE"];
						tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60_O["BUNKER_NAME"];
						tmmsm89["BUNKER_NAME_ORIGINAL"] = cd_stock_wt;
						bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
						bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
						tmmsm85_Z1.Insert();
						tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
						tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
						tmmsm60["LASTACTDATE"] = datetime;
						tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
						tmmsm60.Update("LASTACTDATE,LOT_NO,QUALITY_BATCH_NO", "BUNKER_NO");

						tmmsm81V["C_ORDERID"] = tmmsm85_Z1["VOUCHER_ID"].ToString();
						tmmsm81V.Query("C_ORDERID");
						tmmsm81V["MAT_WT"] = tmmsm81V["MAT_WT"].ToDecimal() + tmmsm85_Z1["STOCK_WT"].ToDecimal();
						tmmsm81V.Update("MAT_WT", "C_ORDERID");

						break;
					}

					
					i++;
				}
				cmd_inq1.Close();

				tmmsm83["WEIGH_NO"] = weigh_no;
				tmmsm83["FLAG1"] = "1";
				tmmsm83["REC_REVISOR"] = s.userid;
				tmmsm83["REC_REVISE_TIME"] = datetime;
				tmmsm83.Update("FLAG1,WEIGH_NO,REC_REVISOR,REC_REVISE_TIME", "SEQ_CODE");
			}
			else
			{

				CString SeqNo1 = "";
				CString serial_number = "";

				// 更新85表数据
				sql = "  SELECT * FROM  TMMSM85"
					" WHERE 1=1 AND BUNKER_NO= @BUNKER_NO"
					" and QUALITY_BATCH_NO <> ' ' AND STOCK_WT <> 0  "
					" ORDER BY SEQ_NO,TIME_INSTOCK ASC "
					;
				int  i = 0;
				cmd_inq1.SetCommandText(sql);
				cmd_inq1.Parameters.Set("BUNKER_NO", tmmsm85_O["BUNKER_NO"].ToString());
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
						cd_stock_wt = cd_stock_wt - tmmsm85_Z["STOCK_WT"];
						tmmsm85_Z.Delete("BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
						tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
						tmmsm85_Z1["BUNKER_NO"] = tmmsm60["BUNKER_NO"];
						tmmsm85_Z1["BUNKER_TYPE"] = tmmsm60_O["BUNKER_TYPE"];
						tmmsm85_Z1["BUNKER_NAME"] = tmmsm60_O["BUNKER_NAME"];
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
						tmmsm89["EVENT_NAME"] = "自动上料";
						tmmsm89["REC_CREATOR"] = s.userid;
						tmmsm89["REC_CREATE_TIME"] = datetime;
						tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60_O["BUNKER_NO"];
						tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60_O["BUNKER_TYPE"];
						tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60_O["BUNKER_NAME"];
						bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
						bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
						tmmsm85_Z1.Insert();
						tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
						tmmsm60["SERIAL_NUMBER"] = serial_number;
						tmmsm60["LASTACTDATE"] = datetime;
						tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
						tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
						tmmsm60.Update("SERIAL_NUMBER,LOT_NO,LASTACTDATE,QUALITY_BATCH_NO", "BUNKER_NO");
						tmmsm83.Update("FLAG1,REC_REVISOR,REC_REVISE_TIME", "SEQ_CODE");
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
						Log::Trace(" ", __FUNCTION__, "serial_number =[{0}]", serial_number);
						tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"] - cd_stock_wt;
						tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
						tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
						tmmsm85_Z1["STOCK_WT"] = cd_stock_wt;
						tmmsm85_Z1["BUNKER_NO"] = tmmsm60["BUNKER_NO"];
						tmmsm85_Z1["BUNKER_TYPE"] = tmmsm60["BUNKER_TYPE"];
						tmmsm85_Z1["BUNKER_NAME"] = tmmsm60["BUNKER_NAME"];
						//tmmsm85_Z1["BUNKER_NAME_ORIGINAL"] = tmmsm85_O["BUNKER_NAME"];
						tmmsm85_Z1["BUNKER_NO_ORIGINAL"] = tmmsm85_O["BUNKER_NO"];
						tmmsm85_Z1["TIME_1"] = datetime;
						tmmsm85_Z1["SERIAL_NUMBER"] = serial_number;
						tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
						tmmsm85_Z1["TIME_INSTOCK"] = datetime;
						tmmsm85_Z1["REC_CREATOR"] = s.userid;
						tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
						tmmsm89.CopyFrom(tmmsm85_Z1);
						Log::Trace(" ", __FUNCTION__, "3=[{0}]", 1);
						tmmsm89["EVENT_CODE"] = "MOVE";
						tmmsm89["EVENT_DESC"] = "移库";
						tmmsm89["EVENT_NAME"] = "自动上料";
						tmmsm89["REC_CREATOR"] = s.userid;
						tmmsm89["REC_CREATE_TIME"] = datetime;
						tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60_O["BUNKER_NO"];
						tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60_O["BUNKER_TYPE"];
						tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60_O["BUNKER_NAME"];
						tmmsm89["BUNKER_NAME_ORIGINAL"] = cd_stock_wt;
						bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
						bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
						tmmsm85_Z1.Insert();

						tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
						tmmsm60["SERIAL_NUMBER"] = serial_number;
						tmmsm60["LASTACTDATE"] = datetime;
						tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
						tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
						tmmsm60.Update("SERIAL_NUMBER,LOT_NO,LASTACTDATE,QUALITY_BATCH_NO", "BUNKER_NO");
						tmmsm83.Update("FLAG1,REC_REVISOR,REC_REVISE_TIME", "SEQ_CODE");
						break;
					}


					i++;
				}
				cmd_inq1.Close();
			}
			
			
			


				bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YB";
				bcls_rec->Tables["MMLCSND"].Rows[0]["ACTION"] = "I";
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

				if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) == "A")
				{
					//AOD
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y1";
					bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_AOD_BIN";
				}
				else if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) == "B")
				{
					//转炉
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y3";
					bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_BOF_BIN";
				}
				else if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) == "E" || tmmsm60["BUNKER_NO"].ToString().SubstringNE(0, 1) == "E")
				{
					//电炉
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y5";
					bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_EAF_BIN";
				}
				else if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) == "I")
				{
					//IF
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y8";
					bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_IF_BIN";
				}
				else if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) == "F")
				{
					//LF
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2Y9";
					bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_LF_BIN";
				}
				else if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) == "T") //待确认
				{
					//LTS
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YA";
					bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_LTS_BIN";
				}
				else if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) == "R")
				{
					//RH
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YF";
					bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_RH_BIN";
				}
				else if (tmmsm60["BUNKER_TYPE"].ToString().SubstringNE(0, 1) == "V")
				{
					//VOD
					bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YI";
					bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "INT_VOD_BIN";
				}
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


				if (tmmsm85_O["BUNKER_NO"].ToString() == "H01" || tmmsm85_O["BUNKER_NO"].ToString() == "H02")  //如果是石灰，则按石灰模式上料
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
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TABLE_NAME");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_KEY"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PRIMARY_KEY");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PRIMARY_DATA"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PRIMARY_DATA");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("VOUCHER_ID"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "VOUCHER_ID");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SHIP_NAME"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SHIP_NAME");
					}
					if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_NO"))
					{
						bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WEIGH_NO");
					}

					//SYSTEM_ID_MAT 物料来源系统   资源-C 铁区-B
					bcls_rec->Tables["MMLCSND"].Rows.Add();
					bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "I";	 
					
					
						bcls_rec->Tables["MMLCSND"].Rows[0]["TABLE_NAME"] = "TMMSM85";
						bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_KEY"] = "WEIGH_NO";
						bcls_rec->Tables["MMLCSND"].Rows[0]["PRIMARY_DATA"] = tmmsm85_Z1["WEIGH_NO"];
						bcls_rec->Tables["MMLCSND"].Rows[0]["VOUCHER_ID"] = tmmsm85_Z1["C_ORDERID"];
						bcls_rec->Tables["MMLCSND"].Rows[0]["SHIP_NAME"] = s_bunker_no_original + "#PD";

						bcls_rec->Tables["MMLCSND"].Rows[0]["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
						bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = tmmsm85_Z1["MAT_CODE"];


						if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SEQ_NO"))
						{
							bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SEQ_NO");
						}
						bcls_rec->Tables["MMLCSND"].Rows[0]["SEQ_NO"] = tmmsm85_Z1["SEQ_NO"];

						bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21B005";
						doFlag = f_mmsm_21b005_snd(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21b005_snd失败-------");
							throw CApplicationException(-1, s.msg, log.Location);
						}
				}

			if (bcls_rec_tmmsm89_log.Tables[0].Rows.get_Count() > 0)
			{
				doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			EIClass bcls_rec_updlc;
			bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
			bcls_rec_updlc.Tables[0].Rows.Add();
			bcls_rec_updlc.Tables[0].Rows[0]["BUNKER_NO"] = tmmsm85_O["BUNKER_NO"].ToString();
			bcls_rec_updlc.Tables[0].Rows.Add();
			bcls_rec_updlc.Tables[0].Rows[1]["BUNKER_NO"] = tmmsm85["BUNKER_NO"].ToString();
			doFlag = f_mmsm_updlc(&bcls_rec_updlc, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);

		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (const CApplicationException& ex)
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
