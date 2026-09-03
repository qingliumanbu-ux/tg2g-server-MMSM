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
BM2F_ENTERACE(mmsm868v_upd)

int f_mmsm868v_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString mat_code = "";
	//CDecimal cd_seq_no = 0;
	CString weigh_no = "";
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
	CDecimal cd_seq_no = 0;
	CDecimal dd_seq_no = 0;
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

			//bunker_no = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString().Trim();//高位料仓
			//Log::Info("", __FUNCTION__, "Tables =[{0}]", 222);
			//wt = bcls_rec->Tables[1].Rows[0]["STOCK_WT"].ToDecimal();//输入重量
			//Log::Info("", __FUNCTION__, "Tables =[{0}]", 111);
			//bunker_no1 = bcls_rec->Tables[2].Rows[0]["BUNKER_NO"].ToString().Trim(); //低位料仓
			//wt1 = bcls_rec->Tables[2].Rows[0]["STOCK_WT"].ToDecimal();
			//Log::Info("", __FUNCTION__, "bunker_noq =[{0}]", bunker_no1);
			//Log::Info("", __FUNCTION__, "wt1 =[{0}]", wt1);
			bunker_no = "WIRE";
			bunker_no1 = bcls_rec->Tables[0].Rows[0]["BUNKER_NO_ORIGINAL"].ToString().Trim();//退料料仓
			wt = bcls_rec->Tables[0].Rows[0]["STOCK_WT_1"].ToDecimal();//输入重量
			wt1 = bcls_rec->Tables[0].Rows[0]["STOCK_WT"].ToDecimal();//原始重量
			mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();//投料代码
			cd_seq_no = bcls_rec->Tables[0].Rows[0]["SEQ_NO"].ToDecimal();//序号
			weigh_no = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString().Trim();//计量单号
			Log::Info("", __FUNCTION__, "bunker_no1 =[{0}]，wt =[{1}],wt1 =[{2}]，cd_seq_no =[{3}]，weigh_no =[{4}]，mat_code =[{5}]", bunker_no1, wt, wt1, cd_seq_no, weigh_no,mat_code);
			
			if (wt<0)
			{
				sprintf(s.msg, "退料重量不能小于0");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			if (bunker_no1.Trim()=="")
			{
				sprintf(s.msg, "退料料仓不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (wt1 < wt)
			{
				sprintf(s.msg, "退料重量不能大于原本重量");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if ( 0 > wt )
			{
				sprintf(s.msg, "退料重量不能小于0");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO = @BUNKER_NO ";
			cmd_inq.Parameters.Set("BUNKER_NO", bunker_no1);
			Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", bunker_no1);
			//分页获取
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				dd_seq_no = cmd_inq.GetDecimal(1) + 1;
			}
			cmd_inq.Close();

			//更新60表
			tmmsm60["BUNKER_NO"] = bunker_no;
			tmmsm60.Query("BUNKER_NO");
			stock_wt_60 = tmmsm60["STOCK_WT"].ToDecimal() - wt1 + wt;
			tmmsm60["STOCK_WT"] = stock_wt_60;
			tmmsm60.Update("STOCK_WT", "BUNKER_NO");

			tmmsm60_o["BUNKER_NO"] = bunker_no1;
			tmmsm60_o.Query("BUNKER_NO");
			stock_wt_60 = tmmsm60_o["STOCK_WT"].ToDecimal() - wt + wt1;
			tmmsm60_o["STOCK_WT"] = stock_wt_60;
			tmmsm60_o.Update("STOCK_WT", "BUNKER_NO");

			// 更新85表数据
			sql = "  SELECT * FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO	= @BUNKER_NO and  WEIGH_NO = @WEIGH_NO and  MAT_CODE = @MAT_CODE and  SEQ_NO = @SEQ_NO";
			cmd_inq1.Parameters.Set("BUNKER_NO", bunker_no);
			cmd_inq1.Parameters.Set("WEIGH_NO", weigh_no);
			cmd_inq1.Parameters.Set("SEQ_NO", cd_seq_no);
			cmd_inq1.Parameters.Set("MAT_CODE", mat_code);
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
					if (tmmsm85_Z["STOCK_WT"] < wt)
					{
						sprintf(s.msg, "退料重量不能大于原本重量");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					else
					{
						wt = wt - tmmsm85_Z["STOCK_WT"];
						tmmsm85_Z.Delete("BUNKER_NO,WEIGH_NO,MAT_CODE,SEQ_NO");
						tmmsm85_Z1["SEQ_NO"] = dd_seq_no + i;
						tmmsm85_Z1["BUNKER_NO"] = bunker_no1;
						tmmsm85_Z1["BUNKER_TYPE"] = "1";
						tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
						tmmsm85_Z1["BUNKER_NAME"] = bcls_rec->Tables[0].Rows[0]["BACK_CODE_1"].ToString().Trim();
						tmmsm85_Z1["REC_CREATOR"] = s.userid;
						tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
						tmmsm89.CopyFrom(tmmsm85_Z1);
						tmmsm89["EVENT_CODE"] = "MOVE";
						tmmsm89["EVENT_DESC"] = "移库";
						tmmsm89["EVENT_NAME"] = "手投料/丝线退料";
						tmmsm89["REC_CREATOR"] = s.userid;
						tmmsm89["REC_CREATE_TIME"] = datetime;
						tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60["BUNKER_NO"];
						tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60["BUNKER_TYPE"];
						tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"];
						//tmmsm89["EVENT_DESC"] = "上料从" + tmmsm89["BUNKER_NO_ORIGINAL"].ToString() + "到" + tmmsm89["BUNKER_NO"].ToString();
						bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
						bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
						tmmsm85_Z1.TrimOrBlank();
						tmmsm85_Z1.Insert();
						if (wt == 0)
						{
							break;
						}
					}
					
				}
				else
				{
					tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"] - wt1 + wt;
					tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,WEIGH_NO,MAT_CODE,SEQ_NO");
					tmmsm85_Z1["SEQ_NO"] = dd_seq_no + i;
					tmmsm85_Z1["STOCK_WT"] = -wt + wt1;
					tmmsm85_Z1["BUNKER_NO"] = bunker_no1;
					tmmsm85_Z1["BUNKER_TYPE"] = "1";
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["BUNKER_NAME"] = bcls_rec->Tables[0].Rows[0]["BACK_CODE_1"].ToString().Trim();
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;
					tmmsm89.CopyFrom(tmmsm85_Z1);
					tmmsm89["EVENT_CODE"] = "MOVE";
					tmmsm89["EVENT_DESC"] = "移库";
					tmmsm89["EVENT_NAME"] = "手投料/丝线退料";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60["BUNKER_TYPE"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = wt;
					//tmmsm89["EVENT_DESC"] = "上料从" + tmmsm89["BUNKER_NO_ORIGINAL"].ToString() + "到" + tmmsm89["BUNKER_NO"].ToString();
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
					tmmsm85_Z1.TrimOrBlank();
					tmmsm85_Z1.Insert();
					break;
				}
				i++;
			}
			cmd_inq1.Close();
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
