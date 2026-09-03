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
BM2F_ENTERACE(mmsmcr09_inq)

int f_mmsmcr09_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0,8);
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME_1"].ToString().SubstringNE(0, 8);

		if (end_time.Trim() == "" || end_time_1.Trim() == "")
		{
			strcpy(s.msg, "开始时间结束时间不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		end_time += "000000";
		end_time_1 += "235959";
		Log::Info("", __FUNCTION__, "end_time_1   =[{0}]", end_time_1);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = " select t.HEAT_NO,  MOLECULE,  DENOMINATOR "
				" ,ROUND(nvl(CASE WHEN nvl(DENOMINATOR,0) = 0 OR MOLECULE = 0 THEN 0 ELSE MOLECULE / DENOMINATOR END, 0), 4) * 1000 GTL "
				" , END_TIME as TAP_END_TIME "
				" from tmmsm21 t"
				" left join "
				" (select heat_no, sum(OUT_STOCK_WT / 1000) as MOLECULE "
				" from tmmsm56 t1 "
				" where  MAT_CODE  in ( SELECT MAT_CODE FROM TMMSM50 WHERE SEND_FLAG != '1' and MAT_TYPE in('2','4'))"
				//" and dev_code like 'B%'"
				"  and exists(select 1 from tmmsm21 t2 where t2.heat_no = t1.heat_no and t2.st_no != 'DeP' AND END_TIME <= @end_time_1 and END_TIME >= @end_time)"				
				" group by heat_no ) bp on t.heat_no=bp.heat_no" 				
				" left join (  SELECT HEAT_NO,SUM(MAT_ACT_WT) DENOMINATOR, SUM(RECEIVE_WEIGHT)RECEIVE_WEIGHT, MAX(DEV_CODE) DEV_CODE, max(SLAB_CUT_TIME) SLAB_CUT_TIME "
				" FROM VMMSMCPCL_BB1  t1"					   
				" where 1=1"
				" and exists (select 1 from tmmsm21 t2 where t2.heat_no=t1.heat_no and t2.st_no != 'DeP' AND END_TIME <= @end_time_1 and END_TIME >= @end_time)"
				" GROUP BY  HEAT_NO "
				" ) p   on bp.HEAT_NO = p.HEAT_NO  "
				" where  t.st_no != 'DeP' AND t.END_TIME <= @end_time_1 and t.END_TIME >= @end_time"
				" order by HEAT_NO  "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("end_time", end_time);
			cmd_inq.Parameters.Set("end_time_1", end_time_1); 			
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
