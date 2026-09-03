/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-03 17:13:56
Description: 原辅料主信息处理
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// 原辅料主信息新增、删除、修改
/// <para>
/// 1.原辅料主信息新增、删除、修改。
///
/// </para>
/// <para>数据库表：tmmsm60(炼钢原辅料主信息表)					</para>
/// <para>主调用函数：			                                </para>
/// </summary>
/// <param name=" ">     </param>
/// <param name=" ">                </param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口


BM2F_ENTERACE(mmsm60_upd)


int f_mmsm60_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	CModel tmmsm60("TMMSM60");
	/* ***** 自定义变量 ***** */
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;
	CDbCommand cmd_sql(conn); //与DB 建立连接。
	CString sqlstr = "";
	CString   v_proc_div = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_update = "";  //修改的字段信息
	CString v_condi = "";  //过滤的字段信息。
	CString v_func_id = ""; //功能号。 
	CString  v_item_ename = "";
	CString c_sql_condition = "";


	try
	{
		blkNum = bcls_rec->Tables.IndexOf("MMSM50A");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM50A");
		}
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm60.Reset();
			tmmsm60.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm60.TrimOrBlank();
			//if (tmmsm60.QueryCount("BUNKER_NO") == 1)
			//{
			//	sprintf(s.msg, "物料代码重复");
			//	throw CApplicationException(-1, s.msg, log.Location);
			//};
			//if (tmmsm60["BUNKER_NO"].ToString().Trim() == "")
			//{
			//	sprintf(s.msg, "物料代码不能为空"); //系统错误信息
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			Log::Info("", __FUNCTION__, "BUNKER_NO   =[{0}]", tmmsm60["BUNKER_NO"].ToString());
			Log::Info("", __FUNCTION__, "MAT_SIMPLE_ENAME   =[{0}]", tmmsm60["MAT_SIMPLE_ENAME"].ToString());
			tmmsm60["REC_CREATE_TIME"] = dateNow;
			tmmsm60["REC_CREATOR"] = s.userid;
			//前台查的BACK_C5 后台存的MAT_SIMPLE_ENAME，这里同时保存
			tmmsm60["BACK_C5"] = tmmsm60["MAT_SIMPLE_ENAME"];
			tmmsm60.Print();
			tmmsm60.Update("REC_CREATE_TIME,REC_CREATOR,MAT_CODE,MAT_NAME,UPPER_LIMIT_VALUE,GM_VALUE,STOCK_WT_WARN,MAT_SIMPLE_ENAME,BACK_C5,CO_BUNKER","BUNKER_NO");
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