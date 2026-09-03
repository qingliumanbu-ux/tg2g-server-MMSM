/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      KE1921
Version:     1.0
Date:        2022-10-08 13:32:47
Description: 炉次成分规则静态表维护
**************************************************/

#include "stdafx.h"

/******后台文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// 炉次成分规则静态表审核
/// <para>
/// 输入参数：
/// 新增Tables["ADD"]
/// 修改Tables["UPD"]
/// 删除Tables["DEL"]
/// </para>
/// <para>
///  数据库表：tmmsm50 炉次成分规则静态表
/// </para>
/// </summary>
===========================================================</remark>*/

BM2F_ENTERACE(mmsm45a_pro)


int f_mmsm45a_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDecimal seq_no = 0;

	CModel tmmsm50 = CModel("TMMSM50");
	CDbCommand cmd_sql(conn);
	CDataTable temp_table;
	try
	{
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				tmmsm50.Reset();
				tmmsm50.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				Log::Trace("", "", "MAT_CODE=[{0}] MAT_NAME=[{1}]", tmmsm50["MAT_CODE"].ToString(), tmmsm50["MAT_NAME"].ToString());
				//校验物料代码
				if (tmmsm50["MAT_CODE"].ToString().Trim()=="")
				{
					sprintf(s.msg, "物料代码为空,请检查！");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				tmmsm50["REC_CREATE_TIME"] = nowTime;
				tmmsm50["REC_CREATOR"] = s.userid;
				tmmsm50.TrimOrBlank();
				tmmsm50.Insert();
			}
		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{

				//修改在线数据
				tmmsm50.Reset();
				tmmsm50.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);

				tmmsm50["REC_REVISE_TIME"] = nowTime;
				tmmsm50["REC_REVISOR"] = s.userid;
				tmmsm50.TrimOrBlank();
				tmmsm50.Update("*");
			}
		}


		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				//删除直接修改归档标记
				tmmsm50.Reset();
				tmmsm50.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
				tmmsm50.TrimOrBlank();
				tmmsm50.Delete("MAT_CODE");
				Log::Trace("", "", "删除成功=[{0}]", tmmsm50["MAT_CODE"].ToString());
			}
		}



		// -----End IPLAT4C::IPLAT4CServiceCompositeStatementObj()----- //
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


