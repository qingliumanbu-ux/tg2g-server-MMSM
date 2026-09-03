/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2014-05-24
Description: 板坯切断新增
**************************************************/
//框架头文件
#include "stdafx.h" 

//业务头文件


//外部函数声明

int f_mmsm33_insert(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm009b_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);		//2022-10-26 新增给智慧质量发送电文


BM2F_ENTERACE(mmsm33f3_ins)

int f_mmsm33f3_ins(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */


	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		Log::Info("", __FUNCTION__, "进入mmsm33f3_ins程序");
		doFlag = f_mmsm33_insert(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//增加智慧质量切断实绩电文  王佳倩 2022-10-26
		Log::Trace("", "", "发送智慧质量板坯切断20QX05开始----");
		/*doFlag = f_mmsm009b_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		Log::Trace("", "", "发送智慧质量板坯切断20QX05结束----");
	}

	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


