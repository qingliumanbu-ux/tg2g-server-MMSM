/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2013
Author:     ljnie
Version:    1.0
Date:       2016/7/7 16:28:52
Description: 库存信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 
//#include "tmmsm12b.h"

/*<remark>=========================================================
/// <summary>
/// 库存信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(mmsm12b_inq)

int f_mmsm12b_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString	datetime("");
	CDecimal totalCount = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	CDecimal d_thick_fr = 0;
	CDecimal d_thick_to = 0;
	CString v_time_fr = "";
	CString v_time_to = "";
	CString roll_plan_no = "";
	CString ingot_code = "";
	CString flag = "";
	CString raw_origin = "";
	CString prod_shift_no = "";
	CString prod_shift_group = "";

	/* 实体类定义 */
	//CTWMA1 tmmsm12b(conn);
	CModel tmmsm12b = CModel("TMMSM12B");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString s_userid("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		// 获取前台传入参数
		s_userid = s.userid;
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
	
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();			//炉号
		CString pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();	//制造命令号


		if (bcls_rec->Tables[0].Columns.Contains("TPD_START_TIME"))
			v_time_fr = bcls_rec->Tables[0].Rows[0]["TPD_START_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("TPD_END_TIME"))
			v_time_to = bcls_rec->Tables[0].Rows[0]["TPD_END_TIME"].ToString();


		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"]; //每页记录数
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];       //需查询的页号


		Log::Trace("", __FUNCTION__, "传入参数 in_stock_time_from					= [{0}]", v_time_fr);
		Log::Trace("", __FUNCTION__, "传入参数 in_stock_time_to					= [{0}]", v_time_to);
		
		Log::Trace("", __FUNCTION__, "传入参数 heat_no			= [{0}]", heat_no);
		Log::Trace("", __FUNCTION__, "传入参数 pono			= [{0}]", pono);
	
		Log::Trace("", __FUNCTION__, "传入参数 record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "传入参数 current_page_no		= [{0}]", current_page_no);
		


		sqlstr = " SELECT * FROM TMMSM12B t1 WHERE 1=1 ";

		
		if (heat_no != "")
		{
			sqlstr += " AND t1.HEAT_NO = @heat_no ";
		}
		if (pono != "")
		{
			sqlstr += " AND t1.PONO = @pono ";
		}
	
		if (prod_shift_no != "")
		{
			sqlstr += " AND t1.prod_shift_no = @prod_shift_no ";
		}
		if (prod_shift_group != "")
		{
			sqlstr += " AND t1.prod_shift_group = @prod_shift_group ";
		}

		
		if (v_time_fr != "")
		{
			sqlstr += " AND t1.TPD_START_TIME >= @in_stock_time_from ";
		}
		if (v_time_to != "")
		{
			sqlstr += " AND t1.TPD_END_TIME <= @in_stock_time_to ";
		}
		
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.Parameters.Set("pono", pono);
		cmd_inq.Parameters.Set("in_stock_time_from", v_time_fr);
		cmd_inq.Parameters.Set("in_stock_time_to", v_time_to);
		cmd_inq.Parameters.Set("prod_shift_no", prod_shift_no);
		cmd_inq.Parameters.Set("prod_shift_group", prod_shift_group);

		sqlstr_count = "SELECT COUNT(1) FROM (" + sqlstr + ") T ";
		Log::Trace("", __FUNCTION__, "sqlstr_count = [{0}]", sqlstr_count);
		cmd_inq.SetCommandText(sqlstr_count);
		totalCount = cmd_inq.ExecuteScalar();
		Log::Trace("", __FUNCTION__, "totalCount = [{0}]", totalCount);
		cmd_inq.Close();

		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > totalCount.ToDouble())
		{
			start_row = 0;
		}
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0]["TOTAL_RECORD"] = totalCount.ToInt32();
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
