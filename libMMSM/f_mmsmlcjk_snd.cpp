/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-11 14:13:28
Description: 集控大屏数据添加
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsmlcjk_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_insert(conn);
	CString datetime(" ");
	CString table_name = " ";
	CString v_proc_div = " ";
	CString v_mat_id = " ";
	CDecimal id = 0;
	CModel tmmsm81ah("TMMSM81AH");
	CModel tmmsm81al("TMMSM81AL");
	CModel tmmsm81("TMMSM81");
	CModel tmmsm50("TMMSM50");
	
	CModel mat("ZJ_MAT_ELEMENT");
	CModel scrap("ZJ_SCRAP_ELEMENT");//废钢
	

	try
	{
		v_mat_id = bcls_rec->Tables["JKDP"].Rows[0]["MAT_ID"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "MAT_ID=[{0}]", v_mat_id);
		
		
		for (int i = 0; i < bcls_rec->Tables["JKDP"].Rows.get_Count(); i++)
		{
			mat.Reset();

			Log::Info("", __FUNCTION__, "111");

			//物料代码
			mat["MAT_ID"] = bcls_rec->Tables["JKDP"].Rows[i]["MAT_ID"].ToString();
			//物料名称
			mat["DESCRIPTION"] = bcls_rec->Tables["JKDP"].Rows[i]["DESCRIPTION"].ToString();
			//ELM_VALUE
			mat["CR"] = bcls_rec->Tables["JKDP"].Rows[i]["CR"].ToDecimal();
			mat["NI"] = bcls_rec->Tables["JKDP"].Rows[i]["NI"].ToDecimal();
			//物料类型
			mat["TYPE"] = bcls_rec->Tables["JKDP"].Rows[i]["TYPE"].ToString();
			Log::Trace("", __FUNCTION__, "MAT_ID=[{0}]", mat["MAT_ID"].ToString());
			Log::Trace("", __FUNCTION__, "TYPE=[{0}]", mat["TYPE"].ToString());
			if (mat.QueryCount("MAT_ID"))
			{
				mat.Delete("MAT_ID");
			}
			if (mat["TYPE"].ToString() == "2")
			{
				Log::Info("", __FUNCTION__, "222");
				scrap["MAT_ID"] = bcls_rec->Tables["JKDP"].Rows[i]["MAT_ID"].ToString();
				//物料名称
				scrap["DESCRIPTION"] = bcls_rec->Tables["JKDP"].Rows[i]["DESCRIPTION"].ToString();
				//ELM_VALUE
				scrap["CR"] = bcls_rec->Tables["JKDP"].Rows[i]["CR"].ToDecimal();
				scrap["NI"] = bcls_rec->Tables["JKDP"].Rows[i]["NI"].ToDecimal();
				//物料类型
				scrap["TYPE"] = bcls_rec->Tables["JKDP"].Rows[i]["TYPE"].ToString();
				mat.Insert();
				scrap.Delete();
				scrap.Insert();
			}
			else{
				mat.Insert();
			}


		}
			
		
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


