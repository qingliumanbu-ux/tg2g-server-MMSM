/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2010
Author:			zhouyueqi
Version:		1.0
Date:			2023年5月7日
Description:	实绩关键字段校验规则查询
Update:
**************************************************/

//框架头文件
#include "stdafx.h"

//业务头文件

//外部函数声明

BM2F_ENTERACE(mmsmrl_inq)

int f_mmsmrl_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;

	/*业务变量*/
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_condition = "";
	CString sqlstr_order = "";
	
	int TotalRecordCount = 0;

	CPageInfo pageInfo;

	CModel tmmsmrl("TMMSMRL");

	CDbCommand cmd_inq(conn);

	try
	{
		CDateTime datetime = CDateTime::Now();

		try
		{
			//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		//获取传入参数
		tmmsmrl.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count =
				" SELECT COUNT(1) FROM TMMSMRL A"
				" WHERE 1 = 1"
				;

			sqlstr =
				" SELECT A.* FROM TMMSMRL A"
				" WHERE 1 = 1"
				;

			//获取查询条件
			CModel& model = tmmsmrl;
			CString tbl = "A";
			for (int i = 0; i < model.GetFields().get_Count(); i++)
			{
				if (model.GetFields()[i].ColumnType == DT_STRING && model[i].ToString().Trim() != "")
				{
					sqlstr_condition += " AND " + tbl + "." + model.GetFields()[i].ColumnName + " = @" + model.GetFields()[i].ColumnName;
					cmd_inq.Parameters.Set(model.GetFields()[i].ColumnName, model[i].ToString().Trim());
				}
				else if (model.GetFields()[i].ColumnType == DT_DECIMAL && model[i].ToDecimal() != 0)
				{
					sqlstr_condition += " AND " + tbl + "." + model.GetFields()[i].ColumnName + " = @" + model.GetFields()[i].ColumnName;
					cmd_inq.Parameters.Set(model.GetFields()[i].ColumnName, model[i].ToDecimal());
				}
			}
			
			sqlstr_order = "";

			sqlstr_count = sqlstr_count + sqlstr_condition;
			sqlstr = sqlstr + sqlstr_condition + sqlstr_order;
			break;
		}

		//Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		//Log::Info("", __FUNCTION__, "sqlstr_condition[{0}]", sqlstr_condition);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

		//返回提示栏信息
		CFormattable arguments[] = { TotalRecordCount }; // 定义参数列表的数组
		CMessageFormat::Format(s.msg, "查询到信息[{0}]条。", arguments, 1); //查询到[{0}]条记录。
		CString ts = ((CDecimal)(CDateTime::Now() - datetime).TotalMilliseconds()).Round(0).ToString();
		CFormattable arguments2[] = { ts };
		CMessageFormat::Format(s.sysmsg, "SVC用时[{0}ms]", arguments2, 1);

		//strcpy(s.msg, "测试报错");
		//throw CApplicationException(-1, s.msg, log.Location);
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


