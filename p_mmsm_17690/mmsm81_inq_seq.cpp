/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 获取计量单号
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm81_inq_seq)

int f_mmsm81_inq_seq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	int		TotalRecordCount = 0;
	CString datetime1("");
	CString datetime("");
	datetime1 = CDateTime::Today().ToString("yyyyMMdd");
	datetime1 = datetime1.Substring(2, 6);
	CString SeqNo1 = "";

	//系统的分页类信息。

	CDbCommand cmd_inq(conn);

	try
	{ 		

		bcls_ret->Tables[0].Columns.Add(DT_STRING, "WEIGH_NO");
		bcls_ret->Tables[0].Rows.Add();
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");	
		//CString  dh = "S" + datetime + EPGetNextSeq("SQ_JLYLID", conn);
		sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_ZXH.NEXTVAL),4,0) FROM DUAL ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			SeqNo1 = cmd_inq.GetString(1).Trim();

		}
		cmd_inq.Close();

		//CString dh = "S2S" + datetime.Substring(0, 8) + SeqNo1;
		CString dh = "S" + datetime + SeqNo1;
		bcls_ret->Tables[0].Rows[0]["WEIGH_NO"] = dh; 
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
