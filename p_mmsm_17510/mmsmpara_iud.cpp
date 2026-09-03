/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2014-07-08
Description: TMMSMPARA参数设置增删改
**************************************************/
//框架头文件
#include "stdafx.h" 
 



//业务头文件
   

//外部函数声明

BM2F_ENTERACE(mmsmpara_iud)

int f_mmsmpara_iud(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	CString	datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsmpara("TMMSMPARA");
	
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{	
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		/* 维护事件表 */
		// 新增事件
		if (bcls_rec->Tables.IndexOf("MMSMPARA_PARA_INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSMPARA_PARA_INS"].Rows.get_Count(); i++)
			{
				tmmsmpara.Reset();
				tmmsmpara.MergeFrom(bcls_rec->Tables["MMSMPARA_PARA_INS"].Rows[i]);
				tmmsmpara.TrimOrBlank();

				//tmmsmpara.Print();
			
				
				/* 新增事件信息 */
				tmmsmpara["REC_CREATOR"] = s.userid;   //记录创建责任者
				tmmsmpara["REC_CREATE_TIME"] = datetime;   //记录创建时刻
				tmmsmpara.TrimOrBlank();
				tmmsmpara.Insert();

				
			}
		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MMSMPARA_PARA_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSMPARA_PARA_UPD"].Rows.get_Count(); i++)
			{
				tmmsmpara.Reset();
				
				tmmsmpara.MergeFrom(bcls_rec->Tables["MMSMPARA_PARA_UPD"].Rows[i]);
				tmmsmpara.TrimOrBlank();

				/* 修改事件信息 */
				tmmsmpara["REC_REVISOR"] = s.userid;
				tmmsmpara["REC_REVISE_TIME"] = datetime;
				tmmsmpara.TrimOrBlank();
				tmmsmpara.Update("PARA_NAME,"
							   "PARA,"
							   "PARA_DESC",
							   "PROGRAM_NAME,SEQ_NO");
			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSMPARA_PARA_DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSMPARA_PARA_DEL"].Rows.get_Count(); i++)
			{
				tmmsmpara.Reset();
				tmmsmpara.MergeFrom(bcls_rec->Tables["MMSMPARA_PARA_DEL"].Rows[i]);
				tmmsmpara.TrimOrBlank();

			
				/* 删除事件信息 */
				tmmsmpara.Delete("PROGRAM_NAME,SEQ_NO"); 
			}
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


