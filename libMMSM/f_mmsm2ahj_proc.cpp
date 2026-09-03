/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-07-04
Description:
B:高位料仓：根据料仓号，不看物料编码，根据先进先出原则取物料编码。如果减核完成不够，则物料编码取物料主数据的编码 ；库存为空时给二级发送电文
X:料槽料篮：根据料仓号一次性将料槽内的消耗全部扣除，以库存重量作为消耗，不看二级上传的量  ；如果料仓无数据则取二级的物料编码和量。置空后给二级发送电文，并将料槽置为未上传，未使用
M:手投料：以二级物料编码、重量、料仓号为准扣库存,根据机组代码来确定扣减哪个库存信息，其中LF和RH精炼双工位
W:丝线：以二级物料编码、重量、料仓号为准，扣库存,根据机组代码来确定扣减哪个库存信息	，其中LF和RH精炼双工位

addby20251010
进料加工钢种：进料加工高铬(AB070576)或者进口高铬(AT000324)
     AOD工位：先核减VMA01料仓的数据，再核减VMA02料仓数据，空仓按VMA02数据核减
	 合金熔化炉：先核减VMI01料仓的数据，VMI01料仓中不够再核减VMI02料仓数据，空仓按VMI02
非进料加工钢种：进料加工高铬(AB070576)或者进口高铬(AT000324)，
	AOD工位：去436-AOD/IF专用虚拟仓 中的VMMA02核减消耗，空仓按VMA02数据核减
	合金熔化炉，去436-AOD/IF专用虚拟仓 中的VM102核减消耗，空仓按VMI02数据核减
***********************************************************************/


