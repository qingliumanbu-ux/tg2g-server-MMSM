/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2016-01-22
Description: 缓冷退火实绩画面的材料信息查询
**************************************************/
//框架头文件
#include "stdafx.h"


//业务头文件


//外部函数声明
//int f_mmsm2e_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsmmzsjf2_inq)

int f_mmsmmzsjf2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* ***** 自定义变量 ***** */
	int		TotalRecordCount = 0;

	//系统的分页类信息。
	CPageInfo pageInfo;

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString tableName = "";
	CString factory_div = "";
	CString station_id = "";
	CString proc_flag = "";
	CString heat_no = "";
	CString pono = "";
	CString mat_no = "";
	CString in_mat_no = "";
	CString prod_shift_no = "";
	CString prod_shift_group = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}


		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_GROUP"))
			prod_shift_group = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_GROUP"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_NO"))
			prod_shift_no = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_NO"].ToString();


		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			tableName = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString();

		if (bcls_rec->Tables[0].Columns.Contains("PROC_FLAG"))
			proc_flag = bcls_rec->Tables[0].Rows[0]["PROC_FLAG"].ToString();

	
		Log::Trace("", __FUNCTION__, "tableName[{0}]  ", tableName);
		Log::Trace("", __FUNCTION__, "proc_flag[{0}]  ", proc_flag);
		Log::Trace("", __FUNCTION__, "station_id[{0}]  ", station_id);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			if (tableName == "TMMSM01")
			{
				sqlstr = "SELECT * FROM TMMSM01 WHERE 1=1 AND " + proc_flag + " = '1'";

				if (station_id.Trim() != "")
				{
					sqlstr_temp += " AND IC_CC_FLAG = @station_id";
				}

				//if ((proc_flag.Trim() != "COOL_FLAG") && (proc_flag.Trim() != "ANNEAL_FLAG"))
				//{
				//	sqlstr = "SELECT * FROM TMMSM01 WHERE 1=1 AND " + proc_flag + " = '1'";
				//}
				//else
				//{
				//	/*sqlstr = "SELECT HEAT_NO,PONO,ST_NO,COUNT(MAT_NO) AS ";
				//	sqlstr = sqlstr + '"';
				//	sqlstr = sqlstr + "MAT_NO"; 
				//	sqlstr = sqlstr + '"';
				//	sqlstr = sqlstr + " FROM TMMSM01 WHERE " + proc_flag + " = '1'";*/

				//	sqlstr = "SELECT HEAT_NO,PONO,ST_NO,MAT_NO ";
				//	/*sqlstr = sqlstr + '"';
				//	sqlstr = sqlstr + "MAT_NO";
				//	sqlstr = sqlstr + '"';*/
				//	sqlstr = sqlstr + " FROM TMMSM01 WHERE " + proc_flag + " = '1'";

				//}
			}
			else
			{
				sqlstr = " SELECT * FROM " + tableName + " WHERE 1=1 ";
			}

			if (factory_div.Trim() != "")
			{
				sqlstr_temp += " AND factory_div = @factoryDiv";
			}

			if (heat_no.Trim() != "")
			{
				sqlstr_temp += " AND HEAT_NO = @heat_no";
			}
			if (prod_shift_no.Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_NO = @prod_shift_no";
			}
			if (prod_shift_group.Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_GROUP = @prod_shift_group";
			}

			if (pono.Trim() != "")
			{
				sqlstr_temp += " AND PONO = @pono";
			}

			if (tableName == "TMMSM36")
			{
				if (station_id.Trim() != "")
				{
					sqlstr_temp += "  AND STATION_ID = @station_id ";
				}

				if (proc_flag.Trim() == "COOL_FLAG")
				{
					sqlstr_temp += "  AND HOT_FLAG = '1' ";
				}
				else if (proc_flag.Trim() == "ANNEAL_FLAG")
				{
					sqlstr_temp += "  AND HOT_FLAG = '2' ";
				}
			}

			if (tableName == "TPSSM81")
			{
				if (proc_flag.Trim() == "COOL_FLAG")
				{
					sqlstr_temp += "  AND  PLAN_TYPE = 'SMH' ";
				}

				else if (proc_flag.Trim() == "FINISH_FLAG"){
					sqlstr_temp += "  AND  PLAN_TYPE = 'SMJ' ";
				}
			}


		
			if (tableName == "TMMSM01")
			{
				if ((proc_flag.Trim() != "COOL_FLAG") && (proc_flag.Trim() != "ANNEAL_FLAG")){
					sqlstr_temp += " ORDER BY MAT_NO ";
				}
				/*else{
					sqlstr_temp += " GROUP BY HEAT_NO,PONO,ST_NO ORDER BY HEAT_NO ";
				}*/
			}
			else
			{
				sqlstr_temp += " ORDER BY HEAT_NO ";
			}
	
			sqlstr = sqlstr + sqlstr_temp;
			sqlstr_count = " SELECT COUNT(1) FROM (" + sqlstr + ") WHERE 1=1 ";
			break;
		}

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
		Log::Trace("", __FUNCTION__, "sqlstr_count[{0}]  ", sqlstr_count);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("factory_div", factory_div);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.Parameters.Set("prod_shift_no", prod_shift_no);
		cmd_inq.Parameters.Set("prod_shift_group", prod_shift_group);
		cmd_inq.Parameters.Set("pono", pono);
		cmd_inq.Parameters.Set("station_id", station_id);

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


