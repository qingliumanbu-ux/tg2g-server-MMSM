/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2016-01-22
Description: 生产总实绩查询
**************************************************/
//框架头文件
#include "stdafx.h"




//业务头文件
   

//外部函数声明

BM2F_ENTERACE(mmsm10_inq)

int f_mmsm10_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	int	TotalRecordCount = 0;
	int i;
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString ch_start_time_f = "";
	CString ch_start_time_t = "";
	
	
	CPageInfo pageInfo;

	/* 业务变量 */
	
	CModel tmmsm10("TMMSM10");
	/* 实体类定义 */
	
	/* 数据库SQL操作字符串 */
	


	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{	
		
		tmmsm10.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString();

	
		////Log::Info("", __FUNCTION__, "PONO      =[{0}]", tmmsm10["PONO"].ToString());
		////Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", tmmsm10["HEAT_NO"].ToString());
		////Log::Info("", __FUNCTION__, "PROD_SHIFT_NO =[{0}]", tmmsm10["PROD_SHIFT_NO"].ToString());
		////Log::Info("", __FUNCTION__, "PROD_SHIFT_GROUP =[{0}]", tmmsm10["PROD_SHIFT_GROUP"].ToString());
		////Log::Info("", __FUNCTION__, "start_time_f  =[{0}]", ch_start_time_f);
		////Log::Info("", __FUNCTION__, "start_time_t  =[{0}]", ch_start_time_t);


		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1)  FROM  TMMSM10 WHERE 1=1 ";

			sqlstr = " SELECT *  FROM TMMSM10 WHERE 1=1 ";

			if (tmmsm10["HEAT_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND HEAT_NO = @tmmsm10.HEAT_NO";
			}
			if (tmmsm10["PONO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND PONO	= @tmmsm10.PONO";
			}
			if (tmmsm10["PROD_SHIFT_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_NO	= @tmmsm10.PROD_SHIFT_NO";
			}
			if (tmmsm10["PROD_SHIFT_GROUP"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_GROUP = @tmmsm10.PROD_SHIFT_GROUP";
			}
			if (ch_start_time_f.Trim() != "")
			{
				sqlstr_temp += " AND PROD_TIME	>= @ch_start_time_f";
			}
			if (ch_start_time_t.Trim() != "")
			{
				sqlstr_temp += " AND PROD_TIME	<= @ch_start_time_t";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;

			sqlstr_temp += " ORDER BY HEAT_NO DESC";
			
			sqlstr = sqlstr + sqlstr_temp;

			//Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			break;
		}

	

		cmd_inq.Parameters.Set("tmmsm10.HEAT_NO", tmmsm10["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm10.PONO", tmmsm10["PONO"].ToString());
		cmd_inq.Parameters.Set("tmmsm10.PROD_SHIFT_NO", tmmsm10["PROD_SHIFT_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm10.PROD_SHIFT_GROUP", tmmsm10["PROD_SHIFT_GROUP"].ToString());
		cmd_inq.Parameters.Set("ch_start_time_f", ch_start_time_f);
		cmd_inq.Parameters.Set("ch_start_time_t", ch_start_time_t);
		
		

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//增加非表中字段
		if (!bcls_ret->Tables[0].Columns.Contains("CAST_SHOW"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "CAST_SHOW");
		}


		for (i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm10["CAST_NO"] = bcls_ret->Tables[0].Rows[i]["CAST_NO"].ToString();
			tmmsm10["CAST_DIV_NO"] = bcls_ret->Tables[0].Rows[i]["CAST_DIV_NO"].ToDecimal();

			if (tmmsm10["CAST_NO"].ToString().Trim() != "")
			{
				tmmsm10["CAST_DIV_NO"] = tmmsm10["CAST_DIV_NO"].ToDecimal().ToInt32();
				bcls_ret->Tables[0].Rows[i]["CAST_SHOW"] = tmmsm10["CAST_NO"].ToString().Trim() + "-" + tmmsm10["CAST_DIV_NO"].ToDecimal().ToString();
			}

		}

	

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;


	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{

		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 


