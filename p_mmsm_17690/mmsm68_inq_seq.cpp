/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 获取计量单号
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm68_inq_seq)

int f_mmsm68_inq_seq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	int		TotalRecordCount = 0;
	CString datetime1("");
	CString datetime("");
	datetime1 = CDateTime::Today().ToString("yyyyMMdd");
	datetime1 = datetime1.Substring(2, 6);

	//系统的分页类信息。
	CModel tmmsm68("TMMSM68");
	CModel tmmsm81_S("TMMSM81_S");
	CDbCommand cmd_inq(conn);

	try
	{ 		

		bcls_ret->Tables[0].Columns.Add(DT_STRING, "WEIGH_NO");
		bcls_ret->Tables[0].Rows.Add();
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");	
		CString  dh = "S" + datetime + EPGetNextSeq("SQ_JLYLID", conn);
		bcls_ret->Tables[0].Rows[0]["WEIGH_NO"] = dh; 
		tmmsm81_S.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm68.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString().Trim() == "")
		{
			tmmsm81_S["WEIGH_NO"] = dh;
		}
		tmmsm68["REC_CREATE_TIME"] = datetime;
		tmmsm68["REC_CREATOR"] = s.userid; 
		tmmsm81_S.TrimOrBlank();
		tmmsm81_S["FORM_EDIT_FLAG"] = "0";
		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm81_S["BUNKER_NO"].ToString());
		tmmsm81_S.Insert();
		Log::Info("", __FUNCTION__, "WEIGH_NO1 =[{0}]", tmmsm81_S["WEIGH_NO"].ToString());
	
		Log::Info("", __FUNCTION__, "RESUME_SEQ_NO =[{0}]", tmmsm68["RESUME_SEQ_NO"].ToString());
		Log::Info("", __FUNCTION__, "dh =[{0}]", dh);
		Log::Info("", __FUNCTION__, "GROSS_WT =[{0}]", tmmsm68["GROSS_WT"].ToDecimal());
		if (tmmsm68["WEIGH_NO"].ToString().Trim() == "")
		{
			tmmsm68["RESUME_SEQ_NO"] = bcls_rec->Tables[0].Rows[0]["RESUME_SEQ_NO"].ToString().Trim();
			tmmsm68["GROSS_WT"] = bcls_rec->Tables[0].Rows[0]["GROSS_WT"].ToDecimal();
			tmmsm68["TARE_WT"] = bcls_rec->Tables[0].Rows[0]["TARE_WT"].ToDecimal();
			tmmsm68["BUCKLE_WT"] = bcls_rec->Tables[0].Rows[0]["BUCKLE_WT"].ToDecimal();
			tmmsm68["NET_WT"] = bcls_rec->Tables[0].Rows[0]["NET_WT"].ToDecimal();
			if (bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString().Trim() == "")
			{
				tmmsm68["WEIGH_NO"] = dh;
			}
			tmmsm68["USE_LOGO"] = "1";
			tmmsm68.Update("GROSS_WT,TARE_WT,BUCKLE_WT,NET_WT,WEIGH_NO,USE_LOGO", "RESUME_SEQ_NO");
		}
		else
		{
			tmmsm68["RESUME_SEQ_NO"] = bcls_rec->Tables[0].Rows[0]["RESUME_SEQ_NO"].ToString().Trim();
			tmmsm68["GROSS_WT"] = bcls_rec->Tables[0].Rows[0]["GROSS_WT"].ToDecimal();
			tmmsm68["TARE_WT"] = bcls_rec->Tables[0].Rows[0]["TARE_WT"].ToDecimal();
			tmmsm68["BUCKLE_WT"] = bcls_rec->Tables[0].Rows[0]["BUCKLE_WT"].ToDecimal();
			tmmsm68["NET_WT"] = bcls_rec->Tables[0].Rows[0]["NET_WT"].ToDecimal();
			if (bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString().Trim() == "")
			{
				tmmsm68["WEIGH_NO"] = dh;
			}
			tmmsm68["USE_LOGO"] = "1";
			tmmsm68["REC_CREATE_TIME"] = datetime;
			tmmsm68["REC_CREATOR"] = s.userid;
			tmmsm68["REC_CREATOR"] = s.userid;
			tmmsm68.TrimOrBlank();
			tmmsm68.Delete();
			tmmsm68.Insert();
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
