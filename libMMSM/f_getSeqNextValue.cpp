/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   李晓明
Version:    1.0
Date:     2023-11-15
Description: 获取序列号字符串
***********************************************************************/


/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

int f_getSeqNextValue(CString SEQ_NAME, CString &SEQ_VALUE, CDbConnection * conn)
{
	int doFlag = 0;			//返回值

	CDbCommand cmd(conn);	//sql执行对象

	CString sqlstr = "";	//sql语句

	CString datetime = CDateTime::Now().ToString("yyyyMMdd");

	try{
		sqlstr = "SELECT TO_CHAR(T.SEQ_NOW),LENGTH(T.SEQ_NOW),T.SEQ_LEN,T.SEQ_PRE FROM TED21 T WHERE T.SEQ_NAME = @SEQ_NAME";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("SEQ_NAME", SEQ_NAME);

		Log::Trace("", __FUNCTION__, "序列ID：{0}", SEQ_NAME);
		Log::Trace("", __FUNCTION__, "序列号：{0}", sqlstr);

		cmd.ExecuteReader();
		if (cmd.Read()){
			CString seq = cmd.GetString(1);
			int seqLen = cmd.GetInt32(2);
			int countLen = cmd.GetInt32(3);
			CString seqPre = cmd.GetString(4);

			for (int i = 0; i < (countLen - seqLen); i++){
				SEQ_VALUE += "0";
			}

			SEQ_VALUE =  seqPre + datetime + SEQ_VALUE + seq;
		}

		Log::Trace("", __FUNCTION__, "序列号：", SEQ_VALUE);
		
		//当前序列号自增长
		cmd.Close();
		sqlstr = "UPDATE TED21 T SET T.SEQ_NOW = T.SEQ_NOW + 1 WHERE T.SEQ_NAME = @SEQ_NAME";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("SEQ_NAME", SEQ_NAME);
		cmd.ExecuteNonQuery();
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
	}
	cmd.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}