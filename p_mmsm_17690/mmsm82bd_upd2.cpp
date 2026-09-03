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
int f_mmsm_updlc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm82bd_upd2)

int f_mmsm82bd_upd2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString s_bunker_no1 = "";
	CString s_bunker_no_original = "";
	CString bunker_no1 = "";
	CString mat_code = "";
	CString upload_301 = "";
	int		TotalRecordCount = 0;
	int i_idx = 0;
	int n_idx = 0;
	int d_idx = 0;
	int n_rows = 0;
	CDecimal n_count = 1;
	CDecimal cd_stock_wt = 0;
	CDecimal cd_seq_no = 0;
	CDecimal nd_seq_no = 0;
	//系统的分页类信息。
	CPageInfo pageInfo;
	CModel tmmsm60("TMMSM60");
	CModel tmmsm60_O("TMMSM60");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm85_j("TMMSM85");

	CModel tmmsm85_O("TMMSM85");
	CModel tmmsm85_Z("TMMSM85");
	CModel tmmsm89("TMMSM89");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_inq2(conn);
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);

	EIClass bcls_rec_updlc;
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");
	try
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
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WORK_SEQ_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WORK_SEQ_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("FACTORY_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "FACTORY_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DST_STOCK_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DST_STOCK_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WORK_DATE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WORK_DATE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BASKET_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BASKET_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CNAME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CNAME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SRC_STOCK_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SRC_STOCK_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SRC_STOCK_PLACE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SRC_STOCK_PLACE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUY_ORDER_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUY_ORDER_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("NET_WGT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "NET_WGT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_TIME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WEIGH_TIME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUNKER_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "LOT_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("QUALITY_BATCH_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STOCK_WT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STOCK_WT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STATION_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STATION_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("ACTION"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "ACTION");
		}

		n_rows = bcls_rec->Tables[0].Rows.get_Count();
		CString SeqNo1 = "";
		CString serial_number = "";

		if (n_rows>0)
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
		}

		s_bunker_no1 = bcls_rec->Tables[1].Rows[0]["BUNKER_NO1"].ToString();   //退废 1、删除

		for (int i = 0; i < n_rows; i++)
		{
			tmmsm85_j.Reset();
			tmmsm85_j.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			// 先判断是否可以卸装料槽/料篮
			tmmsm60["BUNKER_NO"] = bcls_rec->Tables[0].Rows[i]["BUNKER_NO"].ToString();
			tmmsm60.Query("BUNKER_NO");
			/*if (tmmsm60["BACK_C3"].ToString() == "1")
			{
				sprintf(s.msg, "炉前状态不能卸装料槽/料篮");
				throw CApplicationException(-1, s.msg, log.Location);
			}*/

			tmmsm85_O.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm85_O.Query("BUNKER_NO,MAT_CODE,SEQ_NO,WEIGH_NO");

			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TABLE_NAME");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TABLE_NAME_CHILD"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TABLE_NAME_CHILD");
			}
			//料仓成分
			bcls_rec->Tables["MMLCSND"].Rows.Clear();
			bcls_rec->Tables["MMLCSND"].Rows.Add();

			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "T8E2YB";
			bcls_rec->Tables["MMLCSND"].Rows[0]["ACTION"] = "D";
			bcls_rec->Tables["MMLCSND"].Rows[0]["STATION_NO"] = tmmsm85_O["BUNKER_TYPE"].ToString().Substring(0, 1);
			bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = tmmsm85_O["MAT_CODE"].ToString();
			bcls_rec->Tables["MMLCSND"].Rows[0]["QUALITY_BATCH_NO"] = tmmsm85_O["QUALITY_BATCH_NO"].ToString();
			/*	doFlag = f_mmsm_t8e2yb_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
			Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t8e2yb_snd失败-------");
			throw CApplicationException(-1, s.msg, log.Location);
			}*/

			//1、退废情况
			if (s_bunker_no1.Trim() == "1")
			{
				tmmsm85_O.Delete("BUNKER_NO,WEIGH_NO,MAT_CODE,SEQ_NO");
				tmmsm89.CopyFrom(tmmsm85_O);
				tmmsm89["STOCK_WT"] = tmmsm89["STOCK_WT"].ToDecimal();
				tmmsm89["EVENT_CODE"] = "CORR";
				tmmsm89["EVENT_DESC"] = "盘亏";
				tmmsm89["EVENT_NAME"] = "料槽/料篮废料";

				bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
				bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);

			}
			else  //源料仓库存减少，目标库存新增
			{
				//判断是否有目标库存
				if (tmmsm85_j["BUNKER_NO_ORIGINAL"].ToString().Trim() == "")
				{
					sqlstr = "  SELECT max(BUNKER_NO) FROM  TMMSM60     WHERE 1=1   AND MAT_CODE = @MAT_CODE ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("MAT_CODE", tmmsm85_O["MAT_CODE"].ToString());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						tmmsm85_j["BUNKER_NO_ORIGINAL"] = cmd_inq.GetString(1);
					}
					cmd_inq.Close();
				}

				//将源目标库存插入，然后删除料仓
				tmmsm85_Z.CopyFrom(tmmsm85_O);
				tmmsm85_O.Delete("BUNKER_NO,WEIGH_NO,MAT_CODE,SEQ_NO");

				sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85  WHERE 1=1 AND BUNKER_NO = @BUNKER_NO  ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("BUNKER_NO", tmmsm85_j["BUNKER_NO_ORIGINAL"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cd_seq_no = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				tmmsm85_Z["SEQ_NO"] = cd_seq_no + 1;
				tmmsm85_Z["BUNKER_NO_ORIGINAL"] = tmmsm85_O["BUNKER_NO"];
				tmmsm85_Z["BUNKER_NO"] = tmmsm85_j["BUNKER_NO_ORIGINAL"].ToString();
				//退回的库存需要将扣杂的返回到工艺称库存
				tmmsm85_Z["STOCK_WT"] = tmmsm85_O["STOCK_WT"].ToDecimal() + tmmsm85_O["BUNKER_DEDUCT_WT"].ToDecimal();
				tmmsm85_Z["BUNKER_DEDUCT_WT"] = 0;
				tmmsm60["BUNKER_NO"] = tmmsm85_Z["BUNKER_NO"];
				tmmsm60.Query("BUNKER_NO");
				tmmsm85_Z["BUNKER_TYPE"] = tmmsm60["BUNKER_TYPE"];
				tmmsm85_Z["BUNKER_NAME"] = tmmsm60["BUNKER_NAME"];
				tmmsm85_Z["SERIAL_NUMBER"] = serial_number;
				tmmsm85_Z["TIME_1"] = datetime;
				tmmsm85_Z["REC_CREATOR"] = s.userid;
				tmmsm85_Z["REC_CREATE_TIME"] = datetime;
				tmmsm85_Z.TrimOrBlank();
				tmmsm85_Z.Insert();

				tmmsm89.CopyFrom(tmmsm85_Z);
				tmmsm89["EVENT_CODE"] = "MOVE";
				tmmsm89["EVENT_DESC"] = "移库";
				tmmsm89["EVENT_NAME"] = "料槽/料篮退料";
				bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
				bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);

				bcls_rec_updlc.Tables[0].Rows.Add();
				bcls_rec_updlc.Tables[0].Rows[i]["BUNKER_NO"] = tmmsm85_O["BUNKER_NO"].ToString();
				bcls_rec_updlc.Tables[0].Rows[i]["QUALITY_BATCH_NO"] = tmmsm85_O["QUALITY_BATCH_NO"].ToString();
				bcls_rec_updlc.Tables[0].Rows[i]["MAT_CODE"] = tmmsm85_O["MAT_CODE"].ToString();

			}

			//更新库存信息
			sqlstr = "UPDATE TMMSM60 SET STOCK_WT = (select nvl(sum(STOCK_WT),0) FROM tmmsm85 WHERE BUNKER_NO = @BUNKER_NO) "
				" WHERE BUNKER_NO=@BUNKER_NO "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("BUNKER_NO", tmmsm85_j["BUNKER_NO"].ToString());
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = "UPDATE TMMSM60 SET STOCK_WT = (select nvl(sum(STOCK_WT),0) FROM tmmsm85 WHERE BUNKER_NO = @BUNKER_NO),BACK_C1 = '1' "
				" WHERE BUNKER_NO=@BUNKER_NO "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("BUNKER_NO", tmmsm85_j["BUNKER_NO_ORIGINAL"].ToString());
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

		sqlstr = "UPDATE TMMSM60 SET BACK_C1='0', BACK_C2='0', BACK_C3='0',  UPLOAD_301='0' "
			" WHERE BUNKER_NO=@BUNKER_NO "
			" AND NOT EXISTS (SELECT 1 FROM tmmsm85 T2 WHERE BUNKER_NO = @BUNKER_NO)"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = " select count(1) from tmmsm85  WHERE BUNKER_NO = @BUNKER_NO ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			n_count = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();

		if (n_count == 0)
		{
			if (bcls_rec_updlc.Tables[0].Rows.get_Count() > 0)
			{
				doFlag = f_mmsm_updlc(&bcls_rec_updlc, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
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
