/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:     563167 -563167
Version:    1.0
Date:       2026-03-10
Description: 铁料装车强制取消
**************************************************/
//框架头文件
#include "stdafx.h" 




//业务头文件


//外部函数声明   

BM2F_ENTERACE(mmsm67car_cancel)

int f_mmsm67car_cancel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString	datetime("");

	/* 业务变量 */

	/* 实体类定义 */
	CModel twmsm61("TWMSM61");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		

		if (bcls_rec->Tables.IndexOf("cancel") >= 0)
		{

			for (int i = 0; i < bcls_rec->Tables["cancel"].Rows.get_Count(); i++)
			{
				twmsm61.Reset();
				twmsm61.MergeFrom(bcls_rec->Tables["cancel"].Rows[i]);

				if (bcls_rec->Tables["cancel"].Rows[i]["DEAL_FLAG"].ToString() == "I" && bcls_rec->Tables["cancel"].Rows[i]["UNLOAD_STATE"].ToString() == "1")
				{
					twmsm61["DEAL_FLAG"] = "D";
					twmsm61["UNLOAD_STATE"] = "2";
					twmsm61["AUTO_FLAG"] = "1";
					Log::Trace("", __FUNCTION__, "DEAL_FLAG = [{0}]", twmsm61["DEAL_FLAG"].ToString());
					Log::Trace("", __FUNCTION__, "UNLOAD_STATE = [{0}]", twmsm61["UNLOAD_STATE"].ToString());
					twmsm61.Update("UNLOAD_STATE,DEAL_FLAG,AUTO_FLAG", "PRACTICE_NO,MAT_NO");
				}
				else{
					strcpy(s.msg, "处置标记和状态不符合操作条件！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}