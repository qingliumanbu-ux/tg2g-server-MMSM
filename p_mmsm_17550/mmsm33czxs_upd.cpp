/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2024-01-11 17:13:56
Description: 板坯修正系数信息修改
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsm33czxs_upd)

int f_mmsm33czxs_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;


	CString v_coe_a = "";
	CString v_update_time_limit = "";
	CString v_strand_no = "";
	CString v_st_no = "";

	 
	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm33czxs("TMMSM33CZXS");

	CDbCommand cmd_inq(conn);

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tmmsm33czxs.Reset();
			tmmsm33czxs.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			tmmsm33czxs.Update("*", "STRAND_NO,ST_NO,C_DIV");

		}
		/*try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}*/


		//--------------------------------
		//获取传入参数
		//tmmsm33czxs.MergeFrom(bcls_rec->Tables[0].Rows[0]);


		/*if (bcls_rec->Tables[0].Columns.Contains("STRAND_NO"))
			v_strand_no = bcls_rec->Tables[0].Rows[0]["STRAND_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();

		if (bcls_rec->Tables[0].Columns.Contains("COE_A"))
			v_coe_a = bcls_rec->Tables[0].Rows[0]["COE_A"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("UPDATE_TIME_LIMIT"))
			v_update_time_limit = bcls_rec->Tables[0].Rows[0]["UPDATE_TIME_LIMIT"].ToString();


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM TMMSM33CZXS "
				"  WHERE 1=1 "
				;
			//sqlstr = " SELECT * "
			//	"   FROM TMMSM33CZXS "
			//	"  WHERE 1=1 "
			//	;
			 
			
			sqlstr = "UPDATE TMMSM33CZXS"
				"	SET COE_A = @v_coe_a "
				"   ,UPDATE_TIME_LIMIT = @v_update_time_limit "
				"  WHERE STRAND_NO = @v_strand_no AND ST_NO = @v_st_no "
				;


			sqlstr_count = sqlstr_count + sqlstr_temp;

			sqlstr = sqlstr + sqlstr_temp;
			break;
		}

		cmd_inq.Parameters.Set("v_coe_a", v_coe_a);
		cmd_inq.Parameters.Set("v_update_time_limit", v_update_time_limit);
		cmd_inq.Parameters.Set("v_strand_no", v_strand_no);
		cmd_inq.Parameters.Set("v_st_no", v_st_no);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;*/

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
