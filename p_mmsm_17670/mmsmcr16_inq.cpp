/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2024
Description: 铬镍收得率明细表
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmcr16_inq)

int f_mmsmcr16_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString sql_group = " ";
	CString end_time = " ";
	CString end_time_1 = " ";
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			end_time = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME_1"))
			end_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME_1"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM") && bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
				current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
			}
		}
		else {
			record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		}
	
			Log::Info("", __FUNCTION__, "end_time_1   =[{0}]", end_time_1);
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr_count = " SELECT COUNT(1) FROM "
					" (SELECT  ZXH.ST_NO_DESC FROM TMMSMZXHBB ZXH "
					" LEFT JOIN VMMSMCPCL_BB1 A ON A.HEAT_NO=ZXH.HEAT_NO "
					" where 1=1";


				sqlstr = " SELECT ZXH.ST_NO_DESC,MAX(ZXH.TIME_1) DATE_1,MAX(ZXH.USER_NAME) DAMIN,SUM(ZXH.PROD_OUT_WT) MAT_WT,SUM(A.MAT_ACT_WT) MAT_ACT_WT,SUM(DEVO_WT) DEVO_WT, "
					" ROUND(nvl(CASE WHEN SUM(DEVO_WT) = 0 OR SUM(ZXH.PROD_OUT_WT) = 0 THEN 0 ELSE SUM(DEVO_WT) / SUM(ZXH.PROD_OUT_WT) END, 0), 5) * 1000 XHL "
					" FROM TMMSMZXHBB ZXH LEFT JOIN VMMSMCPCL_BB1 A ON A.HEAT_NO=ZXH.HEAT_NO   "
					"  WHERE 1=1 AND SUBSTR(ZXH.ST_NO, 0, 1) IN ('1','4')  "     //钢种第一位1，4为不锈钢

					;
				if (end_time.Trim() != "")
				{
					sqlstr_temp += " AND ZXH.TAP_END_TIME >= @end_time";
				}
				if (end_time_1.Trim() != "")
				{
					sqlstr_temp += " AND ZXH.TAP_END_TIME <= @end_time_1";
				}

				sql_group = " GROUP BY ZXH.ST_NO_DESC  ";
				
				sqlstr_count = sqlstr_count + sqlstr_temp + sql_group + " )";
				Log::Info("", __FUNCTION__, "sqlstr_count   =[{0}]", sqlstr_count);
				sqlstr = sqlstr + sqlstr_temp + sql_group;
				cmd_inq.Parameters.Set("end_time", end_time);
				cmd_inq.Parameters.Set("end_time_1", end_time_1);
				cmd_inq.SetCommandText(sqlstr_count);
				cd_count = cmd_inq.ExecuteScalar();
				start_row = record_count_per_page * (current_page_no - 1);
				if (start_row > cd_count.ToDouble())
				{
					start_row = 0;
				}
				cmd_inq.Close();
				cmd_inq.SetCommandText(sqlstr);
				Log::Info("", __FUNCTION__, "sqlstr   =[{0}]", sqlstr);
				
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
				cmd_inq.Close();

				//返回分页总数量信息 
				bcls_ret->Tables.Add("PAGEINFO");	//增加块
				bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
				bcls_ret->Tables["PAGEINFO"].Rows.Add();
				bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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
