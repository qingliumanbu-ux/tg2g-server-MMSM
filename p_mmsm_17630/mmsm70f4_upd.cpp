/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2015-04-14
Description: 电渣锭实绩修正
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 电渣锭实绩修正
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


//外部函数声明
int f_mmsm7003_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm70f4_upd)

// service对应主体函数体
int f_mmsm70f4_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */
	int doFlag = 0; 
	int blkNum = 0;
	int i = 0;
    int	rows = 0;
	//EIClass  bcls_tmp;
    /* 业务变量 */
	CString  datetime("");

	/* 实体类定义 */
	CModel tmmsm70("TMMSM70");

	CString  sqlstr("");              // 数据库SQL操作字符串
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

    //调用子函数 根据轧制实绩创建主档表	
   

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		
		
		//Log::Trace("", __FUNCTION__, "块MMSM70 = [{0}]", bcls_rec->Tables["MMSM70"].Rows.get_Count());
		
		doFlag = f_mmsm7003_proc(bcls_rec, bcls_ret, conn);
		if(doFlag < 0)
		{
		   throw CApplicationException(doFlag, s.msg, log.Location);
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

	return doFlag;
}
