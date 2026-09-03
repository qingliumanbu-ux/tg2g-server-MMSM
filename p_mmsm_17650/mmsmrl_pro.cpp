/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2010
Author:			zhouyueqi
Version:		1.0
Date:			2023年5月7日
Description:	实绩关键字段校验规则修改
Update:
**************************************************/

//框架头文件
#include "stdafx.h"

//业务头文件

//外部函数声明

BM2F_ENTERACE(mmsmrl_pro)

int f_mmsmrl_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	CString proc_div = "操作类型";	//操作类型
	CString msg = "";
	int proc_sum_add = 0;
	int proc_sum_upd = 0;
	int proc_sum_del = 0;

	try
	{
		CDateTime datetime = CDateTime::Now();

		//处理数据
		CString table_name = "TMMSMRL";
		CModel model(table_name);
		if (bcls_rec->Tables.Contains("ADD"))
		{
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				model.Reset();
				model.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				model["REC_CREATOR"] = s.userid;
				model["REC_CREATE_TIME"] = s.datetime;
				model.TrimOrBlank();

				if (model.Query())
				{
					Log::Trace("", __FUNCTION__, "第[{0}]条记录，主键重复，跳过新增！", i + 1);
					continue;
				}
				sqlstr = "INSERT INTO " + table_name;
				proc_sum_add += model.Insert();
			}
		}
		if (bcls_rec->Tables.Contains("UPD"))
		{
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				model.Reset();
				model.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				model.TrimOrBlank();
				model["REC_REVISOR"] = s.userid;
				model["REC_REVISE_TIME"] = s.datetime;
				sqlstr = "UPDATE " + table_name + " SET ";
				proc_sum_upd += model.Update("*");
			}
		}
		if (bcls_rec->Tables.Contains("DEL"))
		{
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				model.Reset();
				model.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
				if (!model.Query())
				{
					Log::Trace("", __FUNCTION__, "第[{0}]条记录不存在，跳过删除！", i + 1);
					continue;
				}
				sqlstr = "DELETE FROM " + table_name;
				proc_sum_del += model.Delete();
			}
		}

		//返回提示栏信息
		if (proc_sum_add > 0)
		{
			msg += "成功新增[" + to_string(proc_sum_add) + "]条数据！\n";
		}
		if (proc_sum_upd > 0)
		{
			msg += "成功修改[" + to_string(proc_sum_upd) + "]条数据！\n";
		}
		if (proc_sum_del > 0)
		{
			msg += "成功删除[" + to_string(proc_sum_del) + "]条数据！\n";
		}
		if (msg == "")
		{
			msg = "没有可修改的数据！";
		}
		strcpy(s.msg, msg);
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


