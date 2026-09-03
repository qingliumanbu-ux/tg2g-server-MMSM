/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2016-01-22
Description: 炼钢班报保存
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明
int f_mmsm62_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsm62_save)

int f_mmsm62_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString procDiv = "";


	/* 业务变量 */
	CModel tmmsm62("TMMSM62");

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */

	try
	{
		tmmsm62.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (tmmsm62.QueryCount("FACTORY_DIV,PROD_DATE,PROD_SHIFT_GROUP") > 0)
		{
			tmmsm62["REC_REVISE_TIME"] = s.datetime;
			tmmsm62["REC_REVISOR"] = s.userid;
			tmmsm62.Update("REC_REVISE_TIME,REC_REVISOR,PROD_OVERVIEW");
		}
		else
		{
			tmmsm62["REC_CREATE_TIME"] = s.datetime;
			tmmsm62["REC_CREATOR"] = s.userid;
			tmmsm62.Insert();
		}
		


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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


