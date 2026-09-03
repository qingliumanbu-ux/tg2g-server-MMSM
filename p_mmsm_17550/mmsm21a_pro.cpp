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
BM2F_ENTERACE(mmsm21a_pro)

int f_mmsm21a_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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
	CModel tmmsm21a("TMMSM21A");

	try
	{
		Log::Trace("", __FUNCTION__, "输入表", bcls_rec->Tables.Contains("TMMSM21A_DELETE"));
		if (bcls_rec->Tables.Contains("TMMSM21A_DELETE")){
			Log::Trace("", __FUNCTION__, "删除");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21A_DELETE"].Rows.get_Count(); i++)
			{
				tmmsm21a.Reset();
				tmmsm21a.MergeFrom(bcls_rec->Tables["TMMSM21A_DELETE"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21a.HEAT_NO = [{0}] ---", tmmsm21a["HEAT_NO"].ToString());
				tmmsm21a.Delete("HEAT_NO");
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM21A_ADD")){
			Log::Trace("", __FUNCTION__, "新增");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21A_ADD"].Rows.get_Count(); i++)
			{
				tmmsm21a.Reset();
				tmmsm21a.MergeFrom(bcls_rec->Tables["TMMSM21A_ADD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21a.HEAT_NO = [{0}] ---", tmmsm21a["HEAT_NO"].ToString());
				
				if (tmmsm21a["HEAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "请输入正确熔炼号。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				int cnt = tmmsm21a.QueryCount("HEAT_NO");
				if (0 < cnt){
					strcpy(s.msg, "熔炼号：" + tmmsm21a["HEAT_NO"].ToString() + ",其他实绩信息已存在。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm21a["PROD_TIME"].ToString().Trim() == "")
				{
					tmmsm21a["PROD_TIME"] = dateNow;
				}
				if (tmmsm21a["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm21a["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SM", tmmsm21a["PROD_TIME"].ToString(), prodShiftNo, prodShiftGroup, conn);
					tmmsm21a["PROD_SHIFT_NO"] = prodShiftNo;
					tmmsm21a["PROD_SHIFT_GROUP"] = prodShiftGroup;
				}

				tmmsm21a["REC_CREATE_TIME"] = dateNow;
				tmmsm21a["REC_CREATOR"] = s.userid;
				tmmsm21a.Insert();
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM21A_MODIFY")){
			Log::Trace("", __FUNCTION__, "修改");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21A_MODIFY"].Rows.get_Count(); i++)
			{
				tmmsm21a.Reset();
				tmmsm21a.MergeFrom(bcls_rec->Tables["TMMSM21A_MODIFY"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21a.HEAT_NO = [{0}] ---", tmmsm21a["HEAT_NO"].ToString());
				if (tmmsm21a["PROD_TIME"].ToString().Trim() == "")
				{
					tmmsm21a["PROD_TIME"] = dateNow;
				}
				if (tmmsm21a["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm21a["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SM", tmmsm21a["PROD_TIME"].ToString(), prodShiftNo, prodShiftGroup, conn);
					tmmsm21a["PROD_SHIFT_NO"] = prodShiftNo;
					tmmsm21a["PROD_SHIFT_GROUP"] = prodShiftGroup;
				}
				tmmsm21a["REC_REVISE_TIME"] = dateNow;
				tmmsm21a["REC_REVISOR"] = s.userid;
				tmmsm21a.Update("REC_REVISE_TIME,REC_REVISOR,PONO, PROD_SHIFT_GROUP, PROD_SHIFT_NO, STATION_ID, STATION_NO, ST_NO, STOP_SLAG_MAKER, STOP_SLAG_STATUS, STOP_SLAG_TYPE, SLAG_SAMPLE_STATUS, SPLASH_STATUS, SPLASH_REASON, GAS_RECLAIM_ABN_CAUSE, GAS_RECLAIM_QTY, REBLOW_CAUSE_CODE, WAITING_TIME1, WAITING_TIME2, WAITING_TIME3, WAITING_TIME4, WAITING_TIME5, WAITING_TIME6, WAITING_TIME7, WAITING_TIME8, WAITING_TIME9, WAITING_CAUSE1, WAITING_CAUSE2, WAITING_CAUSE3, WAITING_CAUSE4, WAITING_CAUSE5, WAITING_CAUSE6, WAITING_CAUSE7, WAITING_CAUSE8, WAITING_CAUSE9, REMARK", "HEAT_NO");
			}
		}
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