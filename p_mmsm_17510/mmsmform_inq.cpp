//*************************************************
//Copyright: Baosight Software LTD.co Copyright (c) 2010
//Author:   向萍
//Version:    1.0
//Date:     2015-11-25
//Description: 炼钢物料画面查询
//***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmform_inq)

int f_mmsmform_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";

	CString v_program_name = "";

	CString v_curr_part_name = "";

	int	  TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;


	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}



		if (bcls_rec->Tables[0].Columns.Contains("PROGRAM_NAME"))
			v_program_name = bcls_rec->Tables[0].Rows[0]["PROGRAM_NAME"].ToString();

		if (bcls_rec->Tables[0].Columns.Contains("CURR_PART_NAME"))
			v_curr_part_name = bcls_rec->Tables[0].Rows[0]["CURR_PART_NAME"].ToString();

		Log::Info("", __FUNCTION__, "v_program_name =[{0}]", v_program_name);


		/****** 打印输入参数 ******/
		Log::Info("", __FUNCTION__, "v_curr_part_name =[{0}]", v_curr_part_name);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			/*sqlstr = " select a.name form_ename , a.description form_cname , b.form_base_name form_bname  from  tesformresinfo a, tesformpara b"
			" where a.name = b.form_name "
			" and a.name like @program_name ||'%'"
			" order by a.name "
			;*/

			/*	sqlstr = "  SELECT A.NAME FORM_ENAME , A.DESCRIPTION FORM_CNAME , B.*  FROM (SELECT NAME NAME,MAX(DESCRIPTION) DESCRIPTION FROM TESFORMRESINFO WHERE 1=1 GROUP BY NAME)  A ,TESFORMPARA B "
			" WHERE A.NAME = B.FORM_NAME "
			" AND A.NAME LIKE @program_name ||'%'"
			" ORDER BY A.NAME "
			;*/



			sqlstr = " SELECT A.NAME FORM_ENAME, A.DESCRIPTION FORM_CNAME, A.ABBREV ABBREV, B.* FROM (SELECT NAME NAME, MAX(DESCRIPTION) DESCRIPTION, ABBREV FROM TESFORMRESINFO WHERE 1 = 1 GROUP BY NAME, ABBREV)  A, TESFORMPARA B"
				" WHERE A.NAME = B.FORM_NAME "
				" AND A.ABBREV =  @curr_part_name "
				" AND A.NAME LIKE   @program_name ||'%'"
				" ORDER BY A.NAME"
				;




			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("program_name", v_program_name);
			cmd_inq.Parameters.Set("curr_part_name", v_curr_part_name);
			TotalRecordCount = cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
			cmd_inq.Close();
		}



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

