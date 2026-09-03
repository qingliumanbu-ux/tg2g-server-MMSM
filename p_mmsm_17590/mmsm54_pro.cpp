/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2014-07-08
Description: 原辅料计划增删改
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件
   

//外部函数声明
int f_mmsm54_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn); 
int f_mmsm54a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(mmsm54_pro)

int f_mmsm54_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */

	/* 实体类定义 */
	
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString v_handle_div;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{	

		if (bcls_rec->Tables[0].Columns.Contains("HANDLE_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_handle_div = bcls_rec->Tables[0].Rows[0]["HANDLE_DIV"].ToString().TrimOrBlank().ToUpper();

		//Log::Trace("", __FUNCTION__, "v_handle_div=[{0}]", v_handle_div);

		if (v_handle_div=='A')//领用计划
		{
			doFlag = f_mmsm54a_proc(bcls_rec, bcls_ret, conn);
		}
		else
		{
			doFlag = f_mmsm54_proc(bcls_rec, bcls_ret, conn);
		}
		
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

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


