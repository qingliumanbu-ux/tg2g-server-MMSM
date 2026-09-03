/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2015-02-01
Description: 炼钢缺陷信息管理_缺陷信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢缺陷信息管理_缺陷信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件  
  
  

//外部函数声明

BM2F_ENTERACE(mmsm06a1a1_inq)

int f_mmsm06a1a1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	

	/* 实体类定义 */ 
	CModel tmmsm06("TMMSM06");
	CModel tmmsm061("TMMSM061");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmmsm06["MAT_NO"]			= bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
	
		//Log::Trace("",__FUNCTION__,"传入参数tmmsm06.MAT_NO		= [{0}]",tmmsm06["MAT_NO"].ToString());
		
		/* 检查输入参数合法性 */
		if(tmmsm06["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"材料号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}	  

		/* 查询厚板缺陷信息档 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
						 "  FROM TMMSM06 "
						 " WHERE MAT_NO = @tmmsm06.MAT_NO "
						 " ORDER BY DEFECT_NO ";
				break;
		}     
		//Log::Trace("",__FUNCTION__,"sqlstr = [{0}]",sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmmsm06.MAT_NO",tmmsm06["MAT_NO"].ToString()); 
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],0,100);
		cmd_inq.Close();

		/* 查询厚板缺陷履历信息档 */
		bcls_ret->Tables.Add("MMSM061");
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
						 "  FROM TMMSM061 "
						 " WHERE MAT_NO = @tmmsm06.MAT_NO "
						 " ORDER BY REC_CREATE_TIME DESC,DEFECT_NO ASC ";
				break;
		}     
		//Log::Trace("",__FUNCTION__,"sqlstr = [{0}]",sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmmsm06.MAT_NO",tmmsm06["MAT_NO"].ToString()); 
		cmd_inq.ExecuteQuery(bcls_ret->Tables["MMSM061"],0,500);
		cmd_inq.Close();
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



