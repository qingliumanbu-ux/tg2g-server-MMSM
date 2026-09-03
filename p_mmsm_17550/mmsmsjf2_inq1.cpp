/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-11-25
Description: 工序实绩明细查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

/* ***** 静态函数申明 ***** */


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// CC工序实绩查询
/// <para>
/// 1.根据熔炼号、处理号等条件进行CC实绩查询。
/// 
/// </para>
/// <para>数据库表：          </para>
/// <para>主调用函数：        </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns> CC实绩 </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsmsjf2_inq1)

int f_mmsmsjf2_inq1(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr					= "";
	CString sqlstr_count			= "";
	CString sqlstr_temp				= "";
	
	CString ch_start_time_f			= "";
	CString ch_start_time_t			= "";
	CString v_table_type = "";//表名称
	CString v_proc_no = "";
	CString v_heat_no = "";
	CString dev_code = "";
	

	//系统的分页类信息。
	CPageInfo pageInfo; 
 
	
	CDbCommand cmd_inq(conn);

	try
	{
		
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("L2_PROC_NO"))
			v_proc_no= bcls_rec->Tables[0].Rows[0]["L2_PROC_NO"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("DEV_CODE"))
			dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().TrimOrBlank().ToUpper();
		Log::Info("", __FUNCTION__, "dev_code   =[{0}]", dev_code);
		if (v_table_type.Trim() == "")
		{
			sprintf(s.msg, "【表名称】不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		} 
		
			
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "v_table_type =[{0}]", v_table_type);
		Log::Info("", __FUNCTION__, "v_proc_no   =[{0}]", v_proc_no);
		Log::Info("", __FUNCTION__, "v_heat_no   =[{0}]", v_heat_no);
		
	
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

			
				sqlstr = " SELECT *  FROM " + v_table_type + " WHERE 1=1 ";

			
				/*if(v_heat_no.Trim() != "")
				{
					sqlstr_temp	+= " AND HEAT_NO = @v_heat_no"; 
				}*/
				if(v_proc_no.Trim() != "")
				{
					sqlstr_temp	+= " AND L2_PROC_NO = @v_proc_no"; 
				}
				if (dev_code.Trim() != "")
				{
					sqlstr_temp += " AND DEV_CODE like '%" + dev_code.SubstringNE(0, 1) + "%' ";
				}
		
				sqlstr_temp += " ORDER BY PROC_COUNT ASC";
		
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr		 = sqlstr + sqlstr_temp;

				Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
				break;
		}
		   
		
		cmd_inq.Parameters.Set("v_heat_no" , v_heat_no);
		cmd_inq.Parameters.Set("v_proc_no" , v_proc_no);
		cmd_inq.Parameters.Set("dev_code", dev_code.SubstringNE(0,1));
		
		cmd_inq.SetCommandText(sqlstr);
		bcls_ret->Tables[0].set_TableName(v_table_type);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

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
