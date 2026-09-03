/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:
Date:     2023/3/11
Description: 导入查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmlcl_inq)

int f_mmsmlcl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int TotalRecordCount = 0;

	CString table_name = "";
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);

	try
	{
		try
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " with table1 as (select mat_no,heat_no,st_no,mat_act_wt from tmmsm01 where RECV_MAT_TIME >= '20240301000000' "
				" and RECV_MAT_TIME <= '20240330999999' "
				" and MAT_STATUS != '9' "
				" union select mat_no, heat_no, st_no, mat_act_wt from hmmsm01 where RECV_MAT_TIME >= '20240301000000' "
				" and RECV_MAT_TIME <= '20240330999999' "
				" and MAT_STATUS != '9' ) "
				" select ST_NO, HEAT_NO, ACTRESULT, "
				" nvl((select sum(mat_act_wt) from table1 where st_no = t.st_no),0)                                                   AS HG_WT, "
				" NVL((select SUM(ACTRESULT) "
				" from tmmsm27 "
				" where heat_no in(select heat_no from table1) "
				" and st_no = t.st_no),0)                                                                                      AS TC_WT, "
				" case when (ACTRESULT * (select sum(mat_act_wt) from table1 where st_no = t.st_no) / "
				" (select SUM(ACTRESULT) from tmmsm27 where heat_no in(select heat_no from table1) and st_no = t.st_no)) is null then 0 else ROUND((ACTRESULT * (select sum(mat_act_wt) from table1 where st_no = t.st_no) / "
				" (select SUM(ACTRESULT) from tmmsm27 where heat_no in(select heat_no from table1) and st_no = t.st_no)), 3) end PH_WT	"
				" from tmmsm27 t "
				" where heat_no in(select heat_no from table1) "
				" union all "
				" select ST_NO, HEAT_NO, ACTRESULT, "
				" nvl((select sum(mat_act_wt) from table1 where st_no = t.st_no),0)                                                   AS HG_WT, "
				" NVL((select SUM(ACTRESULT) "
				" from tmmsm20 "
				" where heat_no in(select heat_no from table1) "
				" and st_no = t.st_no),0)                                                                                       AS TC_WT, "
				" case when (ACTRESULT * (select sum(mat_act_wt) from table1 where st_no = t.st_no) / "
				" (select SUM(ACTRESULT) from tmmsm20 where heat_no in(select heat_no from table1) and st_no = t.st_no)) is null then 0 else ROUND((ACTRESULT * (select sum(mat_act_wt) from table1 where st_no = t.st_no) / "
				" (select SUM(ACTRESULT) from tmmsm20 where heat_no in(select heat_no from table1) and st_no = t.st_no)), 3) end PH_WT " 
				" from tmmsm20 t "
				" where heat_no in(select heat_no from table1) "
				" union all "
				" select ST_NO, HEAT_NO, ACTRESULT, "
				" nvl((select sum(mat_act_wt) from table1 where st_no = t.st_no),0)                                                   AS HG_WT, "
				" NVL((select SUM(ACTRESULT) "
				" from tmmsm20 "
				" where heat_no in(select heat_no from table1) "
				" and st_no = t.st_no),0)                                                                                      AS TC_WT, "
				" case when (ACTRESULT * (select sum(mat_act_wt) from table1 where st_no = t.st_no) / "
				" (select SUM(ACTRESULT) from tmmsm21 where heat_no in(select heat_no from table1) and st_no = t.st_no)) is null then 0 else ROUND((ACTRESULT * (select sum(mat_act_wt) from table1 where st_no = t.st_no) / "
				" (select SUM(ACTRESULT) from tmmsm21 where heat_no in(select heat_no from table1) and st_no = t.st_no)), 3) end PH_WT " 
				" from tmmsm21 t "
				" where heat_no in(select heat_no from table1) "
				;
			sqlstr_count = "select count(*) from (" + sqlstr + ")" + sqlstr_temp;
			sqlstr_temp += " ";
			sqlstr = sqlstr + sqlstr_temp;
			Log::Trace("", "", "sqlstr[{0}]", sqlstr);
			break;
		}

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

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
