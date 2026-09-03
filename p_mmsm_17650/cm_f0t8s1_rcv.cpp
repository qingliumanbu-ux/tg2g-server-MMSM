/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"


/*<remark>=========================================================
/// <summary>
/// 能源返回消耗
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

BM2F_ENTERACE_TELE(cm_f0t8s1_rcv)

int f_cm_f0t8s1_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tmmsmt8s1("TMMSMT8S1");
	CString itemid = " ";
	CString itemname = " ";
	CString reportvalue = " ";
	CString clock = " ";
	CString sourceid = " ";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString seq("");
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			Log::Trace("", "Rows", "Rows = {0}", bcls_rec->Tables[0].Rows.get_Count());
			//获取电文的值
			itemid = bcls_rec->Tables[0].Rows[0]["itemid"].ToString();
			itemname = bcls_rec->Tables[0].Rows[0]["itemname"].ToString();
			reportvalue = bcls_rec->Tables[0].Rows[0]["reportvalue"].ToString();
			clock = bcls_rec->Tables[0].Rows[0]["clock"].ToString();
			if (clock != " "){
				cmd_inq.SetCommandText(" SELECT TO_CHAR(TO_DATE('" + clock + "', 'YYYY-MM-DD HH24:MI:SS'), 'yyyyMMdd') from dual ");
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					clock = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
			}
			//sourceid
			sourceid = bcls_rec->Tables[0].Rows[0]["sourceid"].ToString();
			//给表赋值
			tmmsmt8s1["DATE_TIME"] = clock;
			tmmsmt8s1["METE_NAME"] = itemname;
			tmmsmt8s1["METE_CODE_2"] = itemid;
			if (sourceid == "MIRT"){
				tmmsmt8s1["MIRT"] = reportvalue;
			}
			if (sourceid == "MIRL"){
				tmmsmt8s1["MIRL"] = reportvalue;
			}
			if (tmmsmt8s1.QueryCount("DATE_TIME,METE_CODE_2")){
				tmmsmt8s1["REC_REVISE_TIME"] = datetime;
				tmmsmt8s1["REC_REVISOR"] = s.userid;
				if (sourceid == "MIRT"){
					tmmsmt8s1.Update("MIRT,REC_REVISE_TIME,REC_REVISOR", "DATE_TIME,METE_CODE_2");
				}
				if (sourceid == "MIRL"){
					tmmsmt8s1.Update("MIRL,REC_REVISE_TIME,REC_REVISOR", "DATE_TIME,METE_CODE_2");
				}
			}
			else{
				tmmsmt8s1["REC_CREATE_TIME"] = datetime;
				tmmsmt8s1["REC_CREATOR"] = s.userid;
				tmmsmt8s1.Insert();
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


