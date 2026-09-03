/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:songwei
Date:2023-11-28
Version:1.0
Description: 接收资源系统
质量数据**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
#include <regex>
/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */



// service入口
BM2F_ENTERACE_TELE(cm_c02102_rcv)

int f_cm_c02102_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString div_flag = "";
	CString elm_code = "";
	CString quality_batch_no = "";
	CDecimal elm_value = 0;
	CModel tmmsm81ah("TMMSM81AH");
	CModel tmmsm81al("TMMSM81AL");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		div_flag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();			
		tmmsm81ah.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm81al.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", __FUNCTION__, "QUALITY_BATCH_NO=[{0}]", tmmsm81al["QUALITY_BATCH_NO"].ToString());
		if ("" == div_flag.Trim())
		{
			strcpy(s.msg, "处理标记不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
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
		if (0 == tmmsm81al["ANALYSE_ITEM_COUNT"].ToDecimal())
		{
			strcpy(s.msg, "检验项目数为0!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//资源传 原带和复验成分的质检批号一样，此处在质检批号前加一位（Y-原带 F-复验） 区分
		//成分标记 REMARK_1
		if ("Y" == tmmsm81al["REMARK_1"].ToString())
		{
			tmmsm81al["QUALITY_BATCH_NO"] = "Y" + tmmsm81al["QUALITY_BATCH_NO"].ToString();
		}
		else if ("F" == tmmsm81al["REMARK_1"].ToString())
		{
			tmmsm81al["QUALITY_BATCH_NO"] = "F" + tmmsm81al["QUALITY_BATCH_NO"].ToString();
		}
		quality_batch_no = tmmsm81al["QUALITY_BATCH_NO"].ToString();

		if (0 < tmmsm81al.QueryCount("QUALITY_BATCH_NO,MAT_CODE"))
		{
			tmmsm81al.Delete("QUALITY_BATCH_NO,MAT_CODE");
		}
		for (int i = 0; i < tmmsm81al["ANALYSE_ITEM_COUNT"].ToDecimal(); i++)
		{
			if (30 <= i)
			{
				Log::Trace("", __FUNCTION__, "最后一个检验项目");
				break;
			}
			tmmsm81al.Reset();
			tmmsm81al.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			tmmsm81al["QUALITY_BATCH_NO"] = quality_batch_no;
			tmmsm81al["FACTORY_DIV"] = "S2N";
			tmmsm81al["REC_CREATE_TIME"] = datetime;
			tmmsm81al["REC_CREATOR"] = s.userid;
		

			elm_code = bcls_rec->Tables[0].Rows[i]["ELM_CODE"].ToString();
			//if (elm_code.GetLength()>3)tmmsm81al["ELM_CODE"] = elm_code.SubstringNE(elm_code.GetLength() - 3);
			tmmsm81al["ELM_NAME"] = bcls_rec->Tables[0].Rows[i]["ELM_NAME"].ToString();
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

			tmmsm81al["ANALYSE_DATA_TYPE"] = bcls_rec->Tables[0].Rows[i]["ANALYSE_DATA_TYPE"].ToString();
			tmmsm81al["ANALYSE_MEASURE_UNIT"] = bcls_rec->Tables[0].Rows[i]["ANALYSE_MEASURE_UNIT"].ToString();
			tmmsm81al["ELM_VALUE"] = bcls_rec->Tables[0].Rows[i]["ELM_VALUE"].ToDecimal();
			tmmsm81al["FLAG1"] = "1";
			//CString elm_name = tmmsm81al["ELM_NAME"].ToString();
			//string dest = string((const char*)elm_name);
			//regex pattern("^[A-Za-z]+$");
			//bool is_match = regex_search(dest, pattern);
			//if (is_match)
			//{
			//	//Log::Info("", __FUNCTION__, "全是英文");
			//	tmmsm81al.TrimOrBlank();
			//	tmmsm81al.Insert();
			//}	

			if (tmmsm81al["ELM_CODE"].ToString().Trim() != "")  //用来判断是否是组合元素
			{
				tmmsm81al.TrimOrBlank();
				tmmsm81al.Insert();
			} 
		}
		//新增质检批头信息
		tmmsm81ah.Reset();
		tmmsm81ah.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm81ah["QUALITY_BATCH_NO"] = quality_batch_no;
		if (0<tmmsm81ah.QueryCount("MAT_CODE,QUALITY_BATCH_NO"))
		{
			tmmsm81ah.Delete("MAT_CODE,QUALITY_BATCH_NO");
		}
		//FLAG1 代表是资源传的成分
		tmmsm81ah["FLAG1"] = "1";
		tmmsm81ah["REC_CREATE_TIME"] = datetime;
		tmmsm81ah["REC_CREATOR"] = s.userid;
		if (bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME"].ToString().Trim() == "")
		{
			tmmsm81ah["DATI_MSG_SENT"] = datetime;
		}
		else
		{
			tmmsm81ah["DATI_MSG_SENT"] = bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME"].ToString();
		}		
		tmmsm81ah["STATUS"] = "0";
		tmmsm81ah.TrimOrBlank();
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
			tmmsm81al.MergeFrom(bcls_rec->Tables[0].Rows[0]);
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
					tmmsm81al["FLAG1"] = "1";
					tmmsm81al.TrimOrBlank();
					tmmsm81al.Insert();
				}
				
			}
			cmd_inq.Close();
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
