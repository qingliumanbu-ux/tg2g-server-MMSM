/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 皮带秤自动上传

**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmq1_inq)
int f_mmsmq1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cx_date = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString heat_no = "";

	CDbCommand cmd_inq(conn);

	CModel tmmsm83("TMMSM83");

	try
	{
		cx_date = bcls_rec->Tables[0].Rows[0]["CX_DATE"].ToString().SubstringNE(0, 8);

		sqlstr = " select BOF0_S as PRE_HEAT_NO,BOF0_E as HEAT_NO"
			" from V_DA_HEAT_S_E1"
			" where 1=1"
			" and WEEK_DAY = @cx_date"
			" union all"
			" select BOF1_S as PRE_HEAT_NO,BOF1_E as HEAT_NO"
			" from V_DA_HEAT_S_E1"
			" where 1=1"
			" and WEEK_DAY = @cx_date"
			" union all"
			" select BOF2_S as PRE_HEAT_NO,BOF2_E as HEAT_NO"
			" from V_DA_HEAT_S_E1"
			" where 1=1"
			" and WEEK_DAY = @cx_date"
			" union all"
			" select BOF9_S as PRE_HEAT_NO,BOF9_E as HEAT_NO"
			" from V_DA_HEAT_S_E1"
			" where 1=1"
			" and WEEK_DAY = @cx_date"
			" union all"
			" select AOD0_S as PRE_HEAT_NO,AOD0_E  as HEAT_NO"
			" from V_DA_HEAT_S_E1"
			" where 1=1"
			" and WEEK_DAY = @cx_date"
			" union all"
			" select AOD1_S as PRE_HEAT_NO,AOD1_E as HEAT_NO"
			" from V_DA_HEAT_S_E1"
			" where 1=1"
			" and WEEK_DAY = @cx_date"
			" union all"
			" select AOD2_S as PRE_HEAT_NO,AOD2_E as HEAT_NO"
			" from V_DA_HEAT_S_E1"
			" where 1=1"
			" and WEEK_DAY = @cx_date"
			" union all"
			" select AOD6_S as PRE_HEAT_NO,AOD6_E as HEAT_NO"
			" from V_DA_HEAT_S_E1"
			" where 1=1"
			" and WEEK_DAY = @cx_date"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("cx_date", cx_date);		
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
