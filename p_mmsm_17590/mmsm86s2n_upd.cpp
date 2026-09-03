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
BM2F_ENTERACE(mmsm86s2n_upd)

int f_mmsm86s2n_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sql = "";
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
	CModel tmmsm851("TMMSM85");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm60_o("TMMSM60");
	CModel tmmsm89("TMMSM89");



	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);

	try
	{
		if (bcls_rec->Tables.get_Count() !=2 )
		{
			sprintf(s.msg, "请选择详细库存记录");
			throw CApplicationException(-1, s.msg, log.Location);
		}		
		//原重量 
		stock_wt = bcls_rec->Tables[0].Rows[0]["NET_WT"].ToDecimal();
		tmmsm85.MergeFrom(bcls_rec->Tables[1].Rows[0]);
		tmmsm85.Query("BUNKER_NO,MAT_CODE,SEQ_NO,WEIGH_NO");
		tmmsm851.MergeFrom(bcls_rec->Tables[1].Rows[0]);
		tmmsm851["STOCK_WT"] = stock_wt;
		tmmsm851["ADJUST_REASON"] = bcls_rec->Tables[0].Rows[0]["ADJUST_REASON"].ToString();
		tmmsm851.Update("STOCK_WT,ADJUST_REASON", "BUNKER_NO,MAT_CODE,SEQ_NO,WEIGH_NO");

		////修改后重量		

		Log::Trace(" ", __FUNCTION__, "bunker_no =[{0}],weigh_no =[{1}],MAT_CODE =[{2}],SEQ_NO =[{3}]", tmmsm85["BUNKER_NO"].ToString(), tmmsm85["WEIGH_NO"].ToString(), tmmsm85["MAT_CODE"].ToString(), tmmsm85["SEQ_NO"].ToDecimal());
		tmmsm60["BUNKER_NO"] = tmmsm851["BUNKER_NO"];
		tmmsm60.Query("BUNKER_NO");	
		if (0 < stock_wt)
		{
			sprintf(s.msg, "重量不能小于0");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/*tmmsm60["STOCK_WT"] = tmmsm60["STOCK_WT"].ToDecimal() - tmmsm85["STOCK_WT"].ToDecimal() + stock_wt;
		tmmsm60.Update("STOCK_WT", "BUNKER_NO");*/

		tmmsm89.CopyFrom(tmmsm85);
		tmmsm89["STOCK_WT"] = stock_wt - tmmsm85["STOCK_WT"].ToDecimal();
		if (tmmsm89["STOCK_WT"].ToDecimal() > 0)
		{
			tmmsm89["EVENT_DESC"] = "盘盈";
			tmmsm89["EVENT_CODE"] = "INVE";
		}
		else
		{
			tmmsm89["EVENT_DESC"] = "盘亏";
			tmmsm89["EVENT_CODE"] = "CORR";
			tmmsm89["STOCK_WT"] = 0 - tmmsm89["STOCK_WT"].ToDecimal();
		}
		
		tmmsm89["BUNKER_NO_ORIGINAL"] = tmmsm85["BUNKER_NO"];
		tmmsm89["BUNKER_TYPE_ORIGINAL"] = tmmsm85["BUNKER_TYPE"];
		tmmsm89["BUNKER_NAME_ORIGINAL"] = tmmsm85["BUNKER_NAME"];
		
		bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
		bcls_rec_tmmsm89_log.Tables[0].Rows[0].Merge(tmmsm89);


		//更新库存信息
		sqlstr = "UPDATE TMMSM60 SET STOCK_WT = (select nvl(sum(STOCK_WT),0) FROM tmmsm85 WHERE BUNKER_NO = @BUNKER_NO) "
			" WHERE BUNKER_NO=@BUNKER_NO "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm60["BUNKER_NO"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = "UPDATE TMMSM60 SET BACK_C1='0', BACK_C2='0', BACK_C3='0',  UPLOAD_301='0' "
			" WHERE BUNKER_NO=@BUNKER_NO  AND BUNKER_TYPE IN ('EAFBOX','BOFBOX','AODBOX') "
			" AND NOT EXISTS (SELECT 1 FROM tmmsm85 T2 WHERE BUNKER_NO = @BUNKER_NO)"
			;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm60["BUNKER_NO"].ToString());
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
