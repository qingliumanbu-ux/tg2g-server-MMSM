/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面维护
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */
// service入口
BM2F_ENTERACE(mmsmzd_pro)

int f_mmsmzd_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int blkNum = 0;
	int doFlag = 0;
	CString s_formname = "";
	CString v_proc_div = "";
	CString sqlstr = "";
	CString sqlstr_temp = "";
	CString v_table_name = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
	
		v_table_name = bcls_rec->Tables["PARA"].Rows[0]["TABLE_NAME"].ToString();
		CModel tmmsmyl(v_table_name);
		Log::Info("", __FUNCTION__, "v_table_name =[{0}]", v_table_name);

		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "--------------DEL-------------");
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				tmmsmyl.Reset();
				tmmsmyl.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Delete("CODE");
				
			}
		}
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "--------------ADD-------------");
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				tmmsmyl.Reset();
				tmmsmyl.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				if (bcls_rec->Tables["ADD"].Rows[i]["CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, "代码不允许为空！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsmyl.QueryCount("CODE") == 1)
				{
					strcpy(s.msg, "代码" + tmmsmyl["CODE"].ToString()+"已存在！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsmyl["REC_CREATE_TIME"] = nowTime;
				tmmsmyl["REC_CREATOR"] = s.userid;
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Insert();
			}
		}
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "--------------UPD-------------");
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				tmmsmyl.Reset();
				tmmsmyl.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM 	" + v_table_name + " "
					"   WHERE  CODE	= @CODE   ";

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("CODE", tmmsmyl["CODE"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsmyl["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsmyl["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsmyl["REC_REVISE_TIME"] = nowTime;
				tmmsmyl["REC_REVISOR"] = s.userid;

				tmmsmyl.Delete("CODE");
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Insert();
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
