/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 炼钢转炉作业喷补实绩增删改
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

// service入口
BM2F_ENTERACE(mmsm21c_pro)

int f_mmsm21c_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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
	CModel tmmsm21c("TMMSM21C");

	try
	{
		if (bcls_rec->Tables.Contains("TMMSM21C_DELETE")){
			Log::Trace("", __FUNCTION__, "删除");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21C_DELETE"].Rows.get_Count(); i++)
			{
				tmmsm21c.Reset();
				tmmsm21c.MergeFrom(bcls_rec->Tables["TMMSM21C_DELETE"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21c.HEAT_NO = [{0}] ---", tmmsm21c["HEAT_NO"].ToString());
				tmmsm21c.Delete("REC_ID");
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM21C_ADD")){
			Log::Trace("", __FUNCTION__, "新增");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21C_ADD"].Rows.get_Count(); i++)
			{
				tmmsm21c.Reset();
				tmmsm21c.MergeFrom(bcls_rec->Tables["TMMSM21C_ADD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21c.HEAT_NO = [{0}] ---", tmmsm21c["HEAT_NO"].ToString());
				
				//获取最大序号
				tmmsm21c["REC_ID"] = EPGetNextSeq("BOF_SPURT_NO", conn);
				Log::Trace("", __FUNCTION__, "--- tmmsm21c.REC_ID = [{0}] ---", tmmsm21c["REC_ID"].ToString());
				if ("" == tmmsm21c["REC_ID"].ToString().Trim()){
					strcpy(s.msg, "喷补顺序号获取失败。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm21c["SPURT_START_TIME"].ToString().Trim() == "")
				{
					tmmsm21c["SPURT_START_TIME"] = dateNow;
				}
				if (tmmsm21c["SPURT_END_TIME"].ToString().Trim() == "")
				{
					tmmsm21c["SPURT_END_TIME"] = dateNow;
				}
				if (tmmsm21c["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm21c["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SM", tmmsm21c["SPURT_START_TIME"].ToString(), prodShiftNo, prodShiftGroup, conn);
					tmmsm21c["PROD_SHIFT_NO"] = prodShiftNo;
					tmmsm21c["PROD_SHIFT_GROUP"] = prodShiftGroup;
				}
				//计算喷补时间
				CDateTime startDateTime = CDateTime::Parse(tmmsm21c["SPURT_START_TIME"].ToString());
				CDateTime endDateTime = CDateTime::Parse(tmmsm21c["SPURT_END_TIME"].ToString());
				CTimeSpan timeSpan = endDateTime - startDateTime;
				CDecimal diffTime = timeSpan.TotalMinutes();
				if (0 >= diffTime){
					strcpy(s.msg, "喷补时间需大于0，请输入正确喷补开始、结束时间。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm21c["SPURT_TIME"] = diffTime;

				tmmsm21c["REC_CREATE_TIME"] = dateNow;
				tmmsm21c["REC_CREATOR"] = s.userid;
				tmmsm21c.Insert();
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM21C_MODIFY")){
			Log::Trace("", __FUNCTION__, "修改");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21C_MODIFY"].Rows.get_Count(); i++)
			{
				tmmsm21c.Reset();
				tmmsm21c.MergeFrom(bcls_rec->Tables["TMMSM21C_MODIFY"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21c.HEAT_NO = [{0}] ---", tmmsm21c["HEAT_NO"].ToString());
				if (tmmsm21c["HEAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "请输入正确熔炼号。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm21c["SPURT_START_TIME"].ToString().Trim() == "")
				{
					tmmsm21c["SPURT_START_TIME"] = dateNow;
				}
				if (tmmsm21c["SPURT_END_TIME"].ToString().Trim() == "")
				{
					tmmsm21c["SPURT_END_TIME"] = dateNow;
				}
				if (tmmsm21c["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm21c["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SM", tmmsm21c["SPURT_START_TIME"].ToString(), prodShiftNo, prodShiftGroup, conn);
					tmmsm21c["PROD_SHIFT_NO"] = prodShiftNo;
					tmmsm21c["PROD_SHIFT_GROUP"] = prodShiftGroup;
				}
				//计算喷补时间
				CDateTime startDateTime = CDateTime::Parse(tmmsm21c["SPURT_START_TIME"].ToString());
				CDateTime endDateTime = CDateTime::Parse(tmmsm21c["SPURT_END_TIME"].ToString());
				CTimeSpan timeSpan = endDateTime - startDateTime;
				CDecimal diffTime = timeSpan.TotalMinutes();
				if (0 >= diffTime){
					strcpy(s.msg, "喷补时间需大于0，请输入正确喷补开始、结束时间。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm21c["SPURT_TIME"] = diffTime;

				tmmsm21c["REC_REVISE_TIME"] = dateNow;
				tmmsm21c["REC_REVISOR"] = s.userid;
				tmmsm21c.Update("REC_REVISE_TIME,REC_REVISOR, HEAT_NO, STATION_NO, SPURT_MAT_NAME, SPURT_MAT_FACTORY, SPURT_MAT_WT, SPURT_PART, SPURT_RESULT, SPURT_START_TIME, SPURT_END_TIME, SPURT_TIME, PROD_SHIFT_NO, PROD_SHIFT_GROUP", "REC_ID");
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