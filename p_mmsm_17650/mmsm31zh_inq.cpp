/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 不锈钢全线消耗
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm31zh_inq)

int f_mmsm31zh_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString end_time = " ";
	CString end_time_1 = " ";
	CString heat_no = " ";
	CString send_t823 = " ";
	CString czhi = " ";

	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("LADLE_ARRIVE_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["LADLE_ARRIVE_TIME"].ToString().SubstringNE(0, 8);
		if (bcls_rec->Tables[0].Columns.Contains("LADLE_ARRIVE_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["LADLE_ARRIVE_TIME_1"].ToString().SubstringNE(0, 8);
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		CString div = bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SEND_T823"))
			send_t823 = bcls_rec->Tables[0].Rows[0]["SEND_T823"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("CZHI"))
			czhi = bcls_rec->Tables[0].Rows[0]["CZHI"].ToString().TrimOrBlank().ToUpper();
		Log::Info("", __FUNCTION__, "end_time_1   =[{0}]", end_time_1);
		Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", heat_no);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "  select a.SM_PLAN_NOL2,A.L2_PROC_NO, A.HEAT_NO, a.LADLE_ARRIVE_TIME,C.SEND_T823,D.CZHI "
				"  from TMMSM31 A "
				"  LEFT JOIN TMMSMGY05 C ON A.SM_PLAN_NOL2 = C.SM_PLAN_NOL2 "
				"  LEFT JOIN(select T.HEAT_NO, NVL(T.DEVO_WT, 0) - NVL(C.DEVO_WT, 0) AS CZHI "
				"  from(SELECT sum(DEVO_WT) DEVO_WT, HEAT_NO, SM_PLAN_NOL2 "
				"  FROM tmmsm56 "
				"  group by HEAT_NO, SM_PLAN_NOL2) T "
				"  LEFT JOIN(select A.HEAT_NO, sum(A.DEVO_WT) DEVO_WT "
				"  from HMMSM2A A "
				"  left join(select MAX(SUBSTR(REC_CREATE_TIME, 0, 10)) REC_CREATE_TIME, HEAT_NO "
				"  from HMMSM2A "
				"  where REC_CREATE_TIME != ' ' "
				"  group by HEAT_NO) B "
				"  on SUBSTR(A.REC_CREATE_TIME, 0, 10) = b.REC_CREATE_TIME and a.HEAT_NO = b.HEAT_NO "
				"  where a.REC_CREATE_TIME != ' ' "
				"  and b.REC_CREATE_TIME != ' ' "
				"  group by A.HEAT_NO) C ON T.HEAT_NO = C.HEAT_NO) D ON A.HEAT_NO = D.HEAT_NO"
			" where 1=1 ";
			if (end_time.Trim() != "")
			{
				end_time += "000000";
				sqlstr_temp += " AND a.LADLE_ARRIVE_TIME >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
				end_time_1 += "606060";
				sqlstr_temp += " AND a.LADLE_ARRIVE_TIME <= @end_time_1";
			}
			if (heat_no.Trim() != "")
			{
				sqlstr_temp += " AND a.HEAT_NO = @heat_no";
			}
			if (send_t823.Trim() != ""){
				if (send_t823 == "3"){
					sqlstr_temp += " AND C.SEND_T823 !='1' ";
				}
				else{
					sqlstr_temp += " AND C.SEND_T823 = @send_t823";
				}
				
			}
			if (czhi != " "){
				if (czhi == "0"){
					sqlstr_temp += " AND D.CZHI = @CZHI";
				}
				if (czhi == "1"){
					sqlstr_temp += " AND D.CZHI > 0";
				}

				if (czhi == "2"){
					sqlstr_temp += " AND D.CZHI < 0";
				}

				if (czhi == "3"){
					sqlstr_temp += " AND D.CZHI != 0";
				}
				
			}
			sql_group = "    ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time_1", end_time_1);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("send_t823", send_t823);
			cmd_inq.Parameters.Set("CZHI", czhi);
			cmd_inq.SetCommandText(sqlstr);
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
