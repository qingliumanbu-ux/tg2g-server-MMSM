/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 工艺路线确定并发送电文
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsmgylx_upd)

int f_mmsmgylx_upd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd_inq_code(conn);
	CModel tmmsmgy06("TMMSMGY06");
	CString dev_code = "";
	CString sm_plan_no2 = "";//炼钢计划号

	//加入函数的表

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	Log::Trace("", __FUNCTION__, "heat_no= [{0}]");
	try
	{
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:						// 所有数据库适用，通用SQL语句
		//	sqlstr = " select * from TMMSMGY06 where TC_SEND_FLAG='1' ";
		//	break;
		//}
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	cmd_inq.Fetch(tmmsmgy06);
		//}
		//cmd_inq.Close();
		////调用
		////调用发送电文
		//blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		//if (blkNum < 0)
		//{
		//bcls_rec->Tables.Add("MMSMSND");
		//}

		//if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
		//{
		//bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
		//}

		//if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
		//{
		//bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
		//}

		//if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("HEAT_NO"))
		//{
		//bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "HEAT_NO");
		//}
		//if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
		//{
		//bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "L2_PROC_NO");
		//}
		//if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("DEV_CODE"))
		//{
		//bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "DEV_CODE");
		//}

		//bcls_rec->Tables["MMSMSND"].Rows.Add();
		//bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM21S";
		//bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = "I";
		//bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tmmsmgy06["HEAT_NO"];
		//bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tmmsmgy06["L2_PROC_NO"];
		//bcls_rec->Tables["MMSMSND"].Rows[0]["DEV_CODE"] = tmmsmgy06["DEV_CODE"];
		//if (tmmsmgy06["TOGETHER_TIME"].ToDecimal() != 0){
		//	doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		//	if (doFlag < 0)
		//	{
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//	tmmsmgy06["TOGETHER_TIME"] = "0";
		//	tmmsmgy06.Update("TOGETHER_TIME", "HEAT_NO,L2_PROC_NO,DEV_CODE");
		//	doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		//	if (doFlag < 0)
		//	{
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//}
		//else{
		//	
		//}
		//tmmsmgy06["TC_SEND_FLAG"] = "2";
		//tmmsmgy06["DATI_MSG_SENT"] = datetime;
		//tmmsmgy06.Update("TC_SEND_FLAG,DATI_MSG_SENT", "HEAT_NO,L2_PROC_NO,DEV_CODE");
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


