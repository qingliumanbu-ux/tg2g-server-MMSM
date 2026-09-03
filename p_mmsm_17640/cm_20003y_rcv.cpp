/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    向萍
Version:    1.0
Date:       2015-04-01
Description: 炼钢缺陷信息电文接受
**************************************************/
//框架头文件
#include "stdafx.h" 
#include "epex.h"

/*<remark>=========================================================
/// <summary>
/// 炼钢缺陷信息电文接受
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/


//业务头文件




//外部函数声明
int f_mmsm0603_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm0604_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm0605_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE_TELE(cm_20003y_rcv)

int f_cm_20003y_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm06("TMMSM06");
	CModel tmmsm061("TMMSM061");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM0603");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM0603");
		}
		blkNum = bcls_rec->Tables.IndexOf("MMSM0604");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM0604");
		}
		blkNum = bcls_rec->Tables.IndexOf("MMSM0605");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM0605");
		}

		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm061.Reset();
			tmmsm061.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm061.TrimOrBlank();

			/* 打印输入参数 */
			//Log::Trace("", __FUNCTION__, "传入参数,tmmsm061.PROD_SEQ_NO				= [{0}]", tmmsm061["PROD_SEQ_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "传入参数,tmmsm061.MAT_NO					= [{0}]", tmmsm061["MAT_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "传入参数,tmmsm061.CLEAR_DIV				= [{0}]", tmmsm061["CLEAR_DIV"].ToString());

			if (tmmsm061["MAT_NO"].ToString().Trim() == "")
			{
				break;
			}

			if (tmmsm061["CLEAR_DIV"].ToString().Trim() == "I")
			{
				/* 缺陷新增 */
				bcls_rec->Tables["MMSM0603"].Clear();
				tmmsm061.MergeTo(bcls_rec->Tables["MMSM0603"], false);
				doFlag = f_mmsm0603_proc(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else if (tmmsm061["CLEAR_DIV"].ToString().Trim() == "U")
			{
				/* 缺陷修改 */
				bcls_rec->Tables["MMSM0604"].Clear();
				tmmsm061.MergeTo(bcls_rec->Tables["MMSM0604"], false);
				doFlag = f_mmsm0604_proc(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else if (tmmsm061["CLEAR_DIV"].ToString().Trim() == "D")
			{
				/* 缺陷删除 */
				bcls_rec->Tables["MMSM0605"].Clear();
				tmmsm061.MergeTo(bcls_rec->Tables["MMSM0605"], false);
				doFlag = f_mmsm0605_proc(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
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
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
