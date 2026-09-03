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
BM2F_ENTERACE(mmsmbof_inq)

int f_mmsmbof_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString v_heat_no = " ";

	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		Log::Info("", __FUNCTION__, "v_heat_no   =[{0}]", v_heat_no);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "  SELECT A.DEV_CODE as A_DEV_CODE, "
				"  A.ST_NO A_ST_NO, "
				"  A.HEAT_NO A_HEAT_NO, "
				"  A.TAP_END_TIME A_TAP_END_TIME, "
				"  ROUND(TO_NUMBER(TO_DATE(B.START_TIME, 'YYYYMMDDhh24miss') - "
				"  TO_DATE(A.TAP_START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60) B_A_TIME, "
				"  ROUND(TO_NUMBER(TO_DATE(B.END_TIME, 'YYYYMMDDhh24miss') - "
				"  TO_DATE(B.START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60) B_B_TIME, "
				"  ROUND(TO_NUMBER(TO_DATE(C.START_TIME, 'YYYYMMDDhh24miss') - "
				"  TO_DATE(B.END_TIME, 'YYYYMMDDhh24miss')) * 24 * 60) B_C_TIME, "
				"  ROUND(TO_NUMBER(TO_DATE(C.END_TIME, 'YYYYMMDDhh24miss') - "
				"  TO_DATE(C.START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60) C_C_TIME, "
				"  D.PROC_NO D_PROC_NO, "
				"  D.IRON_LADLE_NO D_IRON_LADLE_NO, "
				"  D.DEV_CODE D_DEV_CODE, "
				"  B.DEV_CODE B_DEV_CODE, "
				"  C.DEV_CODE C_DEV_CODE, "
				"  ROUND(TO_NUMBER(TO_DATE(k.DES_END, 'YYYYMMDDhh24miss') - "
				"  TO_DATE(k.DES_START, 'YYYYMMDDhh24miss')) * 24 * 60) KR_KR_TIME, "
				"  ROUND(TO_NUMBER(TO_DATE(A.TAP_END_TIME, 'YYYYMMDDhh24miss') - "
				"  TO_DATE(P.START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60) A_TAP, "
				"  ROUND(TO_NUMBER(TO_DATE(K.DES_START, 'YYYYMMDDhh24miss') - "
				"  TO_DATE(T.TIME_TORPEDO_OUT, 'YYYYMMDDhh24miss')) * 24 * 60) KR_DG_TIME, "
				"  ROUND(TO_NUMBER(TO_DATE(K.DES_START, 'YYYYMMDDhh24miss') - "
				"  TO_DATE(P.START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60) KR_A_TIME "
				"  FROM tmmsm21 A "
				"  left join TMMSM24 B "
				"  ON A.HEAT_NO = B.HEAT_NO "
				"  left join TMMSM23 C "
				"  ON A.HEAT_NO = C.HEAT_NO "
				"  LEFT JOIN TMMSM14 D "
				"  ON A.HEAT_NO = D.HEAT_NO "
				"  left join tmmsmKR14 k "
				"  on a.HEAT_NO = k.HEAT_NO "
				"  LEFT JOIN(select START_TIME, heat_no "
				"  from TPSSM34 "
				"  WHERE substr(DEV_CODE, 1, 1) = 'B' "
				"  and STATUS_NAME = 'CHARGEHM' "
				"  and EVENT_ID = '3') P "
				"  ON A.HEAT_NO = P.HEAT_NO "
				"  left join tmmsm11 T "
				"  ON A.HEAT_NO = T.HEAT_NO WHERE A.HEAT_NO != ' ' ";
				
			if (v_heat_no.Trim() != "")
			{
				sqlstr_temp += " AND A.HEAT_NO = @v_heat_no";
			}

			sqlstr = sqlstr + sqlstr_temp;
			cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
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
