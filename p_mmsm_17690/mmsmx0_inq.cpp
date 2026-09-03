/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      wcm
Version:     1.0
Date:        2024-09-27
Description: 设备检修记录查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(mmsmx0_inq)


int f_mmsmx0_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString date_c = CDateTime::Now().ToString("yyyyMMdd");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sql = "";
	CString sqlwhere = "";
	CString s_userid("");
	CString get_columnname = " ";
	CString begin_time = "";
	
	//CModel ttmsm201("TTMSM201");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_con(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		begin_time = bcls_rec->Tables[0].Rows[0]["YEAR_MON"].ToString();
		
		//查询条件改为日期范围

		sqlstr = " select * from TMMSMX0 where 1=1";
		
		if (begin_time.Trim() != "")
		{
			sql += " and FINISH_TIME >=@begin_time";
		}
		
		sqlstr += sql;
		sqlstr = sqlstr + " order by FINISH_TIME desc";
		Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "Database processing error，sqlcode = [{0}]." /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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

	return doFlag;
}

