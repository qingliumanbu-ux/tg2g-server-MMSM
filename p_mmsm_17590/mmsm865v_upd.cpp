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
BM2F_ENTERACE(mmsm865v_upd)

int f_mmsm865v_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sql = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString bunker_no = "";
	CString bunker_no1 = "";
	int TotalRecordCount = 0;
	CString  now = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CModel tmmsm85("TMMSM85");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm60_o("TMMSM60");
	CModel tmmsm851("TMMSM85");
	CString d_seq_no = "";
	CString mat_code = "";
	CString weigh_no = "";
	int d_seq_noleng = 0;
	CDecimal cd_seq_no = 0;
	CDecimal wt = 0;
	CDecimal wt1 = 0;
	CDecimal stock_wt = 0;
	CDecimal stock_wt_60 = 0;
	CModel tmmsm85_Z("TMMSM85");
	CModel tmmsm85_Z1("TMMSM85");
	CModel tmmsm89("TMMSM89");
	CModel tmmsm60_O("TMMSM60");
	EIClass bcls_rec_tmmsm89_log;
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	try
	{
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			Log::Info("", __FUNCTION__, "Tables =[{0}]", bcls_rec->Tables.get_Count());
			Log::Info("", __FUNCTION__, "Tables =[{0}]", bcls_rec->Tables[3].Rows.get_Count());

			bunker_no = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString().Trim();//高位料仓
			Log::Info("", __FUNCTION__, "Tables =[{0}]", 222);
			wt = bcls_rec->Tables[1].Rows[0]["STOCK_WT"].ToDecimal();//输入重量
			Log::Info("", __FUNCTION__, "Tables =[{0}]", 111);
			bunker_no1 = bcls_rec->Tables[2].Rows[0]["BUNKER_NO"].ToString().Trim(); //低位料仓
			wt1 = bcls_rec->Tables[0].Rows[0]["STOCK_WT"].ToDecimal();
			Log::Info("", __FUNCTION__, "bunker_noq =[{0}]", bunker_no1);
			Log::Info("", __FUNCTION__, "wt1 =[{0}]", wt1);
			//s.formname
			Log::Info("", __FUNCTION__, "formname =[{0}]", s.formname);
			if (wt1 < wt)
			{
				sprintf(s.msg, "输入的重量已超过高位料仓总量");
				throw CApplicationException(-1, s.msg, log.Location);
			}

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
			Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", tmmsm85["BUNKER_NO"].ToString());
			//分页获取
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cd_seq_no = cmd_inq.GetDecimal(1) + 1;
			}
			cmd_inq.Close();

			tmmsm60["BUNKER_NO"] = bunker_no;
			tmmsm60.Query("BUNKER_NO");
			if (tmmsm60["STOCK_WT"].ToDecimal() < wt)
			{
				sprintf(s.msg, "输入的重量已超过高位料仓总量");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			for (int i = 0; i < bcls_rec->Tables[3].Rows.get_Count(); i++)
			{
				d_seq_no = d_seq_no +"," +"'"+ bcls_rec->Tables[3].Rows[i]["SEQ_NO"].ToString()+"'";
				weigh_no = weigh_no + "," + "'" + bcls_rec->Tables[3].Rows[i]["WEIGH_NO"].ToString() + "'";
				mat_code = mat_code + "," + "'" + bcls_rec->Tables[3].Rows[i]["MAT_CODE"].ToString() + "'";
			}
			if (d_seq_no.GetLength()>0)
			{
				d_seq_noleng = d_seq_no.GetLength();
				d_seq_no = d_seq_no.Substring(1, d_seq_noleng-1);
			/*	d_seq_no = "(" + d_seq_no + ")";*/
				Log::Trace(" ", __FUNCTION__, "d_seq_no =[{0}]", d_seq_no);
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
			
			
			//更新60表
		
			/*stock_wt_60 = tmmsm60["STOCK_WT"].ToDecimal() - wt;
			tmmsm60["STOCK_WT"] = stock_wt_60;
			tmmsm60.Update("STOCK_WT", "BUNKER_NO");*/

			/*tmmsm60_o["BUNKER_NO"] = bunker_no1;
			tmmsm60_o.Query("BUNKER_NO");
			stock_wt_60 = tmmsm60_o["STOCK_WT"].ToDecimal() + wt;
			tmmsm60_o["STOCK_WT"] = stock_wt_60;
			tmmsm60_o.Update("STOCK_WT", "BUNKER_NO");*/

			// 更新85表数据
			sql = "  SELECT * FROM  TMMSM85     WHERE 1=1 AND STOCK_WT <>0    AND BUNKER_NO = @BUNKER_NO and SEQ_NO in ( " + d_seq_no + " )   and MAT_CODE in ( " + mat_code + " ) and  WEIGH_NO in ( " + weigh_no + " )  ORDER BY SEQ_NO ASC ";
			cmd_inq1.Parameters.Set("BUNKER_NO", bunker_no);
			//分页获取
			cmd_inq1.SetCommandText(sql);
			Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", sql);
			int  i = 0;
			cmd_inq1.ExecuteReader();
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
					tmmsm85_Z1["BUNKER_NAME"] = bcls_rec->Tables[2].Rows[0]["BUNKER_NAME"].ToString().Trim();
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
					tmmsm85_Z1["TIME_INSTOCK"] = datetime;
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

					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60["BUNKER_TYPE"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"];

					//tmmsm89["EVENT_DESC"] = "上料从" + tmmsm89["BUNKER_NO_ORIGINAL"].ToString() + "到" + tmmsm89["BUNKER_NO"].ToString();
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
					tmmsm85_Z1.TrimOrBlank();
					tmmsm85_Z1.Insert();
					i++;
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
					tmmsm85_Z1["BUNKER_NAME"] = bcls_rec->Tables[2].Rows[0]["BUNKER_NAME"].ToString().Trim();
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
					tmmsm85_Z1["TIME_INSTOCK"] = datetime;
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
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60["BUNKER_TYPE"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = wt;
					//tmmsm89["EVENT_DESC"] = "上料从" + tmmsm89["BUNKER_NO_ORIGINAL"].ToString() + "到" + tmmsm89["BUNKER_NO"].ToString();
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
					tmmsm85_Z1.TrimOrBlank();
					tmmsm85_Z1.Insert();
					i++;
					break;
				}
				
			}
			cmd_inq1.Close();

			//更新库存
			sqlstr = " update tmmsm60 set stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = @bunker_no)"
				" where BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新库存
			sqlstr = " update tmmsm60 set stock_wt = (select nvl(sum(stock_wt),0) from tmmsm85 where BUNKER_NO = @bunker_no)"
				" where BUNKER_NO = @bunker_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("bunker_no", bunker_no1);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			if (bcls_rec_tmmsm89_log.Tables[0].Rows.get_Count() > 0)
			{
				doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
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
