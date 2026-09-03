/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    1.0
Date:       2016-07-21
Description: 铸坯组批信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/
  


 
/*<remark>=========================================================
/// <summary>
/// 铸坯组批信息查询
/// <para>
/// 铸坯组批信息查询
/// </para>
/// </summary>
===========================================================</remark>*/
/* ***** 静态函数申明 ***** */
//从字符串中根据指定分隔符拆分数据
// 入口字符，分隔字符，函数是返回字符信息。
CString f_mmsm_get_multi_value2(CString v_in_str, CString v_spilit_flag, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm38f2_inq)


int f_mmsm38f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString cs_mat_no = "";
	int blkNum = 0;
	int fetchRowCount = 0;
	CString cs_pono = "";
	CString cs_heat_no = "";
	CString cs_factory_div = "";
	CString cs_stock_place_no = "";
	CString cs_MERGE_FLAG = "";
	CString	cs_tmp("");

	CString sqlstr01 = "";
	CString sqlstr38 = "";
	CString sqlcount = "";
	CString sqlstr("");

	CModel tmmsm38("TMMSM38");
	CModel tqmts29("TQMTQQ0");
	CModel tep0002("TEP0002");

	CDbCommand tmmsm01cmd(conn);
	CDbCommand tmmsm38cmd(conn);
	CDbCommand cmd_inq(conn);

	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	try
	{
		/* ***** 获取输入参数 ***** */
		cs_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		cs_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		cs_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		cs_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		cs_MERGE_FLAG = bcls_rec->Tables[0].Rows[0]["MERGE_FLAG"].ToString().Trim();

		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];

		/* ***** 打印输入参数 ***** */
		EDLog(1, 1, "cs_mat_no			= [%s]", (const char*)cs_mat_no);
		EDLog(1, 1, "cs_pono			= [%s]", (const char*)cs_pono);
		EDLog(1, 1, "cs_heat_no			= [%s]", (const char*)cs_heat_no);
		EDLog(1, 1, "cs_factory_div     =[%s]", (const char*)cs_factory_div);
		EDLog(1, 1, "cs_stock_place_no  =[%s]", (const char*)cs_stock_place_no);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr01 = " SELECT * "
				"   FROM TMMSM01 "
				"  WHERE MAT_LINE_TYPE = 'SM' "
				"    AND IN_FLAG = '1' "
				"    AND MAT_STATUS='29' ";
			sqlstr38 = " SELECT * FROM TMMSM38 WHERE 1 = 1 ";
			sqlcount = " SELECT COUNT(MAT_NO) "
				"   FROM TMMSM01 "
				"  WHERE MAT_LINE_TYPE = 'SM' "
				"    AND IN_FLAG = '1' "
				"    AND MAT_STATUS='29' ";
			break;
		}

		if (cs_mat_no.Trim() != "")
		{
			//根据指定分隔符，拆分字符信息。
			cs_tmp = f_mmsm_get_multi_value2(cs_mat_no, "\n", conn);

			if (cs_tmp.Trim() != "")
			{//返回的信息，不为空。
				if (cs_mat_no.Trim().GetLength() <= 8)
				{//进行模糊查询==LIKE .
					sqlstr01 += " AND MAT_NO LIKE @cs_mat_no ||'%' ";
					sqlstr38 += " AND MAT_NO LIKE @cs_mat_no ||'%' ";
					sqlcount += " AND MAT_NO LIKE @cs_mat_no ||'%' ";
				}
				else
				{
					//进行字符拆分处理。 ===f_get_multi_value() 
					sqlstr01 += " AND MAT_NO in ( " + cs_tmp + " ) ";
					sqlstr38 += " AND MAT_NO in ( " + cs_tmp + " ) ";
					sqlcount += " AND MAT_NO in ( " + cs_tmp + " ) ";
				}
			}
		}
		if (cs_pono.Trim() != "")
		{
			//根据指定分隔符，拆分字符信息。
			cs_tmp = f_mmsm_get_multi_value2(cs_pono, "\n", conn);

			if (cs_tmp.Trim() != "")
			{//返回的信息，不为空。
				if (cs_pono.Trim().GetLength() <= 4)
				{//进行模糊查询==LIKE .
					sqlstr01 += " AND PONO LIKE @cs_pono ||'%' ";
					sqlstr38 += " AND PONO LIKE @cs_pono ||'%' ";
					sqlcount += " AND PONO LIKE @cs_pono ||'%' ";
				}
				else
				{
					//进行字符拆分处理。 ===f_get_multi_value() 
					sqlstr01 += " AND PONO in ( " + cs_tmp + " ) ";
					sqlstr38 += " AND PONO in ( " + cs_tmp + " ) ";
					sqlcount += " AND PONO in ( " + cs_tmp + " ) ";
				}
			}
		}
		if (cs_heat_no.Trim() != "")
		{
			//根据指定分隔符，拆分字符信息。
			cs_tmp = f_mmsm_get_multi_value2(cs_heat_no, "\n", conn);

			if (cs_tmp.Trim() != "")
			{//返回的信息，不为空。
				if (cs_heat_no.Trim().GetLength() <= 4)
				{//进行模糊查询==LIKE .
					sqlstr01 += " AND HEAT_NO LIKE @cs_heat_no ||'%' ";
					sqlstr38 += " AND HEAT_NO LIKE @cs_heat_no ||'%' ";
					sqlcount += " AND HEAT_NO LIKE @cs_heat_no ||'%' ";
				}
				else
				{
					//进行字符拆分处理。 ===f_get_multi_value() 
					sqlstr01 += " AND HEAT_NO in ( " + cs_tmp + " ) ";
					sqlstr38 += " AND HEAT_NO in ( " + cs_tmp + " ) ";
					sqlcount += " AND HEAT_NO in ( " + cs_tmp + " ) ";
				}
			}
		}
		if (cs_factory_div.Trim() != "")
		{
			sqlstr01 = sqlstr01 + "AND FACTORY_DIV = @cs_factory_div ";
			sqlstr38 = sqlstr38 + "AND FACTORY_DIV = @cs_factory_div ";
			sqlcount = sqlcount + "AND FACTORY_DIV = @cs_factory_div ";
		}
		if (cs_MERGE_FLAG.Trim() != "")
		{
			if (cs_MERGE_FLAG == "M")
			{
				sqlstr01 = sqlstr01 + "AND MERGE_FLAG = 'M' ";
				sqlcount = sqlcount + "AND MERGE_FLAG = 'M' ";
			}
			else
			{
				sqlstr01 = sqlstr01 + "AND MERGE_FLAG <> 'M' ";
				sqlcount = sqlcount + "AND MERGE_FLAG <> 'M' ";
			}
		}
		sqlstr01 = sqlstr01 + "ORDER BY MAT_NO";
		sqlstr38 = sqlstr38 + "ORDER BY RESUME_SEQ_NO,HEAT_NO,MAT_NO";
		//sqlcount = sqlcount + "ORDER BY MAT_NO";

		//Log::Trace("", "", "sqlstr01 = [{0}]", sqlstr01);
		//Log::Trace("", "", "sqlstr38 = [{0}]", sqlstr38);
		//Log::Trace("", "", "sqlcount = [{0}]", sqlcount);

		tmmsm01cmd.SetCommandText(sqlcount);
		tmmsm01cmd.Parameters.Set("cs_mat_no", cs_mat_no);
		tmmsm01cmd.Parameters.Set("cs_pono", cs_pono);
		tmmsm01cmd.Parameters.Set("cs_heat_no", cs_heat_no);
		tmmsm01cmd.Parameters.Set("cs_factory_div", cs_factory_div);

		cd_count = tmmsm01cmd.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		tmmsm01cmd.SetCommandText(sqlstr01);
		tmmsm01cmd.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		tmmsm01cmd.Close();
		//Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr01);
		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();

		bcls_ret->Tables.Add();
		tmmsm38cmd.SetCommandText(sqlstr38);
		tmmsm38cmd.Parameters.Set("cs_mat_no", cs_mat_no);
		tmmsm38cmd.Parameters.Set("cs_pono", cs_pono);
		tmmsm38cmd.Parameters.Set("cs_factory_div", cs_factory_div);
		tmmsm38cmd.ExecuteQuery(bcls_ret->Tables[2]);
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
