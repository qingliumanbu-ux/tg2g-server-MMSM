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
BM2F_ENTERACE(mmsmc_inq)

int f_mmsmc_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME_1"].ToString().TrimOrBlank().ToUpper();
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

			sqlstr = " select aq.HEAT_NO, "
				" d.PROC_NUMBER, "
				" bwc.MAT_TYPE                                                                                              as mat_type, "
				" d.ST_NO, "
				" decode(substr(aq.ST_NO, 0, 1), '1', "
				" decode(substr(aq.ST_NO, 2, 1), 'A', '镍钢', 'D', '镍钢', 'F', '铬钢', 'M', '铬钢', '不锈钢'), '2', '碳钢', '3', '硅钢', "
				" '4', decode(substr(aq.ST_NO, 2, 1), 'A', '镍钢', 'D', '镍钢', 'F', '铬钢', 'M', '铬钢', '不锈钢'), '5', "
				" '碳钢')                                                                                              as gradeact_type, "
				" DAG.GRADE_TYPE3, "
				" aq.ACTRESULT                                                                                              as WEIGHTACT, "
				" d.MAT_CODE                                                                                                   MATID, "
				" d.MAT_NAME, "
				" sum(d.weight)                                                                                                DEVO_WT, "
				" f.MAT_ACT_WT                                                                                              as MAT_ACT_WT, "
				" aq.END_TIME, "
				" d.AGGREGATE_NAME                                                                                                   AGGREGATE_NAME, "
				" d.cr                                                                                                      as mat_cr, "
				" d.ni                                                                                                      as mat_ni, "
				" d.mo                                                                                                      as mat_mo, "
				" d.c                                                                                                       as mat_c, "
				" d.si                                                                                                      as mat_si, "
				" d.p                                                                                                       as mat_p, "
				" d.s                                                                                                       as mat_s, "
				" h.ELM_001, "
				" h.ELM_002, "
				" h.ELM_003, "
				" h.ELM_004, "
				" h.ELM_005, "
				" h.ELM_006, "
				" h.ELM_007, "
				" h.ELM_008, "
				" h.ELM_009, "
				" h.ELM_010, "
				" h.ELM_011, "
				" h.ELM_012, "
				" h.ELM_013, "
				" h.ELM_014 "
				" from(select daa.HEAT_NO, daa.ST_NO, daa.ACTRESULT, daa.START_TIME, daa.END_TIME "
				" from TMMSM27 daa "
				" WHERE to_date(daa.END_TIME, 'yyyy-MM-dd hh24:MI:SS') > sysdate - 400 "
				" union all "
				" select dab.HEAT_NO, dab.ST_NO, dab.ACTRESULT, dab.START_TIME, dab.END_TIME "
				" from TMMSM21 dab "
				" where dab.ST_NO != 'DeP' "
				" and to_date(dab.END_TIME, 'yyyy-MM-dd hh24:MI:SS') > sysdate - 400) aq "
				" left join(select T.PROC_NUMBER, "
				" T.HEAT_NUMBER, "
				" n.START_TIME, "
				" X.ST_NO, "
				" t.PROC_NUMBER proc_no, "
				" t.AGGREGATE_NAME, "
				" x.MAT_CODE, "
				" a.MAT_NAME, "
				" a.matclass, "
				" a.MAT_CODE_L2, "
				" x.weight                                                           as weight, "
				" sum(x.weight) over(partition by t.HEAT_NUMBER, t.PROC_NUMBER, t.AGGREGATE_NAME) as agg_hzweigh, "
				" sum(x.weight) over(partition by t.HEAT_NUMBER)                        as heat_hzweight, "
				" ab.L2_PROC_NO                                                      as proc_number_c, "
				" ab.nu, "
				" X.WEIGH_NO, "
				" MT.QUALITY_BATCH_NO, "
				" BE.CR                                                              as CR, "
				" BE.NI                                                              AS NI, "
				" BE.MO                                                              AS MO, "
				" BE.C                                                               AS C, "
				" BE.SI                                                              AS SI, "
				" BE.P                                                               AS P, "
				" BE.S                                                               AS S "
				" from DA_HEAT_RELATION T "
				" LEFT JOIN(select c.HEAT_NO, "
				" c.DEV_CODE, "
				" MAT_CODE, "
				" sum(c.weight) as weight, "
				" C.WEIGH_NO, "
				" C.ST_NO,C.L2_PROC_NO "
				" from(select HEAT_NO             HEAT_NO, "
				" DEV_CODE, "
				" MAT_CODE, "
				" SUM(OUT_STOCK_WT / 1000) weight, "
				" WEIGH_NO, "
				" ST_NO,L2_PROC_NO "
				" from TMMSM56 "
				" where to_date(REC_CREATE_TIME, 'yyyy-MM-dd hh24:MI:SS') > sysdate - 470 "
				" group by HEAT_NO, DEV_CODE, MAT_CODE, WEIGH_NO, ST_NO,L2_PROC_NO) c "
				" group by c.DEV_CODE, c.HEAT_NO, MAT_CODE, C.WEIGH_NO, ST_NO,L2_PROC_NO) X "
				" ON X.L2_PROC_NO=T.PROC_NUMBER AND substr(X.DEV_CODE, 0, 1) = substr(T.AGGREGATE_NAME, 0, 1) "
				" LEFT JOIN DA_HEAT_RELATION_SYN N ON T.HEAT_NUMBER = N.HEAT_NUMBER "
				" LEFT JOIN(select * from TEP0002 where CODE_CLASS = 'MMLC01') E ON X.MAT_CODE = e.CODE "
				" LEFT JOIN TMMSM50 A ON X.MAT_CODE = A.MAT_CODE "
				" left join(SELECT A.L2_PROC_NO, A.HEAT_NO, COUNT(*) nu "
				" FROM TMMSM27 A "
				" LEFT JOIN TMMSM21 B ON A.HEAT_NO = B.HEAT_NO "
				" LEFT JOIN TMMSM19 C ON A.HEAT_NO = C.HEAT_NO "
				" LEFT JOIN TMMSM20 D ON A.HEAT_NO = D.HEAT_NO "
				" GROUP BY A.L2_PROC_NO, A.HEAT_NO) ab on t.PROC_NUMBER = ab.L2_PROC_NO "
				" left join TMMSM81 MT ON X.WEIGH_NO = MT.WEIGH_NO "
				" left join(SELECT QUALITY_BATCH_NO, "
				" MAT_CODE, "
				" WEIGH_NO, "
				" SUM(DECODE(ELM_NAME, 'Cr', ELM_VALUE, 0)) CR, "
				" SUM(DECODE(ELM_NAME, 'Ni', ELM_VALUE, 0)) NI, "
				" SUM(DECODE(ELM_NAME, 'Mo', ELM_VALUE, 0)) MO, "
				" SUM(DECODE(ELM_NAME, 'C', ELM_VALUE, 0))  C, "
				" SUM(DECODE(ELM_NAME, 'Si', ELM_VALUE, 0)) SI, "
				" SUM(DECODE(ELM_NAME, 'P', ELM_VALUE, 0))  P, "
				" SUM(DECODE(ELM_NAME, 'S', ELM_VALUE, 0))  S "
				" FROM TMMSM81AL "
				" group by QUALITY_BATCH_NO, MAT_CODE, WEIGH_NO) BE "
				" ON MT.QUALITY_BATCH_NO = BE.QUALITY_BATCH_NO "
				" where substr(t.AGGREGATE_NAME, 0, 1) != 'C' "
				" and nvl(T.GRADEACT, 0) != 'DeP' "
				" and substr(T.HEAT_NUMBER, 0, 1) || substr(T.GRADEACT, 0, 1) != 'B1') d on aq.heat_no = d.HEAT_NUMBER "
				" left join TQMTSB0 h on aq.HEAT_NO = h.HEAT_NO "
				" left join(SELECT HEAT_NO, "
				" SUM(MAT_ACT_WT)     MAT_ACT_WT, "
				" SUM(RECEIVE_WEIGHT) RECEIVE_WEIGHT, "
				" max(SLAB_CUT_TIME)  SLAB_CUT_TIME "
				" FROM(SELECT HEAT_NO, MAT_ACT_WT, RECEIVE_WEIGHT, DEV_CODE, substr(SLAB_CUT_TIME, 0, 8) SLAB_CUT_TIME "
				" FROM TMMSM01 "
				" UNION ALL "
				" SELECT HEAT_NO, "
				" case "
				" when mat_no in(SELECT MAT_NO FROM HMMSM96 WHERE EVENT_ID = 'QM05' GROUP BY MAT_NO) "
				" then 0 "
				" else MAT_ACT_WT end     MAT_ACT_WT, "
		" RECEIVE_WEIGHT, "
		" DEV_CODE, "
		" substr(SLAB_CUT_TIME, 0, 8) SLAB_CUT_TIME "
		" FROM HMMSM01) "
		" GROUP BY HEAT_NO) f on d.HEAT_NUMBER = f.HEAT_NO "
		" LEFT JOIN TQMTSB10 DAG ON D.ST_NO = DAG.STEEL_GRADE "
		" LEFT JOIN TMMSM50 bwc ON d.MAT_CODE = BWC.MAT_CODE "
		" where d.MAT_CODE != ' ' "
			" and d.AGGREGATE_NAME not like 'D' ";
			if (end_time.Trim() != "")
			{
				end_time += "000000";
				sqlstr_temp += " AND aq.END_TIME >= @end_time";
			}
			if (end_time_1.Trim() != "")
			{
				end_time_1 += "606060";
				sqlstr_temp += " AND aq.END_TIME <= @end_time_1";
			}
			if (heat_no.Trim() != "")
			{
				sqlstr_temp += " AND aq.HEAT_NO = @heat_no";
			}
			sql_group = " group by aq.HEAT_NO, d.PROC_NUMBER, bwc.MAT_TYPE, d.ST_NO, decode(substr(aq.ST_NO, 0, 1), '1', "
				" decode(substr(aq.ST_NO, 2, 1), 'A', '镍钢', 'D', '镍钢', 'F', '铬钢', 'M', '铬钢', '不锈钢'), '2', '碳钢', '3', '硅钢', "
				" '4', decode(substr(aq.ST_NO, 2, 1), 'A', '镍钢', 'D', '镍钢', 'F', '铬钢', 'M', '铬钢', '不锈钢'), '5', "
				" '碳钢'), DAG.GRADE_TYPE3, aq.ACTRESULT, d.MAT_CODE, d.MAT_NAME, f.MAT_ACT_WT, aq.END_TIME, d.AGGREGATE_NAME, d.cr, d.ni, d.mo, d.c, d.si, d.p, d.s, h.ELM_001, h.ELM_002, h.ELM_003, h.ELM_004, h.ELM_005, h.ELM_006, h.ELM_007, h.ELM_008, h.ELM_009, h.ELM_010, h.ELM_011, h.ELM_012, h.ELM_013, h.ELM_014 "
				" ORDER BY aq.HEAT_NO, bwc.MAT_TYPE, d.MAT_CODE, aq.END_TIME desc   ";
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
