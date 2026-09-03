/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-13 17:13:56  
Description: 铁水包受铁实绩查询
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

// service入口
BM2F_ENTERACE(mmsm12_inq)

int f_mmsm12_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr					= "";
	CString sqlstr_count			= "";
	CString sqlstr_temp				= "";
	int		TotalRecordCount		= 0  ;

	CString ch_start_time_f			= "";
	CString ch_start_time_t			= "";
	

	//系统的分页类信息。
	
 
	CModel tmmsm12("TMMSM12");

	CDbCommand cmd_inq(conn);

	try
	{
		


		//--------------------------------
		//获取传入参数
		tmmsm12.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//开始时刻
		if(bcls_rec->Tables[0].Columns.Contains("START_TIME_F"))
			ch_start_time_f = bcls_rec->Tables[0].Rows[0]["START_TIME_F"].ToString();
		//结束时刻
		if(bcls_rec->Tables[0].Columns.Contains("START_TIME_T"))
			ch_start_time_t = bcls_rec->Tables[0].Rows[0]["START_TIME_T"].ToString();

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = "SELECT COUNT(1) FROM TMMSM12 WHERE 1=1 ";
				sqlstr	 = "SELECT * FROM TMMSM12 WHERE 1=1 ";

				//倒完确认标志
				if (tmmsm12["POUR_FLAG"].ToString().Trim() != "")
				{
					sqlstr_temp += "AND POUR_FLAG = @tmmsm12.POUR_FLAG  ";
				}

				//处理号
				if(tmmsm12["TPD_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= "AND TPD_NO LIKE '%' || @tmmsm12.TPD_NO || '%' "; 
				}

				//作业班次
				if (tmmsm12["PROD_SHIFT_NO"].ToString().Trim() != "")
				{
					sqlstr_temp += "AND PROD_SHIFT_NO = @tmmsm12.PROD_SHIFT_NO ";
				}

				//作业班组
				if (tmmsm12["PROD_SHIFT_GROUP"].ToString().Trim() != "")
				{
					sqlstr_temp += "AND PROD_SHIFT_GROUP = @tmmsm12.PROD_SHIFT_GROUP ";
				}
				
				//开始时刻
				if(ch_start_time_f.Trim() != "")
				{
					sqlstr_temp	+= "AND START_TIME >= @ch_start_time_f "; 
				}
				//结束时刻
				if(ch_start_time_t.Trim() != "")
				{
					sqlstr_temp	+= "AND START_TIME <= @ch_start_time_t "; 
				}

				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += "ORDER BY  TPD_NO";
				sqlstr		 = sqlstr + sqlstr_temp;
				break;
		}

		cmd_inq.Parameters.Set("tmmsm12.POUR_FLAG", tmmsm12["POUR_FLAG"].ToString());
		cmd_inq.Parameters.Set("tmmsm12.PROD_SHIFT_NO", tmmsm12["PROD_SHIFT_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm12.PROD_SHIFT_GROUP", tmmsm12["PROD_SHIFT_GROUP"].ToString());
		cmd_inq.Parameters.Set("tmmsm12.TPD_NO"	,tmmsm12["TPD_NO"].ToString()); 
		cmd_inq.Parameters.Set("ch_start_time_f" ,ch_start_time_f); 
		cmd_inq.Parameters.Set("ch_start_time_t" ,ch_start_time_t); 


		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32(); 

		Log::Trace("", __FUNCTION__, "sqlstr = {0}", sqlstr);
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		//返回分页总数量信息 
	

	 }
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
