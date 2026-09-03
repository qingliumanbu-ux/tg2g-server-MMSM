/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 倒罐实绩查询
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 



// service入口
BM2F_ENTERACE(mmsm11_tpd_wt_inq)

int f_mmsm11_tpd_wt_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
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
	
	CString flag = "";
	//系统的分页类信息。
	CPageInfo pageInfo; 
 
	CModel tmmsm11("TMMSM11");

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
		tmmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		
		
		/* ***** 打印输入参数 ***** */
		
		Log::Info("", __FUNCTION__, "IRON_NO  =[{0}]", tmmsm11["IRON_NO"].ToString());
		Log::Info("", __FUNCTION__, "TPC_YL_NO  =[{0}]", tmmsm11["TPC_YL_NO"].ToString());


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			
			sqlstr = " SELECT TPD_NO, IRON_LADLE_NO, MOLTIRON_WT FROM ("
				" SELECT  TPD_NO, IRON_LADLE_NO, MOLTIRON_WT1 MOLTIRON_WT,END_TIME"
				" FROM  TMMSM12"
				" WHERE  IRON_NO1 =@tmmsm11.IRON_NO"
				" AND  TPC_YL_NO1 =@tmmsm11.TPC_YL_NO"
				" UNION"
				" SELECT  TPD_NO, IRON_LADLE_NO, MOLTIRON_WT2 MOLTIRON_WT,END_TIME"
				" FROM  TMMSM12"
				" WHERE  IRON_NO2 =@tmmsm11.IRON_NO"
				" AND  TPC_YL_NO2 =@tmmsm11.TPC_YL_NO"
				" UNION"
				" SELECT  TPD_NO, IRON_LADLE_NO, MOLTIRON_WT3 MOLTIRON_WT,END_TIME"
				" FROM  TMMSM12"
				" WHERE  IRON_NO3 =@tmmsm11.IRON_NO"
				" AND  TPC_YL_NO3 =@tmmsm11.TPC_YL_NO"
				" UNION"
				" SELECT  TPD_NO, IRON_LADLE_NO, MOLTIRON_WT4 MOLTIRON_WT,END_TIME"
				" FROM  TMMSM12"
				" WHERE  IRON_NO4 =@tmmsm11.IRON_NO"
				" AND  TPC_YL_NO4 =@tmmsm11.TPC_YL_NO"
				" ORDER  BY END_TIME DESC )"
				;

			

			break;
		}
		
		cmd_inq.Parameters.Set("tmmsm11.IRON_NO", tmmsm11["IRON_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm11.TPC_YL_NO", tmmsm11["TPC_YL_NO"].ToString());

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
