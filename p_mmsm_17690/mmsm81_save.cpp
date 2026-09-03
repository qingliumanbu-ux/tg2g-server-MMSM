/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      郑强强
Version:     1.0
Date:        2023-01-12 13:44:44
Description: 单表通用保存-信融专用后台
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(mmsm81_save)


int f_mmsm81_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString sqlstr = " ";
	CString table_name = " ";
	CString msgstr = "提示信息:";	//提示信息。
	int proc_sum = 0;				//操作总数
	CModel tmmsm85 = CModel("TMMSM85");
	CModel tmmsm81 = CModel("TMMSM81");
	CModel tmmsm81_bak = CModel("TMMSM81_BAK");
	CModel tmmsm60 = CModel("TMMSM60");
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inq(conn);
	CDataTable temp_table;
	try
	{
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		table_name = "TMMSM81";
			//bcls_rec->Tables["PARA"].Rows[0]["TABLE_NAME"].ToString();

		//先删除库存数据
		

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				tmmsm81.Reset();
				tmmsm81.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				tmmsm81_bak.Reset();
				tmmsm81_bak.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				tmmsm81["REC_CREATOR"] = "QC";
				tmmsm81["REC_CREATE_TIME"] = nowTime;
				tmmsm81["FACTORY_DIV"] = "LG1";
				tmmsm81["RECEIVING_STATUS"] = " ";
				/*tmmsm81["STATION_NO"] = tmmsm81["STATION_NO"].ToString().SubstringNE(0,2);*/
				tmmsm81_bak["REC_CREATOR"] = "QC";
				tmmsm81_bak["REC_CREATE_TIME"] = nowTime;
				tmmsm81_bak["FACTORY_DIV"] = "LG1";
				tmmsm81_bak["RECEIVING_STATUS"] = " ";
				tmmsm81_bak.TrimOrBlank();
				tmmsm81_bak.Insert();
				if (tmmsm81.Query("WEIGH_NO"))
				{
					Log::Trace("", "", "新增开始,MAT_CODE=[{0}];WEIGH_NO=[{1}],BUNKER_NO=[{2}]", tmmsm81["MAT_CODE"].ToString(), tmmsm81["WEIGH_NO"].ToString(), tmmsm81["BUNKER_NO"].ToString());
					continue;
				}

				tmmsm81.Delete("WEIGH_NO");
				
				//Log::Trace("", "", "新增开始,count=[{0}];[{1}]", tmmsm85["WEIGH_NO"].ToString(), i);
				tmmsm81.TrimOrBlank();
				tmmsm81.Insert();				
			} 
		}

		

		

		msgstr += msgstr.Format("%d条记录操作成功。", bcls_rec->Tables["ADD"].Rows.get_Count());
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


