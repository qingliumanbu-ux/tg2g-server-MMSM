/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      郑强强
Version:     1.0
Date:        2023-01-12 13:44:44
Description: 单表通用保存-信融专用后台
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(mmsm81alqc_save)


int f_mmsm81alqc_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString sqlstr = " ";
	CString table_name = " ";
	CString msgstr = "提示信息:";	//提示信息。
	int proc_sum = 0;				//操作总数

	CDbCommand cmd_sql(conn);
	CDataTable temp_table;
	CModel tmmsm81al = CModel("TMMSM81AL");
	CModel tmmsm81al_bak = CModel("TMMSM81AL_BAK");
	CDbCommand cmd_inq(conn);
	try
	{
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		table_name = "TMMSM81AL";  		

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				tmmsm81al.Reset();
				tmmsm81al.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				tmmsm81al_bak.Reset();
				tmmsm81al_bak.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				tmmsm81al["REC_CREATOR"] = "QC";
				tmmsm81al["REC_CREATE_TIME"] = nowTime;
				tmmsm81al.TrimOrBlank();
				tmmsm81al_bak["REC_CREATOR"] = "QC";
				tmmsm81al_bak["REC_CREATE_TIME"] = nowTime;
				tmmsm81al_bak.TrimOrBlank();
				tmmsm81al_bak.Insert();

				tmmsm81al.Delete("QUALITY_BATCH_NO,MAT_CODE,ELM_NAME");

				tmmsm81al.Insert();
			}	
		}


		sqlstr = "delete from tmmsm81ah"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("nowTime", nowTime);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		sqlstr = "insert into tmmsm81ah(REC_CREATOR,REC_CREATE_TIME,QUALITY_BATCH_NO,MAT_CODE)"
				" select 'QC',@nowTime,QUALITY_BATCH_NO,MAT_CODE"
				" from tmmsm81al"
				" group by QUALITY_BATCH_NO,MAT_CODE"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("nowTime", nowTime);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		sqlstr = 
			" UPDATE TMMSM81AL tm"
			" SET tm.ELM_CODE= (SELECT sm.ELM_CODE FROM  (SELECT CODE ELM_CODE ,CODE_DESC_1_CONTENT  ELM_NAME FROM TEP0002 WHERE CODE_CLASS='MMLC02') sm where sm.ELM_NAME=tm.ELM_NAME  )"
			" where exists"
			" (SELECT 1 FROM  (SELECT CODE ELM_CODE ,CODE_DESC_1_CONTENT  ELM_NAME FROM TEP0002 WHERE CODE_CLASS='MMLC02') sm where sm.ELM_NAME=tm.ELM_NAME  ) AND ELM_CODE=' ' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();


		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);

	}
	catch (CDbException& ex)  //捕获数据库操作异常 
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
		////Log::Warn("", __FUNCTION__, "CDbException: {0}", s.msg);
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
		////Log::Error("", __FUNCTION__, "CApplicationException: {0}", ex.GetMsg());
	}
	catch (CException& ex)
	{
		strncpy(s.sysmsg, (const char*)ex.GetMsg(), sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
		////Log::Fatal("", __FUNCTION__, "CException: {0}", ex.GetMsg());
	}
	return doFlag;
}


