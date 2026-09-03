/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2015-04-14
Description:化渣炉实绩新增
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 化渣炉实绩新增
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


//外部函数声明
int f_mmsm7101_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm71f3_ins)

// service对应主体函数体
int f_mmsm71f3_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CModel tmmsm71("TMMSM71");

	CString  sqlstr("");              // 数据库SQL操作字符串
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

    

	try
	{

		//实绩收集标记  0-后备   1-电文
		if (!bcls_rec->Tables[0].Columns.Contains("PRACT_COLL_MODE"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "PRACT_COLL_MODE");
		}

		if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		}

		bcls_rec->Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";
		bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "I";

		doFlag = f_mmsm7101_proc(bcls_rec, bcls_ret, conn);

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

	return doFlag;
}
