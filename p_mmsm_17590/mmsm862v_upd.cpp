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
int f_mmsm_updlc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm862v_upd)

int f_mmsm862v_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sql= "";
	CString sqlstr_temp = "";
	CString weigh_no = "";
	//BUNKER_NO
	CString bunker_no = "";
	CDecimal stock_wt = 0;
	CDecimal stock_wt_1 = 0;
	CDecimal stock_wt_60 = 0;
	CString back_code_1 = "";
	int TotalRecordCount = 0;
	CString  now = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CModel tmmsm85("TMMSM85");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm60_o("TMMSM60");
	CModel tmmsm89("TMMSM89");

	CDecimal n_count = 1;
	EIClass bcls_rec_updlc;
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "QUALITY_BATCH_NO");
	bcls_rec_updlc.Tables[0].Columns.Add(DT_STRING, "MAT_CODE");



	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);

	try
	{ 
		//计量单号
		weigh_no = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString().Trim();
		//料仓号
		bunker_no = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString().Trim();
		//原重量 
		stock_wt = bcls_rec->Tables[0].Rows[0]["STOCK_WT"].ToDecimal();
		//修改后重量
		stock_wt_1 = bcls_rec->Tables[0].Rows[0]["STOCK_WT_1"].ToDecimal();
		//备注原因
		back_code_1 = bcls_rec->Tables[0].Rows[0]["BACK_CODE_1"].ToString().Trim();
		tmmsm85["MAT_CODE"] = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		tmmsm85["SEQ_NO"] = bcls_rec->Tables[0].Rows[0]["SEQ_NO"];
		if (back_code_1=="")
		{
			back_code_1 = " ";
		}
		tmmsm85["ADJUST_REASON"] = back_code_1;


		Log::Trace(" ", __FUNCTION__, "bunker_no =[{0}],weigh_no =[{1}],MAT_CODE =[{2}],SEQ_NO =[{3}]", bunker_no, weigh_no, tmmsm85["MAT_CODE"].ToString(), tmmsm85["SEQ_NO"].ToDecimal());
		
		tmmsm85["BUNKER_NO"] = bunker_no;
		tmmsm85["WEIGH_NO"] = weigh_no;
		tmmsm60["BUNKER_NO"] = bunker_no;
		
		//查询原始数据
		tmmsm85.Query("ADJUST_REASON,BUNKER_NO,MAT_CODE,SEQ_NO,WEIGH_NO");
		tmmsm60.Query("BUNKER_NO");

		if (0 > stock_wt_1)
		{
			sprintf(s.msg, "输入的重量不能小于0");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//2024.9.11-郭松 去掉料仓容积限制
		if (tmmsm60["UPPER_LIMIT_VALUE"]< stock_wt_1)
		{
			Log::Trace(" ", __FUNCTION__, "料仓库存不能大于料仓的容积上限");
	/*		sprintf(s.msg, "料仓库存不能大于料仓的容积上限");
			throw CApplicationException(-1, s.msg, log.Location);*/
		}
		
		stock_wt_60 = tmmsm60["STOCK_WT"].ToDecimal() - stock_wt + stock_wt_1;

		Log::Trace(" ", __FUNCTION__, "STOCK_WT =[{0}]", tmmsm60["STOCK_WT"].ToDecimal());
		Log::Trace(" ", __FUNCTION__, "stock_wt_60 =[{0}]", stock_wt_60);
		/*tmmsm60["STOCK_WT"] = stock_wt_60;
		tmmsm60.Update("STOCK_WT","BUNKER_NO");*/

		tmmsm89.CopyFrom(tmmsm85);
	/*	if (stock_wt< stock_wt_1)
		{
			
		}*/
		/*else
		{
			tmmsm89["STOCK_WT"] = stock_wt - stock_wt_1;
			tmmsm89["EVENT_CODE"] = "CORR";
			tmmsm89["EVENT_DESC"] = "盘亏";
			tmmsm89["EVENT_NAME"] = "库存修正-盘亏";
		}*/

		tmmsm89["STOCK_WT"] = tmmsm85["STOCK_WT"].ToDecimal() - stock_wt_1;
		tmmsm89["EVENT_CODE"] = "INVE";
		tmmsm89["EVENT_DESC"] = "盘盈";
		tmmsm89["EVENT_NAME"] = "库存修正-盘盈";
		
		tmmsm89["REC_CREATOR"] = s.userid;
		tmmsm89["REC_CREATE_TIME"] = now;
		tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm85["BUNKER_NO"];
		tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm85["BUNKER_TYPE"];
		tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm85["BUNKER_NAME"];
		//tmmsm89["BUNKER_NAME_ORIGINAL"] = stock_wt;
		bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
		bcls_rec_tmmsm89_log.Tables[0].Rows[0].Merge(tmmsm89);

		bcls_rec_updlc.Tables[0].Rows.Add();
		bcls_rec_updlc.Tables[0].Rows[0]["BUNKER_NO"] = tmmsm85["BUNKER_NO"].ToString();
		bcls_rec_updlc.Tables[0].Rows[0]["QUALITY_BATCH_NO"] = tmmsm85["QUALITY_BATCH_NO"].ToString();
		bcls_rec_updlc.Tables[0].Rows[0]["MAT_CODE"] = tmmsm85["MAT_CODE"].ToString();

		if (stock_wt_1==0)
		{
			tmmsm85.Delete("BUNKER_NO,MAT_CODE,SEQ_NO,WEIGH_NO");
		}
		else
		{
			tmmsm85["STOCK_WT"] = stock_wt_1;
			tmmsm85.Update("STOCK_WT", "BUNKER_NO,MAT_CODE,SEQ_NO,WEIGH_NO");
		}
		



		//更新库存信息
		sqlstr = "UPDATE TMMSM60 SET STOCK_WT = (select nvl(sum(STOCK_WT),0) FROM tmmsm85 WHERE BUNKER_NO = @BUNKER_NO) "
			" WHERE BUNKER_NO=@BUNKER_NO "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", bunker_no);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = "UPDATE TMMSM60 SET BACK_C1='0', BACK_C2='0', BACK_C3='0',  UPLOAD_301='0' "
			" WHERE BUNKER_NO=@BUNKER_NO  AND BUNKER_TYPE IN ('EAFBOX','BOFBOX','AODBOX') "
			" AND NOT EXISTS (SELECT 1 FROM tmmsm85 T2 WHERE BUNKER_NO = @BUNKER_NO)"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", bunker_no);
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