/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/
int f_mmsm_updlc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm2ahj_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlstr2 = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_proc_div = "";
	CString v_pract_coll_mode = "";
	CString v_factory_div = "";
	CString s_stno = "";
	CString s_old_mat = "";
	CString s_old_BNO = "";
	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";
	CDecimal nd_seq_no = 0;
	CDecimal cd_seq_no = 0;

	CDecimal devo_wt = 0;

	int v_count = 0;
	int z_stock_wt = 0;
	int z_stock_wt_o = 0;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsm2a("TMMSM2A_YL");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm60_O("TMMSM60");

	CModel tmmsm85("TMMSM85");
	CModel tmmsm85_O("TMMSM85");
	CModel tmmsm85_Z("TMMSM85");
	CModel tmmsm85_Z1("TMMSM85");
	CModel tmmsm89("TMMSM89");
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString c_date = datetime.Substring(0, 8);
	CString SeqNo = "";


	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	bcls_rec_tmmsm89_log.Tables[0].Rows.Clear();

	EIClass bcls_rec_updlc;
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");

	try
	{

		int n = 0;
		int j = 0;
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm2a.Reset();
			tmmsm60_O.Reset();
			devo_wt = 0;
			tmmsm2a.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm2a["FACTORY_DIV"] = v_factory_div;
			tmmsm2a["REC_CREATE_TIME"] = datetime;
			tmmsm2a["REC_CREATOR"] = s.userid;
			Log::Trace("", __FUNCTION__, "HEAT_NO=[{0}],mat_code = [{1}],PROC_COUNT = [{2}],devo_wt = [{3}],DEV_CODE=[{4}]", tmmsm2a["HEAT_NO"].ToString(), tmmsm2a["MAT_CODE"].ToString(), tmmsm2a["PROC_COUNT"].ToString(), tmmsm2a["DEVO_WT"].ToDecimal(), tmmsm2a["DEV_CODE"].ToString());
			//tmmsm2a.Print();
			tmmsm2a.TrimOrBlank();
			devo_wt = tmmsm2a["DEVO_WT"].ToDecimal();

			if (tmmsm2a["CHARGE_TYPE"].ToString() == "X")	//料篮料槽仓扣除料仓的全部信息
			{
				sqlstr = " select * from tmmsm85 "
					" where 1=1"
					" and BUNKER_NO !=' '"
					" and BUNKER_NO = @BUNKER_NO "
					" and QUALITY_BATCH_NO <> ' ' "
					" ORDER BY SEQ_NO,TIME_INSTOCK ASC "
					;
				cmd_inq.Parameters.Set("BUNKER_NO", tmmsm2a["STK_NO"].ToString());
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					tmmsm85_Z.Reset();
					cmd_inq.Fetch(tmmsm85_Z);
					tmmsm2a["LOT_NO"] = tmmsm85_Z["LOT_NO"];
					tmmsm2a["WEIGH_NO"] = tmmsm85_Z["WEIGH_NO"];
					tmmsm2a["QUALITY_BATCH_NO"] = tmmsm85_Z["QUALITY_BATCH_NO"].ToString();
					tmmsm2a["MAT_CODE"] = tmmsm85_Z["MAT_CODE"];
					tmmsm50["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
					tmmsm50.Query("MAT_CODE");
					tmmsm2a["MAT_NAME"] = tmmsm50["MAT_NAME"].ToString();
					tmmsm2a["DEVO_WT"] = tmmsm85_Z["STOCK_WT"].ToDecimal();
					tmmsm2a["BUNKER_DEDUCT_WT"] = tmmsm85_Z["BUNKER_DEDUCT_WT"].ToDecimal();
					tmmsm2a["REMARK_2"] = "1";
					sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.ExecuteReader();
					if (cmd_sql.Read())
					{
						SeqNo = cmd_sql.GetString(1).Trim();
					}
					cmd_sql.Close();
					SeqNo = c_date + SeqNo;
					tmmsm2a["SEQ_NO_2A"] = SeqNo;
					tmmsm2a.TrimOrBlank();
					tmmsm2a.Insert();

					tmmsm89.Reset();
					tmmsm89.CopyFrom(tmmsm85_Z);
					tmmsm85_Z.Delete("BUNKER_NO,SEQ_NO,MAT_CODE,LOT_NO");

					tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
					tmmsm89["BUNKER_DEDUCT_WT"] = tmmsm2a["BUNKER_DEDUCT_WT"].ToDecimal();
					tmmsm89["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
					tmmsm89["HEAT_NO"] = tmmsm2a["HEAT_NO"];
					tmmsm89["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];
					tmmsm89["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"];
					tmmsm89["ID_2A"] = tmmsm2a["ID_2A"];
					tmmsm89["DEV_CODE"] = tmmsm2a["DEV_CODE"];
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm85_Z["BUNKER_NO"].ToString();
					tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
					tmmsm89["EVENT_CODE"] = "CHARGE";
					tmmsm89["EVENT_DESC"] = "一般加料";
					tmmsm89["EVENT_NAME"] = "电文核减库存";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);

					//判断减核信息
					bcls_rec_updlc.Tables[0].Rows.Add();
					bcls_rec_updlc.Tables[0].Rows[j]["BUNKER_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
					bcls_rec_updlc.Tables[0].Rows[j]["QUALITY_BATCH_NO"] = tmmsm85_Z["QUALITY_BATCH_NO"].ToString();
					bcls_rec_updlc.Tables[0].Rows[j]["MAT_CODE"] = tmmsm85_Z["MAT_CODE"].ToString();
					j++;

				}
				cmd_inq.Close();
				//判断该炉该料仓号是否已经扣减，如果没有扣减则将数据插入数据
				sqlstr = " select count(1)"
					" from TMMSM2A_YL"
					" where STK_NO =@stk_no"
					" and REMARK_2 = '1'"
					" and dev_code = @dev_code"
					" and l2_proc_no = @l2_proc_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("stk_no", tmmsm2a["STK_NO"].ToString());
				cmd_inq.Parameters.Set("l2_proc_no", tmmsm2a["L2_PROC_NO"].ToString());
				cmd_inq.Parameters.Set("dev_code", tmmsm2a["DEV_CODE"].ToString());
				v_count = cmd_inq.ExecuteScalar().ToInt32();
				if (v_count == 0)  //如果料仓没有被扣除过，则扣除二级的量
				{
					tmmsm2a["LOT_NO"] = " ";
					tmmsm2a["WEIGH_NO"] = " ";
					tmmsm2a["QUALITY_BATCH_NO"] = " ";
					//没有则取料仓最后一次的批次号
					sqlstr = " select QUALITY_BATCH_NO,LOT_NO from tmmsm60 "
						" where 1=1"
						" and mat_code = @mat_code"
						" and bunker_no = @bunker_no "
						;

					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
					cmd_sql.Parameters.Set("bunker_no", tmmsm2a["STK_NO"].ToString());
					cmd_sql.ExecuteReader();
					if (cmd_sql.Read())
					{
						tmmsm2a["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
						tmmsm2a["LOT_NO"] = cmd_sql.GetString(2);
					}
					cmd_sql.Close();
					tmmsm50["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
					tmmsm50.Query("MAT_CODE");
					tmmsm2a["MAT_NAME"] = tmmsm50["MAT_NAME"].ToString();

					if (tmmsm2a["QUALITY_BATCH_NO"].ToString().Trim() == "")
					{
						if (tmmsm50["QUALITY_FLAS"].ToString() == "1")
						{
							sqlstr = " select QUALITY_BATCH_NO,LOT_NO from tmmsm81 "
								" where 1=1"
								" and mat_rcv_time != ' '  and quality_batch_no != ' '"
								" and LOT_NO!=' '"
								" and mat_code = @mat_code"
								" order by MAT_RCV_TIME desc "
								;

						}
						else
						{
							sqlstr = " select QUALITY_BATCH_NO,LOT_NO from tmmsm81 "
								" where 1=1"
								" and mat_rcv_time != ' '  and quality_batch_no != ' '"
								" and mat_code = @mat_code"
								" order by MAT_RCV_TIME desc "
								;
						}
						cmd_sql.SetCommandText(sqlstr);
						cmd_sql.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
						cmd_sql.ExecuteReader();
						if (cmd_sql.Read())
						{
							tmmsm2a["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
							tmmsm2a["LOT_NO"] = cmd_sql.GetString(2);
						}
						cmd_sql.Close();
					}

					tmmsm2a["REMARK_2"] = "0";
					sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.ExecuteReader();
					if (cmd_sql.Read())
					{
						SeqNo = cmd_sql.GetString(1).Trim();
					}
					cmd_sql.Close();
					SeqNo = c_date + SeqNo;
					tmmsm2a["SEQ_NO_2A"] = SeqNo;
					tmmsm2a.TrimOrBlank();
					tmmsm2a.Insert();

					tmmsm89.Reset();
					tmmsm89.CopyFrom(tmmsm2a);
					tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm2a["STK_NO"];
					tmmsm89["HEAT_NO"] = tmmsm2a["HEAT_NO"];
					tmmsm89["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];
					tmmsm89["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"];
					tmmsm89["ID_2A"] = tmmsm2a["ID_2A"];
					tmmsm89["DEV_CODE"] = tmmsm2a["DEV_CODE"];
					tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
					tmmsm89["EVENT_CODE"] = "EXTRA";
					tmmsm89["WEIGH_NO"] = "UNKNOWN";
					tmmsm89["EVENT_DESC"] = "EXTRA加料";
					tmmsm89["EVENT_NAME"] = "电文核减库存";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);
				}

				cmd_inq.Close();


				//更新上传标记
				tmmsm60["BUNKER_NO"] = tmmsm2a["STK_NO"].ToString();
				tmmsm60["BACK_C3"] = "0";
				tmmsm60["BACK_C1"] = "0";
				tmmsm60["BACK_C2"] = "0";
				tmmsm60.Update("BACK_C3,BACK_C1,BACK_C2", "BUNKER_NO");
			}
			else
			{
				//20251010，物料编码是进料加工高铬(AB070576)或者进口高铬(AT000324)
				s_old_mat = tmmsm2a["MAT_CODE"].ToString();
				s_stno = tmmsm2a["ST_NO"].ToString();
				if ((tmmsm2a["DEV_CODE"].ToString().SubstringNE(0, 1) == "A" || tmmsm2a["DEV_CODE"].ToString().SubstringNE(0, 1) == "Z") && (s_old_mat == "AB070576" || s_old_mat == "AT000324"))
				{
					//判断是否为进料加工钢种
					sqlstr1 = "SELECT ST_NO FROM TPSSMSS WHERE EVENT_TIME = (SELECT MAX(EVENT_TIME) FROM TPSSMSS  A  WHERE 1 = 1 and DEV_CODE like @DEV_CODE||'%' and EVENT_ID = '3' AND TRIM(ST_NO) IS NOT NULL AND HEAT_NO =@heat_no) and DEV_CODE like @DEV_CODE||'%' and EVENT_ID = '3' AND TRIM(ST_NO) IS NOT NULL AND HEAT_NO =@heat_no";
					cmd_inq.SetCommandText(sqlstr1);
					cmd_inq.Parameters.Set("heat_no", tmmsm2a["L2_PROC_NO"].ToString());
					cmd_inq.Parameters.Set("DEV_CODE", tmmsm2a["DEV_CODE"].ToString().Substring(0, 1));
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						s_stno = cmd_inq.GetString(1).SubstringNE(0,6);
					}
					cmd_inq.Close();

					sqlstr1 = " select COUNT(1) AS COUNT_I from TQMTS0X T "
						"WHERE  OLD_SAP_ERP_MATNR!=' ' and ST_NO=@st_no"
						;
					cmd_inq.SetCommandText(sqlstr1);
					cmd_inq.Parameters.Set("st_no", s_stno);
					cmd_inq.ExecuteReader();
					int count0 = 0;
					if (cmd_inq.Read())
					{
						count0 = cmd_inq.GetInt16(1);
					}
					if (count0 == 1)
					{
						if (tmmsm2a["DEV_CODE"].ToString() == "A0" || tmmsm2a["DEV_CODE"].ToString() == "A1" || tmmsm2a["DEV_CODE"].ToString() == "A2")
						{
							//AOD使用VMA01,不够则使用VMA02
							tmmsm2a["STK_NO"] = "VMA01";
							sqlstr = " select * from tmmsm85 "
								" where 1=1 "
								" AND BUNKER_NO in ('VMA01','VMA02')"
								" ORDER BY BUNKER_NO,SEQ_NO,TIME_INSTOCK ASC "
								;
						}
						if (tmmsm2a["DEV_CODE"].ToString().Substring(0, 1) == "Z")
						{
							//合金熔化炉使用VMI01,不够则使用VMI02
							sqlstr = " select * from tmmsm85 "
								" where 1=1 "
								" AND BUNKER_NO in ('VMI01','VMI02')"
								" ORDER BY BUNKER_NO,SEQ_NO,TIME_INSTOCK ASC "
								;
							tmmsm2a["STK_NO"] = "VMI01";
						}
					}
					else
					{
						if (tmmsm2a["DEV_CODE"].ToString() == "A0" || tmmsm2a["DEV_CODE"].ToString() == "A1" || tmmsm2a["DEV_CODE"].ToString() == "A2")
						{
							//AOD使用VMA02
							tmmsm2a["STK_NO"] = "VMA02";
							sqlstr = " select * from tmmsm85 "
								" where 1=1 "
								" AND BUNKER_NO in ('VMA02')"
								" ORDER BY BUNKER_NO,SEQ_NO,TIME_INSTOCK ASC "
								;
						}
						if (tmmsm2a["DEV_CODE"].ToString().Substring(0, 1) == "Z")
						{
							//合金熔化炉使用VMI02
							tmmsm2a["STK_NO"] = "VMI02";
							sqlstr = " select * from tmmsm85 "
								" where 1=1 "
								" AND BUNKER_NO in ('VMI02')"
								" ORDER BY BUNKER_NO,SEQ_NO,TIME_INSTOCK ASC "
								;
						}
					}

					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					while (cmd_inq.Read())
					{
						tmmsm85_Z.Reset();
						cmd_inq.Fetch(tmmsm85_Z);
						tmmsm2a["STK_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
						tmmsm2a["LOT_NO"] = tmmsm85_Z["LOT_NO"].ToString();
						tmmsm2a["WEIGH_NO"] = tmmsm85_Z["WEIGH_NO"].ToString();
						tmmsm2a["QUALITY_BATCH_NO"] = tmmsm85_Z["QUALITY_BATCH_NO"].ToString();
						tmmsm2a["MAT_CODE"] = tmmsm85_Z["MAT_CODE"];
						tmmsm50["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
						tmmsm50.Query("MAT_CODE");
						tmmsm2a["MAT_NAME"] = tmmsm50["MAT_NAME"].ToString();

						if (tmmsm85_Z["STOCK_WT"].ToDecimal() <= devo_wt)
						{
							tmmsm2a["DEVO_WT"] = tmmsm85_Z["STOCK_WT"].ToDecimal();
							tmmsm2a["REMARK_2"] = "1";
							devo_wt = devo_wt - tmmsm85_Z["STOCK_WT"].ToDecimal();
							sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
							cmd_sql.SetCommandText(sqlstr);
							cmd_sql.ExecuteReader();
							if (cmd_sql.Read())
							{
								SeqNo = cmd_sql.GetString(1).Trim();
							}
							cmd_sql.Close();
							SeqNo = c_date + SeqNo;
							tmmsm2a["SEQ_NO_2A"] = SeqNo;
							tmmsm2a.TrimOrBlank();
							tmmsm2a.Insert();

							tmmsm89.Reset();
							tmmsm89.CopyFrom(tmmsm85_Z);
							tmmsm85_Z.Delete("BUNKER_NO,SEQ_NO,MAT_CODE,LOT_NO");
							tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
							tmmsm89["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
							tmmsm89["HEAT_NO"] = tmmsm2a["HEAT_NO"];
							tmmsm89["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];
							tmmsm89["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"];
							tmmsm89["ID_2A"] = tmmsm2a["ID_2A"];
							tmmsm89["DEV_CODE"] = tmmsm2a["DEV_CODE"];
							tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm85_Z["BUNKER_NO"].ToString();
							tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
							tmmsm89["EVENT_CODE"] = "CHARGE";
							tmmsm89["EVENT_DESC"] = "一般加料";
							tmmsm89["EVENT_NAME"] = "电文核减库存";
							tmmsm89["REC_CREATOR"] = s.userid;
							tmmsm89["REC_CREATE_TIME"] = datetime;

							tmmsm89["ST_NO"] = s_stno;
							tmmsm89["BUNKER_NO_ORIGINAL2"] = tmmsm85_Z["BUNKER_NO_ORIGINAL2"].ToString();
							tmmsm89["BUNKER_NO_L2"] = bcls_rec->Tables[0].Rows[i]["STK_NO"].ToString();

							bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
							bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);
							//判断减核信息
							bcls_rec_updlc.Tables[0].Rows.Add();
							bcls_rec_updlc.Tables[0].Rows[j]["BUNKER_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
							bcls_rec_updlc.Tables[0].Rows[j]["QUALITY_BATCH_NO"] = tmmsm85_Z["QUALITY_BATCH_NO"].ToString();
							bcls_rec_updlc.Tables[0].Rows[j]["MAT_CODE"] = tmmsm85_Z["MAT_CODE"].ToString();
							j++;
							if (devo_wt == 0)
							{
								break;
							}
						}
						else
						{
							tmmsm2a["DEVO_WT"] = devo_wt;
							tmmsm2a["REMARK_2"] = "1";
							tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"].ToDecimal() - tmmsm2a["DEVO_WT"];
							devo_wt = 0;
							tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,SEQ_NO,LOT_NO");
							sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
							cmd_sql.SetCommandText(sqlstr);
							cmd_sql.ExecuteReader();
							if (cmd_sql.Read())
							{
								SeqNo = cmd_sql.GetString(1).Trim();
							}
							cmd_sql.Close();
							SeqNo = c_date + SeqNo;
							tmmsm2a["SEQ_NO_2A"] = SeqNo;
							tmmsm2a.TrimOrBlank();
							tmmsm2a.Insert();

							tmmsm89.Reset();
							tmmsm89.CopyFrom(tmmsm85_Z);
							tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
							tmmsm89["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
							tmmsm89["HEAT_NO"] = tmmsm2a["HEAT_NO"];
							tmmsm89["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];
							tmmsm89["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"];
							tmmsm89["ID_2A"] = tmmsm2a["ID_2A"];
							tmmsm89["DEV_CODE"] = tmmsm2a["DEV_CODE"];
							tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm85_Z["BUNKER_NO"].ToString();
							tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
							tmmsm89["EVENT_CODE"] = "CHARGE";
							tmmsm89["EVENT_DESC"] = "一般加料";
							tmmsm89["EVENT_NAME"] = "电文核减库存";
							tmmsm89["REC_CREATOR"] = s.userid;
							tmmsm89["REC_CREATE_TIME"] = datetime;

							tmmsm89["ST_NO"] = s_stno;
							tmmsm89["BUNKER_NO_ORIGINAL2"] = tmmsm85_Z["BUNKER_NO_ORIGINAL2"].ToString();
							tmmsm89["BUNKER_NO_L2"] = bcls_rec->Tables[0].Rows[i]["STK_NO"].ToString();
							bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
							bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);

							bcls_rec_updlc.Tables[0].Rows.Add();
							bcls_rec_updlc.Tables[0].Rows[j]["BUNKER_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
							j++;
							break;
						}

					}
					cmd_inq.Close();

					if (devo_wt != 0)
					{
						tmmsm2a["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString();

						if (tmmsm2a["DEV_CODE"].ToString() == "A0" || tmmsm2a["DEV_CODE"].ToString() == "A1" || tmmsm2a["DEV_CODE"].ToString() == "A2")
						{
							//AOD使用VMA02
							tmmsm2a["STK_NO"] = "VMA02";
							// 20260105Add85表无库存，从tmmsm60表取当前料仓对应的物料编码
							sqlstr = "SELECT MAT_CODE FROM tmmsm60 WHERE BUNKER_NO = @bunker_no";
							cmd_sql.SetCommandText(sqlstr);
							cmd_sql.Parameters.Set("bunker_no", tmmsm2a["STK_NO"].ToString());
							cmd_sql.ExecuteReader();
							if (cmd_sql.Read())
							{
								tmmsm2a["MAT_CODE"] = cmd_sql.GetString(1);
							}
							cmd_sql.Close();
						}
						if (tmmsm2a["DEV_CODE"].ToString().Substring(0, 1) == "Z")
						{
							//合金熔化炉使用VMI02
							tmmsm2a["STK_NO"] = "VMI02";
							// 20260105Add85表无库存，从tmmsm60表取当前料仓对应的物料编码
							sqlstr = "SELECT MAT_CODE FROM tmmsm60 WHERE BUNKER_NO = @bunker_no";
							cmd_sql.SetCommandText(sqlstr);
							cmd_sql.Parameters.Set("bunker_no", tmmsm2a["STK_NO"].ToString());
							cmd_sql.ExecuteReader();
							if (cmd_sql.Read())
							{
								tmmsm2a["MAT_CODE"] = cmd_sql.GetString(1);
							}
							cmd_sql.Close();
						}
						

						tmmsm50["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
						tmmsm50.Query("MAT_CODE");
						tmmsm2a["MAT_NAME"] = tmmsm50["MAT_NAME"].ToString();
						tmmsm2a["LOT_NO"] = " ";
						tmmsm2a["WEIGH_NO"] = " ";
						tmmsm2a["QUALITY_BATCH_NO"] = " ";

						sqlstr = " select QUALITY_BATCH_NO,LOT_NO from tmmsm60 "
							" where 1=1"
							" and mat_code = @mat_code"
							" and bunker_no = @bunker_no "
							;
						cmd_sql.SetCommandText(sqlstr);
						cmd_sql.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
						cmd_sql.Parameters.Set("bunker_no", tmmsm2a["STK_NO"].ToString());
						cmd_sql.ExecuteReader();
						if (cmd_sql.Read())
						{
							tmmsm2a["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
							tmmsm2a["LOT_NO"] = cmd_sql.GetString(2);
						}
						cmd_sql.Close();

						//如果质检批号还是空的，则取最新收货的批次号和质检批号
						if (tmmsm50["QUALITY_FLAS"].ToString() == "1"&&tmmsm2a["LOT_NO"].ToString().Trim() == "")
						{
							sqlstr = " select QUALITY_BATCH_NO,LOT_NO from tmmsm81 "
								" where 1=1"
								" and mat_rcv_time != ' '  and quality_batch_no != ' '"
								" and LOT_NO!=' '"
								" and mat_code = @mat_code"
								" order by MAT_RCV_TIME desc "
								;
							cmd_sql.SetCommandText(sqlstr);
							cmd_sql.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
							cmd_sql.ExecuteReader();
							if (cmd_sql.Read())
							{
								tmmsm2a["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
								tmmsm2a["LOT_NO"] = cmd_sql.GetString(2);
							}
							cmd_sql.Close();
						}
						else if (tmmsm2a["QUALITY_BATCH_NO"].ToString().Trim() == "")
						{
							sqlstr = " select QUALITY_BATCH_NO,LOT_NO from tmmsm81 "
								" where 1=1"
								" and mat_rcv_time != ' '  and quality_batch_no != ' '"
								" and mat_code = @mat_code"
								" order by MAT_RCV_TIME desc "
								;
							cmd_sql.SetCommandText(sqlstr);
							cmd_sql.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
							cmd_sql.ExecuteReader();
							if (cmd_sql.Read())
							{
								tmmsm2a["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
								tmmsm2a["LOT_NO"] = cmd_sql.GetString(2);

							}
							cmd_sql.Close();
						}
						tmmsm2a["DEVO_WT"] = devo_wt;
						devo_wt = 0;
						tmmsm2a["REMARK_2"] = "0";
						sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
						cmd_sql.SetCommandText(sqlstr);
						cmd_sql.ExecuteReader();
						if (cmd_sql.Read())
						{
							SeqNo = cmd_sql.GetString(1).Trim();
						}
						cmd_sql.Close();
						SeqNo = c_date + SeqNo;
						tmmsm2a["SEQ_NO_2A"] = SeqNo;
						tmmsm2a.TrimOrBlank();
						tmmsm2a.Insert();

						tmmsm89.Reset();
						tmmsm89.CopyFrom(tmmsm2a);
						tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
						tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm2a["STK_NO"].ToString();
						tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
						tmmsm89["EVENT_CODE"] = "EXTRA";
						tmmsm89["WEIGH_NO"] = "UNKNOWN";
						tmmsm89["EVENT_DESC"] = "EXTRA加料";
						tmmsm89["EVENT_NAME"] = "电文核减库存";
						tmmsm89["REC_CREATOR"] = s.userid;
						tmmsm89["REC_CREATE_TIME"] = datetime;

						tmmsm89["ST_NO"] = s_stno;
						tmmsm89["BUNKER_NO_L2"] = bcls_rec->Tables[0].Rows[i]["STK_NO"].ToString();
						bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
						bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);

						bcls_rec_updlc.Tables[0].Rows.Add();
						bcls_rec_updlc.Tables[0].Rows[j]["BUNKER_NO"] = tmmsm2a["STK_NO"].ToString();
						j++;
					}

				}

				//根据配置取物料编码					
				else
				{
					s_old_BNO = tmmsm2a["STK_NO"].ToString();
					Log::Trace("", __FUNCTION__, "CHARGE_TYPE=[{0}],MAT_CODE = [{1}]", tmmsm2a["CHARGE_TYPE"].ToString(), tmmsm2a["MAT_CODE"].ToString());
					//先如果是废钢先执行废钢虚拟料仓
					if (tmmsm2a["CHARGE_TYPE"].ToString() == "M" && (tmmsm2a["MAT_CODE"].ToString().SubstringNE(0, 3) == "F01" || tmmsm2a["MAT_CODE"].ToString().SubstringNE(0, 3) == "F02" || tmmsm2a["MAT_CODE"].ToString().SubstringNE(0, 3) == "F03" || tmmsm2a["MAT_CODE"].ToString().SubstringNE(0, 3) == "F04" || tmmsm2a["MAT_CODE"].ToString().SubstringNE(0, 3) == "F05" || tmmsm2a["MAT_CODE"].ToString().SubstringNE(0, 3) == "F06"))
					{
						
						//使用BMBL
						sqlstr = " select * from tmmsm85 "
							" where 1=1 "
							" AND BUNKER_NO in ('BMBL')"
							" ORDER BY BUNKER_NO,SEQ_NO,TIME_INSTOCK ASC "
							;
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.ExecuteReader();
						while (cmd_inq.Read())
						{
							tmmsm85_Z.Reset();
							cmd_inq.Fetch(tmmsm85_Z);
							tmmsm2a["STK_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
							tmmsm2a["LOT_NO"] = tmmsm85_Z["LOT_NO"].ToString();
							tmmsm2a["WEIGH_NO"] = tmmsm85_Z["WEIGH_NO"].ToString();
							tmmsm2a["QUALITY_BATCH_NO"] = tmmsm85_Z["QUALITY_BATCH_NO"].ToString();
							tmmsm2a["MAT_CODE"] = tmmsm85_Z["MAT_CODE"];
							tmmsm50["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
							tmmsm50.Query("MAT_CODE");
							tmmsm2a["MAT_NAME"] = tmmsm50["MAT_NAME"].ToString();

							if (tmmsm85_Z["STOCK_WT"].ToDecimal() <= devo_wt)
							{

								tmmsm2a["DEVO_WT"] = tmmsm85_Z["STOCK_WT"].ToDecimal();
								tmmsm2a["REMARK_2"] = "1";
								devo_wt = devo_wt - tmmsm85_Z["STOCK_WT"].ToDecimal();
								sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
								cmd_sql.SetCommandText(sqlstr);
								cmd_sql.ExecuteReader();
								if (cmd_sql.Read())
								{
									SeqNo = cmd_sql.GetString(1).Trim();
								}
								cmd_sql.Close();
								SeqNo = c_date + SeqNo;
								tmmsm2a["SEQ_NO_2A"] = SeqNo;
								tmmsm2a.TrimOrBlank();
								tmmsm2a.Insert();

								tmmsm89.Reset();
								tmmsm89.CopyFrom(tmmsm85_Z);
								tmmsm85_Z.Delete("BUNKER_NO,SEQ_NO,MAT_CODE,LOT_NO");
								tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
								tmmsm89["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
								tmmsm89["HEAT_NO"] = tmmsm2a["HEAT_NO"];
								tmmsm89["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];
								tmmsm89["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"];
								tmmsm89["ID_2A"] = tmmsm2a["ID_2A"];
								tmmsm89["DEV_CODE"] = tmmsm2a["DEV_CODE"];
								tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm85_Z["BUNKER_NO"].ToString();
								tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
								tmmsm89["EVENT_CODE"] = "CHARGE";
								tmmsm89["EVENT_DESC"] = "一般加料";
								tmmsm89["EVENT_NAME"] = "电文核减库存";
								tmmsm89["REC_CREATOR"] = s.userid;
								tmmsm89["REC_CREATE_TIME"] = datetime;

								bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
								bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);
								//判断减核信息
								bcls_rec_updlc.Tables[0].Rows.Add();
								bcls_rec_updlc.Tables[0].Rows[j]["BUNKER_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
								bcls_rec_updlc.Tables[0].Rows[j]["QUALITY_BATCH_NO"] = tmmsm85_Z["QUALITY_BATCH_NO"].ToString();
								bcls_rec_updlc.Tables[0].Rows[j]["MAT_CODE"] = tmmsm85_Z["MAT_CODE"].ToString();
								j++;
								if (devo_wt == 0)
								{
									break;
								}

								
							}
							else
							{
								
								tmmsm2a["DEVO_WT"] = devo_wt;
								tmmsm2a["REMARK_2"] = "1";
								tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"].ToDecimal() - tmmsm2a["DEVO_WT"];
								devo_wt = 0;
								tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,SEQ_NO,LOT_NO");
								sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
								cmd_sql.SetCommandText(sqlstr);
								cmd_sql.ExecuteReader();
								if (cmd_sql.Read())
								{
									SeqNo = cmd_sql.GetString(1).Trim();
								}
								cmd_sql.Close();
								SeqNo = c_date + SeqNo;
								tmmsm2a["SEQ_NO_2A"] = SeqNo;
								tmmsm2a.TrimOrBlank();
								tmmsm2a.Insert();

								tmmsm89.Reset();
								tmmsm89.CopyFrom(tmmsm85_Z);
								tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
								tmmsm89["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
								tmmsm89["HEAT_NO"] = tmmsm2a["HEAT_NO"];
								tmmsm89["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];
								tmmsm89["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"];
								tmmsm89["ID_2A"] = tmmsm2a["ID_2A"];
								tmmsm89["DEV_CODE"] = tmmsm2a["DEV_CODE"];
								tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm85_Z["BUNKER_NO"].ToString();
								tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
								tmmsm89["EVENT_CODE"] = "CHARGE";
								tmmsm89["EVENT_DESC"] = "一般加料";
								tmmsm89["EVENT_NAME"] = "电文核减库存";
								tmmsm89["REC_CREATOR"] = s.userid;
								tmmsm89["REC_CREATE_TIME"] = datetime;

								bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
								bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);

								bcls_rec_updlc.Tables[0].Rows.Add();
								bcls_rec_updlc.Tables[0].Rows[j]["BUNKER_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
								j++;
								break;
							}

						}
						cmd_inq.Close();
					}

					
					if (devo_wt != 0)
					{
						
						tmmsm2a["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString();
						tmmsm2a["STK_NO"] = bcls_rec->Tables[0].Rows[i]["STK_NO"].ToString();

					
						sqlstr = " select * from tmmsm85 "
							" where 1=1 "
							" AND BUNKER_NO != ' ' "
							;
						if (tmmsm2a["CHARGE_TYPE"].ToString() == "M" || tmmsm2a["CHARGE_TYPE"].ToString() == "W")	  //手头料或是丝线					
						{
							sqlstr = sqlstr + " AND MAT_CODE = @mat_code and BUNKER_NO in ('DM','BM','EM','AM','RM','FM','BW','FW')";

							if (tmmsm2a["DEV_CODE"].ToString() == "F1" || tmmsm2a["DEV_CODE"].ToString() == "F2")   //精炼为双工位的
							{
								sqlstr = sqlstr + " AND STATION_NO = 'F0' ";
							}
							else if (tmmsm2a["DEV_CODE"].ToString() == "F3" || tmmsm2a["DEV_CODE"].ToString() == "F4")
							{
								sqlstr = sqlstr + " AND STATION_NO = 'F1'";
							}
							else if (tmmsm2a["DEV_CODE"].ToString() == "F5" || tmmsm2a["DEV_CODE"].ToString() == "F6")
							{
								sqlstr = sqlstr + " AND STATION_NO = 'F2'";
							}
							else if (tmmsm2a["DEV_CODE"].ToString() == "R1" || tmmsm2a["DEV_CODE"].ToString() == "R2")
							{
								sqlstr = sqlstr + " AND STATION_NO = 'R1'";
							}
							else if (tmmsm2a["DEV_CODE"].ToString() == "R3" || tmmsm2a["DEV_CODE"].ToString() == "R4")
							{
								sqlstr = sqlstr + " AND STATION_NO = 'R2'";
							}
							else
							{
								sqlstr = sqlstr + " AND STATION_NO=@station_no";
							}

						}
						else
						{
							sqlstr = sqlstr + " AND BUNKER_NO = @bunker_no";
						}
						sqlstr = sqlstr + " and QUALITY_BATCH_NO <> ' ' "
							" ORDER BY SEQ_NO,TIME_INSTOCK ASC "
							;
						if (tmmsm2a["CHARGE_TYPE"].ToString() == "M" || tmmsm2a["CHARGE_TYPE"].ToString() == "W")
						{
							cmd_inq.Parameters.Set("station_no", tmmsm2a["DEV_CODE"].ToString());  //为机组号
							cmd_inq.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
						}
						else
						{
							cmd_inq.Parameters.Set("bunker_no", tmmsm2a["STK_NO"].ToString());
						}

						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.ExecuteReader();
						while (cmd_inq.Read())
						{
							tmmsm85_Z.Reset();
							cmd_inq.Fetch(tmmsm85_Z);
							if (tmmsm2a["CHARGE_TYPE"].ToString() == "M" && (tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F01" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F02" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F03" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F04" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F05" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F06"))
							{
								tmmsm2a["STK_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
							}
							//判断重量是否够扣
							if (tmmsm2a["CHARGE_TYPE"].ToString() == "M" || tmmsm2a["CHARGE_TYPE"].ToString() == "W")
							{
								tmmsm2a["STK_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
							}

							tmmsm2a["LOT_NO"] = tmmsm85_Z["LOT_NO"].ToString();
							tmmsm2a["WEIGH_NO"] = tmmsm85_Z["WEIGH_NO"].ToString();
							tmmsm2a["QUALITY_BATCH_NO"] = tmmsm85_Z["QUALITY_BATCH_NO"].ToString();
							tmmsm2a["MAT_CODE"] = tmmsm85_Z["MAT_CODE"];
							tmmsm50["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
							tmmsm50.Query("MAT_CODE");

							tmmsm2a["MAT_NAME"] = tmmsm50["MAT_NAME"].ToString();

							if (tmmsm85_Z["STOCK_WT"].ToDecimal() <= devo_wt)
							{

								tmmsm2a["DEVO_WT"] = tmmsm85_Z["STOCK_WT"].ToDecimal();
								tmmsm2a["REMARK_2"] = "1";
								devo_wt = devo_wt - tmmsm85_Z["STOCK_WT"].ToDecimal();
								sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
								cmd_sql.SetCommandText(sqlstr);
								cmd_sql.ExecuteReader();
								if (cmd_sql.Read())
								{
									SeqNo = cmd_sql.GetString(1).Trim();
								}
								cmd_sql.Close();
								SeqNo = c_date + SeqNo;
								tmmsm2a["SEQ_NO_2A"] = SeqNo;
								tmmsm2a.TrimOrBlank();
								tmmsm2a.Insert();

								tmmsm89.Reset();
								tmmsm89.CopyFrom(tmmsm85_Z);
								tmmsm85_Z.Delete("BUNKER_NO,SEQ_NO,MAT_CODE,LOT_NO");
								tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
								tmmsm89["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
								tmmsm89["HEAT_NO"] = tmmsm2a["HEAT_NO"];
								tmmsm89["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];
								tmmsm89["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"];
								tmmsm89["ID_2A"] = tmmsm2a["ID_2A"];
								tmmsm89["DEV_CODE"] = tmmsm2a["DEV_CODE"];
								tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm85_Z["BUNKER_NO"].ToString();
								tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
								tmmsm89["EVENT_CODE"] = "CHARGE";
								tmmsm89["EVENT_DESC"] = "一般加料";
								tmmsm89["EVENT_NAME"] = "电文核减库存";
								tmmsm89["REC_CREATOR"] = s.userid;
								tmmsm89["REC_CREATE_TIME"] = datetime;

								bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
								bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);
								//判断减核信息
								bcls_rec_updlc.Tables[0].Rows.Add();
								bcls_rec_updlc.Tables[0].Rows[j]["BUNKER_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
								bcls_rec_updlc.Tables[0].Rows[j]["QUALITY_BATCH_NO"] = tmmsm85_Z["QUALITY_BATCH_NO"].ToString();
								bcls_rec_updlc.Tables[0].Rows[j]["MAT_CODE"] = tmmsm85_Z["MAT_CODE"].ToString();
								j++;
								if (devo_wt == 0)
								{
									break;
								}
							}
							else
							{
								tmmsm2a["DEVO_WT"] = devo_wt;
								tmmsm2a["REMARK_2"] = "1";
								tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"].ToDecimal() - tmmsm2a["DEVO_WT"];
								devo_wt = 0;
								tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,SEQ_NO,LOT_NO");
								sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
								cmd_sql.SetCommandText(sqlstr);
								cmd_sql.ExecuteReader();
								if (cmd_sql.Read())
								{
									SeqNo = cmd_sql.GetString(1).Trim();
								}
								cmd_sql.Close();
								SeqNo = c_date + SeqNo;
								tmmsm2a["SEQ_NO_2A"] = SeqNo;
								tmmsm2a.TrimOrBlank();
								tmmsm2a.Insert();

								tmmsm89.Reset();
								tmmsm89.CopyFrom(tmmsm85_Z);
								tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
								tmmsm89["PROC_COUNT"] = tmmsm2a["PROC_COUNT"];
								tmmsm89["HEAT_NO"] = tmmsm2a["HEAT_NO"];
								tmmsm89["L2_PROC_NO"] = tmmsm2a["L2_PROC_NO"];
								tmmsm89["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"];
								tmmsm89["ID_2A"] = tmmsm2a["ID_2A"];
								tmmsm89["DEV_CODE"] = tmmsm2a["DEV_CODE"];
								tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm85_Z["BUNKER_NO"].ToString();
								tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
								tmmsm89["EVENT_CODE"] = "CHARGE";
								tmmsm89["EVENT_DESC"] = "一般加料";
								tmmsm89["EVENT_NAME"] = "电文核减库存";
								tmmsm89["REC_CREATOR"] = s.userid;
								tmmsm89["REC_CREATE_TIME"] = datetime;

								bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
								bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);

								bcls_rec_updlc.Tables[0].Rows.Add();
								bcls_rec_updlc.Tables[0].Rows[j]["BUNKER_NO"] = tmmsm85_Z["BUNKER_NO"].ToString();
								j++;
								break;
							}

						}
						cmd_inq.Close();
					}

					if (devo_wt != 0)
					{
						tmmsm2a["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString();

						tmmsm50["MAT_CODE"] = tmmsm2a["MAT_CODE"].ToString();
						tmmsm50.Query("MAT_CODE");
						tmmsm2a["MAT_NAME"] = tmmsm50["MAT_NAME"].ToString();
						tmmsm2a["LOT_NO"] = " ";
						tmmsm2a["WEIGH_NO"] = " ";
						tmmsm2a["QUALITY_BATCH_NO"] = " ";
						//20251106
						if (tmmsm2a["CHARGE_TYPE"].ToString() == "M" && (tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F01" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F02" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F03" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F04" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F05" || tmmsm2a["MAT_CODE"].ToString().Substring(0, 3) == "F06"))
						{
							tmmsm2a["STK_NO"] = s_old_BNO;
						}

						//没有则取料仓最后一次的批次号
						if ((tmmsm2a["CHARGE_TYPE"].ToString() == "M" || tmmsm2a["CHARGE_TYPE"].ToString() == "W") && tmmsm2a["STK_NO"].ToString().Trim() == "")
						{
							tmmsm2a["STK_NO"] = tmmsm2a["CHARGE_TYPE"].ToString() + tmmsm2a["DEV_CODE"].ToString().SubstringNE(0, 1);
						}
						sqlstr = " select QUALITY_BATCH_NO,LOT_NO from tmmsm60 "
							" where 1=1"
							" and mat_code = @mat_code"
							" and bunker_no = @bunker_no "
							;
						cmd_sql.SetCommandText(sqlstr);
						cmd_sql.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
						cmd_sql.Parameters.Set("bunker_no", tmmsm2a["STK_NO"].ToString());
						cmd_sql.ExecuteReader();
						if (cmd_sql.Read())
						{
							tmmsm2a["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
							tmmsm2a["LOT_NO"] = cmd_sql.GetString(2);
						}
						cmd_sql.Close();

						//如果质检批号还是空的，则取最新收货的批次号和质检批号
						if (tmmsm50["QUALITY_FLAS"].ToString() == "1"&&tmmsm2a["LOT_NO"].ToString().Trim() == "")
						{
							sqlstr = " select QUALITY_BATCH_NO,LOT_NO from tmmsm81 "
								" where 1=1"
								" and mat_rcv_time != ' '  and quality_batch_no != ' '"
								" and LOT_NO!=' '"
								" and mat_code = @mat_code"
								" order by MAT_RCV_TIME desc "
								;
							cmd_sql.SetCommandText(sqlstr);
							cmd_sql.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
							cmd_sql.ExecuteReader();
							if (cmd_sql.Read())
							{
								tmmsm2a["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
								tmmsm2a["LOT_NO"] = cmd_sql.GetString(2);

							}
							cmd_sql.Close();
						}
						else if (tmmsm2a["QUALITY_BATCH_NO"].ToString().Trim() == "")
						{
							sqlstr = " select QUALITY_BATCH_NO,LOT_NO from tmmsm81 "
								" where 1=1"
								" and mat_rcv_time != ' '  and quality_batch_no != ' '"
								" and mat_code = @mat_code"
								" order by MAT_RCV_TIME desc "
								;
							cmd_sql.SetCommandText(sqlstr);
							cmd_sql.Parameters.Set("mat_code", tmmsm2a["MAT_CODE"].ToString());
							cmd_sql.ExecuteReader();
							if (cmd_sql.Read())
							{
								tmmsm2a["QUALITY_BATCH_NO"] = cmd_sql.GetString(1);
								tmmsm2a["LOT_NO"] = cmd_sql.GetString(2);
							}
							cmd_sql.Close();
						}
						tmmsm2a["DEVO_WT"] = devo_wt;
						devo_wt = 0;
						tmmsm2a["REMARK_2"] = "0";
						sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_2AYL.NEXTVAL),5 ,'0') FROM DUAL ";
						cmd_sql.SetCommandText(sqlstr);
						cmd_sql.ExecuteReader();
						if (cmd_sql.Read())
						{
							SeqNo = cmd_sql.GetString(1).Trim();
						}
						cmd_sql.Close();
						SeqNo = c_date + SeqNo;
						tmmsm2a["SEQ_NO_2A"] = SeqNo;
						tmmsm2a.TrimOrBlank();
						tmmsm2a.Insert();

						tmmsm89.Reset();
						tmmsm89.CopyFrom(tmmsm2a);
						tmmsm89["STOCK_WT"] = tmmsm2a["DEVO_WT"];
						tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm2a["STK_NO"].ToString();
						tmmsm89["BUNKER_NO"] = tmmsm2a["DEV_CODE"].ToString();
						tmmsm89["EVENT_CODE"] = "EXTRA";
						tmmsm89["WEIGH_NO"] = "UNKNOWN";
						tmmsm89["EVENT_DESC"] = "EXTRA加料";
						tmmsm89["EVENT_NAME"] = "电文核减库存";
						tmmsm89["REC_CREATOR"] = s.userid;
						tmmsm89["REC_CREATE_TIME"] = datetime;

						bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
						bcls_rec_tmmsm89_log.Tables[0].Rows[n++].Merge(tmmsm89);

						bcls_rec_updlc.Tables[0].Rows.Add();
						bcls_rec_updlc.Tables[0].Rows[j]["BUNKER_NO"] = tmmsm2a["STK_NO"].ToString();
						j++;
					}
				}
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

		if (bcls_rec_updlc.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm_updlc(&bcls_rec_updlc, bcls_ret, conn);
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
