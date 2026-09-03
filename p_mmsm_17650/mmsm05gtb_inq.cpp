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
BM2F_ENTERACE(mmsm05gtb_inq)

int f_mmsm05gtb_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME_1"].ToString().TrimOrBlank().ToUpper();
		Log::Info("", __FUNCTION__, "end_time_1   =[{0}]", end_time_1);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select "
				" SUBSTR(c.END_TIME, 1, 8) as TAP_TIME, "
				" sum(c.weight) as MOLECULE,  "
				" sum(c.MAT_ACT_WT) as DENOMINATOR,  "
				" round(sum(c.weight) / sum(c.MAT_ACT_WT) * 1000, 4) as GTL  "
				" from "
				" (select BP.HEAT_NO, BP.ST_NO, bp.END_TIME, p.MAT_ACT_WT, sum(x.weight) as weight "
				" from "
				" (select t.HEAT_NO, t.ST_NO, t.END_TIME, t.ACTRESULT, t.ladle_depart_wt - t.empty_ladle_wt as weightact_count from TMMSM21 t) bp "
				" left join "
				" (select c.heat_no, c.DEV_CODE, MAT_CODE, sum(c.weight) as weight from "
				" (select heat_no, DEV_CODE, MAT_CODE, sum(DEVO_WT / 1000) as weight from TMMSMGY08 where DEV_CODE like 'R%' or DEV_CODE like 'F%' or DEV_CODE like 'B%'  group by heat_no, DEV_CODE, MAT_CODE )c "
				" group by c.DEV_CODE, c.heat_no, MAT_CODE)x on bp.HEAT_NO = x.HEAT_NO "
				" LEFT JOIN TMMSM50 A ON X.MAT_CODE = A.MAT_CODE "
				" left join TMMSMBW b on a.MAT_CODE_L2 = b.mat_division "
				" left join( "
				" SELECT HEAT_NO, ST_NO, SUM(MAT_ACT_WT)MAT_ACT_WT, SUM(RECEIVE_WEIGHT)RECEIVE_WEIGHT, MAX(DEV_CODE) DEV_CODE, max(SLAB_CUT_TIME) SLAB_CUT_TIME FROM( "
				" SELECT HEAT_NO, MAT_ACT_WT, RECEIVE_WEIGHT, st_no, DEV_CODE, substr(SLAB_CUT_TIME, 0, 8) SLAB_CUT_TIME FROM TMMSM01 "
				" UNION  ALL "
				" SELECT HEAT_NO, case when mat_no in(SELECT MAT_NO FROM HMMSM96 WHERE EVENT_ID = 'QM05' GROUP BY MAT_NO) then 0 else MAT_ACT_WT end MAT_ACT_WT, "
				" RECEIVE_WEIGHT, st_no, DEV_CODE, substr(SLAB_CUT_TIME, 0, 8) SLAB_CUT_TIME FROM HMMSM01) GROUP BY  HEAT_NO, ST_NO "
				" ) p on bp.HEAT_NO = p.HEAT_NO "
				" where bp.ST_NO != 'DeP'  and substr(bp.HEAT_NO, 0, 1) || substr(bp.ST_NO, 0, 1) != 'B1' "
				" AND to_date(BP.END_TIME, 'yyyy-MM-dd hh24:MI:SS') > SYSDATE - 120 and b.class1 in('铁水') and b.mat_class_des != '矿石' "
				" group by BP.HEAT_NO, BP.ST_NO, bp.END_TIME, P.MAT_ACT_WT)c  "
				"  ";
			if (end_time.Trim() != "")
			{
				sqlstr_temp += " WHERE C.END_TIME >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
				sqlstr_temp += " AND C.END_TIME <= @end_time_1";
			}
			sql_group = " group by SUBSTR(c.END_TIME, 1, 8) order by SUBSTR(c.END_TIME, 1, 8)  ";
			sqlstr = sqlstr + sqlstr_temp + sql_group;
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time_1", end_time_1);
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
