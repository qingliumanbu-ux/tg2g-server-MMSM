/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      sw
Version:     1.0
Date:        2024-05-27 16:59:33
Description: 获取CE系统原料价格
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(mmsm_cejg_get)
int f_tran_json_func(EIClass* blks_in, EIClass * blks_out, CDbConnection* conn);
int f_tableObjectCheck9999(ITableObject2& obj);

int f_mmsm_cejg_get(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);
	EIClass iplat_Tab;
	CModel tmmsmwp("TMMSMWP");
	CString datetime = CDateTime::Now().AddDays(-1).ToString("yyyyMMddHHmmss").SubstringNE(0, 8);
	CString currtime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		Log::Trace("", "", "-----------------执行程序-------------");
		iplat_Tab.Tables.Clear();
		iplat_Tab.Tables.Add();
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "JSON");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "URL");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "COL");
		iplat_Tab.Tables[0].Rows.Add();


		iplat_Tab.Tables[0].Rows[0][0] = "{\"date\":\"" + datetime + "\"}";

		Log::Trace("", "", "-----------------执行最新程序的识别-221-------------");
		iplat_Tab.Tables[0].Rows[0]["URL"] = "http://eplat.tisco.com.cn/tgjy/service/S_SC_21";
		iplat_Tab.Tables[0].Rows[0]["COL"] = "EJKData";
		Log::Trace("", "", "-----------------调用sql代理-------------");

		doFlag = f_tran_json_func(&iplat_Tab, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", "", "[{0}]", doFlag);
		}
		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmwp.Reset();
			tmmsmwp.MergeFrom(bcls_ret->Tables[0].Rows[i]);
			tmmsmwp["DATE_C"] = datetime;
			tmmsmwp["COMM_FMLY_DESC"] = bcls_ret->Tables[0].Rows[i]["MATERIAL_CLASS_NAME"];
			tmmsmwp["MAT_DIVISION_DES"] = bcls_ret->Tables[0].Rows[i]["MATERIAL_MID_NAME"];
			tmmsmwp["MAT_CODE"] = bcls_ret->Tables[0].Rows[i]["MATERIAL_CODE"];
			tmmsmwp["MAT_NAME"] = bcls_ret->Tables[0].Rows[i]["MATERIAL_CODE_NAME"];
			tmmsmwp["UNIT"] = bcls_ret->Tables[0].Rows[i]["UNIT_DESC"];
			tmmsmwp["CKSL"] = bcls_ret->Tables[0].Rows[i]["CKSL"].ToString().SubstringNE(0, bcls_ret->Tables[0].Rows[i]["CKSL"].ToString().Find('.') + 7);
			tmmsmwp["DWJG"] = bcls_ret->Tables[0].Rows[i]["DWJG"].ToString().SubstringNE(0, bcls_ret->Tables[0].Rows[i]["DWJG"].ToString().Find('.') + 7);
			tmmsmwp["ZSJG"] = bcls_ret->Tables[0].Rows[i]["ZSJG"].ToString().SubstringNE(0, bcls_ret->Tables[0].Rows[i]["ZSJG"].ToString().Find('.') + 7);

			//f_tableObjectCheck9999(tmmsmwp);
			tmmsmwp.Delete("MAT_CODE,DATE_C");
			tmmsmwp["REC_CREATE_TIME"] = currtime;
			tmmsmwp["REC_CREATOR"] = s.userid;
			tmmsmwp.TrimOrBlank();//如果有Null
			//TrimOrBlank没生效
			if (tmmsmwp["UNIT"].ToString().Trim()=="")
			{
				tmmsmwp["UNIT"] = " ";
			}
			tmmsmwp.Insert();
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