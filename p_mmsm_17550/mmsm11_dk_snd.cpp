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

/* ***** 静态函数申明 ***** */
//int f_cm_jojm01_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_cm_geg001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm11_dk_snd)
//-EP_SYSTEM_HEAD_END                                                  
int f_mmsm11_dk_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	CModel tmmsm11("TMMSM11");

	try
	{
		EIClass bcls_rec_sm;

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm11.Reset();
			tmmsm11.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm11.TrimOrBlank();
			tmmsm11["REC_REVISOR"] = s.userid;
			tmmsm11["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm11["RETURN_FLAG"] = 1;
			tmmsm11.Update("REC_REVISOR, REC_REVISE_TIME, RETURN_FLAG,RETURN_TIME", "TICODE");

			/*tmmsm11.MergeTo(bcls_rec_sm.Tables[0], false);

			if (!bcls_rec_sm.Tables[0].Columns.Contains("OP_FLAG"))
			{
				bcls_rec_sm.Tables[0].Columns.Add(DT_STRING, "OP_FLAG");
			}

			bcls_rec_sm.Tables[0].Rows[i]["OP_FLAG"] = bcls_rec->Tables[0].Rows[i]["OP_FLAG"].ToString().Trim();
			*/


		}

		/*doFlag = f_cm_jojm01_snd(&bcls_rec_sm, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/


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




