/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李晓明
Version:    1.0
Date:       2024-03-05
Description: 鱼雷罐重量履历查询
**************************************************/
//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(mmsm11f_inq)

int f_mmsm11f_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	int doFlag = 0;				//程序返回值

	/* 分页信息 */
	CPageInfo pageInfo;
	int	TotalRecordCount = 0;

	/* 数据库表实体类定义 */
	CModel tmmsm11f("TMMSM11F");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	/* sql语句变量 */
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";

	try
	{
		tmmsm11f.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		sqlstr_count = " SELECT COUNT(1)  FROM  TMMSM11F WHERE 1=1 ";
		sqlstr = " SELECT *  FROM TMMSM11F WHERE 1=1 ";

		if (tmmsm11f["PLAN_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND PLAN_NO = @tmmsm11f.PLAN_NO";
		}

		if (tmmsm11f["TPC_YL_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND TPC_YL_NO = @tmmsm11f.TPC_YL_NO";
		}

		sqlstr_count = sqlstr_count + sqlstr_temp;

		sqlstr_temp += " ORDER BY PLAN_NO DESC";

		sqlstr = sqlstr + sqlstr_temp;

		cmd_inq.Parameters.Set("tmmsm11f.PLAN_NO", tmmsm11f["PLAN_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm11f.TPC_YL_NO", tmmsm11f["TPC_YL_NO"].ToString());

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