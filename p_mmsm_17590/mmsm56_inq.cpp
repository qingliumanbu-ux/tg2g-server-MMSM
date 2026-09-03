/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 原辅料出库信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm56_inq)

int f_mmsm56_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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
	CString v_mat_kind = "";
	CString v_mat_type1 = "";
	CString v_mat_type2 = "";

	
	//系统的分页类信息。
	CPageInfo pageInfo; 
 
	CModel tmmsm56("TMMSM56");

	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch(CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize   = 1000;
		}


		//--------------------------------
		//获取传入参数
		tmmsm56.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND"))//材料类型 有可能是1，2拼在一块传到后台
			v_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();

		//Log::Info("", __FUNCTION__, "FACTORY_DIV  =[{0}]", tmmsm56["FACTORY_DIV"].ToString());
		//Log::Info("", __FUNCTION__, "HANDLE_DIV  =[{0}]", tmmsm56["HANDLE_DIV"].ToString());//出库类型
		//Log::Info("", __FUNCTION__, "MAT_TYPE  =[{0}]", tmmsm56["MAT_TYPE"].ToString());
		//Log::Info("", __FUNCTION__, "MAT_KIND  =[{0}]", v_mat_kind);
		//Log::Info("", __FUNCTION__, "AFFIRM_FLAG  =[{0}]", tmmsm56["AFFIRM_FLAG"].ToString());

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TMMSM56 "
					"  WHERE 1=1";
					;
				sqlstr	 = " SELECT * "
					"   FROM TMMSM56 "
					"  WHERE 1=1";
					;

				//厂别
				if (tmmsm56["FACTORY_DIV"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND FACTORY_DIV = @tmmsm56.FACTORY_DIV";
				}

				//材料类型
				if (tmmsm56["MAT_TYPE"].ToString().Trim() != "") //优先级最高,如果查询条件中的材料类型为空，取EPESPARA配置的参数
				{
					sqlstr_temp += " AND MAT_TYPE = @tmmsm56.MAT_TYPE";
				}
				else
				{
					if (v_mat_kind.Trim() != "")
					{
						sqlstr_temp += " AND MAT_TYPE in(" + v_mat_kind + ")";

					}
				}



				//出库类型
				if (tmmsm56["HANDLE_DIV"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND HANDLE_DIV	= @tmmsm56.HANDLE_DIV";
				}

				if(tmmsm56["MAT_CODE"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND MAT_CODE = @tmmsm56.MAT_CODE"; 
				}
				if(tmmsm56["MAT_NAME"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND MAT_NAME	like '%'|| @tmmsm56.MAT_NAME||'%'"; 
				}


				//确认标记
				if (tmmsm56["AFFIRM_FLAG"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND AFFIRM_FLAG = @tmmsm56.AFFIRM_FLAG";
				}
				
				//确认TAB页，才需要根据确认时间查询
				if (tmmsm56["AFFIRM_FLAG"].ToString().Trim() == "1")
				{
					if (tmmsm56["AFFIRM_TIME"].ToString().Trim() != "")
					{
						sqlstr_temp += " AND AFFIRM_TIME  LIKE @tmmsm56.AFFIRM_TIME||'%' ";

					}
				}

			
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY FACTORY_DIV,MAT_CODE DESC";
				sqlstr		 = sqlstr + sqlstr_temp;

				//Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

				break;
		}

		cmd_inq.Parameters.Set("tmmsm56.FACTORY_DIV", tmmsm56["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("tmmsm56.HANDLE_DIV", tmmsm56["HANDLE_DIV"].ToString());
		cmd_inq.Parameters.Set("tmmsm56.MAT_CODE",tmmsm56["MAT_CODE"].ToString()); 
		cmd_inq.Parameters.Set("tmmsm56.MAT_NAME",tmmsm56["MAT_NAME"].ToString()); 
		cmd_inq.Parameters.Set("tmmsm56.MAT_TYPE", tmmsm56["MAT_TYPE"].ToString());
		cmd_inq.Parameters.Set("tmmsm56.AFFIRM_FLAG", tmmsm56["AFFIRM_FLAG"].ToString());
		cmd_inq.Parameters.Set("tmmsm56.AFFIRM_TIME", tmmsm56["AFFIRM_TIME"].ToString());
	

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32(); 
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],pageInfo.RecordFrom,pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL,"TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;	

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
