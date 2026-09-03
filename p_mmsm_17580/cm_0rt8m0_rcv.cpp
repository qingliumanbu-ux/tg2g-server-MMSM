/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      lizhen
Version:     1.0
Date:        2023-11-08
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 倒灌电文
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



BM2F_ENTERACE_TELE(cm_0rt8m0_rcv)

int f_cm_0rt8m0_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);




	try
	{
		if (bcls_rec->Tables.IndexOf("MM0099") < 0) {
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
			
		}
		bcls_rec->Tables["MM0099"].Rows.Clear();
		if (bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim() == "") {
			sprintf(s.msg, "材料号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm96.CopyFrom(tmmsm01);
		tmmsm96["EVENT_ID"] = "WM20";
		tmmsm96["EVENT_LINE_TYPE"] = "SM";
		tmmsm96["SYSTEM_ID"] = "MMSM";
		tmmsm96["FUNC_ID"] = "cm_0rt8m0_rcv";
		tmmsm96["EVENT_DESC"] = "倒灌新增";
		tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
		tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0) {
			throw CApplicationException(-1, s.msg, s.svc_name);
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


