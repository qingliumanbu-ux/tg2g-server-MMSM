/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    1.0
Date:
Description: 导入报表
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件




BM2F_ENTERACE(mmsmjccjl_add2)

int f_mmsmjccjl_add2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	//CString table_name = " ";
	//CString	datetime("");
	//datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{

		//table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().TrimOrBlank().ToUpper();
		CModel tmmsmjccjladd2("TMMSMJCCJLADD2");
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmjccjladd2.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			//tmmsmjccjladd1["REC_CREATOR"] = s.userid;
			//tmmsmjccjladd1["REC_CREATE_TIME"] = datetime;
			tmmsmjccjladd2.Insert();
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		Log::Trace("", __FUNCTION__, "ex.GetCode() =[{0}]", ex.GetCode());
		if (ex.GetCode() == 1)
		{
			CMessageFormat::Format(s.msg, "导入重复，请重新导入！sqlcode=[{0}]", arguments, 1);
		}
		else
		{
			CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		}
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


