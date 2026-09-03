/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 炉次信息
中频炉：tmmsm19
转炉:tmmsm21
电炉:tmmsm20
AOD:tmmsm27
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82h_inq)
int f_mmsm_gyhl(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82h_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString heat_no = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDbCommand cmd_inq(conn);

	CModel tpssm35("TPSSM35");

	try
	{ 	

		begin_time = bcls_rec->Tables[0].Rows[0]["START_TIME_S"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["START_TIME_E"].ToString();
		tpssm35.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Info("", __FUNCTION__, "begin_time =[{0}],end_time =[{1}]", begin_time, end_time);
		

		if (end_time.Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and RET_TIME <=@end_time";
		}
		if (begin_time.Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and RET_TIME >=@begin_time";
		} 

		sqlstr = " select HEAT_NO,RET_HEAT_NO,RET_TIME,RATE"
			" from tpssm35 t"
			" where 1=1" + sqlstr_where	 +
			" union all"
			" select heat_no,' ',max(RET_TIME),1-sum(RATE) "
			" from tpssm35 t"
			" where 1=1" + sqlstr_where +
			" group by heat_no"
			" having sum(RATE)!=1"
			" order by RET_TIME desc"
			;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("heat_no", tpssm35["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("ret_heat_no", tpssm35["RET_HEAT_NO"].ToString());
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close(); 

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
