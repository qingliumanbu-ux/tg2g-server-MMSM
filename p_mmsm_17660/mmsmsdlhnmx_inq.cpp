/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-09-11
Description: 炉成本查询
**************************************************/
//框架头文件
#include "stdafx.h"	 

BM2F_ENTERACE(mmsmsdlhnmx_inq)

int f_mmsmsdlhnmx_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	CString heat_nr = "";
	CString cast_seq = "";
	CString grade_id = "";
	CString grade_type1 = "";
	CString f_route1 = "";
	CString c_flag = "0";
	CString ss_flag = "0";
	CString v_from = "";//开始时刻
	CString v_to = "";//开始时刻
	CPageInfo pageInfo;



	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_from = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().SubstringNE(0, 8);
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_to = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		}

		if (bcls_rec->Tables[0].Columns.Contains("HEATNR"))
		{
			heat_nr = bcls_rec->Tables[0].Rows[0]["HEATNR"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CAST_SEQ"))
		{
			cast_seq = bcls_rec->Tables[0].Rows[0]["CAST_SEQ"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("GRADE_ID"))
		{
			grade_id = bcls_rec->Tables[0].Rows[0]["GRADE_ID"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE1"))
		{
			grade_type1 = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("F_ROUTE1"))
		{
			f_route1 = bcls_rec->Tables[0].Rows[0]["F_ROUTE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("C_FLAG"))
		{
			c_flag = bcls_rec->Tables[0].Rows[0]["C_FLAG"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("SS_FLAG"))
		{
			ss_flag = bcls_rec->Tables[0].Rows[0]["SS_FLAG"].ToString().Trim();
		}
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr = "select * from ZTC_V_DTCB_HEATNR_RATIO t where 1=1";

			if (heat_nr != ""){
				sqlstr += " AND t.HEATNR = @heat_nr";
			}
			if (cast_seq != ""){
				sqlstr += " AND t.CAST_SEQ = @cast_seq";
			}
			if (grade_id != ""){
				sqlstr += " AND t.GRADE_ID = @grade_id";
			}
			if (grade_type1 != ""){
				sqlstr += " AND t.GRADE_TYPE1 = @grade_type1";
			}
			if (f_route1 != ""){
				sqlstr += " AND t.F_ROUTE1 = @f_route1";
			}
			if (v_from.Trim() != "")
			{
				sqlstr += " AND t.AOD_BOF_E_DTIME>= @v_from";
			}
			if (v_to.Trim() != "")
			{
				sqlstr += " AND t.AOD_BOF_E_DTIME<= @v_to";
			}
			if (c_flag == "1"&&ss_flag == "0")
			{
				sqlstr += " AND (t.GRADE_ID like '2%' or t.GRADE_ID like '3%' or t.GRADE_ID like '5%') ";
			}
			if (ss_flag == "1"&&c_flag == "0")
			{
				sqlstr += " AND (t.GRADE_ID like '1%' or t.GRADE_ID like '4%') ";
			}
			sqlstr += " order by t.AOD_BOF_E_DTIME desc";
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", v_from);
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", v_to);
			cmd_inq.SetCommandText(sqlstr);

			cmd_inq.Parameters.Set("heat_nr", heat_nr);
			cmd_inq.Parameters.Set("cast_seq", cast_seq);
			cmd_inq.Parameters.Set("grade_id", grade_id);
			cmd_inq.Parameters.Set("grade_type1", grade_type1);
			cmd_inq.Parameters.Set("f_route1", f_route1);
			cmd_inq.Parameters.Set("v_from", v_from);
			cmd_inq.Parameters.Set("v_to", v_to);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}