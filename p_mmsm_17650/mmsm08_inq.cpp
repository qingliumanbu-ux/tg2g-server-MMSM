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
BM2F_ENTERACE(mmsm08_inq)

int f_mmsm08_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME_1"].ToString().SubstringNE(0, 8);
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		Log::Info("", __FUNCTION__, "end_time_1   =[{0}]", end_time_1);
		Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", heat_no);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " SELECT DEVO_WT,L2_PROC_NO,case when HEAT_NO not like 'A%' then ' ' else HEAT_NO  end HEAT_NO,REC_CREATE_TIME,case when REC_ERASE_TIME=' ' then REC_ERASE_TIME_1 else REC_ERASE_TIME end REC_ERASE_TIME,PROD_ROUTE  FROM ( "
				" SELECT A.L2_PROC_NO, A.HEAT_NO, SUM(A.DEVO_WT) / 1000 DEVO_WT, MAX(A.REC_CREATE_TIME) REC_CREATE_TIME, nvl(D.REC_ERASE_TIME, '北区电炉') REC_ERASE_TIME, nvl(C.REC_ERASE_TIME, '北区转炉') REC_ERASE_TIME_1, case "
				" when INSTR(t.heatno_premelt1 || t.heatno_premelt2 || t.heatno_premelt3, 'B')>0 THEN "
				" CASE WHEN INSTR(t.heatno_premelt1 || t.heatno_premelt2 || t.heatno_premelt3, 'F')>0  then 'IF+BOF' ELSE 'BOF'  END "
				" when  INSTR(t.heatno_premelt1 || t.heatno_premelt2 || t.heatno_premelt3, 'D')>0 THEN   '三脱' "
				" when  INSTR(t.heatno_premelt1 || t.heatno_premelt2 || t.heatno_premelt3, 'E')>0 THEN "
				" CASE WHEN INSTR(t.heatno_premelt1 || t.heatno_premelt2 || t.heatno_premelt3, 'F')>0  then 'EAF+IF' ELSE 'EAF'   END "
				" when  INSTR(t.heatno_premelt1 || t.heatno_premelt2 || t.heatno_premelt3, 'A')>0 THEN   '回炉钢' "
				" else  ' ' end as PROD_ROUTE "
		" FROM TMMSM2A A "
		" LEFT JOIN SP_MAT_DEFINE_NI B ON A.MAT_CODE = B.MAT_CODE "
		" left join TMMSM20 C ON C.HEAT_NO = A.HEAT_NO "
		" left join TMMSM21 D ON D.HEAT_NO = A.HEAT_NO "
		" LEFT JOIN TMMSM27 T ON T.HEAT_NO = A.HEAT_NO "
		" WHERE B.MAT_NAME_LARGE = '红泥球' "
		" and(a.STATION_ID = 'E' or a.STATION_ID = 'B') "
		" group by A.L2_PROC_NO, A.HEAT_NO, D.REC_ERASE_TIME, C.PROD_ROUTE, C.REC_ERASE_TIME, T.HEATNO_PREMELT1, T.HEATNO_PREMELT2, T.HEATNO_PREMELT3 order by A.L2_PROC_NO desc) WHERE HEAT_NO != ' ' ";
			if (end_time.Trim() != "")
			{
				end_time += "000000";
				sqlstr_temp += " AND REC_CREATE_TIME >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
				end_time_1 += "606060";
				sqlstr_temp += " AND REC_CREATE_TIME <= @end_time_1";
			}
			if (heat_no.Trim() != "")
			{
				sqlstr_temp += " AND HEAT_NO = @heat_no";
			}
			sql_group = "     ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time_1", end_time_1);
			cmd_inq.Parameters.Set("heat_no", heat_no);
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
