/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     郝东炜
Version:    1.0
Date:       2016-10-12
Description: 炼钢钢坯材料信息查询（汇总）
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢钢坯材料信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2F_ENTERACE(mmsm0021a1_inq)

int f_mmsm0021a1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_prod_time_from("");
	CString	cs_prod_time_to("");
	CString	cs_mat_no("");
	CString	cs_order_no("");
	CString	cs_heat_no("");
	CString	cs_pono("");
	CString	cs_sg_sign("");
	CString	cs_mat_status("");
	CString	cs_in_flag("");
	CString	cs_archive_flag("");
	CDecimal cd_mat_thick = 0;
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_temp;
	CPageInfo pageInfo;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		cs_archive_flag = bcls_rec->Tables[0].Rows[0]["ARCHIVE_FLAG"].ToString().Trim();
		//record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
		//current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];

		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		//Log::Trace("", __FUNCTION__, "cs_archive_flag			= [{0}]", cs_archive_flag);
		//Log::Trace("", __FUNCTION__, "record_count_per_page	= [{0}]", record_count_per_page);
		//Log::Trace("", __FUNCTION__, "current_page_no			= [{0}]", current_page_no);

		/* 检查输入参数合法性 */
		if (cs_archive_flag.Trim() == "")
		{
			sprintf(s.msg, "记录类型不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		cmd_inq.Parameters.Clear();
		int count_row = bcls_rec->Tables[1].Columns.get_Count();
		//Log::Trace("", __FUNCTION__, "count_row			= [{0}]", count_row);

		for (int i = 0; i < count_row; i++)
		{
			if (bcls_rec->Tables[1].Rows[0][i].ToString().Trim().GetLength() == 0)
			{
				continue;
			}
			Log::Trace("", __FUNCTION__, "bcls_rec->Tables[1].Rows[0][i].ToString()			= [{0}]", bcls_rec->Tables[1].Rows[0][i].ToString());

			if (bcls_rec->Tables[1].Columns[i].get_ColumnName().Trim() == "MAT_NUM"
				|| bcls_rec->Tables[1].Columns[i].get_ColumnName().Trim() == "MAT_WT"
				|| bcls_rec->Tables[1].Columns[i].get_ColumnName().Trim() == "MAT_ACT_WT"
				|| bcls_rec->Tables[1].Columns[i].get_ColumnName().Trim() == "MAT_THEORY_WT")
			{

			}
			else
			{
				Log::Trace("", __FUNCTION__, "bcls_rec->Tables[1].Rows[0][i].ToString()			= [{0}]", bcls_rec->Tables[1].Rows[0][i].ToString());

				sqlstr_temp += " AND " + bcls_rec->Tables[1].Columns[i].get_ColumnName() + " = @" + bcls_rec->Tables[1].Columns[i].get_ColumnName();
				cmd_inq.Parameters.Set(bcls_rec->Tables[1].Columns[i].get_ColumnName(), bcls_rec->Tables[1].Rows[0][i]);
			}
		}

		/* 查询材料信息 */
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			if (cs_archive_flag.Trim() == "T")
			{
				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TMMSM01 "
					"  WHERE MAT_LINE_TYPE = 'SM' ";
				sqlstr = " SELECT * "
					"   FROM TMMSM01 "
					"  WHERE MAT_LINE_TYPE = 'SM' ";
			}
			else 
			{
				sqlstr_count = " SELECT COUNT(1) "
					"   FROM HMMSM01 "
					"  WHERE MAT_LINE_TYPE = 'SM' ";
				sqlstr = " SELECT * "
					"   FROM HMMSM01 "
					"  WHERE MAT_LINE_TYPE = 'SM' ";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY MAT_NO ASC";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		//Log::Trace("", __FUNCTION__, "sqlstr_temp			= [{0}]", (const char*)sqlstr_temp);
		//Log::Trace("", __FUNCTION__, "sqlstr_count		= [{0}]", (const char*)sqlstr_count);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);

		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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
