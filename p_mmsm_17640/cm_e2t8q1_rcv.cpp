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
/// 质量等级值电文接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/
 
//业务头文件




BM2F_ENTERACE_TELE(cm_e2t8q1_rcv)

int f_cm_e2t8q1_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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

	CModel tmmsm3d("TMMSM3D");

	//将table【0】设置为table【TMMSM3D】,并将结构体37表结构赋给table【TMMSM3D】

	EIClass bcls_rec_MMSM3D;
	bcls_rec_MMSM3D.Tables[0].set_TableName("TMMSM3D");
	bcls_rec_MMSM3D.Tables[0].Clear();
	bcls_rec_MMSM3D.Tables[0].Columns.Add(tmmsm3d);
	bcls_rec_MMSM3D.Tables[0].Rows.Add();
	try
	{


#pragma region  将接口字段与表字段对应
		if (!bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Columns.Contains("TABLE_NAME") && !bcls_rec->Tables["AGGREGATE_NAME"].Columns.Contains("AGGREGATE_NAME"))
		{
			Log::Trace("", "", "该表没有字段");
		}
		else
		{
			tmmsm3d["DEV_CODE"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["AGGREGATE_NAME"];
			tmmsm3d["HEAT_NO"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["HEAT_NUMBER"];
			tmmsm3d["PLAN_NO"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["PLAN_NUMBER"];
			tmmsm3d["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["SPLIT_INDICATION"];
			tmmsm3d["TREATMENT_COUNTER"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["TREATMENT_COUNTER"];
			tmmsm3d["STRAND_NO"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["STRAND_NUMBER"];
			tmmsm3d["SLAB_NO"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["SLAB_NUMBER"];
			tmmsm3d["PROD_DATE"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["GEN_TIME"];
			tmmsm3d["ITEM_ID"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE_DETAIL"].Rows[0]["ITEM_ID"];
			tmmsm3d["ITEM_VALUE1"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE_DETAIL"].Rows[0]["ITEM_VALUE"];

			tmmsm3d.Insert();
		}
#pragma endregion 

		//tmmsm3d.MergeFrom(bcls_rec_MMSM3D.Tables["TMMSM3D"].Rows[0]);
		//tmmsm3d.Insert();

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


