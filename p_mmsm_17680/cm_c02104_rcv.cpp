/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:songwei
Date:2023-11-28
Version:1.0
Description: 接收资源系统
质量数据**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */


// service入口
BM2F_ENTERACE_TELE(cm_c02104_rcv)

int f_cm_c02104_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString div_flag = "";
	CString matPlmsCode = "";
	CString matCode = "";
	CDecimal ele_vaule =0;
	CModel tmmsm81("TMMSM81");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm89("TMMSM89");
	CDbCommand cmd_inq(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		return 0;
		div_flag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();
		tmmsm81["QUALITY_BATCH_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ENTR_NO"].ToString();

		if ("" == div_flag.Trim())
		{
			strcpy(s.msg, "处理标记不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == tmmsm81["QUALITY_BATCH_NO"].ToString().Trim())
		{
			strcpy(s.msg, "质检批号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = "SELECT ELM_VALUE FROM TMMSM81AL WHERE QUALITY_BATCH_NO ='" + tmmsm81["QUALITY_BATCH_NO"].ToString() + "' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			ele_vaule = ele_vaule+ cmd_inq.GetDecimal(1);
		}
		if (ele_vaule < 95 )
		{
			strcpy(s.msg, "质检批号对应成分之和小于95!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		Log::Trace("", __FUNCTION__, "WEIGH_NO=[{0}] QUALITY_BATCH_NO=[{1}] ele_vaule_sum=[{2}]",
			tmmsm81["WEIGH_NO"].ToString(), tmmsm81["QUALITY_BATCH_NO"].ToString(), ele_vaule);

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm81["WEIGH_NO"] = bcls_rec->Tables[0].Rows[i]["WEIGH_NO"].ToString();
			if (tmmsm81["WEIGH_NO"].ToString().Trim() == "") break;
			if (0 < tmmsm81.QueryCount("WEIGH_NO"))
			{
				tmmsm81["REC_REVISE_TIME"] = datetime;
				tmmsm81["REC_REVISOR"] = s.userid;
				tmmsm81.Update("QUALITY_BATCH_NO,REC_REVISE_TIME,REC_REVISOR", "WEIGH_NO");

				tmmsm81.Query("WEIGH_NO");
				if (tmmsm81["FORM_EDIT_FLAG"].ToString().Trim() != "1")
				{
		
					tmmsm85["WEIGH_NO"] = tmmsm81["WEIGH_NO"];
					tmmsm85["QUALITY_BATCH_NO"] = tmmsm81["QUALITY_BATCH_NO"];
					tmmsm85.Update("QUALITY_BATCH_NO", "WEIGH_NO");
					tmmsm89["WEIGH_NO"] = tmmsm81["WEIGH_NO"];
					tmmsm89["QUALITY_BATCH_NO"] = tmmsm81["QUALITY_BATCH_NO"];
					tmmsm89.Update("QUALITY_BATCH_NO", "WEIGH_NO");
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
