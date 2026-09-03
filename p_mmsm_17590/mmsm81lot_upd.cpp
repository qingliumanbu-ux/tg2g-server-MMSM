/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sw
Version:    1.0
Date:     2014-06-03 17:13:56
Description: 供应商管理
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///供应商管理新增、删除、修改
/// <para>
///供应商管理新增、删除、修改。
///
/// </para>
/// <para>数据库表：tmmsm81(供应商管理表)					</para>
/// <para>主调用函数：			                            </para>
/// </summary>
/// <param name=" ">     </param>
/// <param name=" ">                </param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口


BM2F_ENTERACE(mmsm81lot_upd)


int f_mmsm81lot_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_weigh_no = "";
	CString v_new_lot_no = "";
	CString v_lot_no = "";
	CString v_factory_div = "";
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CModel tmmsm81("TMMSM81");
	CDbCommand cmd_sql(conn);
	try
	{
		Log::Info("", __FUNCTION__, "get_TableName =[{0}]", bcls_rec->Tables[0].get_TableName());
		Log::Info("", __FUNCTION__, "get_Count =[{0}]", bcls_rec->Tables[0].Rows.get_Count());
		
		Log::Info("", __FUNCTION__, "v_new_lot_no =[{0}]", bcls_rec->Tables[0].Rows[0]["NEW_LOT_NO"].ToString());
		Log::Info("", __FUNCTION__, "v_lot_no =[{0}]", bcls_rec->Tables[0].Rows[0]["LOT_NO"].ToString());
		v_weigh_no = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString();
		v_new_lot_no = bcls_rec->Tables[0].Rows[0]["NEW_LOT_NO"].ToString();
		v_lot_no = bcls_rec->Tables[0].Rows[0]["LOT_NO"].ToString();
		if (v_weigh_no.Trim()=="")
		{
			sprintf(s.msg, "计量单号不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (v_new_lot_no.Trim() == "")
		{
			sprintf(s.msg, "批次号不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm81["WEIGH_NO"] = v_weigh_no;
		tmmsm81["LOT_NO"] = v_new_lot_no;
		tmmsm81.Update("LOT_NO", "WEIGH_NO");
	
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
