/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 炼钢转炉作业铸余实绩增删改
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

// service入口
BM2F_ENTERACE(mmsm21d_pro)

int f_mmsm21d_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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
	CModel tmmsm21d("TMMSM21D");

	try
	{
		if (bcls_rec->Tables.Contains("TMMSM21D_DELETE")){
			Log::Trace("", __FUNCTION__, "删除");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21D_DELETE"].Rows.get_Count(); i++)
			{
				tmmsm21d.Reset();
				tmmsm21d.MergeFrom(bcls_rec->Tables["TMMSM21D_DELETE"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21d.HEAT_NO = [{0}] ---", tmmsm21d["HEAT_NO"].ToString());
				tmmsm21d.Delete("MIX_REMAIN_NO");
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM21D_ADD")){
			Log::Trace("", __FUNCTION__, "新增");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21D_ADD"].Rows.get_Count(); i++)
			{
				tmmsm21d.Reset();
				tmmsm21d.MergeFrom(bcls_rec->Tables["TMMSM21D_ADD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21d.HEAT_NO = [{0}] ---", tmmsm21d["HEAT_NO"].ToString());
				if (tmmsm21d["HEAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "请输入正确熔炼号。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//获取最大序号
				CString mixRemainNo = EPGetNextSeq("BOF_MIX_NO", conn);
				if ("" == mixRemainNo.Trim()){
					strcpy(s.msg, "兑铸余顺序号获取失败。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm21d["MIX_REMAIN_NO"] = "ZY" + dateNow.Substring(2, 2) + mixRemainNo;
				Log::Trace("", __FUNCTION__, "--- tmmsm21d.MIX_REMAIN_NO = [{0}] ---", tmmsm21d["MIX_REMAIN_NO"].ToString());
				if (tmmsm21d["STEEL_MIX_REMAIN_TIME"].ToString().Trim() == "")
				{
					tmmsm21d["STEEL_MIX_REMAIN_TIME"] = dateNow;
				}
				if (tmmsm21d["IRON_BOTTOM_END_TIME"].ToString().Trim() == "")
				{
					tmmsm21d["IRON_BOTTOM_END_TIME"] = dateNow;
				}
				if (tmmsm21d["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm21d["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SM", tmmsm21d["STEEL_MIX_REMAIN_TIME"].ToString(), prodShiftNo, prodShiftGroup, conn);
					tmmsm21d["PROD_SHIFT_NO"] = prodShiftNo;
					tmmsm21d["PROD_SHIFT_GROUP"] = prodShiftGroup;
				}
				tmmsm21d["REC_CREATE_TIME"] = dateNow;
				tmmsm21d["REC_CREATOR"] = s.userid;
				tmmsm21d.Insert();
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM21D_MODIFY")){
			Log::Trace("", __FUNCTION__, "修改");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21D_MODIFY"].Rows.get_Count(); i++)
			{
				tmmsm21d.Reset();
				tmmsm21d.MergeFrom(bcls_rec->Tables["TMMSM21D_MODIFY"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21d.HEAT_NO = [{0}] ---", tmmsm21d["HEAT_NO"].ToString());
				if (tmmsm21d["STEEL_MIX_REMAIN_TIME"].ToString().Trim() == "")
				{
					tmmsm21d["STEEL_MIX_REMAIN_TIME"] = dateNow;
				}
				if (tmmsm21d["IRON_BOTTOM_END_TIME"].ToString().Trim() == "")
				{
					tmmsm21d["IRON_BOTTOM_END_TIME"] = dateNow;
				}
				if (tmmsm21d["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm21d["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SM", tmmsm21d["STEEL_MIX_REMAIN_TIME"].ToString(), prodShiftNo, prodShiftGroup, conn);
					tmmsm21d["PROD_SHIFT_NO"] = prodShiftNo;
					tmmsm21d["PROD_SHIFT_GROUP"] = prodShiftGroup;
				}
				tmmsm21d["REC_REVISE_TIME"] = dateNow;
				tmmsm21d["REC_REVISOR"] = s.userid;
				tmmsm21d.Update("REC_REVISE_TIME,REC_REVISOR,HEAT_NO, ST_NO, MOLTIRON_WT, STEEL_WT, PROD_SHIFT_GROUP, PROD_SHIFT_NO, IRON_BOTTOM_END_TIME, STEEL_MIX_REMAIN_TIME, TOTAL_TIME, HEAT_NO1, HEAT_NO2, HEAT_NO3, HEAT_NO4, HEAT_NO5, HEAT_NO6, ST_NO1, ST_NO2, ST_NO3, ST_NO4, ST_NO5, ST_NO6, STEEL_WT1, STEEL_WT2, STEEL_WT3, STEEL_WT4, STEEL_WT5, STEEL_WT6", "MIX_REMAIN_NO");
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