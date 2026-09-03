/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      Simon Li
Version:     1.0
Date:        2024-01-17
Description:质量数据接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

int f_t8ed02_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_getSeqNextValue(CString SEQ_NAME, CString& SEQ_VALUE, CDbConnection * conn);

BM2F_ENTERACE_TELE(cm_b02104_rcv)

int f_cm_b02104_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{

	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CDecimal itemCount = 0;
	CString sqlstr = "";
	CString div_flag = "";
	CString elm_code = "";
	CDecimal elm_value = 0;
	CModel tmmsm81ah("TMMSM81AH");
	CModel tmmsm81al("TMMSM81AL");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		return 0;	//铁水成分不需要接收20240518


		div_flag = bcls_rec->Tables["B02104"].Rows[0]["DEAL_FLAG"].ToString();
		itemCount = bcls_rec->Tables["B02104"].Rows[0]["ANALYSE_ITEM_NUM"].ToDecimal();
		tmmsm81al["QUALITY_BATCH_NO"] = bcls_rec->Tables["B02104"].Rows[0]["SAMPLE_NO"].ToString();
		tmmsm81al["MAT_CODE"] = bcls_rec->Tables["B02104"].Rows[0]["MAT_CODE"].ToString();
		tmmsm81al["LOT_NO"] = bcls_rec->Tables["B02104"].Rows[0]["BATCH_NO"].ToString();
		Log::Trace("", __FUNCTION__, "QUALITY_BATCH_NO=[{0}]", tmmsm81al["QUALITY_BATCH_NO"].ToString());
		
		if ("" == tmmsm81al["QUALITY_BATCH_NO"].ToString().Trim())
		{
			strcpy(s.msg, "质检单号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == tmmsm81al["MAT_CODE"].ToString().Trim())
		{
			strcpy(s.msg, "物料代码不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (0 == itemCount)
		{
			strcpy(s.msg, "检验项目数为0!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (0 < tmmsm81al.QueryCount("QUALITY_BATCH_NO,MAT_CODE"))
		{
			tmmsm81al.Delete("QUALITY_BATCH_NO,MAT_CODE");
		}
		Log::Trace("", __FUNCTION__, "itemCount=[{0}]", itemCount);
		for (int i = 0; i < itemCount; i++)
		{
			if (50 <= i)
			{
				Log::Trace("", __FUNCTION__, "最后一个检验项目");
				break;
			}
			tmmsm81al.Reset();
			tmmsm81al["QUALITY_BATCH_NO"] = bcls_rec->Tables["B02104"].Rows[0]["SAMPLE_NO"].ToString();
			tmmsm81al["MAT_CODE"] = bcls_rec->Tables["B02104"].Rows[0]["MAT_CODE"].ToString();
			tmmsm81al["LOT_NO"] = bcls_rec->Tables["B02104"].Rows[0]["BATCH_NO"].ToString();
			tmmsm81al["FACTORY_DIV"] = "S2N";
			tmmsm81al["REC_CREATE_TIME"] = datetime;
			tmmsm81al["REC_CREATOR"] = s.userid;
			elm_code = bcls_rec->Tables["B02104_1"].Rows[i]["ANALYSE_ITEM_CODE"].ToString(); 			
			tmmsm81al["ELM_NAME"] = bcls_rec->Tables["B02104_1"].Rows[i]["ANALYSE_ITEM_CNAME"].ToString();
			tmmsm81al["ANALYSE_DATA_TYPE"] = bcls_rec->Tables["B02104_1"].Rows[i]["ANALYSE_DATA_TYPE"].ToString();
			tmmsm81al["ELM_VALUE"] = bcls_rec->Tables["B02104_1"].Rows[i]["ANALYSE_ITEM_VALUE"].ToDecimal();
			tmmsm81al["FLAG1"] = "2";
			//根据成份值取MES的成份代码
			sqlstr = " SELECT CODE  FROM TEP0002 WHERE CODE_DESC_1_CONTENT=@elm_name and  CODE_CLASS='MMLC02' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("elm_name", tmmsm81al["ELM_NAME"].ToString());
			cmd_inq.ExecuteReader();
			tmmsm81al["ELM_CODE"] = " ";
			if (cmd_inq.Read())
			{
				tmmsm81al["ELM_CODE"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			tmmsm81al.TrimOrBlank();
			if (tmmsm81al["ELM_CODE"].ToString().Trim() != "")  //用来判断是否是组合元素
			{
				tmmsm81al.Insert();
			}

		}
		//新增质检批头信息
		tmmsm81ah["QUALITY_BATCH_NO"] = bcls_rec->Tables["B02104"].Rows[0]["SAMPLE_NO"].ToString();
		tmmsm81ah["MAT_CODE"] = bcls_rec->Tables["B02104"].Rows[0]["MAT_CODE"].ToString();
		if (0<tmmsm81ah.QueryCount("MAT_CODE,QUALITY_BATCH_NO"))
		{
			tmmsm81ah.Delete("MAT_CODE,QUALITY_BATCH_NO");
		}
		//FLAG1=2 代表是铁区传的成分 
		tmmsm81ah["LOT_NO"] = bcls_rec->Tables["B02104"].Rows[0]["BATCH_NO"].ToString();
		tmmsm81ah["FLAG1"] = "2";
		tmmsm81ah["REC_CREATE_TIME"] = datetime;
		tmmsm81ah["REC_CREATOR"] = s.userid;
		tmmsm81ah["DATI_MSG_SENT"] = datetime;
		tmmsm81ah["STATUS"] = "0";
		tmmsm81ah.Insert();

		//判断成份值是否够95，如果不够则根据物料编码凑够到95%

		sqlstr = " select sum(elm_value) from tmmsm81al"
			" where 1=1"
			"  and MAT_CODE = @mat_code "
			" and QUALITY_BATCH_NO = @quality_batch_no"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("mat_code", tmmsm81ah["MAT_CODE"].ToString());
		cmd_inq.Parameters.Set("quality_batch_no", tmmsm81ah["QUALITY_BATCH_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			elm_value = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();
		if (elm_value < 95)
		{
			Log::Info("", __FUNCTION__, "elm_value =[{0}]", elm_value);
			tmmsm81al.Reset();
			//tmmsm81al.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			sqlstr = " select ELM_CODE,ELM_NAME"
				" from tmmsmw5"
				" where 1=1"
				" and VALIDE_FLAG='1'"
				" and mat_code = @mat_code"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_code", tmmsm81ah["MAT_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm81al["FACTORY_DIV"] = "S2N";
				tmmsm81al["REC_CREATE_TIME"] = datetime;
				tmmsm81al["REC_CREATOR"] = s.userid;
				tmmsm81al["LOT_NO"] = bcls_rec->Tables["B02104"].Rows[0]["BATCH_NO"].ToString();
				//tmmsm81al["ANALYSE_DATA_TYPE"] = bcls_rec->Tables["B02104_1"].Rows[i]["ANALYSE_DATA_TYPE"].ToString();
				tmmsm81al["MAT_CODE"] = tmmsm81ah["MAT_CODE"].ToString();
				tmmsm81al["MAT_NAME"] = tmmsm81ah["MAT_NAME"].ToString();
				tmmsm81al["QUALITY_BATCH_NO"] = tmmsm81ah["QUALITY_BATCH_NO"].ToString();
				tmmsm81al["ELM_CODE"] = cmd_inq.GetString(1);
				tmmsm81al["ELM_NAME"] = cmd_inq.GetString(2);
				//Log::Info("", __FUNCTION__, "ELM_CODE =[{0}]", tmmsm81al["ELM_CODE"].ToString());
				if (tmmsm81al.QueryCount("QUALITY_BATCH_NO,ELM_CODE,MAT_CODE") > 0)
				{
					sqlstr = " update tmmsm81al set elm_value = elm_value+@dif_wt"
						" where 1=1"
						" and ELM_CODE=@elm_code"
						"  and MAT_CODE = @mat_code "
						" and QUALITY_BATCH_NO = @quality_batch_no"
						;
					cmd_inq_1.SetCommandText(sqlstr);
					cmd_inq_1.Parameters.Set("mat_code", tmmsm81al["MAT_CODE"].ToString());
					cmd_inq_1.Parameters.Set("quality_batch_no", tmmsm81al["QUALITY_BATCH_NO"].ToString());
					cmd_inq_1.Parameters.Set("elm_code", tmmsm81al["ELM_CODE"].ToString());
					cmd_inq_1.Parameters.Set("dif_wt", 95 - elm_value);
					cmd_inq_1.ExecuteNonQuery();
					cmd_inq_1.Close();

				}
				else
				{
					//Log::Info("", __FUNCTION__, "insert ELM_CODE =[{0}]", tmmsm81al["ELM_CODE"].ToString());
					tmmsm81al["ELM_VALUE"] = 95 - elm_value;
					tmmsm81al["FLAG1"] = "2";
					tmmsm81al.TrimOrBlank();
					tmmsm81al.Insert();
				}

			}
			cmd_inq.Close();
		}

		//质量信息转发倒罐站
		CString matCode = bcls_rec->Tables["B02104"].Rows[0]["MAT_CODE"].ToString();
		CString fid = "";
		if (f_getSeqNextValue("ZL_FID", fid, conn) != 0){
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (matCode == "TS0000"){
			EIClass temp;
			temp.Tables.Add();
			temp.Tables[0].Columns.Add(DT_DECIMAL, "FID");
			temp.Tables[0].Columns.Add(DT_STRING, "BATCH_NO");
			temp.Tables[0].Columns.Add(DT_STRING, "ELE_NAME_01");
			temp.Tables[0].Columns.Add(DT_DECIMAL, "ELE_VALUE_01");
			temp.Tables[0].Columns.Add(DT_STRING, "ELE_NAME_02");
			temp.Tables[0].Columns.Add(DT_DECIMAL, "ELE_VALUE_02");
			temp.Tables[0].Columns.Add(DT_STRING, "ELE_NAME_03");
			temp.Tables[0].Columns.Add(DT_DECIMAL, "ELE_VALUE_03");
			temp.Tables[0].Columns.Add(DT_STRING, "TIME_STAMPS");
			temp.Tables[0].Rows.Add();
			temp.Tables[0].Rows[0]["FID"] = fid;
			temp.Tables[0].Rows[0]["BATCH_NO"] = bcls_rec->Tables["B02104"].Rows[0]["BATCH_NO"].ToString().Trim();
			temp.Tables[0].Rows[0]["TIME_STAMPS"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			for (int i = 0; i < bcls_rec->Tables["B02104_1"].Rows.get_Count(); i++){
				if (bcls_rec->Tables["B02104_1"].Rows[0]["ANALYSE_ITEM_CNAME"].ToString().Trim() == "Si"){
					temp.Tables[0].Rows[0]["ELE_NAME_01"] = "Si";
					temp.Tables[0].Rows[0]["ELE_VALUE_01"] = CDecimal::Parse(bcls_rec->Tables["B02104_1"].Rows[0]["ANALYSE_ITEM_VALUE"].ToString().Trim());
				}
				if (bcls_rec->Tables["B02104_1"].Rows[0]["ANALYSE_ITEM_CNAME"].ToString().Trim() == "P"){
					temp.Tables[0].Rows[0]["ELE_NAME_02"] = "P";
					temp.Tables[0].Rows[0]["ELE_VALUE_02"] = CDecimal::Parse(bcls_rec->Tables["B02104_1"].Rows[0]["ANALYSE_ITEM_VALUE"].ToString().Trim());
				}
				if (bcls_rec->Tables["B02104_1"].Rows[0]["ANALYSE_ITEM_CNAME"].ToString().Trim() == "S"){
					temp.Tables[0].Rows[0]["ELE_NAME_03"] = "S";
					temp.Tables[0].Rows[0]["ELE_VALUE_03"] = CDecimal::Parse(bcls_rec->Tables["B02104_1"].Rows[0]["ANALYSE_ITEM_VALUE"].ToString().Trim());
				}
			}

			if (f_t8ed02_snd(&temp, bcls_ret, conn)){
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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

	cmd_inq.Close();

	return doFlag;
}