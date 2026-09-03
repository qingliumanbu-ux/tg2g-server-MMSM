/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-12-21
Description: 查预溶液画面的2a详细数据
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


BM2F_ENTERACE(mmsmg2a_inq)


int f_mmsmg2a_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CDecimal FURNACE_AGE_SM = 0;
	int i = 0;
	int fetchRowCount = 0;
	CString v_proc_no = "";
	CString v_heat_no = "";
	CString sqlstr_temp = "";
	CString dev_code = "";

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tmmsm27("TMMSM27");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		CString div = bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		if (div == "GGL"){
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " select distinct A.HEAT_NO,A.ST_NO,A.DEV_CODE,A.PROC_NO,c.AFFIRM_FLAG,c.CONFIRM_TIME from ( "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from( "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm19 "
					" UNION ALL "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm20 "
					" UNION ALL "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm21 "
					" UNION ALL "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm22 "
					" UNION ALL "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm23 "
					" UNION ALL "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm24 "
					" UNION ALL "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm25 "
					" UNION ALL "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm26 "
					" UNION ALL "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm27 "
					" UNION ALL "
					" select HEAT_NO, ST_NO, DEV_CODE, PROC_NO from tmmsm31 "
					" ) "
					" ) A left join TMMSMGY06 C ON A.HEAT_NO = C.HEAT_NO where 1=1 ";
				if (v_heat_no.Trim() != "")
				{
					sqlstr += " AND A.HEAT_NO = @v_heat_no";
				}

				sqlstr_temp += " ";

				sqlstr = sqlstr + sqlstr_temp;

				break;
			}
			cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		if (div == "GYLX"){
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " select HEAT_NO,DEV_CODE,PROC_NO,START_TIME,END_TIME,REMARK from ( "
					" select HEAT_NO, DEV_CODE, PROC_NO, START_TIME, END_TIME, ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) REMARK from tmmsm19 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO, START_TIME, END_TIME, ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) REMARK from tmmsm20 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO, START_TIME, END_TIME, ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) REMARK from tmmsm21 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO, START_TIME, END_TIME, ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) REMARK from tmmsm22 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO, START_TIME, END_TIME, ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) REMARK from tmmsm23 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO, START_TIME, END_TIME, ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) REMARK from tmmsm24 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO, START_TIME, END_TIME, ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) REMARK from tmmsm25 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO, START_TIME, END_TIME, ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) REMARK from tmmsm26 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO, START_TIME, END_TIME,ROUND(ROUND(TO_NUMBER(TO_DATE(END_TIME, 'YYYYMMDDhh24miss') -TO_DATE(START_TIME, 'YYYYMMDDhh24miss')) * 24 * 60 * 60)/60,0) REMARK from tmmsm27 "
					" ) where 1=1 ";
				if (v_heat_no.Trim() != "")
				{
					sqlstr += " AND HEAT_NO = @v_heat_no";
				}

				sqlstr_temp += " ";

				sqlstr = sqlstr + sqlstr_temp;

				break;
			}
			cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
			cmd_inq.Close();
		}
		if (div == "ZPH"){
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " select * from VMMSM01 where 1=1 ";
				if (v_heat_no.Trim() != "")
				{
					sqlstr += " AND HEAT_NO = @v_heat_no";
				}

				sqlstr_temp += " ";

				sqlstr = sqlstr + sqlstr_temp;

				break;
			}
			cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
			cmd_inq.Close();
		}
		if (div == "HMMSMGY06"){
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " select * from HMMSMGY06 where 1=1 ";
				if (v_heat_no.Trim() != "")
				{
					sqlstr += " AND HEAT_NO = @v_heat_no";
				}

				sqlstr_temp += " ";

				sqlstr = sqlstr + sqlstr_temp;

				break;
			}
			cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
			cmd_inq.Close();
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetMsg() };
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
