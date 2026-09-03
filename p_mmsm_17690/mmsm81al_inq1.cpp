/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm81al_inq1)

int f_mmsm81al_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;


	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm81al("TMMSM81AL");

	CDbCommand cmd_inq(conn);

	try
	{
		//获取传入参数
		tmmsm81al.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		//tmmsm81al.Print();
		if (tmmsm81al["QUALITY_BATCH_NO"].ToString().Trim() != "")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr =" SELECT  * FROM ( SELECT QUALITY_BATCH_NO ,ELM_VALUE,ELM_NAME FROM  TMMSM81AL WHERE QUALITY_BATCH_NO=@QUALITY_BATCH_NO )"
					" PIVOT ( SUM(ELM_VALUE) FOR ELM_NAME IN ( 'C' AS C ,'Si' AS SI, 'Mn' AS MN,'P' AS P, 'S' AS S, 'Cr' AS Cr, 'Ni' AS Ni, 'Mo' AS Mo)) ";
					;
				sqlstr = sqlstr + sqlstr_temp;
				break;
			}
			cmd_inq.Parameters.Set("QUALITY_BATCH_NO", tmmsm81al["QUALITY_BATCH_NO"].ToString());

			Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);

			//分页获取
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

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
