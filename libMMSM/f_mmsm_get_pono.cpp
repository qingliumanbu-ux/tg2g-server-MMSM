/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2014-10-30
Version:1.0
Description: 根据熔炼号返回PONO号
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件
//#include "tpssm14.h"

BM2_FUNCTION_EXPORT
int f_mmsm_get_pono(const CString& heat_no, CString& pono, CString& factory_div, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int ret = 0;
	CString sqlstr="";
	

	try
	{
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

	
		Log::Info("", __FUNCTION__, "heat_no=[{0}]",heat_no);

		//获得输入参数
		switch (conn->DatabaseKind)
		{
						case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:	        // MS SQL Server数据库
						case DB_KIND_ORACLE:	    // Oracle 数据库
						default:
							sqlstr = "SELECT  PONO,FACTORY_DIV "
								" FROM    TPSSM11 "
								" WHERE   HEAT_NO = @heat_no";
						break;
		}
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("heat_no", heat_no);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			pono = cmd_sql.GetString(1);
			factory_div = cmd_sql.GetString(2);
		}
		cmd_sql.Close();

		Log::Info("", __FUNCTION__, "pono=[{0}]",pono);
		Log::Info("", __FUNCTION__, "factory_div=[{0}]", factory_div);
			
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

	return doFlag;

}
