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
/// <para>数据库表：tmmsm81v(炼钢原辅料主信息表)					</para>
/// <para>主调用函数：			                                </para>
/// </summary>
/// <param name=" ">     </param>
/// <param name=" ">                </param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口


BM2F_ENTERACE(mmsm81bd_pro)


int f_mmsm81bd_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;
	int  proc_sum = 0;				//操作总数
	CDbCommand cmd_sql(conn); //与DB 建立连接。
	CString msgstr = "提示信息:";	//提示信息。
	CString sqlstr = "";
	CString   v_proc_div = "";
	CString  table_name = "";
	CString c_sql_condition = "";


	try
	{
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		table_name = "TMMSM81V";

		Log::Trace("", "", "获取传入参数...");
		Log::Trace("", "", "传入表名：table_name=[{0}]", table_name);
		Log::Trace("", "", "当前时间：nowTime=[{0}]", nowTime);

		CModel tmmsm81v(table_name);
		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				tmmsm81v.Reset();
				tmmsm81v.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				tmmsm81v["REC_CREATOR"] = s.userid;
				tmmsm81v["REC_CREATE_TIME"] = nowTime;
				tmmsm81v.TrimOrBlank();

				Log::Trace("", "", "11C_ORDERID=[{0}]", tmmsm81v["C_ORDERID"].ToString());
				if (tmmsm81v.Query("C_ORDERID"))
				{
					Log::Trace("", "", "22C_ORDERID=[{0}]", tmmsm81v["C_ORDERID"].ToString());
					msgstr += msgstr.Format("第%d条记录已存在，无法新增。", i + 1);
					continue;
				}

				Log::Trace("", "", "3获取传入参数...");

				sqlstr = "INSERT INTO " + table_name;
				tmmsm81v.Insert();

			}


		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				tmmsm81v.Reset();
				tmmsm81v.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				tmmsm81v.TrimOrBlank();
				if (!tmmsm81v.Query("C_ORDERID")){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法修改。(是否新增？)", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法删除。", i + 1);
					continue;
				}
				else
				{
					tmmsm81v.Delete("C_ORDERID");

					tmmsm81v.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
					tmmsm81v["REC_REVISOR"] = s.userid;
					tmmsm81v["REC_REVISE_TIME"] = nowTime;
					tmmsm81v.TrimOrBlank();
					tmmsm81v.Insert();
				}

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				tmmsm81v.Reset();
				tmmsm81v.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				//if (tmmsm81v.QueryCount(condition) != 1){
				if (!tmmsm81v.Query("C_ORDERID")){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法删除。", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法删除。", i + 1);
					continue;
				}
				sqlstr = "DELETE FROM " + table_name;
				proc_sum += tmmsm81v.Delete("C_ORDERID");
			}
		}

		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);

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