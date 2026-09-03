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
BM2F_ENTERACE(mmsmtf08_inq)

int f_mmsmtf08_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString v_heat_no = " ";
	CString l2_proc_no = " ";

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
				sqlstr = " select HEAT_NO,DEV_CODE,PROC_NO,L2_PROC_NO from ( "
					" select HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO from tmmsm19 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO from tmmsm20 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO from tmmsm21 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO from tmmsm23 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO from tmmsm24 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO from tmmsm25 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO from tmmsm26 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO from tmmsm27 "
					" UNION ALL "
					" select HEAT_NO, DEV_CODE, PROC_NO,L2_PROC_NO from tmmsm31 "
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
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
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
		if (div == "NY"){
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " select * from TMMSMT8F0 where 1=1 ";
				if (v_heat_no.Trim() != "")
				{
					sqlstr += " AND HEAT_NO = @v_heat_no";
				}

				sqlstr_temp += " ";

				sqlstr = sqlstr + sqlstr_temp+" order by EMREADTIME desc ";

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
