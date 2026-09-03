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
/// <para>数据库表：TMMSM58(供应商管理表)					</para>
/// <para>主调用函数：			                            </para>
/// </summary>
/// <param name=" ">     </param>
/// <param name=" ">                </param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口


BM2F_ENTERACE(mmsm58_pro)


int f_mmsm58_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_proc_div = "";
	CString v_factory_div = "";
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CModel tmmsm58("TMMSM58");
	CDbCommand cmd_sql(conn);
	try
	{
;
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm58.Reset();
			tmmsm58.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm58.TrimOrBlank();

			Log::Trace("", __FUNCTION__, "SUPPLIER_ID =[{0}]", tmmsm58["SUPPLIER_ID"].ToString());

			if (v_proc_div == "I")
			{
				tmmsm58["REC_CREATE_TIME"] = dateNow;
				tmmsm58["REC_CREATOR"] = s.userid;
				tmmsm58.Insert();
			}
			else if (v_proc_div == "U")
			{
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM	tmmsm58 "
					"   WHERE  SUPPLIER_ID	= @SUPPLIER_ID";

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("SUPPLIER_ID", tmmsm58["SUPPLIER_ID"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm58["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsm58["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsm58["REC_REVISE_TIME"] = dateNow;
				tmmsm58["REC_REVISOR"] = s.userid;

				tmmsm58.Delete();
				tmmsm58.Insert();

			}
			else if (v_proc_div == "D")
			{
				tmmsm58.Delete();
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
