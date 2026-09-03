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
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 板坯检验数据电文接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件




BM2F_ENTERACE_TELE(cm_e2t8q0_rcv)

int f_cm_e2t8q0_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */

	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);

	CModel tmmsm3c("TMMSM3C");

	//将table【0】设置为table【TMMSM3C】,并将结构体37表结构赋给table【TMMSM3C】

	/*EIClass bcls_rec_MMSM3C;
	bcls_rec_MMSM3C.Tables[0].set_TableName("TMMSM3C");
	bcls_rec_MMSM3C.Tables[0].Clear();
	bcls_rec_MMSM3C.Tables[0].Columns.Add(tmmsm3c);
	bcls_rec_MMSM3C.Tables[0].Rows.Add();*/

	try 
	{

#pragma region  将接口字段与表字段对应
		if (!bcls_rec->Tables["INT_MES_SLAB_INSPECT"].Columns.Contains("TABLE_NAME") && !bcls_rec->Tables["AGGREGATE_NAME"].Columns.Contains("AGGREGATE_NAME"))
		{
			Log::Trace("", "", "该表没有字段");
		}
		else
		{
			tmmsm3c["DEV_CODE"] = bcls_rec->Tables["INT_MES_SLAB_INSPECT"].Rows[0]["AGGREGATE_NAME"];
			tmmsm3c["SLAB_NO"] = bcls_rec->Tables["INT_MES_SLAB_INSPECT"].Rows[0]["SLAB_NUMBER"];
			tmmsm3c["VIRTUAL_SLAB_NO"] = bcls_rec->Tables["INT_MES_SLAB_INSPECT"].Rows[0]["VIRTUAL_SLAB_ID"];
			tmmsm3c["INSPECTION"] = bcls_rec->Tables["INT_MES_SLAB_INSPECT"].Rows[0]["INSPECTION"];
			tmmsm3c["DESCRIPTION"] = bcls_rec->Tables["INT_MES_SLAB_INSPECT"].Rows[0]["DESCRIPTION"];

			tmmsm3c.Insert();
		}
#pragma endregion

		//tmmsm3c.MergeFrom(bcls_rec_MMSM3C.Tables["TMMSM3C"].Rows[0]);
		//tmmsm3c.Insert();

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


