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
BM2F_ENTERACE(mmsmx1_inq)

int f_mmsmx1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
		CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
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
		tmmsmyl["YEAR_MON"] = tmmsmyl["YEAR_MON"].ToString().SubstringNE(0, 6);
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
		if (tmmsmyl.GetFields().Contains("YEAR_MON") && tmmsmyl["YEAR_MON"].ToString().Trim() != "")
		{
			Log::Info("", __FUNCTION__, "YEAR_MON =[{0}]", tmmsmyl["YEAR_MON"].ToString());
			sqlstr_temp += " AND YEAR_MON		like '%'|| @YEAR_MON||'%'";
			cmd_inq.Parameters.Set("YEAR_MON", tmmsmyl["YEAR_MON"].ToString());
		}
		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr_temp += " ORDER BY REC_CREATE_TIME DESC";
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

		tmmsmyl["YEAR_MON"] = dateNow.SubstringNE(0, 6);
		if (tmmsmyl.QueryCount("YEAR_MON") == 0)
		{
			sqlstr = " SELECT * FROM  TMMSMX1 WHERE  YEAR_MON=  (SELECT TO_CHAR(ADD_MONTHS(SYSDATE,-1),'yyyyMM')  FROM DUAL) ";;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				tmmsmyl.Reset();
				cmd_inq.Fetch(tmmsmyl);
				tmmsmyl["REC_CREATOR"] = "AUTO";
				tmmsmyl["REC_CREATE_TIME"] = dateNow;
				tmmsmyl["FIN_CONFM_FLAG"] = "0";
				tmmsmyl["YEAR_MON"] = dateNow.SubstringNE(0, 6);
				tmmsmyl.Insert();
			}
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
