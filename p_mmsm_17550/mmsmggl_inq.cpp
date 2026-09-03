/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-05-25
Description: 板坯切断炉次查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmggl_inq)

int f_mmsmggl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sqlstr_group = " ";
	CString v_heat_no = " ";
	CString dev_code = " ";
	CString ladle_arrive_time = " ";
	CString ladle_arrive_time_1 = " ";
	CString st_no = " ";
	CString tc_send_flag = " ";

	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("DEV_CODE"))
			dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("LADLE_ARRIVE_TIME"))
			ladle_arrive_time = bcls_rec->Tables[0].Rows[0]["LADLE_ARRIVE_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("LADLE_ARRIVE_TIME_1"))
			ladle_arrive_time_1 = bcls_rec->Tables[0].Rows[0]["LADLE_ARRIVE_TIME_1"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("TC_SEND_FLAG"))
			tc_send_flag = bcls_rec->Tables[0].Rows[0]["TC_SEND_FLAG"].ToString().TrimOrBlank().ToUpper();
		Log::Info("", __FUNCTION__, "v_heat_no   =[{0}]", v_heat_no);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select C.HEAT_NO,C.ST_NO,C.LADLE_ARRIVE_TIME,C.DEV_CODE, MIN(NVL(G.AFFIRM_FLAG,0)) AFFIRM_FLAG,MAX(TC_SEND_FLAG) TC_SEND_FLAG "
				" from TMMSM31 C "
				" LEFT JOIN TMMSMGY06 G ON C.HEAT_NO = G.HEAT_NO where 1=1  ";

			if (v_heat_no.Trim() != "")
			{
				sqlstr_temp += " AND C.HEAT_NO = @v_heat_no";
			}
			if (dev_code.Trim() != "")
			{
				sqlstr_temp += " AND C.DEV_CODE = @dev_code";
			}
			if (st_no.Trim() != "")
			{
				sqlstr_temp += " AND C.ST_NO = @st_no ";
			}
			if (ladle_arrive_time.Trim() != "")
			{
				sqlstr_temp += " AND C.LADLE_ARRIVE_TIME >= @ladle_arrive_time";
			}
			if (ladle_arrive_time_1.Trim() != "")
			{
				sqlstr_temp += " AND C.LADLE_ARRIVE_TIME <= @ladle_arrive_time_1";
			}
			if (tc_send_flag.Trim() != ""){
				sqlstr_temp += " AND G.TC_SEND_FLAG = @TC_SEND_FLAG";
			}
			sqlstr_group = " GROUP BY C.HEAT_NO,C.ST_NO,C.LADLE_ARRIVE_TIME,C.DEV_CODE ";
			sqlstr = sqlstr + sqlstr_temp + sqlstr_group;
			cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
			cmd_inq.Parameters.Set("dev_code", dev_code);
			cmd_inq.Parameters.Set("st_no", st_no);
			cmd_inq.Parameters.Set("ladle_arrive_time", ladle_arrive_time);
			cmd_inq.Parameters.Set("ladle_arrive_time_1", ladle_arrive_time_1);
			cmd_inq.Parameters.Set("TC_SEND_FLAG", tc_send_flag);
			cmd_inq.SetCommandText(sqlstr);
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
