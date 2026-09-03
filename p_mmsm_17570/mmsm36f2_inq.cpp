/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wangshuling
Version:    1.0
Date:       2023-09-10
Description: 板坯缓冷操作实绩查询
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明
int f_mmsm36f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsm36f2_inq)

int f_mmsm36f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString qy_table = "";//表名
	CString v_start_time = "";//开始时刻
	CString v_end_time = ""; //结束时刻
	CString v_heat_no = "";
	CString v_mat_no = "";

	//2024-05-09
	CString v_prod_shift_group = ""; //班组

	CDecimal cd_count = 0;

	//2024-05-15
	CString archive_flag = "";//记录类型

	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */


	/* 业务变量 */
	CModel tmmsm36("TMMSM36");

	CModel tmmsm01("TMMSM01");

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	CString sqlstr_count;
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{


		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM")
				&& bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = 100000;
				current_page_no = 1;
			}
		}
		else {
			record_count_per_page = 100000;
			current_page_no = 1;
		}

		tmmsm36.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("QUERY_DIV"))
		{
			qy_table = bcls_rec->Tables[0].Rows[0]["QUERY_DIV"].ToString();
			
		}
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_start_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_GROUP"))
		{
			v_prod_shift_group = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_GROUP"].ToString().Trim();
		}
		
		//2024-05-15
		if (bcls_rec->Tables[0].Columns.Contains("ARCHIVE_FLAG"))
		{
			archive_flag = bcls_rec->Tables[0].Rows[0]["ARCHIVE_FLAG"].ToString().Trim();
			
		}


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			Log::Trace("", archive_flag, "archive_flag[{0}]  ", archive_flag);
			Log::Trace("", archive_flag, "archive_flag[{0}]  ", qy_table);

			//这里需要判断一下，如果选择是：历史数据H,在线T ----针对：F2
			if (archive_flag.Trim() != "")
			{
				if (archive_flag == "H"&&qy_table!="TMMSM36")
				{
					qy_table = "HMMSM01";
				}
				if (archive_flag == "T"&&qy_table != "TMMSM36")
				{
					qy_table = "TMMSM01";
				}				
			}
			/*
				日期：2024-05-24
				原因：F3按钮，无法获取archive_flag对于新增的数据，如果是历史数据是查不出来的
				针对F3
			*/
			if (archive_flag.Trim() == "")
			{
				if (tmmsm36["MAT_NO"].ToString().Trim() != ""&&qy_table != "TMMSM36"){
					tmmsm01["MAT_NO"] = tmmsm36["MAT_NO"];
					int count = tmmsm01.QueryCount("MAT_NO");
					if (count == 0)
					{
						qy_table = "HMMSM01";
					}
				}
			}


			sqlstr_count = " SELECT COUNT(1) FROM " + qy_table + " WHERE 1=1 ";

			sqlstr = " SELECT *  FROM  " + qy_table + " WHERE 1=1 ";

		

			if (tmmsm36["HEAT_NO"].ToString().Trim() != "")
			{
				sqlstr_count += " AND HEAT_NO like '%" + v_heat_no + "%'";
				sqlstr += " AND HEAT_NO like '%" + v_heat_no + "%'";
			}
			if (qy_table == "TMMSM01")
			{
				if (v_start_time.Trim() != "")
				{
					//sqlstr += " AND PROD_TIME >= '" + v_start_time + "'";
				}
				if (v_end_time.Trim() != "")
				{
					//sqlstr += " AND PROD_TIME <= '" + v_end_time + "'";
				}
			}
			else if (qy_table == "TMMSM36")
			{
				if (tmmsm36["START_TIME"].ToString().Trim() != ""){
					//sqlstr += " AND START_TIME >= @START_TIME";
					//sqlstr += " AND REC_CREATE_TIME >= @START_TIME";
				}
				if (tmmsm36["END_TIME"].ToString().Trim() != "")
				{
					//sqlstr += " AND END_TIME <= @END_TIME";
					//sqlstr += " AND REC_CREATE_TIME <= @END_TIME";
				}
			}
			if (tmmsm36["PROD_SHIFT_NO"].ToString().Trim() != "")
			{
				sqlstr_count += "  AND PROD_SHIFT_NO = @PROD_SHIFT_NO";
				sqlstr += "  AND PROD_SHIFT_NO = @PROD_SHIFT_NO";
			}
			if (tmmsm36["PROD_SHIFT_GROUP"].ToString().Trim() != ""&&qy_table == "TMMSM36")
			{
				sqlstr_count += " AND INSPECT_SHIFT = '"+v_prod_shift_group+"'";
				sqlstr += " AND INSPECT_SHIFT = '" + v_prod_shift_group + "'";
			}
			if (tmmsm36["MAT_NO"].ToString().Trim() != "")
			{
				sqlstr_count += " AND MAT_NO like '%" + v_mat_no + "%'";
				sqlstr += " AND MAT_NO like '%" + v_mat_no + "%'";
			}
			
			cmd_inq.SetCommandText(sqlstr_count);
			cd_count = cmd_inq.ExecuteScalar();
			start_row = record_count_per_page * (current_page_no - 1);
			if (start_row > cd_count.ToDouble())
			{
				start_row = 0;
			}


			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
			cmd_inq.Parameters.Set("START_TIME", tmmsm36["START_TIME"].ToString());
			cmd_inq.Parameters.Set("END_TIME", tmmsm36["END_TIME"].ToString());
			cmd_inq.Parameters.Set("PROD_SHIFT_NO", tmmsm36["PROD_SHIFT_NO"].ToString());
			//cmd_inq.Parameters.Set("PROD_SHIFT_GROUP", v_prod_shift_group);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
			cmd_inq.Close();

			bcls_ret->Tables.Add("PAGEINFO");	//增加块
			bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
			bcls_ret->Tables["PAGEINFO"].Rows.Add();
			bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count;
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


