/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:   1.0
Date:     2016-01-29 17:13:56  
Description: 原辅料计划管理
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsm54_inq)

int f_mmsm54_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr		= "";
	CString sqlstr_count = "";
	CString sqlstr_temp	 = "";
	CString v_mat_kind = "";
	//CString v_mat_type1 = "";
	//CString v_mat_type2 = "";
	int		TotalRecordCount = 0  ;

	

	//系统的分页类信息。
	CPageInfo pageInfo; 
 
	CModel tmmsm54("TMMSM54");

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
		tmmsm54.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND"))//材料类型 有可能是1，2拼在一块传到后台
			v_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		
		/* ***** 打印输入参数 ***** */
		/*Log::Info("", __FUNCTION__, "FACTORY_DIV  =[{0}]", tmmsm54["FACTORY_DIV"].ToString());*/
		//Log::Info("", __FUNCTION__, "HANDLE_DIV  =[{0}]", tmmsm54["HANDLE_DIV"].ToString());//计划类型
		//Log::Info("", __FUNCTION__, "MAT_TYPE  =[{0}]", tmmsm54["MAT_TYPE"].ToString());
		//Log::Info("", __FUNCTION__, "MAT_KIND  =[{0}]", v_mat_kind);
		//Log::Info("", __FUNCTION__, "PLAN_NO_Y  =[{0}]", tmmsm54["PLAN_NO_Y"].ToString());
		//Log::Info("", __FUNCTION__, "PLAN_STATUS  =[{0}]", tmmsm54["PLAN_STATUS"].ToString());
		//Log::Info("", __FUNCTION__, "PLAN_MAKE_TIME  =[{0}]", tmmsm54["PLAN_MAKE_TIME"].ToString());
		//Log::Info("", __FUNCTION__, "PLAN_RCV_TIME  =[{0}]", tmmsm54["PLAN_RCV_TIME"].ToString());


	
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_count = " SELECT COUNT(1) "
					"   FROM TMMSM54 "
					"  WHERE 1=1 "
					;
				sqlstr = " SELECT * "
					"   FROM TMMSM54 "
					"  WHERE 1=1 ";
					;

				//厂别
				if (tmmsm54["FACTORY_DIV"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND FACTORY_DIV = @tmmsm54.FACTORY_DIV";
				}

				//材料类型
				if (tmmsm54["MAT_TYPE"].ToString().Trim() != "") //优先级最高,如果查询条件中的材料类型为空，取EPESPARA配置的参数
				{
					sqlstr_temp += " AND MAT_TYPE = @tmmsm54.MAT_TYPE";
				}
				else
				{
					if (v_mat_kind.Trim() != "")
					{
						sqlstr_temp += " AND MAT_TYPE in(" + v_mat_kind + ")";

					}
				}



				//计划类型
				if (tmmsm54["HANDLE_DIV"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND HANDLE_DIV	= @tmmsm54.HANDLE_DIV";
				}

				if (tmmsm54["PLAN_NO_Y"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND PLAN_NO_Y			= @tmmsm54.PLAN_NO_Y"; 
				}
				
				if (tmmsm54["PLAN_STATUS"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND PLAN_STATUS			= @tmmsm54.PLAN_STATUS";
				}

				if (tmmsm54["PLAN_MAKE_TIME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND PLAN_MAKE_TIME	 LIKE @tmmsm54.PLAN_MAKE_TIME||'%' ";

				}

				if (tmmsm54["PLAN_RCV_TIME"].ToString().Trim() != "")
				{
					sqlstr_temp += " AND PLAN_RCV_TIME	LIKE  @tmmsm54.PLAN_RCV_TIME||'%' ";
				}
				
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY  PLAN_NO_Y DESC";
				sqlstr		 = sqlstr + sqlstr_temp;
				break;
		}
		cmd_inq.Parameters.Set("tmmsm54.FACTORY_DIV", tmmsm54["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("tmmsm54.HANDLE_DIV", tmmsm54["HANDLE_DIV"].ToString());
		cmd_inq.Parameters.Set("tmmsm54.PLAN_NO_Y", tmmsm54["PLAN_NO_Y"].ToString());
		cmd_inq.Parameters.Set("tmmsm54.PLAN_STATUS", tmmsm54["PLAN_STATUS"].ToString());
		cmd_inq.Parameters.Set("tmmsm54.PLAN_MAKE_TIME", tmmsm54["PLAN_MAKE_TIME"].ToString());
		cmd_inq.Parameters.Set("tmmsm54.PLAN_RCV_TIME", tmmsm54["PLAN_RCV_TIME"].ToString());
		cmd_inq.Parameters.Set("tmmsm54.MAT_TYPE", tmmsm54["MAT_TYPE"].ToString());
	
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

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
