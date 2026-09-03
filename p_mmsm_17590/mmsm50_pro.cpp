/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-03 17:13:56  
Description: 原辅料主信息处理
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
#include<regex>

/***** C++ 的业务头文件部分 *****/ 


/* ***** 静态函数申明 ***** */

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// 原辅料主信息新增、删除、修改
/// <para>
/// 1.原辅料主信息新增、删除、修改。
/// 
/// </para>
/// <para>数据库表：TMMSM50(炼钢原辅料主信息表)					</para>
/// <para>主调用函数：			                                </para>
/// </summary>
/// <param name=" ">     </param>
/// <param name=" ">                </param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口

//int f_mmsm50_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn); 

BM2F_ENTERACE(mmsm50_pro)


int f_mmsm50_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;
	int  proc_sum = 0;				//操作总数
	CDbCommand cmd_sql(conn); //与DB 建立连接。
	CString msgstr = "提示信息:";	//提示信息。
	CString sqlstr = "";
	CString   v_proc_div = "";
	CString  table_name = "";
	CString c_sql_condition = "";
	

	try
	{



		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		table_name = bcls_rec->Tables["PARA"].Rows[0]["TABLE_NAME"].ToString();

		Log::Trace("", "", "获取传入参数...");
		Log::Trace("", "", "传入表名：table_name=[{0}]", table_name);
		Log::Trace("", "", "当前时间：nowTime=[{0}]", nowTime);

		CModel tmmsm50(table_name);
		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				tmmsm50.Reset();
				tmmsm50.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				tmmsm50["REC_CREATOR"] = s.userid;
				tmmsm50["REC_CREATE_TIME"] = nowTime;
				tmmsm50.TrimOrBlank();
				//用于209 废钢进厂临时画面：F01/F02开头物料必须录入批次号
				if (tmmsm50["MAT_CODE"].ToString().SubstringNE(0, 3) == "F01" || tmmsm50["MAT_CODE"].ToString().SubstringNE(0, 3) == "F02")
				{
					tmmsm50["BACK_C3"] = "1";
				}
				Log::Trace("", "", "MAT_CODE=[{0}]", tmmsm50["MAT_CODE"].ToString());
				if (tmmsm50.QueryCount("MAT_CODE")>0)
				{
					Log::Trace("", "", "MAT_CODE=[{0}]", tmmsm50["MAT_CODE"].ToString());
					msgstr += msgstr.Format("第%d条记录已存在，无法新增。", i + 1);
					continue;
				}

				if (tmmsm50["MAT_SIMPLE_ENAME"].ToString().GetLength() > 11)
				{
					strcpy(s.msg, "VAI物料英文名不能超过11位!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				string dest = string((const char*)tmmsm50["MAT_SIMPLE_ENAME"].ToString().Trim());
				Log::Trace("", "", dest);
				regex pattern("[\u4e00-\u9fa5]");
				bool is_match = regex_search(dest, pattern);
				if (is_match)
				{
					strcpy(s.msg, "不能为汉字!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", "", "3获取传入参数...");

				sqlstr = "INSERT INTO " + table_name;
				tmmsm50.Insert();

			}


		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				tmmsm50.Reset();
				tmmsm50.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				Log::Info("", __FUNCTION__, "GROUP_CLASS_DESC =[{0}]", bcls_rec->Tables["UPD"].Rows[i]["GROUP_CLASS_DESC"].ToString());
				tmmsm50.TrimOrBlank();
				if (tmmsm50.QueryCount("MAT_CODE")<1){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法修改。(是否新增？)", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法删除。", i + 1);
					continue;
				}
				else
				{
					
					tmmsm50.Delete("MAT_CODE");

					tmmsm50.Reset();
					tmmsm50.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
					if (tmmsm50["MAT_SIMPLE_ENAME"].ToString().GetLength() > 11)
					{
						strcpy(s.msg, "VAI物料英文名不能超过11位!");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					string dest = string((const char*)tmmsm50["MAT_SIMPLE_ENAME"].ToString().Trim());
					Log::Trace("", "", dest);
					regex pattern("[\u4e00-\u9fa5]");
					bool is_match = regex_search(dest, pattern);
					if (is_match)
					{
						strcpy(s.msg, "不能为汉字!");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					Log::Info("", __FUNCTION__, "GROUP_CLASS_DESC =[{0}],长度=[{1}]", tmmsm50["GROUP_CLASS_DESC"].ToString(), tmmsm50["GROUP_CLASS_DESC"].ToString().GetLength());
					tmmsm50["REC_REVISOR"] = s.userid;
					tmmsm50["REC_REVISE_TIME"] = nowTime;
					tmmsm50.TrimOrBlank();
					tmmsm50.Insert();
				}

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				tmmsm50.Reset();
				tmmsm50.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				//if (tmmsm50.QueryCount(condition) != 1){
				if (tmmsm50.QueryCount("MAT_CODE")<1){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法删除。", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法删除。", i + 1);
					continue;
				}
				sqlstr = "DELETE FROM " + table_name;
				proc_sum += tmmsm50.Delete("MAT_CODE");
			}
		}

		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);
		
		/*doFlag = f_mmsm50_proc(bcls_rec, bcls_ret,conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
