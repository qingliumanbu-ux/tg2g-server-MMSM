/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 炼钢转炉作业测厚实绩增删改
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

// service入口
BM2F_ENTERACE(mmsm21b_pro)

int f_mmsm21b_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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
	CModel tmmsm21b("TMMSM21B");

	try
	{
		if (bcls_rec->Tables.Contains("TMMSM21B_DELETE")){
			Log::Trace("", __FUNCTION__, "删除");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21B_DELETE"].Rows.get_Count(); i++)
			{
				tmmsm21b.Reset();
				tmmsm21b.MergeFrom(bcls_rec->Tables["TMMSM21B_DELETE"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21b.HEAT_NO = [{0}] ---", tmmsm21b["HEAT_NO"].ToString());
				tmmsm21b.Delete("REC_ID");
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM21B_ADD")){
			Log::Trace("", __FUNCTION__, "新增");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21B_ADD"].Rows.get_Count(); i++)
			{
				tmmsm21b.Reset();
				tmmsm21b.MergeFrom(bcls_rec->Tables["TMMSM21B_ADD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21b.HEAT_NO = [{0}] ---", tmmsm21b["HEAT_NO"].ToString());
				if (tmmsm21b["HEAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "请输入正确熔炼号。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//获取最大序号
				tmmsm21b["REC_ID"] = EPGetNextSeq("BOF_THICK_NO", conn);
				Log::Trace("", __FUNCTION__, "--- tmmsm21b.REC_ID = [{0}] ---", tmmsm21b["REC_ID"].ToString());
				if ("" == tmmsm21b["REC_ID"].ToString().Trim()){
					strcpy(s.msg, "测厚顺序号获取失败。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm21b["CALIP_THICK_TIME"].ToString().Trim() == "")
				{
					tmmsm21b["CALIP_THICK_TIME"] = dateNow;
				}
				if (tmmsm21b["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm21b["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SM", tmmsm21b["CALIP_THICK_TIME"].ToString(), prodShiftNo, prodShiftGroup, conn);
					tmmsm21b["PROD_SHIFT_NO"] = prodShiftNo;
					tmmsm21b["PROD_SHIFT_GROUP"] = prodShiftGroup;
				}
				tmmsm21b["REC_CREATE_TIME"] = dateNow;
				tmmsm21b["REC_CREATOR"] = s.userid;
				tmmsm21b.Insert();
			}
		}
		if (bcls_rec->Tables.Contains("TMMSM21B_MODIFY")){
			Log::Trace("", __FUNCTION__, "修改");
			for (int i = 0; i < bcls_rec->Tables["TMMSM21B_MODIFY"].Rows.get_Count(); i++)
			{
				tmmsm21b.Reset();
				tmmsm21b.MergeFrom(bcls_rec->Tables["TMMSM21B_MODIFY"].Rows[i]);
				Log::Trace("", __FUNCTION__, "--- tmmsm21b.HEAT_NO = [{0}] ---", tmmsm21b["HEAT_NO"].ToString());
				if (tmmsm21b["CALIP_THICK_TIME"].ToString().Trim() == "")
				{
					tmmsm21b["CALIP_THICK_TIME"] = dateNow;
				}
				if (tmmsm21b["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm21b["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SM", tmmsm21b["CALIP_THICK_TIME"].ToString(), prodShiftNo, prodShiftGroup, conn);
					tmmsm21b["PROD_SHIFT_NO"] = prodShiftNo;
					tmmsm21b["PROD_SHIFT_GROUP"] = prodShiftGroup;
				}
				tmmsm21b["REC_REVISE_TIME"] = dateNow;
				tmmsm21b["REC_REVISOR"] = s.userid;
				tmmsm21b.Update("REC_REVISE_TIME,REC_REVISOR,HEAT_NO, PRE_HEAT_NO, STATION_NO, PROD_SHIFT_GROUP, PROD_SHIFT_NO, FURNACE_AGE, CALIP_THICK_TIME, FURNACE_COUNT, BOTTOM_BLOW_RESULT, BOTTOM_BLOW_HOLE_NUM, REMARK, THICK_AREA_HEAD, THICK_AREA_BACK, THICK_POOL_HEAD, THICK_POOL_BACK, THICK_POOL_EAST, THICK_POOL_WEST, SLAGLINE_HEAD_WEST, SLAGLINE_HEAD_EAST, SLAGLINE_BACK_EAST, SLAGLINE_BACK_WEST, THICK_BOTTOM_MIN, THICK_BOTTOM_MAX, THICK_TRUNNION_EAST, THICK_TRUNNION_WEST, THICK_CAP_EAST_MAX, THICK_CAP_WEST_MAX, DIFF_AREA_BACK, DIFF_AREA_HEAD, DIFF_POOL_HEAD, DIFF_POOL_BACK, DIFF_POOL_EAST, DIFF_POOL_WEST, DIFF_SLAGLINE_HEAD_WEST, DIFF_SLAGLINE_HEAD_EAST, DIFF_SLAGLINE_BACK_EAST, DIFF_SLAGLINE_BACK_WEST, DIFF_BOTTOM_MIN, DIFF_BOTTOM_MAX, DIFF_TRUNNION_EAST, DIFF_TRUNNION_WEST, DIFF_CAP_EAST_MAX, DIFF_CAP_WEST_MAX, CORRODE_AREA_HEAD, CORRODE_AREA_BACK, CORRODE_POOL_HEAD, CORRODE_POOL_BACK, CORRODE_POOL_EAST, CORRODE_POOL_WEST, CORRODE_BOTTOM_MIN, CORRODE_BOTTOM_MAX, CORRODE_SLAGLINE_HEAD_WEST, CORRODE_SLAGLINE_HEAD_EAST, CORRODE_SLAGLINE_BACK_EAST, CORRODE_SLAGLINE_BACK_WEST, CORRODE_TRUNNION_EAST, CORRODE_TRUNNION_WEST, CORRODE_CAP_EAST, CORRODE_CAP_WEST", "REC_ID");
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