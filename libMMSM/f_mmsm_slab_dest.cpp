/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2014-10-30
Version:1.0
Description: 根据板坯去向返回下产线代码
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

BM2_FUNCTION_EXPORT

int f_mmsm_slab_dest(const CString& slab_plan_dest,  CString& slab_dest_code, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int ret = 0;
	CString sqlstr="";


	try
	{
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

	
		Log::Info("", __FUNCTION__, "slab_plan_dest=[{0}]",slab_plan_dest);

		sqlstr = "SELECT CODE_DESC_3_CONTENT "
								 "  FROM TEP0002 "
								 "  WHERE CODE_CLASS 	=  'PM16'"
								 "  AND CODE =  @slab_plan_dest ";
			
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("slab_plan_dest",slab_plan_dest); 
		cmd_sql.ExecuteReader();
		if(cmd_sql.Read())                                            
		{
			slab_dest_code = cmd_sql.GetString(1);
			
		}
		cmd_sql.Close();
	

		Log::Info("", __FUNCTION__, "slab_dest_code=[{0}]",slab_dest_code);

		
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
