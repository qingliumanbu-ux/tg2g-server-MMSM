/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-13 17:13:56
Description: 装车计划查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmts01_inq)

int f_mmsmts01_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm88_0("TMMSM88_0");

	CDbCommand cmd_inq(conn);

	try{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		//--------------------------------
		//获取传入参数
		tmmsm88_0.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm88_0.Print();

		sqlstr_count = "SELECT COUNT(1) FROM TMMSM88_0 WHERE 1=1 ";
		sqlstr = "SELECT * FROM TMMSM88_0 T WHERE 1 = 1 ";

		//计划号
		if (tmmsm88_0["PLAN_NO"].ToString().Trim() != ""){
			sqlstr_temp += "AND T.PLAT_NO LIKE '%' || @PLAT_NO || '%' ";
			cmd_inq.Parameters.Set("PLAT_NO", tmmsm88_0["PLAN_NO"]);
		}

		//材料号
		if (tmmsm88_0["MAT_NO"].ToString().Trim() != ""){
			sqlstr_temp += "AND T.MAT_NO LIKE '%' || @MAT_NO || '%' ";
			cmd_inq.Parameters.Set("MAT_NO", tmmsm88_0["MAT_NO"]);
		}

		//装点工厂代码
		if(tmmsm88_0["LOAD_CODE_FACTORY"].ToString().Trim() != ""){
			sqlstr_temp += "AND T.LOAD_CODE_FACTORY LIKE '%' || @LOAD_CODE_FACTORY || '%' ";
			cmd_inq.Parameters.Set("LOAD_CODE_FACTORY", tmmsm88_0["LOAD_CODE_FACTORY"]);
		}

		//装点区域代码
		if (tmmsm88_0["LOAD_CODE_AREA"].ToString().Trim() != ""){
			sqlstr_temp += "AND T.LOAD_CODE_AREA LIKE '%' || @LOAD_CODE_AREA || '%' ";
			cmd_inq.Parameters.Set("LOAD_CODE_AREA", tmmsm88_0["LOAD_CODE_AREA"]);
		}

		//装点代码
		if (tmmsm88_0["LOAD_CODE"].ToString().Trim() != ""){
			sqlstr_temp += "AND T.LOAD_CODE LIKE '%' || @LOAD_CODE || '%' ";
			cmd_inq.Parameters.Set("LOAD_CODE", tmmsm88_0["LOAD_CODE"]);
		}

		//卸点工厂代码
		if (tmmsm88_0["UNLOAD_CODE_FACTORY"].ToString().Trim() != ""){
			sqlstr_temp += "AND T.UNLOAD_CODE_FACTORY LIKE '%' || @UNLOAD_CODE_FACTORY || '%' ";
			cmd_inq.Parameters.Set("UNLOAD_CODE_FACTORY", tmmsm88_0["UNLOAD_CODE_FACTORY"]);
		}

		//卸点区域代码
		if (tmmsm88_0["UNLOAD_CODE_AREA"].ToString().Trim() != ""){
			sqlstr_temp += "AND T.UNLOAD_CODE_AREA LIKE '%' || @UNLOAD_CODE_AREA || '%' ";
			cmd_inq.Parameters.Set("UNLOAD_CODE_AREA", tmmsm88_0["UNLOAD_CODE_AREA"]);
		}

		//卸点代码
		if (tmmsm88_0["UNLOAD_CODE"].ToString().Trim() != ""){
			sqlstr_temp += "AND T.UNLOAD_CODE LIKE '%' || @UNLOAD_CODE || '%' ";
			cmd_inq.Parameters.Set("UNLOAD_CODE", tmmsm88_0["UNLOAD_CODE"]);
		}

		sqlstr_count = sqlstr_count + sqlstr_temp;
		sqlstr_temp += "ORDER BY  PLAN_NO";
		sqlstr = sqlstr + sqlstr_temp;

		Log::Trace("", __FUNCTION__, "sqlstr_count = {0}", sqlstr_count);
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();

		
		//分页获取
		Log::Trace("", __FUNCTION__, "sqlstr = {0}", sqlstr);
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