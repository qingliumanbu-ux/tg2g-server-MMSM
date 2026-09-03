/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     Simon Li
Version:    1.0
Date:       2024-03-08
Description: 倒罐站铁水操作信息查询
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(mmsm11c_inq)

int f_mmsm11c_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	int doFlag = 0;				//程序返回值

	/* 分页信息 */
	CPageInfo pageInfo;
	int	TotalRecordCount = 0;

	/* 数据库表实体类定义 */
	CModel tmmsm11c("TMMSM11C");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	/* sql语句变量 */
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";

	CString updateTimeStart = "";
	CString updateTimeEnd = "";

	try
	{
		tmmsm11c.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		updateTimeStart = bcls_rec->Tables[0].Rows[0]["UPDATE_TIME_START"].ToString();
		updateTimeEnd = bcls_rec->Tables[0].Rows[0]["UPDATE_TIME_END"].ToString();

		sqlstr_count = " SELECT COUNT(1)  FROM  TMMSM11C T1, TMMSM11G T2 WHERE T1.ITEM_NAME = T2.ITEM_NAME ";
		sqlstr = " SELECT T1.*,T2.ITEM_DES FROM TMMSM11C T1, TMMSM11G T2 WHERE T1.ITEM_NAME = T2.ITEM_NAME ";

		if (tmmsm11c["ITEM_NAME"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND T1.ITEM_NAME = @tmmsm11c.ITEM_NAME";
		}

		if (updateTimeStart.Trim() != "")
		{
			sqlstr_temp += " AND T1.UPDATE_TIME >= @UPDATE_TIME_START";
		}

		if (updateTimeEnd.Trim() != "")
		{
			sqlstr_temp += " AND T1.UPDATE_TIME <= @UPDATE_TIME_END";
		}

		sqlstr_count = sqlstr_count + sqlstr_temp;

		sqlstr_temp += " ORDER BY T1.UPDATE_TIME DESC";

		sqlstr = sqlstr + sqlstr_temp;
		Log::Trace("", __FUNCTION__, "SQL = {0}", sqlstr);

		cmd_inq.Parameters.Set("tmmsm11c.ITEM_NAME", tmmsm11c["ITEM_NAME"].ToString());
		cmd_inq.Parameters.Set("UPDATE_TIME_START", updateTimeStart);
		cmd_inq.Parameters.Set("UPDATE_TIME_END", updateTimeEnd);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();

		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;
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