/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2016-01-22
Description: 设备停机实绩处理
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明
int f_mmsm2e_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsm2e_pro)

int f_mmsm2e_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString procDiv = "";
	CDecimal ts = 0;


	/* 业务变量 */
	CModel tmmsm2e("TMMSM2E");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */

	try
	{
		procDiv = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tmmsm2e.Reset();
			tmmsm2e.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
			if (procDiv == "I")
			{
				if (tmmsm2e.QueryCount("DEV_CODE,STOP_START_TIME") > 0){
					strcpy(s.msg, "设备代码：" + tmmsm2e["DEV_CODE"].ToString() + ", 停机开始时间：" + tmmsm2e["STOP_START_TIME"].ToString() + " 记录已存在。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				else
				{

				
				tmmsm2e["REC_CREATE_TIME"] = datetime;
				tmmsm2e["REC_CREATOR"] = s.userid;
				//tmmsm2e["CLIENT_IP"] = s.fore_ip;
				if (CDateTime::Parse(tmmsm2e["STOP_START_TIME"]) > CDateTime::Parse(tmmsm2e["STOP_END_TIME"]))
				{
					strcpy(s.sysmsg, "停机开始时间晚于停机结束时间！");
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				ts = (CDateTime::Parse(tmmsm2e["STOP_END_TIME"]) - CDateTime::Parse(tmmsm2e["STOP_START_TIME"])).TotalMinutes();
				tmmsm2e["DEV_STOP_TIME"] = ts.Round(0);
				tmmsm2e.Print();
				tmmsm2e.TrimOrBlank();
				tmmsm2e.Insert();
			   }
			}
			else if (procDiv == "D")
			{
				tmmsm2e.Delete();
			}
			else if (procDiv == "U")
			{
				tmmsm2e["REC_REVISE_TIME"] = datetime;
				tmmsm2e["REC_REVISOR"] = s.userid;
				tmmsm2e.Update("*", "DEV_CODE,STOP_START_TIME");
			}
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


