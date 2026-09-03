/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-25
Description: 制造命令号对应的板坯查询
**************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/
//#include "tmmsm15.h"
/* ***** 静态函数申明 ***** */
//int f_cm_jojm01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_cm_geg001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm50af9_snd)
//-EP_SYSTEM_HEAD_END                                                  
int f_mmsm50af9_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn)
{

	EPEX epex;
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ret = 0;
	/* 业务变量 */
	
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");;
	CString v_table_type = "";
	CString work_date = "";

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//CTMMSM15 tmmsm15(conn);

	try
	{
		/*EIClass bcls_rec_sm;

		for (int i = 1; i <= bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm15.Reset();
			tmmsm15.IRON_NO = bcls_rec->Tables[0].Rows[i - 1]["IRON_NO"].ToString().Trim();
			tmmsm15.CFID = bcls_rec->Tables[0].Rows[i - 1]["CFID"].ToString().Trim();
			tmmsm15.IRON_DEST = bcls_rec->Tables[0].Rows[i - 1]["FACTORY_DIV"].ToString().Trim();
			tmmsm15.TPC_STAND_NO = bcls_rec->Tables[0].Rows[i - 1]["TPC_STAND_NO"].ToString().Trim();
			if (tmmsm15.TPC_STAND_NO == "")
			{
				sprintf(s.msg, "罐架号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm15.USE_UP_TIME = datetime;
			Log::Info("", __FUNCTION__, "tmmsm15.IRON_NO=[{0}]", tmmsm15.IRON_NO);
			Log::Info("", __FUNCTION__, "tmmsm15.CFID=[{0}]", tmmsm15.CFID);
			Log::Info("", __FUNCTION__, "OP_FLAG=[{0}]", bcls_rec->Tables[0].Rows[i - 1]["OP_FLAG"].ToString().Trim());

			tmmsm15.Update("TPC_STAND_NO, USE_UP_TIME", "IRON_NO,CFID");

			tmmsm15.MergeTo(bcls_rec_sm.Tables[0], false);

			if (!bcls_rec_sm.Tables[0].Columns.Contains("OP_FLAG"))
			{
				bcls_rec_sm.Tables[0].Columns.Add(DT_STRING, "OP_FLAG");
			}
			bcls_rec_sm.Tables[0].Rows[i-1]["OP_FLAG"] = bcls_rec->Tables[0].Rows[i - 1]["OP_FLAG"].ToString().Trim();
			if (!bcls_rec_sm.Tables[0].Columns.Contains("LADLE_NO"))
			{
				bcls_rec_sm.Tables[0].Columns.Add(DT_STRING, "LADLE_NO");
			}
			bcls_rec_sm.Tables[0].Rows[i - 1]["LADLE_NO"] = tmmsm15.CFID;
		
			
		}

		doFlag = f_cm_jojm01_snd(&bcls_rec_sm, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		/*doFlag = f_cm_geg001_snd(&bcls_rec_sm, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{

		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 




