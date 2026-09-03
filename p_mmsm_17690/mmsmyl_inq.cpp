/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmyl_inq)

int f_mmsmyl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int		TotalRecordCount = 0;
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_table_name = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CDbCommand cmd_inq(conn);
	//系统的分页类信息。
	CPageInfo pageInfo;
	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
		{
			v_table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString();
		}
		if (v_table_name.Trim() == "")
		{
			sprintf(s.msg, "【表名称】不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		CModel tmmsmyl(v_table_name);
		tmmsmyl.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		Log::Info("", __FUNCTION__, "TABLE_NAME =[{0}]", v_table_name);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr_count = " SELECT COUNT(1) FROM " + v_table_name + " WHERE 1=1 ";
			sqlstr = " SELECT * FROM " + v_table_name + "  WHERE 1=1 ";

			break;
		}
		if (tmmsmyl.GetFields().Contains("MAT_CODE")&&tmmsmyl["MAT_CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_CODE		like '%'|| @MAT_CODE||'%'";
			cmd_inq.Parameters.Set("MAT_CODE", tmmsmyl["MAT_CODE"].ToString());
		}
		if (tmmsmyl.GetFields().Contains("MAT_NAME") && tmmsmyl["MAT_NAME"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND MAT_NAME       like '%'|| @MAT_NAME||'%'";
			cmd_inq.Parameters.Set("MAT_NAME", tmmsmyl["MAT_NAME"].ToString());
		}
		if (tmmsmyl.GetFields().Contains("LOT_NO") && tmmsmyl["LOT_NO"].ToString().Trim() != "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
		{
			sqlstr_temp += " AND LOT_NO       like '%'|| @LOT_NO||'%'";
			cmd_inq.Parameters.Set("LOT_NO", tmmsmyl["LOT_NO"].ToString());
		}

		if (tmmsmyl.GetFields().Contains("DATE_FROM") && tmmsmyl["DATE_FROM"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND PROD_DATE >= @DATE_FROM";
			cmd_inq.Parameters.Set("DATE_FROM", tmmsmyl["DATE_FROM"].ToString());
		}
		if (tmmsmyl.GetFields().Contains("DATE_TO") && tmmsmyl["DATE_TO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND PROD_DATE <= @DATE_TO";
			cmd_inq.Parameters.Set("DATE_TO", tmmsmyl["DATE_TO"].ToString());
		}
		if (tmmsmyl.GetFields().Contains("C_STATE") && tmmsmyl["C_STATE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND C_STATE=@C_STATE";
			cmd_inq.Parameters.Set("C_STATE", tmmsmyl["C_STATE"].ToString());
		}

		if (tmmsmyl.GetFields().Contains("PROD_DATE") && tmmsmyl["PROD_DATE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND PROD_DATE = @PROD_DATE";
			cmd_inq.Parameters.Set("PROD_DATE", tmmsmyl["PROD_DATE"].ToString());
		}
		if (tmmsmyl.GetFields().Contains("CODE") && tmmsmyl["CODE"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND CODE  like '%'|| @CODE||'%'";
			cmd_inq.Parameters.Set("CODE", tmmsmyl["CODE"].ToString());
		}
		if (tmmsmyl.GetFields().Contains("ST_NO") && tmmsmyl["ST_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND ST_NO  like '%'|| @ST_NO||'%'";
			cmd_inq.Parameters.Set("ST_NO", tmmsmyl["ST_NO"].ToString());
		}
		
		sqlstr_count = sqlstr_count + sqlstr_temp;

		if (tmmsmyl.GetFields().Contains("PROD_DATE"))
		{
			sqlstr_temp += " ORDER BY PROD_DATE DESC";
		}
		
		sqlstr = sqlstr + sqlstr_temp;
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		
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
