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
/// 物料数据同步
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



BM2F_ENTERACE_TELE(cm_0rt8s0_rcv)

int f_cm_0rt8s0_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel tmm0097("TMM0097");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
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
		if (bcls_rec->Tables[0].Rows[0]["EVENT_ID"].ToString().Trim() == "")
		{
			sprintf(s.msg, "事件号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables[0].Rows[0]["EVENT_LINE_TYPE"].ToString().Trim() == "")
		{
			sprintf(s.msg, "事件产线类型不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables[0].Rows[0]["SYSTEM_ID"].ToString().Trim() == "")
		{
			sprintf(s.msg, "系统标识不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim() == "")
		{
			sprintf(s.msg, "物料种类不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "材料号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tmm0097.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm96.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (tmm0097.QueryCount("EVENT_ID,EVENT_LINE_TYPE,MAT_KIND") != 1)
		{
			sprintf(s.msg, "事件配置在MES端没有找到!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01.QueryCount("MAT_NO") < 1) {
			sprintf(s.msg, "材料号[%s]不存在!", (const char*)tmmsm01["MAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
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


