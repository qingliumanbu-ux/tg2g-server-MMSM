/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-17
Description: 板坯规格上下限维护
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明
int f_mmsm33bpgg_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsm33bpgg_pro)

int f_mmsm33bpgg_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString procDiv = "";

	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */
	CModel tmmsm33bpgg("TMMSM33BPGG");
	//CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
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
			tmmsm33bpgg.Reset();
			tmmsm33bpgg.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			

			if (procDiv == "I")
			{
				//tmmsm33bpgg["DATE_TIME"] = datetime;
				tmmsm33bpgg.Print();
				tmmsm33bpgg.Insert();
			}
			else if (procDiv == "D")
			{
				tmmsm33bpgg.Delete();
			}
			else if (procDiv == "U")
			{
				tmmsm33bpgg["REC_REVISOR"] = s.userid;
				tmmsm33bpgg["REC_REVISE_TIME"] = datetime;
				tmmsm33bpgg.Update("*", "ST_NO,SLAB_TYPE");
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


