/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wcm
Version:    1.0
Date:       2023-11-15
Description: 工序投料生产实绩保存
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件


//外部函数声明
int f_mmsm2a_proc1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsm2a_pro1)

int f_mmsm2a_pro1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CModel tdemo01 = CModel("TMMSM2A");//以表名为参数新建一个CModel对象以对该表进行操作

	/* 实体类定义 */
	
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->AtBlkName("TMMS_2A_INS") > 0)
		{
			for (int i = 0; i < bcls_rec->Tables["TMMS_2A_INS"].Rows.get_Count(); i++)
			{
				tdemo01.Reset();
				tdemo01.MergeFrom(bcls_rec->Tables["TMMS_2A_INS"].Rows[i]);
				//tdemo01["EMPNO"] = 112030+1;
				tdemo01.Insert();
			}
			Log::Trace("", "", "新增了{0}条记录", bcls_rec->Tables["TMMS_2A_INS"].Rows.get_Count());
		}
		if (bcls_rec->AtBlkName("TMMS_2A_UPD") > 0)
		{
			for (int i = 0; i < bcls_rec->Tables["TMMS_2A_UPD"].Rows.get_Count(); i++)
			{
				tdemo01.Reset();
				tdemo01.MergeFrom(bcls_rec->Tables["TMMS_2A_UPD"].Rows[i]);
				tdemo01.Update("*", "PROC_NO,PROC_COUNT");//根据主键修改
			}
			Log::Trace("", "", "修改了{0}条记录", bcls_rec->Tables["UPD"].Rows.get_Count());
		}
		
		if (bcls_rec->AtBlkName("TMMS_2A_DEL") > 0)
		{
			for (int i = 0; i < bcls_rec->Tables["TMMS_2A_DEL"].Rows.get_Count(); i++)
			{
				tdemo01.Reset();
				tdemo01.MergeFrom(bcls_rec->Tables["TMMS_2A_DEL"].Rows[i]);
				tdemo01.Delete();//根据主键删除
			}
			Log::Trace("", "", "删除了{0}条记录", bcls_rec->Tables["TMMS_2A_DEL"].Rows.get_Count());
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


