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
BM2F_ENTERACE(mmsm83e_inq)
int f_mmsm83e_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString heat_no = "";

	CDbCommand cmd_inq(conn);

	CModel tmmsm83("TMMSM83");

	try
	{ 	

		begin_time = bcls_rec->Tables[0].Rows[0]["START_TIME_S"].ToString();
		end_time = bcls_rec->Tables[0].Rows[0]["START_TIME_E"].ToString();
		tmmsm83.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (tmmsm83["BUNKER_NO"].ToString().Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and BUNKER_NO =@bunker_no";
		} 	
		if (tmmsm83["WEIGH_NO"].ToString().Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and WEIGH_NO =@weigh_no";
		}
		if (tmmsm83["BUNKER_NO_ORIGINAL"].ToString().Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and BUNKER_NO_ORIGINAL =@bunker_no_original";
		}

		if (end_time.Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and BIN_SRART_TIME <=@end_time";
		}
		if (begin_time.Trim() != "")
		{
			sqlstr_where = sqlstr_where + " and BIN_SRART_TIME >=@begin_time";
		} 

		sqlstr = " select *"
			" from tmmsm83"
			" where 1=1" + sqlstr_where	 +
			" order by MSG_ID desc"
			;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("bunker_no", tmmsm83["BUNKER_NO"].ToString());
			cmd_inq.Parameters.Set("weigh_no", tmmsm83["WEIGH_NO"].ToString());
			cmd_inq.Parameters.Set("bunker_no_original", tmmsm83["BUNKER_NO_ORIGINAL"].ToString());
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
