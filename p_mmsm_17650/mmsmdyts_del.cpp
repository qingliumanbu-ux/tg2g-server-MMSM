/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2011-12-13
Version:1.0
Description: 炼钢板坯组批管理
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/




/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmdyts_del)

int f_mmsmdyts_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_st_no = "";//出钢记号
	CString v_c_div = "";//碳锈区分  1  不锈钢  2碳钢
	CString v_operate = "";//操作区分	 I 新增    U 修改
	CString v_resume_seq_no = "";//序号

	CModel tqmtsbwdy("TQMTSBWDY");

	CDbCommand cmd_inq(conn);


	try
	{
		v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tqmtsbwdy.Reset();
			tqmtsbwdy.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (v_operate == "D")
			{
				tqmtsbwdy.Delete("DATE_CODE");
			}
		}

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
