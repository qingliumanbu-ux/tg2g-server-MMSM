/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 铬镍收得率明细表
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmheatno1_inq)

int f_mmsmheatno1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString end_time = " ";
	CString end_time1 = " ";
	CString time_riqi = " ";
	CString start_time = " ";
	CString time_ri = " ";
	CString check_flag = "1";
	CModel tmmsmzxhbb_lh("TMMSMZXHBB_LH");
	CModel tmmsmzxhbb_lh1("TMMSMZXHBB_LH");

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("WEEK_DAY"))
			start_time = bcls_rec->Tables[0].Rows[0]["WEEK_DAY"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CHECK_FLAG"))
			check_flag = bcls_rec->Tables[0].Rows[0]["CHECK_FLAG"].ToString();

		Log::Info("", __FUNCTION__, "start_time   =[{0}]", start_time);
		if (start_time.Trim() == "")
		{
			return 0;
		}
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			if (check_flag == "1")
			{  //按周
				sqlstr = " SELECT WEEK_DAY,BOF0_S,BOF0_E,BOF1_S,BOF1_E,BOF2_S,BOF2_E,BOF9_S,BOF9_E,AOD0_S,AOD0_E,AOD1_S,AOD1_E,AOD2_S,AOD2_E,AOD6_S,AOD6_E,EAF1_S,EAF1_E,EAF2_S,EAF2_E,IF1_S,IF1_E,IF2_S,IF2_E,IF3_S,IF3_E,IF4_S,IF4_E,IF5_S,IF5_E,IF6_S,IF6_E,IF7_S,IF7_E,IF8_S,IF8_E FROM V_DA_HEAT_S_E1 WHERE WEEK_DAY = @WEEK_DAY ";

			}
			else if (check_flag == "2")
			{
				//按炉号
				sqlstr = " SELECT WEEK_DAY,BOF0_S, BOF1_S,BOF2_S,BOF9_S,AOD0_S,AOD1_S,AOD2_S,AOD6_S"
					" nvl((select max(heat_no) from tmmsm21 where heat_no like 'B0%' and end_time<=@end_time),BOF0_E) BOF0_E"
					" nvl((select max(heat_no) from tmmsm21 where heat_no like 'B1%' and end_time<=@end_time),BOF1_E) BOF1_E"
					" nvl((select max(heat_no) from tmmsm21 where heat_no like 'B2%' and end_time<=@end_time),BOF2_E) BOF2_E"
					" nvl((select max(heat_no) from tmmsm21 where heat_no like 'B9%' and end_time<=@end_time),BOF9_E) BOF9_E"
					" nvl((select max(heat_no) from tmmsm27 where heat_no like 'A0%' and end_time<=@end_time),AOD0_E) AOD0_E"
					" nvl((select max(heat_no) from tmmsm27 where heat_no like 'A1%' and end_time<=@end_time),AOD1_E) AOD1_E"
					" nvl((select max(heat_no) from tmmsm27 where heat_no like 'A2%' and end_time<=@end_time),AOD2_E) AOD2_E"
					" nvl((select max(heat_no) from tmmsm27 where heat_no like 'A6%' and end_time<=@end_time),AOD6_E) AOD6_E"
					" FROM V_DA_HEAT_S_E_MONTH1"
					" WHERE WEEK_DAY = @WEEK_DAY "
					;

			}
			else
			{
				//按月
				sqlstr = " SELECT WEEK_DAY,BOF0_S,BOF0_E,BOF1_S,BOF1_E,BOF2_S,BOF2_E,BOF9_S,BOF9_E,AOD0_S,AOD0_E,AOD1_S,AOD1_E,AOD2_S,AOD2_E,AOD6_S,AOD6_E,EAF1_S,EAF1_E,EAF2_S,EAF2_E,IF1_S,IF1_E,IF2_S,IF2_E,IF3_S,IF3_E,IF4_S,IF4_E,IF5_S,IF5_E,IF6_S,IF6_E,IF7_S,IF7_E,IF8_S,IF8_E FROM V_DA_HEAT_S_E_MONTH1 WHERE WEEK_DAY = @WEEK_DAY ";
			}
			cmd_inq.SetCommandText(sqlstr);
			if (check_flag == "1")
			{
				cmd_inq.Parameters.Set("WEEK_DAY", start_time.SubstringNE(0, 8));
			}
			else
			{
				cmd_inq.Parameters.Set("end_time", start_time);
				cmd_inq.Parameters.Set("WEEK_DAY", start_time.SubstringNE(0, 6));
			}
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
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
