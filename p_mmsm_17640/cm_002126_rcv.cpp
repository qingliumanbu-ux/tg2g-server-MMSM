/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-11-22 16:20:24
Description: PES侧接收炼钢板坯信息同步(MMS->炼钢PES)电文
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"



//外部函数声明
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//物料跟踪函数

BM2F_ENTERACE_TELE(cm_002126_rcv)

int f_cm_002126_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tpssm81("TPSSM81");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		tpssm81.MergeFrom(bcls_rec->Tables["tpssm21"].Rows[0]);
		tpssm81.Print();

		if (tpssm81.QueryCount("PLAN_NO,MAT_NO"))
		{
			tpssm81.Update("*", "PLAN_NO,MAT_NO");
		}
		else
		{
			tpssm81.Insert();
		}

		
		Log::Trace("", __FUNCTION__, "----------开始= [{0}]", "-----");
		Log::Trace("", __FUNCTION__, "----------tpssm21.plan_no= [{0}]", bcls_rec->Tables["tpssm21"].Rows[0]["plan_no"].ToString());
		//tpssm81.MergeFrom();
		//tpssm81.Print();
		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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


