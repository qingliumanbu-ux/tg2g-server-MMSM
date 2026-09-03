/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   sw
Version:    1.0
Date:     2014-06-03 17:13:56
Description: 新增质检批
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <para>主调用函数：			                            </para>
/// </summary>
/// <param name=" ">     </param>
/// <param name=" ">                </param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口


BM2F_ENTERACE(mmsm81alkc_upd)


int f_mmsm81alkc_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString matCode = "";
	CString procDiv = "";
	CString qualityBatch = "";
	CString bunker_no = "";
	CString weigh_no = "";
	CString v_lot_no = "";
	CString v_factory_div = "";
	CDecimal elm_value = 0;
	CString   elm_code = "";
	CString	  msgstr = "";
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CModel tmmsm81ah("TMMSM81AH");
	CModel tmmsm81al("TMMSM81AL");
	CModel tmmsm81("TMMSM81");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm50("TMMSM50");
	CDbCommand cmd_sql(conn);
	try
	{
		blkNum = bcls_rec->Tables.IndexOf("QM_ELE");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("QM_ELE");
		}

		procDiv = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		matCode = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
		qualityBatch = bcls_rec->Tables[0].Rows[0]["QUALITY_BATCH_NO"].ToString();
		weigh_no = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString();
		bunker_no = bcls_rec->Tables["QM_ELE1"].Rows[0]["BUNKER_NO"].ToString();

		Log::Info("", __FUNCTION__, "count =[{0}]", bcls_rec->Tables.get_Count());
		/*if (true)
		{

		}*/

		tmmsm85["QUALITY_BATCH_NO"] = qualityBatch;
		tmmsm85["MAT_CODE"] = matCode;
		tmmsm85["BUNKER_NO"] = bunker_no;

		tmmsm85.Update("QUALITY_BATCH_NO","MAT_CODE,BUNKER_NO");

		tmmsm81ah["MAT_CODE"] = matCode;
		tmmsm81ah["QUALITY_BATCH_NO"] = qualityBatch;
		tmmsm81ah["WEIGH_NO"] = weigh_no;
		tmmsm81["WEIGH_NO"] = weigh_no;
		tmmsm81["QUALITY_BATCH_NO"] = qualityBatch;
		Log::Info("", __FUNCTION__, "weigh_no =[{0}]", weigh_no);
		Log::Info("", __FUNCTION__, "QUALITY_BATCH_NO =[{0}]", qualityBatch);
		if (procDiv == "I")
		{
			if (0 < tmmsm81ah.QueryCount("MAT_CODE,QUALITY_BATCH_NO"))
			{
				tmmsm81ah.Delete("MAT_CODE,QUALITY_BATCH_NO");
			}
			tmmsm50["MAT_CODE"] = matCode;
			tmmsm50.Query("MAT_CODE");
			tmmsm81ah["MAT_NAME"] = tmmsm50["MAT_NAME"];
			Log::Info("", __FUNCTION__, "MAT_NAME =[{0}]", tmmsm81ah["MAT_NAME"].ToString());
			tmmsm81ah["REC_CREATE_TIME"] = dateNow;
			tmmsm81ah["REC_CREATOR"] = s.userid;
			tmmsm81ah["STATUS"] = "0";
			Log::Info("", __FUNCTION__, "0MAT_NAME =[{0}]", tmmsm81ah["MAT_NAME"].ToString());
			tmmsm81ah.Insert();
			Log::Info("", __FUNCTION__, "2MAT_NAME =[{0}]", tmmsm81ah["MAT_NAME"].ToString());

			//再点击 关联质检批的时候 更新81表
			//tmmsm81["REC_CREATOR"] = s.userid;
			//tmmsm81["COMBINE_YN"] = "1";
			//tmmsm81.Update("QUALITY_BATCH_NO,COMBINE_YN", "WEIGH_NO");


			//更新成分表质检批号
			tmmsm81al["QUALITY_BATCH_NO"] = qualityBatch;
			tmmsm81al["MAT_CODE"] = matCode;
			tmmsm81al["WEIGH_NO"] = weigh_no;
			Log::Info("", __FUNCTION__, "QM_ELEN_UM =[{0}]", bcls_rec->Tables["QM_ELE"].Rows.get_Count());
			//for (int i = 0; i < bcls_rec->Tables["QM_ELE"].Rows.get_Count(); i++)
			//{
			//	//tmmsm81al["ELM_NAME"] = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_NAME"].ToString();
			//	tmmsm81al["ELM_NAME"] = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_NAME"].ToString();
			//	if (0 < tmmsm81al.QueryCount("MAT_CODE,ELM_NAME,WEIGH_NO"))
			//	{
			//		Log::Info("", __FUNCTION__, "3MAT_NAME =[{0}]", tmmsm81ah["MAT_NAME"].ToString());
			//		//新增成份
			//		tmmsm81al["REC_REVISE_TIME"] = dateNow;
			//		tmmsm81al["REC_REVISOR"] = s.userid;
			//		tmmsm81al.Update("QUALITY_BATCH_NO,REC_REVISE_TIME,REC_REVISOR", "MAT_CODE,ELM_NAME,WEIGH_NO");
			//	}
			//}

			//else
			{
				Log::Info("", __FUNCTION__, "QM_ELEN_UM =[{0}]", bcls_rec->Tables["QM_ELE"].Rows.get_Count());
				for (int i = 0; i <bcls_rec->Tables["QM_ELE"].Rows.get_Count(); i++)
				{
					elm_value = elm_value + bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToDecimal();
					if (bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToDecimal() <= 0)
					{
						strcpy(s.msg, "成分不能小等于0!");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					elm_code = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_CODE"].ToString();
					for (int j = i + 1; j < bcls_rec->Tables["QM_ELE"].Rows.get_Count(); j++)	//判断没有重复的元素
					{
						if (elm_code == bcls_rec->Tables["QM_ELE"].Rows[j]["ELM_CODE"].ToString())
						{
							msgstr = msgstr + "【" + bcls_rec->Tables["QM_ELE"].Rows[j]["ELM_NAME"].ToString() + "】";
							break;
						}
					}
				}
				if (msgstr != "")
				{
					strcpy(s.msg, "成分" + msgstr + "存着重复！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (elm_value<95 || elm_value>100)
				{
					strcpy(s.msg, "成分数据之和必须大于等于95%!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//复制成分
				for (int i = 0; i <bcls_rec->Tables["QM_ELE"].Rows.get_Count(); i++)
				{
					tmmsm81al.Reset();
					tmmsm81al["MAT_CODE"] = matCode;
					tmmsm81al["MAT_NAME"] = tmmsm50["MAT_NAME"];

					tmmsm81al["REC_CREATE_TIME"] = dateNow;
					tmmsm81al["REC_CREATOR"] = s.userid;
					tmmsm81al["WEIGH_NO"] = weigh_no;
					tmmsm81al["QUALITY_BATCH_NO"] = qualityBatch;

					tmmsm81al["ELM_NAME"] = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_NAME"].ToString();
					tmmsm81al["ELM_CODE"] = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_CODE"].ToString();
					tmmsm81al["ELM_VALUE"] = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToString();


					tmmsm81al.Insert();
				}



			}
		}
		else
		{
			tmmsm81ah.Delete("MAT_CODE,QUALITY_BATCH_NO,WEIGH_NO");

			if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV_DETAIL"))
			{
				if (bcls_rec->Tables[0].Rows[0]["PROC_DIV_DETAIL"].ToString() == "D")
				{
					tmmsm81al["MAT_CODE"] = matCode;
					tmmsm81al["QUALITY_BATCH_NO"] = qualityBatch;
					tmmsm81al.Delete("MAT_CODE,QUALITY_BATCH_NO");

				}
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
