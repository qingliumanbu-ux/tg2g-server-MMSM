/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     Simon Li
Version:    1.0
Date:       2024-03-18
Description: 过程跟踪条目查询
**************************************************/

//框架头文件
#include "stdafx.h"

//业务头文件


BM2F_ENTERACE(mmsm11g_inq)

int f_mmsm11g_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 		//服务调用日志输出

	int doFlag = 0;					//服务调用返回值

	/* 分页信息定义 */
	CPageInfo pageInfo;
	int	TotalRecordCount = 0;

	CModel tmmsm11g("TMMSM11G");	//过程跟踪条目定义表实体类

	CDbCommand cmd(conn);			//数据库操作对象定义

	/* sql语句变量定义 */
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";

	try
	{
		//tmmsm11g接收传入参数信息
		tmmsm11g.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//sql语句赋初值
		sqlstr_count = " SELECT COUNT(1)  FROM  TMMSM11G WHERE 1=1 ";
		sqlstr = " SELECT *  FROM TMMSM11G WHERE 1=1 ";

		//------------------ sql拼接 ---------------------------------

		if (tmmsm11g["ITEM_NAME"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND ITEM_NAME = @tmmsm11g.ITEM_NAME";
		}

		if (tmmsm11g["ITEM_DES"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND ITEM_DES = @tmmsm11g.ITEM_DES";
		}

		sqlstr_count = sqlstr_count + sqlstr_temp;

		sqlstr_temp += " ORDER BY ITEM_DES";

		sqlstr = sqlstr + sqlstr_temp;

		//------------------ sql拼接 ---------------------------------

		//sql参数赋值
		cmd.Parameters.Set("tmmsm11g.ITEM_NAME", tmmsm11g["ITEM_NAME"].ToString());
		cmd.Parameters.Set("tmmsm11g.ITEM_DES", tmmsm11g["ITEM_DES"].ToString());

		cmd.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd.ExecuteScalar().ToInt32();

		//分页获取
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd.Close();

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
	cmd.Close();

	return doFlag;		//返回-1时事务将回滚，返回为0是事务将提交
}