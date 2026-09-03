/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167
Version:    1.0
Date:       2023-12-04
Description: MMSM65增删改
**************************************************/
//框架头文件
#include "stdafx.h" 




//业务头文件

//外部函数声明
int f_mmsm_t8t701_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(mmsmnb01_snd)

int f_mmsmnb01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm65("TMMSMNB01");
	CModel tmmsm65_1("TMMSMNB01");

	CModel tmmsm85("TMMSM85");
	CModel tmmsm85_O("TMMSM85");
	CModel tmmsm85_Z("TMMSM85");
	CModel tmmsm85_Z1("TMMSM85");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm60_o("TMMSM60");
	CModel tmmsm89("TMMSM89");
	CModel tmmsm81("TMMSM81");
	CModel tmmsm81V("TMMSM81V");
	CModel tmmsm81s("TMMSM81_S");

	CDecimal n_count = 1;
	EIClass bcls_rec_updlc;
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");

	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);

	/* 数据库SQL操作字符串 */
	CString sql = "";
	CString sqlstr;
	CString cd_seq_no;
	CString bunker_no = "";
	CDecimal stock_wt = 0;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "PURCHASEDOCID");
		/* 获得传入参数 */


		if (bcls_rec->Tables.IndexOf("MMSM65_SND") >= 0)
		{
			CString vdeal_flag = bcls_rec->Tables["MMSM65_SND"].Rows[0]["DEAL_FLAG"].ToString();
			//料仓号
			bunker_no = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString().Trim();
			//原重量 
			stock_wt = bcls_rec->Tables[0].Rows[0]["MAT_WT"].ToDecimal();

			tmmsm60["BUNKER_NO"] = bunker_no;

			//查询原始数据
			tmmsm60.Query("BUNKER_NO");
			Log::Trace(" ", __FUNCTION__, "SW BUNKER_NO=[{0}],stock_wt=[{1}]", bunker_no, stock_wt);

			if (0 > stock_wt)
			{
				sprintf(s.msg, "输入的重量不能小于0");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (bunker_no == "")
			{
				sprintf(s.msg, "料仓号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm60["STOCK_WT"] = tmmsm60["STOCK_WT"].ToDecimal() - stock_wt;

			sql = "  SELECT * FROM  TMMSM85     WHERE 1=1   AND BUNKER_NO	= @BUNKER_NO and QUALITY_BATCH_NO <> ' ' ORDER BY SEQ_NO ASC ";
			cmd_inq1.Parameters.Set("BUNKER_NO", bunker_no);
			cmd_inq1.SetCommandText(sql);
			int  i = 0;
			cmd_inq1.ExecuteReader();
			while (cmd_inq1.Read())
			{
				cmd_inq1.Fetch(tmmsm85_Z);
				/*	Log::Trace(" ", __FUNCTION__, "SW BUNKER_NO=[{0}]", tmmsm85_O["BUNKER_NO"].ToString());*/
				Log::Trace(" ", __FUNCTION__, "SW BUNKER_NO=[{0}]", tmmsm85_Z["BUNKER_NO"].ToString());
				tmmsm85_Z1.CopyFrom(tmmsm85_Z);

				//CModel tmmsm81("TMMSM81");
				/*tmmsm81["WEIGH_NO"] = tmmsm85_Z1["WEIGH_NO"];
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
				}*/

				if (tmmsm85_Z["STOCK_WT"] <= stock_wt)
				{
					Log::Trace(" ", __FUNCTION__, "1=[{0}]", 1);

					tmmsm85_Z.Delete("BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
					stock_wt = stock_wt - tmmsm85_Z["STOCK_WT"];
					tmmsm89.CopyFrom(tmmsm85_Z1);
					tmmsm89["EVENT_CODE"] = "NB";
					tmmsm89["EVENT_DESC"] = "南北互调";
					tmmsm89["EVENT_NAME"] = "南北互调";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60["BUNKER_TYPE"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"];
					//tmmsm89["EVENT_DESC"] = "上料从" + tmmsm89["BUNKER_NO_ORIGINAL"].ToString() + "到" + tmmsm89["BUNKER_NO"].ToString();
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);

					Log::Trace(" ", __FUNCTION__, "SW1 MAT_NAME=[{0}]", tmmsm85_Z["MAT_NAME"].ToString());
					//tmmsm85_Z1.Insert();
					tmmsm60["BUNKER_NO"] = tmmsm85_Z1["BUNKER_NO"];
					tmmsm60["QUALITY_BATCH_NO"] = tmmsm85_Z1["QUALITY_BATCH_NO"];
					tmmsm60["LASTACTDATE"] = datetime;
					tmmsm60["LOT_NO"] = tmmsm85_Z1["LOT_NO"];
					tmmsm60.Update("LASTACTDATE,LOT_NO,QUALITY_BATCH_NO", "BUNKER_NO");
					if (stock_wt == 0)
					{
						break;
					}
				}
				else
				{
					Log::Trace(" ", __FUNCTION__, "2=[{0}]", 1);
					tmmsm85_Z["STOCK_WT"] = tmmsm85_Z["STOCK_WT"] - stock_wt;
					tmmsm85_Z.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");
					/*tmmsm85_Z1["SEQ_NO"] = cd_seq_no + i;
					tmmsm85_Z1["STOCK_WT"] = cd_stock_wt;
					tmmsm85_Z1["BUNKER_NO"] = tmmsm60["BUNKER_NO"];
					tmmsm85_Z1["BUNKER_TYPE"] = tmmsm60["BUNKER_TYPE"];
					tmmsm85_Z1["BUNKER_NAME"] = tmmsm60["BUNKER_NAME"];
					tmmsm85_Z1["PLATE_NUMBER"] = s_bunker_no_original;
					tmmsm85_Z1["BUCKLE_WT"] = (tmmsm85_Z1["STOCK_WT"].ToDecimal() * fl_wt).Round(0);
					tmmsm85_Z1["STOCK_WT"] = tmmsm85_Z1["STOCK_WT"] - (tmmsm85_Z1["STOCK_WT"].ToDecimal() * fl_wt).Round(0);
					tmmsm85_Z1["WEIGH_NO"] = weigh_no;
					tmmsm85_Z1["C_ORDERID"] = c_orderid;
					tmmsm85_Z1["RECEIVE_DATA_TIME"] = datetime;
					tmmsm85_Z1["TIME_INSTOCK"] = datetime;
					tmmsm85_Z1["REC_CREATOR"] = s.userid;
					tmmsm85_Z1["REC_CREATE_TIME"] = datetime;*/
					tmmsm89.CopyFrom(tmmsm85_Z1);
					tmmsm89["EVENT_CODE"] = "NB";
					tmmsm89["EVENT_DESC"] = "南北互调";
					tmmsm89["EVENT_NAME"] = "南北互调";
					tmmsm89["REC_CREATOR"] = s.userid;
					tmmsm89["REC_CREATE_TIME"] = datetime;
					tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm60["BUNKER_NO"];
					tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm60["BUNKER_TYPE"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm60["BUNKER_NAME"];
					tmmsm89["BUNKER_NAME_ORIGINAL"] = stock_wt;
					//tmmsm89["EVENT_DESC"] = "上料从" + tmmsm89["BUNKER_NO_ORIGINAL"].ToString() + "到" + tmmsm89["BUNKER_NO"].ToString();
					bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
					bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
					//tmmsm85_Z1.Insert();
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
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();


			for (int i = 0; i < bcls_rec->Tables["MMSM65_SND"].Rows.get_Count(); i++)
			{
				tmmsm65.Reset();
				tmmsm65.MergeFrom(bcls_rec->Tables["MMSM65_SND"].Rows[i]);
				tmmsm65_1.MergeFrom(bcls_rec->Tables["MMSM65_SND"].Rows[i]);

				tmmsm65_1.Query("PURCHASEDOCID,SEQ_NO,MAT_CODE");
				if (vdeal_flag == "I")
				{
					if (tmmsm65_1["STATUS"].ToString() == "1")
					{
						sprintf(s.msg, "计量单已上传不能再次发送！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (vdeal_flag == "D")
				{
					if (tmmsm65_1["STATUS"].ToString() != "1")
					{
						sprintf(s.msg, "计量单未上传状态，不能撤销！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				//tmmsm65["APTIME"] = bcls_rec->Tables["MMSM65_SND"].Rows[i]["APTIME"].ToString().SubstringNE(0, 14);
				//tmmsm65.Update("*", "PURCHASEDOCID");
				//Log::Trace("", __FUNCTION__, "===tmmsm65[]= [{0}]", tmmsm65["APTIME"].ToString());

				sqlstr = "  SELECT LPAD(TO_CHAR(DB_NO.NEXTVAL), 4, '0') FROM DUAl   ";
				//cmd_inq.Parameters.Set("BUNKER_NO", tmmsm85["BUNKER_NO"].ToString());
				Log::Trace(" ", __FUNCTION__, "sqlstr1 =[{0}]", sqlstr);
				//分页获取
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cd_seq_no = cmd_inq.GetString(1);
				}
				cmd_inq.Close();

				CString  dh = "6240" + CDateTime::Today().ToString("yyyyMMdd") + cd_seq_no;
				Log::Trace("", __FUNCTION__, "DH				= [{0}]", (const char*)dh);
				/* 新增事件信息 */
				tmmsm65["TICODE"] = dh;
				tmmsm65["STATUS"] = "1";

				EIClass inBlock_1000;

				inBlock_1000.Tables[0].Clear();
				tmmsm65.MergeTo(inBlock_1000.Tables[0]);

				if (!inBlock_1000.Tables[0].Columns.Contains("DEAL_FLAG"))
				{
					inBlock_1000.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
					inBlock_1000.Tables[0].Rows[0]["DEAL_FLAG"] = "1";
				}
				tmmsm65.Update("STATUS,TICODE", "PURCHASEDOCID,SEQ_NO,MAT_CODE");

				if (bcls_rec_tmmsm89_log.Tables[0].Rows.get_Count() > 0)
				{
					doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				doFlag = f_mmsm_t8t701_snd(&inBlock_1000, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t8t701_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				/*if (vdeal_flag == "I") tmmsm65["STATUS"] = "1";
				if (vdeal_flag == "D") tmmsm65["STATUS"] = "2";*/


				bcls_ret->Tables[0].Rows.Add();
				bcls_ret->Tables[0].Rows[i]["PURCHASEDOCID"] = tmmsm65["PURCHASEDOCID"];

			}


		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


