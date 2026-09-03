/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-28
Version:1.0
Description: 
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 盘库画面和收货画面，修改钢种时要获取钢牌号，钢种描述等信息
/// 
/// 
/// 
/// 
/// 
/// 
/// 
/// 
/// 
/// 
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsmgetdata_inq)


int f_mmsmgetdata_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int rowCount = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString v_proc_div = "";//根据这个标记区分画面
	CString v_st_no = "";
	CString v_pono = "";
	CString v_heat_no = "";
	CString v_heat_no_01a1 = "";
	CString v_st_no11 = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss"); 
	int		TotalRecordCount = 0;

	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			v_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO_01A1"))
			v_heat_no_01a1 = bcls_rec->Tables[0].Rows[0]["HEAT_NO_01A1"].ToString().Trim();

		Log::Trace("", "", "v_pono【{0}】 v_heat_no [{1}]", v_pono, v_heat_no);
		if (v_proc_div == "MMSMACSHS2N")
		{
			sqlstr = "SELECT  SG_GRADE_1  FROM TQMTS0X  WHERE 1 =1 ";

			if (v_st_no != "")
			{
				sqlstr_temp += " AND ST_NO = '" + v_st_no + "'";
			}



			sqlstr += sqlstr_temp;

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

		}
		//盘库修改事件中，修改mat_no时需要获取的数据
		else  if (v_proc_div == "MMSM01A1_MAT_NO")
		{
			bcls_ret->Tables.Add();
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "CAST_DIV_NO");
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NO");
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "PONO");

			sqlstr = "select ROW_NUMBER() over ( ORDER BY CAST_NUM DESC ),CAST_DIV_NO FROM ( "
				" SELECT SUBSTR(SLAB_NO, 18, 3) CAST_DIV_NO, COUNT(1) AS CAST_NUM FROM TMMSM01 WHERE HEAT_NO = '" + v_heat_no_01a1 + "'  "
				" GROUP BY  SUBSTR(SLAB_NO, 18, 3)																		   "
				" UNION ALL																								   "
				" SELECT SUBSTR(SLAB_NO, 18, 3) CAST_DIV_NO, COUNT(1) AS CAST_NUM  FROM HMMSM01 WHERE HEAT_NO = '" + v_heat_no_01a1 + "' "
				" GROUP BY  SUBSTR(SLAB_NO, 18, 3))";
			Log::Trace("", "", "sqlstr【{0}】",  sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				bcls_ret->Tables[0].Rows[0]["CAST_DIV_NO"] = cmd_inq.GetString(2);
			}
			cmd_inq.Close();


			
			sqlstr = "SELECT SM_PLAN_NOL2,PONO  FROM TPSSM11 WHERE HEAT_NO = '" + v_heat_no_01a1 + "' ";

			Log::Trace("", "", "sqlstr111【{0}】", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				bcls_ret->Tables[0].Rows[0]["SM_PLAN_NO"] = cmd_inq.GetString(1);
				bcls_ret->Tables[0].Rows[0]["PONO"] = cmd_inq.GetString(2);
			}
			cmd_inq.Close();

			if (bcls_ret->Tables[0].Rows[0]["SM_PLAN_NO"].ToString().Trim() == "" ||
				bcls_ret->Tables[0].Rows[0]["PONO"].ToString().Trim() == "")
			{
				
				sqlstr = "SELECT SM_PLAN_NOL2,PONO  FROM TPSSM41 WHERE HEAT_NO = '" + v_heat_no_01a1 + "' ";
				Log::Trace("", "", "sqlstr111【{0}】", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					bcls_ret->Tables[0].Rows[0]["SM_PLAN_NO"] = cmd_inq.GetString(1);
					bcls_ret->Tables[0].Rows[0]["PONO"] = cmd_inq.GetString(2);
				}
				cmd_inq.Close();

				if (bcls_ret->Tables[0].Rows[0]["SM_PLAN_NO"].ToString().Trim() == "" ||
					bcls_ret->Tables[0].Rows[0]["PONO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "没有从计划数据中获取到计划号和制造命令号，请重新确认材料号的数据！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}


			if (bcls_ret->Tables[0].Rows[0]["CAST_DIV_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "计划中没有获取到浇内顺序号，请联系计划人员！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

		}
		else if (v_proc_div == "MMSM01A1_ST_NO")
		{
			if (bcls_rec->Tables[0].Columns.Contains("ST_NO11"))
				v_st_no11 = bcls_rec->Tables[0].Rows[0]["ST_NO11"].ToString().Trim();

			bcls_ret->Tables.Add();
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "SG_GRADE_1");

			sqlstr = "SELECT  SG_GRADE_1  FROM TQMTS0X  WHERE ST_NO = '" + v_st_no11 + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				bcls_ret->Tables[0].Rows[0]["SG_GRADE_1"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();

		}

		if (v_proc_div == "MMSM01A1S2N")
		{

		}


		Log::Trace("", "", "sqlstr【{0}】",sqlstr);





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
