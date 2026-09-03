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


BM2F_ENTERACE(mmsm81al_upd)


int f_mmsm81al_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal elm_value = 0;
	/* 实体类定义 */
	EIClass bcls_rec_mat;
	bcls_rec_mat.Tables[0].set_TableName("MMSM2A");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString proDiv;
	CString weigh_no;
	CString   elm_code = "";
	CString	  msgstr = "";
	CModel tmmsm81("TMMSM81");
	CModel tmmsm81al("TMMSM81AL");
	CModel tmmsm50("TMMSM50");
	CDbCommand cmd_sql(conn);
	try
	{
		blkNum = bcls_rec->Tables.IndexOf("QM_ELE");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("QM_ELE");
		}
		msgstr = "";
		for (int i = 0; i <bcls_rec->Tables["QM_ELE"].Rows.get_Count(); i++)
		{
			elm_value = elm_value + bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToDecimal();
			if (bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToDecimal() <= 0)
			{
				strcpy(s.msg, "成分不能小等于0!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			elm_code = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_CODE"].ToString();
			for (int j = i+1; j < bcls_rec->Tables["QM_ELE"].Rows.get_Count(); j++)	//判断没有重复的元素
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
			strcpy(s.msg, "成分"+msgstr+"存着重复！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (elm_value<95 || elm_value>100)
		{
			strcpy(s.msg, "成分数据之和必须大于等于95%!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		weigh_no = bcls_rec->Tables["PARA"].Rows[0]["WEIGH_NO"].ToString();
		if (weigh_no.Trim()=="")
		{
			strcpy(s.msg, "计量单号不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm81["WEIGH_NO"] = weigh_no;
		tmmsm81.Query("WEIGH_NO");
		
		if ("" != tmmsm81["QUALITY_BATCH_NO"].ToString().Trim() )
		{
			//质检批不为空 ，根据质检批 维护该质检批！
			for (int i = 0; i < bcls_rec->Tables["MMSM_2A_DEL"].Rows.get_Count(); i++){
				Log::Trace("", "", "--------------D-------------");
				tmmsm81al.Reset();
				tmmsm81al.MergeFrom(bcls_rec->Tables["MMSM_2A_DEL"].Rows[i]);
				tmmsm81al.Delete("QUALITY_BATCH_NO,MAT_CODE,ELM_CODE");

			}
		
			for (int i = 0; i < bcls_rec->Tables["MMSM_2A_INS"].Rows.get_Count(); i++){
				Log::Trace("", "", "--------------A-------------");
				tmmsm81al.Reset();
				tmmsm81al.MergeFrom(bcls_rec->Tables["MMSM_2A_INS"].Rows[i]);

				Log::Info("", __FUNCTION__, "WEIGH_NO =[{0}]", tmmsm81al["WEIGH_NO"].ToString());
				if ("" == tmmsm81al["ELM_CODE"].ToString().Trim())
				{
					strcpy(s.msg, "元素名称不能为空!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm50["MAT_CODE"] = tmmsm81al["MAT_CODE"];
				Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm50["MAT_CODE"].ToString());
				tmmsm50.Query("MAT_CODE");
				tmmsm81al["MAT_NAME"] = tmmsm50["MAT_NAME"];

				tmmsm81al["REC_CREATE_TIME"] = dateNow;
				tmmsm81al["REC_CREATOR"] = s.userid;
				tmmsm81al["WEIGH_NO"] = weigh_no;
				Log::Info("", __FUNCTION__, "ELM_NAME =[{0}]", tmmsm81al["ELM_NAME"].ToString());
				Log::Info("", __FUNCTION__, "ELM_VALUE =[{0}]", tmmsm81al["ELM_VALUE"].ToString());
				//tmmsm81al.Insert();

			}
			for (int i = 0; i < bcls_rec->Tables["MMSM_2A_UPD"].Rows.get_Count(); i++){
				Log::Trace("", "", "--------------U-------------");
				tmmsm81al.Reset();
				tmmsm81al.MergeFrom(bcls_rec->Tables["MMSM_2A_UPD"].Rows[i]);
				tmmsm81al.Delete("QUALITY_BATCH_NO,MAT_CODE,ELM_CODE");

				tmmsm50["MAT_CODE"] = tmmsm81al["MAT_CODE"];
				tmmsm50.Query("MAT_CODE");
				tmmsm81al["MAT_NAME"] = tmmsm50["MAT_NAME"];
				tmmsm81al["REC_CREATE_TIME"] = dateNow;
				tmmsm81al["REC_CREATOR"] = s.userid;
				tmmsm81al["WEIGH_NO"] = weigh_no;
				tmmsm81al.Insert();
			}

		}
		else
		{
			//区分 复制过一次成分且保存后，还未生成质检批号，再次修改成分，根据计量单更新
			Log::Info("", __FUNCTION__, "QM_ELEN_UM =[{0}]", bcls_rec->Tables["QM_ELE"].Rows.get_Count());
			
			if (0 < tmmsm81al.QueryCount("WEIGH_NO"))
			{
				//质检批为空 ，根据计量单  维护该质检批！
				for (int i = 0; i < bcls_rec->Tables["MMSM_2A_DEL"].Rows.get_Count(); i++){
					Log::Trace("", "", "--------------D-------------");
					tmmsm81al.Reset();
					tmmsm81al.MergeFrom(bcls_rec->Tables["MMSM_2A_DEL"].Rows[i]);
					tmmsm81al.Delete("QUALITY_BATCH_NO,MAT_CODE,ELM_CODE");

				}
				for (int i = 0; i < bcls_rec->Tables["MMSM_2A_INS"].Rows.get_Count(); i++){
					Log::Trace("", "", "--------------A-------------");
					tmmsm81al.Reset();
					tmmsm81al.MergeFrom(bcls_rec->Tables["MMSM_2A_INS"].Rows[i]);

					Log::Info("", __FUNCTION__, "WEIGH_NO =[{0}]", tmmsm81al["WEIGH_NO"].ToString());
					if ("" == tmmsm81al["ELM_CODE"].ToString().Trim())
					{
						strcpy(s.msg, "元素名称不能为空!");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					tmmsm50["MAT_CODE"] = tmmsm81al["MAT_CODE"];
					Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm50["MAT_CODE"].ToString());
					tmmsm50.Query("MAT_CODE");
					tmmsm81al["MAT_NAME"] = tmmsm50["MAT_NAME"];

					tmmsm81al["REC_CREATE_TIME"] = dateNow;
					tmmsm81al["REC_CREATOR"] = s.userid;
					tmmsm81al["WEIGH_NO"] = weigh_no;
					Log::Info("", __FUNCTION__, "ELM_NAME =[{0}]", tmmsm81al["ELM_NAME"].ToString());
					Log::Info("", __FUNCTION__, "ELM_VALUE =[{0}]", tmmsm81al["ELM_VALUE"].ToString());
					//tmmsm81al.Insert();

				}
				for (int i = 0; i < bcls_rec->Tables["MMSM_2A_UPD"].Rows.get_Count(); i++){
					Log::Trace("", "", "--------------U-------------");
					tmmsm81al.Reset();
					tmmsm81al.MergeFrom(bcls_rec->Tables["MMSM_2A_UPD"].Rows[i]);
					tmmsm81al.Delete("QUALITY_BATCH_NO,MAT_CODE,ELM_CODE");

					tmmsm50["MAT_CODE"] = tmmsm81al["MAT_CODE"];
					tmmsm50.Query("MAT_CODE");
					tmmsm81al["MAT_NAME"] = tmmsm50["MAT_NAME"];
					tmmsm81al["REC_CREATE_TIME"] = dateNow;
					tmmsm81al["REC_CREATOR"] = s.userid;
					tmmsm81al["WEIGH_NO"] = weigh_no;
					tmmsm81al.Insert();
				}
			}
			else
			{
				//质检批为空 ，复制新增成分！
				for (int i = 0; i <bcls_rec->Tables["QM_ELE"].Rows.get_Count(); i++)
				{
					tmmsm81al.Reset();
					tmmsm81al["MAT_CODE"] = tmmsm81["MAT_CODE"];
					tmmsm81al["MAT_NAME"] = tmmsm81["MAT_NAME"];

					tmmsm81al["REC_CREATE_TIME"] = dateNow;
					tmmsm81al["REC_CREATOR"] = s.userid;
					tmmsm81al["WEIGH_NO"] = weigh_no;
					tmmsm81al["ELM_NAME"] = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_NAME"].ToString();
					tmmsm81al["ELM_CODE"] = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_CODE"].ToString();
					tmmsm81al["ELM_VALUE"] = bcls_rec->Tables["QM_ELE"].Rows[i]["ELM_VALUE"].ToString();
					//tmmsm81al.Insert();
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
