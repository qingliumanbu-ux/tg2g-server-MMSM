/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 碳控排通过跨分区查询生成生产的数据
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmtk_inq)

int f_mmsmtk_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	try
	{ 	

			begin_time = bcls_rec->Tables[0].Rows[0]["BEGIN_TIME"].ToString();
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString();
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();

			sqlstr_where = " and heat_no in (select heat_no from tmmsmgy05 where start_time<=@end_time  and start_time>=@begin_time)";
			if (heat_no.Trim() != "")
			{
				sqlstr_where = " and heat_no =@heat_no";
			}

			//取生产实绩信息
			sqlstr = "  select HEAT_NO,ST_NO,START_TIME,END_TIME,LADLE_NO, HEAT_TEMP "
				", IRON_WT, IRON_TEMP, IRON_C, IRON_SI, IRON_MN, IRON_P, IRON_S"
				", FIN_C, FIN_P, FIN_S, dev_code, STEEL_WT "
				", nvl((select sum(slab_wt) prod_wt FROM TMMSM33 t2 where t2.heat_no = t1.heat_no), 0) prod_wt "
				", nvl((select max(dev_code) FROM TMMSM21 t2 where t2.heat_no = t1.heat_no), '')"
				"|| nvl((select max(dev_code) FROM TMMSM20 t2 where t2.heat_no = t1.heat_no), '')"
				"|| nvl((select max(dev_code) FROM TMMSM27 t2 where t2.heat_no = t1.heat_no), '')"
				"|| nvl((select max(dev_code) FROM TMMSM26 t2 where t2.heat_no = t1.heat_no), '')"
				"|| nvl((select max(dev_code) FROM TMMSM24 t2 where t2.heat_no = t1.heat_no), '') "
				"|| nvl((select max(dev_code) FROM TMMSM23 t2 where t2.heat_no = t1.heat_no), '')"
				"|| nvl((select max(dev_code) FROM TMMSM25 t2 where t2.heat_no = t1.heat_no), '')"
				"|| nvl((select max(dev_code) FROM TMMSM31 t2 where t2.heat_no = t1.heat_no), '') as BACKLOG_EA "
				", nvl((select round(sum(melt_duration),0) FROM TMMSM21 t2 where t2.heat_no = t1.heat_no), 0) BOF_TIME "
				", nvl((select round(sum(melt_duration),0) FROM TMMSM20 t2 where t2.heat_no = t1.heat_no), 0) EAF_TIME "
				", nvl((select round(sum(melt_duration),0) FROM TMMSM27 t2 where t2.heat_no = t1.heat_no), 0) AOD_TIME  "
				", nvl((select round(sum(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') - "
				" TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60) ),0) FROM TMMSM26 t2 where t2.heat_no = t1.heat_no), 0) LTS_TIME "
				", nvl((select round(sum(melt_duration),0) FROM TMMSM24 t2 where t2.heat_no = t1.heat_no), 0) LF_TIME  "
				", nvl((select round(sum(melt_duration),0) FROM TMMSM23 t2 where t2.heat_no = t1.heat_no), 0) RH_TIME  "
				", nvl((select round(sum(melt_duration),0) FROM TMMSM25 t2 where t2.heat_no = t1.heat_no), 0) VOD_TIME "
				", nvl((select round(sum(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') - "
				" TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)),0) FROM TMMSM31 t2 where t2.heat_no = t1.heat_no), 0) CC_TIME "
				", nvl((select C_DIV FROM tqmts0x t2 where t2.ST_NO = t1.ST_NO), 0) C_DIV "
				" from"
				" (SELECT HEAT_NO, ST_NO, START_TIME, END_TIME, LADLE_NO, OUT_STEEL_TEMP as HEAT_TEMP "
				" , MOLTIRON_WT as IRON_WT, IRON_TEMP, IRON_C, IRON_SI, IRON_MN, IRON_P, IRON_S"
				" , FIN_C, FIN_P, FIN_S, dev_code, actresult as STEEL_WT "
				" FROM TMMSM21"
				" where 1=1"
				+ sqlstr_where +
				" union all"
				" SELECT HEAT_NO, ST_NO, START_TIME, END_TIME, LADLE_NO, OUT_STEEL_TEMP as HEAT_TEMP"
				" , MOLTIRON_WT as IRON_WT, IRON_TEMP, IRON_C, IRON_SI, IRON_MN, IRON_P, IRON_S	"
				" , 0 FIN_C, 0 FIN_P, 0 FIN_S, dev_code, actresult as STEEL_WT "
				" FROM TMMSM27	"
				" where 1 = 1 "
				+ sqlstr_where +
				" ) t1"
				;
			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("begin_time", begin_time);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
			

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = 
				//转炉氧气、氮气
				" SELECT heat_no, dev_code, '59400' as MAT_CODE, BLOW_NUMBER as PROC_COUNT, BLOW_DURATION as PROC_TIME, OXYGEN_FINAL wt "
				" FROM TMMSM21 "
				" WHERE 1 = 1"
				" and OXYGEN_FINAL!=0"
				+ sqlstr_where +
				//转炉氮气
				" union all "
				" SELECT heat_no, dev_code, '59401' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, TOTAL_N2_CONS wt"
				" FROM TMMSM21"
				" WHERE 1 = 1"
				" and TOTAL_N2_CONS!=0"
				+ sqlstr_where +
				" union all"
				//转炉氩气
				" SELECT heat_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, AR_SUM_COMSUME wt "
				" FROM TMMSM21  "
				" WHERE 1 = 1"
				" and ar_sum_comsume!=0"
				+ sqlstr_where +
				//电炉电
				" union all"
				" SELECT heat_no, dev_code, '59103' as MAT_CODE, heat_count as PROC_COUNT, bil_melt_time as PROC_TIME, power_consume as wt"
				" FROM TMMSM20"
				" WHERE 1 = 1"
				" and power_consume!=0"
				+ sqlstr_where +
				//电炉氧气
				" union all"
				" SELECT heat_no, dev_code, '59400' as MAT_CODE, 0 as PROC_COUNT, blow_duration as PROC_TIME, oxygen_final as wt"
				" FROM TMMSM20"
				" WHERE 1 = 1"
				" and oxygen_final!=0"
				+ sqlstr_where +
				//LF精炼氩气
				" union all"
				" select heat_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, AR_SUM_COMSUME wt"
				" from TMMSM24"
				" WHERE 1 = 1"
				" and AR_SUM_COMSUME!=0"
				+ sqlstr_where +
				//LF精炼电
				" union all"
				" select heat_no, dev_code, '59103' as MAT_CODE, HEAT_COUNT as PROC_COUNT, BIL_MELT_time as PROC_TIME, power_consume wt	"
				" from TMMSM24"
				" WHERE 1 = 1"
				" and power_consume!=0"
				+ sqlstr_where +
				//rh精炼氩气
				" union all"
				" select heat_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, AR_SUM_COMSUME wt"
				" from TMMSM23"
				" WHERE 1 = 1"
				" and AR_SUM_COMSUME!=0"
				+ sqlstr_where +
				//rh精炼氮气
				" union all"
				" select heat_no, dev_code, '59401' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, N_SUM_COMSUME wt "
				" from TMMSM23"
				" WHERE 1 = 1 "
				" and N_SUM_COMSUME!=0"
				+ sqlstr_where +
				//rh精炼蒸汽
				" union all"
				" select heat_no, dev_code, '58001' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, 0 - STEAM_TOT wt"
				" from TMMSM23 "
				" WHERE 1 = 1"
				" and STEAM_TOT!=0"
				+ sqlstr_where +
				//VOD氩气
				" union all"
				" select heat_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, AR_SUM_COMSUME wt	"
				" from TMMSM25"
				" WHERE 1 = 1"
				" and AR_SUM_COMSUME!=0"
				+ sqlstr_where +
				//VOD氧气
				" union all"
				" select heat_no, dev_code, '59400' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, oxygen_final wt"
				" from TMMSM25"
				" WHERE 1 = 1"
				" and oxygen_final!=0"
				+ sqlstr_where +
				//vod蒸汽
				" union all"
				" select heat_no, dev_code, '58001' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, 0 - STEAM_TOT wt"
				" from TMMSM25"
				" WHERE 1 = 1 "
				" and STEAM_TOT!=0"
				+ sqlstr_where +
				//AOD氩气
				" union all"
				" select heat_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, total_ar_cons wt"
				" from TMMSM27"
				" WHERE 1 = 1"
				" and total_ar_cons!=0"
				+ sqlstr_where +
				//AOD 氮气
				" union all"
				" select heat_no, dev_code, '59401' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, total_n2_cons wt "
				" from TMMSM27 "
				" WHERE 1=1"
				" and total_n2_cons!=0"
				+ sqlstr_where +
				//AOD氧气
				" union all"
				" select heat_no, dev_code, '59400' as MAT_CODE, 0 as PROC_COUNT, 0 as PROC_TIME, oxygen_final wt "
				" from TMMSM27 "
				" WHERE 1=1"
				" and oxygen_final!=0"
				+ sqlstr_where +
				//LTS氩气
				" union all"
				" select heat_no, dev_code, '59402' as MAT_CODE, 0 as PROC_COUNT, AR_BLOW_TIME as PROC_TIME, ar_sum_comsume wt"
				" from TMMSM26 t1"
				" WHERE 1 = 1"
				" and ar_sum_comsume!=0"
				+ sqlstr_where +
				//2a实绩消耗
				" union all"
				" select heat_no, dev_code, mat_code, 0 as PROC_COUNT, 0 as PROC_TIME, sum(devo_wt) wt"
				" from tmmsm56 t1"
				" where 1 = 1 "
				+ sqlstr_where +
				" group by heat_no, dev_code, mat_code "
				;
		}
		bcls_ret->Tables.Add();
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.Parameters.Set("heat_no",heat_no);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
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
