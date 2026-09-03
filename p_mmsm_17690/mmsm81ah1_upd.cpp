/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sw
Version:    1.0
Date:     2014-06-03 17:13:56
Description: 新增质检批
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <para>主调用函数：			                            </para>
/// </summary>
/// <param name=" ">     </param>
/// <param name=" ">                </param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口


BM2F_ENTERACE(mmsm81ah1_upd)


int f_mmsm81ah1_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString matCode = "";
	CString procDiv = "";
	CString qualityBatch = "";
	CString bunker_no = "";
	CString weigh_no = "";
	CString v_lot_no = "";
	CString v_factory_div = "";
	CDecimal elm_value = 0;
	CString   elm_code = "";
	CString	  msgstr = "";
	CString quality_batch_no = "";
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CModel tmmsm81ah("TMMSM81AH");
	CModel tmmsm81al("TMMSM81AL");
	CModel tmmsm81("TMMSM81");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm851("TMMSM85");
	CModel tmmsm50("TMMSM50");
	CDbCommand cmd_sql(conn);
	try
	{
		blkNum = bcls_rec->Tables.IndexOf("QM_ELE");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("QM_ELE");
		}
		Log::Info("", __FUNCTION__, "count =[{0}]", bcls_rec->Tables.get_Count());

		tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		quality_batch_no = bcls_rec->Tables[1].Rows[0]["QUALITY_BATCH_NO"].ToString().Trim();

		if (quality_batch_no=="")
		{
			sprintf(s.msg, "请选择质检批号!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		tmmsm81["WEIGH_NO"] = tmmsm85["WEIGH_NO"];
		tmmsm81["QUALITY_BATCH_NO"] = quality_batch_no;
		tmmsm81.Update("QUALITY_BATCH_NO","WEIGH_NO");

		tmmsm85["QUALITY_BATCH_NO"] = quality_batch_no;
		tmmsm85.Update("QUALITY_BATCH_NO", "WEIGH_NO,BUNKER_NO,MAT_CODE,SEQ_NO");

		


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
