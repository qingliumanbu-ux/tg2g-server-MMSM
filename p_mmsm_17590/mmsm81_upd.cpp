/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:     2023-09-22
Description: 质检批成分管理-计量单绑定质检批号
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/
//存表到集控大屏
int f_mmsmlcjk_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm81_upd)

int f_mmsm81_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int doFlag_cf = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString qualityBatchNo = "";
	CString weighNo = "";
	int TotalRecordCount = 0;
	CString v_quality_batch_no = "";
	CString v_cr = "";
	CString v_ni = "";
	CDecimal v_cr_value = 0;
	CDecimal v_ni_value = 0;
	int blkNum = 0;
	CString  now = CDateTime::Now().ToString("yyyyMMddHHmmss");
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CModel tmmsm81("TMMSM81");
	CModel tmmsm81_s("TMMSM81_S");
	CModel tmmsm81_s1("TMMSM81_S");
	CModel tmmsm81a("TMMSM81");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm89("TMMSM89");
	CModel tmmsm81al("TMMSM81AL");
	CModel tmmsm50("TMMSM50");
	

	try
	{
		//调用发送电文
		Log::Trace("", __FUNCTION__, "集控大屏=[{0}]", v_quality_batch_no);
		blkNum = bcls_rec->Tables.IndexOf("JKDP");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("JKDP");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("MAT_ID"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "MAT_ID");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("DESCRIPTION"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "DESCRIPTION");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("CR"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_DECIMAL, "CR");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("NI"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_DECIMAL, "NI");
		}
		if (!bcls_rec->Tables["JKDP"].Columns.Contains("TYPE"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "TYPE");
		}
		bcls_rec->Tables["JKDP"].Rows.Add();

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm81.Reset();
			tmmsm81.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm81["QUALITY_BATCH_NO"] = bcls_rec->Tables[0].Rows[0]["QUALITY_BATCH_NO"].ToString();
			if ("" == tmmsm81["WEIGH_NO"].ToString().Trim())
			{
				strcpy(s.msg, "磅单号不能为空!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			/*if ("" == tmmsm81["QUALITY_BATCH_NO"].ToString().Trim())
			{
				strcpy(s.msg, "质检批号不能为空!");
				throw CApplicationException(-1, s.msg, log.Location);
			}*/

			//20250206wcm
			tmmsm50["MAT_CODE"] = tmmsm81["MAT_CODE"];
			if (tmmsm50.QueryCount("MAT_CODE") == 1)
			{
				CString mat_code = "";
				mat_code = tmmsm50["MAT_CODE"];
				CString mat_type = Db::QueryCString("select mat_type from tmmsm50 where mat_code='" + mat_code + "'");
				Log::Trace(" ", __FUNCTION__, "mat_type = [{0}]", mat_type);
				if (mat_type.Trim() == "")
				{
					strcpy(s.msg, "物料编码" + mat_code + "的物料类型不能为空,请先配置!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			
		
			if (bcls_rec->Tables[0].Columns.Contains("CONN_QUALITY_BATCH_NO"))
			{
				tmmsm81["QUALITY_BATCH_NO"] = bcls_rec->Tables[0].Rows[0]["CONN_QUALITY_BATCH_NO"].ToString();
			}

			sqlstr = "  SELECT  DISTINCT MAT_CODE  FROM tmmsm81al where QUALITY_BATCH_NO = @QUALITY_BATCH_NO ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("QUALITY_BATCH_NO", tmmsm81["QUALITY_BATCH_NO"]);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				if (cmd_inq.GetString(1) != tmmsm81["MAT_CODE"].ToString())
				{
					strcpy(s.msg, "关联质检批号物料代码必须一致");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();
			

			sqlstr = " SELECT SUM(ELM_VALUE) ELM_VALUE FROM TMMSM81AL WHERE 1=1  and QUALITY_BATCH_NO =@QUALITY_BATCH_NO";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("QUALITY_BATCH_NO", tmmsm81["QUALITY_BATCH_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				if (cmd_inq.GetDecimal(1) <95 || cmd_inq.GetDecimal(1) >100)
				{
					strcpy(s.msg, "关联质检批号时成分总数值必须在95-100之间");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();
			
			/*if ("" == tmmsm81["QUALITY_BATCH_NO"].ToString().Trim())
			{
			strcpy(s.msg, "质检批号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
			}*/

			tmmsm81["COMBINE_YN"] = "1";
			tmmsm81["REC_REVISE_TIME"] = now;
			tmmsm81["REC_REVISOR"] = s.userid;
			tmmsm81.Update("REC_REVISE_TIME,REC_REVISOR,QUALITY_BATCH_NO,COMBINE_YN", "WEIGH_NO");
			tmmsm81_s1.CopyFrom(tmmsm81);
			tmmsm81_s1.Update("REC_REVISE_TIME,REC_REVISOR,QUALITY_BATCH_NO,COMBINE_YN", "WEIGH_NO");

			tmmsm81a["WEIGH_NO"] = tmmsm81["WEIGH_NO"];
			tmmsm81_s["WEIGH_NO"] = tmmsm81["WEIGH_NO"];
			if (tmmsm81a.QueryCount("WEIGH_NO")==1)
			{
				tmmsm81a.Query("WEIGH_NO");
				if (tmmsm81a["FORM_EDIT_FLAG"].ToString().Trim()!="1")
				{
					tmmsm85["BUNKER_NO"] = tmmsm81a["BUNKER_NO"];
					tmmsm85["WEIGH_NO"] = tmmsm81a["WEIGH_NO"];
					tmmsm85["QUALITY_BATCH_NO"] = tmmsm81["QUALITY_BATCH_NO"];
					tmmsm85.Update("QUALITY_BATCH_NO", "WEIGH_NO");
					tmmsm89["WEIGH_NO"] = tmmsm81a["WEIGH_NO"];
					tmmsm89["QUALITY_BATCH_NO"] = tmmsm81["QUALITY_BATCH_NO"];
					tmmsm89.Update("QUALITY_BATCH_NO", "WEIGH_NO");
				}
				else
				{
					strcpy(s.msg, "上料计量单不能关联成分!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
			}
			else if (tmmsm81_s.QueryCount("WEIGH_NO") == 1)
			{
				tmmsm81_s.Query("WEIGH_NO");
				if (tmmsm81_s["FORM_EDIT_FLAG"].ToString().Trim() != "1")
				{
					tmmsm85["BUNKER_NO"] = tmmsm81_s["BUNKER_NO"];
					tmmsm85["WEIGH_NO"] = tmmsm81_s["WEIGH_NO"];
					tmmsm85["QUALITY_BATCH_NO"] = tmmsm81_s["QUALITY_BATCH_NO"];
					tmmsm85.Update("QUALITY_BATCH_NO", "WEIGH_NO");
					tmmsm89["WEIGH_NO"] = tmmsm81a["WEIGH_NO"];
					tmmsm89["QUALITY_BATCH_NO"] = tmmsm81["QUALITY_BATCH_NO"];
					tmmsm89.Update("QUALITY_BATCH_NO", "WEIGH_NO");
				}
			}
			else
			{
				strcpy(s.msg, "计量数据有误!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm81al.Reset();
			tmmsm50.Reset();
			tmmsm50["MAT_CODE"] = bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString();
			Log::Info("", __FUNCTION__, "MAT_CODE1 =[{0}]", bcls_rec->Tables[0].Rows[i]["MAT_CODE"].ToString());
			tmmsm50.Query("MAT_CODE");
			tmmsm81al["QUALITY_BATCH_NO"] = tmmsm81["QUALITY_BATCH_NO"].ToString();
			tmmsm81al["MAT_CODE"] = tmmsm81["MAT_CODE"].ToString();
			
			Log::Info("", __FUNCTION__, "QUALITY_BATCH_NO2 =[{0}]", tmmsm81al["QUALITY_BATCH_NO"].ToString());
			Log::Info("", __FUNCTION__, "MAT_CODE2 =[{0}]", tmmsm81al["MAT_CODE"].ToString());
			
			sqlstr = "  SELECT  ELM_NAME,ELM_VALUE  FROM tmmsm81al where QUALITY_BATCH_NO = @QUALITY_BATCH_NO  AND MAT_CODE = @MAT_CODE AND ELM_NAME ='Cr' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("QUALITY_BATCH_NO", tmmsm81["QUALITY_BATCH_NO"]);
			cmd_inq.Parameters.Set("MAT_CODE", tmmsm81["MAT_CODE"]);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{


				
				v_cr = cmd_inq.GetString(1);
				v_cr_value = cmd_inq.GetDecimal(2);
				
				
			}
			cmd_inq.Close();

			sqlstr = "  SELECT  ELM_NAME,ELM_VALUE  FROM tmmsm81al where QUALITY_BATCH_NO = @QUALITY_BATCH_NO  AND MAT_CODE = @MAT_CODE AND ELM_NAME ='Ni' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("QUALITY_BATCH_NO", tmmsm81["QUALITY_BATCH_NO"]);
			cmd_inq.Parameters.Set("MAT_CODE", tmmsm81["MAT_CODE"]);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{



				v_ni = cmd_inq.GetString(1);
				v_ni_value = cmd_inq.GetDecimal(2);


			}
			cmd_inq.Close();
			
			Log::Info("", __FUNCTION__, "MAT_CODE3 =[{0}]", tmmsm81al["MAT_CODE"].ToString());
			Log::Info("", __FUNCTION__, "ELM_NAME3 =[{0}]", v_cr);
			Log::Info("", __FUNCTION__, "ELM_NAME3 =[{0}]", v_ni);
			
			//if (v_cr == "Cr" || v_ni == "Ni")
			//{
			//	Log::Info("", __FUNCTION__, "111");
			//	//同步集控大屏
			//	bcls_rec->Tables["JKDP"].Rows[0]["MAT_ID"] = tmmsm81["MAT_CODE"].ToString();
			//	bcls_rec->Tables["JKDP"].Rows[0]["DESCRIPTION"] = tmmsm50["MAT_NAME"].ToString();
			//	bcls_rec->Tables["JKDP"].Rows[0]["TYPE"] = tmmsm50["MAT_TYPE"];
			//	
			//	bcls_rec->Tables["JKDP"].Rows[0]["CR"] = v_cr_value;
			//	Log::Info("", __FUNCTION__, "ELM_VALUE1 =[{0}]", bcls_rec->Tables["JKDP"].Rows[0]["CR"].ToString());
		
			//	
			//	bcls_rec->Tables["JKDP"].Rows[0]["NI"] = v_ni_value;
			//	Log::Info("", __FUNCTION__, "ELM_VALUE2 =[{0}]", bcls_rec->Tables["JKDP"].Rows[0]["NI"].ToString());
			//}
			//if (v_cr == "Cr" || v_ni == "Ni")
			//{
			//	doFlag_cf = f_mmsmlcjk_snd(bcls_rec, bcls_ret, conn);
			//	if (doFlag_cf < 0)
			//	{
			//		//Log::Trace("", __FUNCTION__, "== f_mmsmlcjk_snd[0] ==", s.msg);

			//		//throw CApplicationException(-1, s.msg, log.Location);
			//	}
			//}
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
