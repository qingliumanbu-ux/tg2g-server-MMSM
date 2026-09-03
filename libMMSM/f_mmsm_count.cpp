/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    3.0
Date:     2014-05-8
Description: 返回最大计数器
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 

//#include "tpssmd1.h"

//外部函数声明
 


BM2_FUNCTION_EXPORT

int f_mmsm_count(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量***** */
	int doFlag = 0;
	
	CString sqlstr = "";
	CString v_factory_div = "";
	CString v_table_type = "";
	CString v_heat_no = "";
	CString v_proc_no = "";
	CDecimal v_proc_count = 0;
	int blkNum = 0;
		

	CDbCommand cmd_sql(conn); //与DB 建立连接。
  
	
	//CTPSSMD1 tpssmd1(conn);

	try
	{
			/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("PROCCOUNT");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 PROCCOUNT 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	
		if (bcls_rec->Tables["PROCCOUNT"].Columns.Contains("TABLE_TYPE"))
			v_table_type = bcls_rec->Tables["PROCCOUNT"].Rows[0]["TABLE_TYPE"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables["PROCCOUNT"].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables["PROCCOUNT"].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables["PROCCOUNT"].Columns.Contains("PROC_NO"))
			v_proc_no = bcls_rec->Tables["PROCCOUNT"].Rows[0]["PROC_NO"].ToString().TrimOrBlank().ToUpper();


	
		
		//Log::Info("", __FUNCTION__, " v_table_type =[{0}]",  v_table_type);
		//Log::Info("", __FUNCTION__, " v_heat_no =[{0}]",  v_heat_no);
		//Log::Info("", __FUNCTION__, " v_proc_no =[{0}]",  v_proc_no);

		sqlstr = " SELECT nvl(MAX(PROC_COUNT),0) FROM " + v_table_type + " WHERE 1=1 ";

			
		if(v_heat_no.Trim() != "")
		{
			sqlstr	+= " AND HEAT_NO = @heat_no"; 
		}
		if(v_proc_no.Trim() != "")
		{
			sqlstr	+= " AND PROC_NO = @proc_no"; 
		}

		//Log::Info("", __FUNCTION__, " sqlstr =[{0}]",  sqlstr);

		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("heat_no", v_heat_no);
		cmd_sql.Parameters.Set("proc_no", v_proc_no);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			v_proc_count = cmd_sql.GetDecimal(1);
		}
		cmd_sql.Close();

	
		Log::Info("", __FUNCTION__, "proc_count=[{0}]",v_proc_count);

		
		bcls_ret->Tables[0].Rows.Add();

		if(!bcls_ret->Tables[0].Columns.Contains("PROC_COUNT"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"PROC_COUNT"); 
		}

		bcls_ret->Tables[0].Rows[0]["PROC_COUNT"] = v_proc_count + 1 ;


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

