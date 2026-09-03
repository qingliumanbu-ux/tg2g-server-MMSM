/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 炼钢转炉作业其它实绩增删改
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsm31b_pro)

int f_mmsm31b_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString prodShiftNo = "";
	CString prodShiftGroup = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	/* 实体类定义 */
	CModel tmmsm31b("TMMSM31B");

	try
	{
		Log::Trace("", __FUNCTION__, "--- name = [{0}] ---", bcls_rec->Tables[0].get_TableName());
		;
		if (bcls_rec->Tables.Contains("TMMSM31B_DELETE")){
			Log::Trace("", __FUNCTION__, "删除");
			for (int i = 0; i < bcls_rec->Tables["tmmsm31b_DELETE"].Rows.get_Count(); i++)
			{
				tmmsm31b.Reset();
				tmmsm31b.MergeFrom(bcls_rec->Tables["tmmsm31b_DELETE"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm31b.HEAT_NO = [{0}] ---", tmmsm31b["HEAT_NO"].ToString());
				tmmsm31b.Delete("HEAT_NO");
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM31B_ADD")){
			Log::Trace("", __FUNCTION__, "新增");
			for (int i = 0; i < bcls_rec->Tables["tmmsm31b_ADD"].Rows.get_Count(); i++)
			{
				tmmsm31b.Reset();
				tmmsm31b.MergeFrom(bcls_rec->Tables["tmmsm31b_ADD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm31b.HEAT_NO = [{0}] ---", tmmsm31b["HEAT_NO"].ToString());

				if (tmmsm31b["HEAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "请输入正确熔炼号。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				int cnt = tmmsm31b.QueryCount("HEAT_NO");
				if (0 < cnt){
					strcpy(s.msg, "熔炼号：" + tmmsm31b["HEAT_NO"].ToString() + ",其他实绩信息已存在。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm31b["REC_CREATE_TIME"] = dateNow;
				tmmsm31b["REC_CREATOR"] = s.userid;
				if (tmmsm31b["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm31b["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SM", tmmsm31b["REC_CREATE_TIME"].ToString(), prodShiftNo, prodShiftGroup, conn);
					tmmsm31b["PROD_SHIFT_NO"] = prodShiftNo;
					tmmsm31b["PROD_SHIFT_GROUP"] = prodShiftGroup;
				}

				
				tmmsm31b.Insert();
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM31B_MODIFY")){
			Log::Trace("", __FUNCTION__, "修改");
			for (int i = 0; i < bcls_rec->Tables["tmmsm31b_MODIFY"].Rows.get_Count(); i++)
			{
				tmmsm31b.Reset();
				tmmsm31b.MergeFrom(bcls_rec->Tables["tmmsm31b_MODIFY"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm31b.HEAT_NO = [{0}] ---", tmmsm31b["HEAT_NO"].ToString());
				tmmsm31b["REC_REVISE_TIME"] = dateNow;
				tmmsm31b["REC_REVISOR"] = s.userid;
				tmmsm31b.Delete();
				tmmsm31b.Insert();
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