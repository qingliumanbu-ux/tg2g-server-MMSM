/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 取炉次的开始信息
中频炉：tmmsm19
转炉:tmmsm21
电炉:tmmsm20
AOD:tmmsm27
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82c_yry)

int f_mmsm82c_yry(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString heatno_premelt1 = "";

	CDbCommand cmd_inq(conn);

	try
	{ 	

		heatno_premelt1 = bcls_rec->Tables[0].Rows[0]["HEATNO_PREMELT1"].ToString();
			sqlstr = " SELECT distinct  sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3"
				" FROM(	"
				" SELECT sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, HEATNO_PREMELT4, HEATNO_PREMELT5, HEATNO_PREMELT6"
				" FROM TMMSM27	"
				" WHERE 1 = 1 AND HEATNO_PREMELT1 = @heatno_premelt1 "
				" UNION ALL"
				" SELECT sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, HEATNO_PREMELT4, HEATNO_PREMELT5, HEATNO_PREMELT6"
				" FROM TMMSM27"
				" WHERE 1 = 1 AND HEATNO_PREMELT2 = @heatno_premelt1 "
				" UNION ALL	"
				" SELECT sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, HEATNO_PREMELT4, HEATNO_PREMELT5, HEATNO_PREMELT6"
				" FROM TMMSM27"
				" WHERE 1 = 1 AND HEATNO_PREMELT3 = @heatno_premelt1 "
				" UNION ALL	"
				" SELECT  sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, HEATNO_PREMELT4, HEATNO_PREMELT5, HEATNO_PREMELT6"
				" FROM TMMSM27 "
				" WHERE 1 = 1 AND HEATNO_PREMELT4 = @heatno_premelt1 "
				" UNION ALL	"
				" SELECT  sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, HEATNO_PREMELT4, HEATNO_PREMELT5, HEATNO_PREMELT6"
				" FROM TMMSM27 "
				" WHERE 1 = 1 AND HEATNO_PREMELT5 = @heatno_premelt1"
				" UNION ALL	 "
				" SELECT  sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, HEATNO_PREMELT4, HEATNO_PREMELT5, HEATNO_PREMELT6"
				" FROM TMMSM27 "
				" WHERE 1 = 1 AND HEATNO_PREMELT6 = @heatno_premelt1" 
				" union all"
				" SELECT sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, ' ' as HEATNO_PREMELT4,  ' ' as HEATNO_PREMELT5,  ' ' as HEATNO_PREMELT6"
				" FROM TMMSM21	"
				" WHERE 1 = 1 AND HEATNO_PREMELT1 = @heatno_premelt1 "
				" UNION ALL"
				" SELECT sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, ' ' as  HEATNO_PREMELT4,  ' ' as HEATNO_PREMELT5,  ' ' as HEATNO_PREMELT6"
				" FROM TMMSM21"
				" WHERE 1 = 1 AND HEATNO_PREMELT2 = @heatno_premelt1 "
				" UNION ALL	"
				" SELECT sm_plan_nol2,heat_no,START_TIME,END_TIME,ST_NO, HEATNO_PREMELT1, WEIGHT_PREMELT1, HEATNO_PREMELT2, WEIGHT_PREMELT2, HEATNO_PREMELT3, WEIGHT_PREMELT3, ' ' as  HEATNO_PREMELT4,  ' ' as HEATNO_PREMELT5,  ' ' as HEATNO_PREMELT6"
				" FROM TMMSM21"
				" WHERE 1 = 1 AND HEATNO_PREMELT3 = @heatno_premelt1 "
				")"
				" order by sm_plan_nol2"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heatno_premelt1", heatno_premelt1);	
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
