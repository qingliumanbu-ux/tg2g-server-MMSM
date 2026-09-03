/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2023-09-22 
Description: 退位料仓
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm861v_upd)
int f_mmsm_updlc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm861v_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sql = "";
	CString bunker_no = "";
	CString bunker_no1 = "";

	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CModel tmmsm85("TMMSM85");
	CModel tmmsm60("TMMSM60");
	CDecimal cd_seq_no = 0;
	CDecimal wt=0;
	CModel tmmsm85_Z("TMMSM85");
	CModel tmmsm85_Z1("TMMSM85");
	CModel tmmsm89("TMMSM89");
	CModel tmmsm60_O("TMMSM60");
	EIClass bcls_rec_tmmsm89_log;
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);

	EIClass bcls_rec_updlc;
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");

	try
	{
		bunker_no = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString().Trim();//高位料仓
		wt = bcls_rec->Tables[1].Rows[0]["STOCK_WT"].ToDecimal();//输入重量
		bunker_no1 = bcls_rec->Tables[2].Rows[0]["BUNKER_NO"].ToString().Trim(); //低位料仓
		if (bunker_no == bunker_no1)
		{
			sprintf(s.msg, "相同料仓不能移库");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm60["BUNKER_NO"] = bunker_no;
		tmmsm60.Query("BUNKER_NO");
		if (tmmsm60["STOCK_WT"].ToDecimal() < wt)
		{
			sprintf(s.msg, "输入的重量已超过高位料仓总量");
			throw CApplicationException(-1, s.msg, log.Location);
		}


		sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO = @BUNKER_NO ";
		cmd_inq.Parameters.Set("BUNKER_NO", bunker_no1);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cd_seq_no = cmd_inq.GetDecimal(1) + 1;
		}
		cmd_inq.Close();

		// 更新85表数据
		sql = "  SELECT * FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO			= @BUNKER_NO  ORDER BY SEQ_NO ASC ";
		cmd_inq1.Parameters.Set("BUNKER_NO", bunker_no);
		cmd_inq1.SetCommandText(sql);
		cmd_inq1.ExecuteReader();
		int  i = 0;
		while (cmd_inq1.Read())
		{
			cmd_inq1.Fetch(tmmsm85_Z);
			tmmsm85_Z1.CopyFrom(tmmsm85_Z);
			if (tmmsm85_Z["STOCK_WT"] <= wt)
			{
				wt = wt - tmmsm85_Z["STOCK_WT"];
				tmmsm85_Z.Delete("BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
				tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
				tmmsm85_Z1["BUNKER_NO"] = bunker_no1;
				tmmsm85_Z1["BUNKER_TYPE"] = "1";
				tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
				tmmsm85_Z1["TIME_INSTOCK"] = datetime;
				tmmsm85_Z1["BUNKER_NAME"] = bcls_rec->Tables[2].Rows[0]["BUNKER_NAME"].ToString().Trim();
				tmmsm85_Z1["REC_CREATOR"] = s.userid;
				tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
				tmmsm85_Z1.TrimOrBlank();
				tmmsm85_Z1.Insert();

				tmmsm89.CopyFrom(tmmsm85_Z1);
				tmmsm89["EVENT_CODE"] = "MOVE";
				tmmsm89["EVENT_DESC"] = "移库";
				if (s.formname == "MMSM861S2N")
				{
					tmmsm89["EVENT_NAME"] = "高位料仓退料";
				}
				else if (s.formname == "MMSM865V")
				{
					tmmsm89["EVENT_NAME"] = "地下料仓物料移库";
				}
				else
				{
					tmmsm89["EVENT_NAME"] = "高位料仓退料";
				}

				tmmsm89["BUNKER_NO_ORIGINAL"] = bunker_no;
				bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
				bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);

				if (wt == 0)
				{
					break;
				}
			}
			else
			{
				tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"] - wt;
				tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
				tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
				tmmsm85_Z1["STOCK_WT"] = wt;
				tmmsm85_Z1["BUNKER_NO"] = bunker_no1;
				tmmsm85_Z1["BUNKER_TYPE"] = "1";
				tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
				tmmsm85_Z1["TIME_INSTOCK"] = datetime;
				tmmsm85_Z1["BUNKER_NAME"] = bcls_rec->Tables[2].Rows[0]["BUNKER_NAME"].ToString().Trim();
				tmmsm85_Z1["REC_CREATOR"] = s.userid;
				tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
				tmmsm85_Z1.TrimOrBlank();
				tmmsm85_Z1.Insert();

				tmmsm89.CopyFrom(tmmsm85_Z1);
				tmmsm89["EVENT_CODE"] = "MOVE";
				tmmsm89["EVENT_DESC"] = "移库";
				if (s.formname == "MMSM861S2N")
				{
					tmmsm89["EVENT_NAME"] = "高位料仓退料";
				}
				else if (s.formname == "MMSM865V")
				{
					tmmsm89["EVENT_NAME"] = "地下料仓物料移库";
				}
				else
				{
					tmmsm89["EVENT_NAME"] = "高位料仓退料";
				}
				tmmsm89["BUNKER_NO_ORIGINAL"] = bunker_no;
				bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
				bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);

				break;
			}
			i++;
		}
		cmd_inq1.Close();


		int j = 0;
		bcls_rec_updlc.Tables[0].Rows.Add();
		bcls_rec_updlc.Tables[0].Rows[0]["BUNKER_NO"] = bunker_no;
		bcls_rec_updlc.Tables[0].Rows.Add();
		bcls_rec_updlc.Tables[0].Rows[1]["BUNKER_NO"] = bunker_no1;

		//更新库存
		if (bcls_rec_updlc.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm_updlc(&bcls_rec_updlc, bcls_ret, conn);
			if (doFlag < 0)
			{
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
