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




BM2F_ENTERACE(mmsmdr_add)

int f_mmsmdr_add(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString table_name = " ";
	CString	datetime("");
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tmmsmdr31("TMMSMDR31");

	try
	{

		table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().TrimOrBlank().ToUpper();
		CModel mmsmsj(table_name);
		for (size_t i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			mmsmsj.MergeFrom(bcls_rec->Tables[1].Rows[i]);
			mmsmsj["REC_CREATOR"] = s.userid;
			mmsmsj["REC_CREATE_TIME"] = datetime;
			mmsmsj.Insert();
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


