/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
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
/// 板坯请求
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
int f_mmsm_e2t8m1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);




BM2F_ENTERACE_TELE(cm_e2t8m0_rcv)

int f_cm_e2t8m0_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);

	CModel tmmsm33("TMMSM33");

	try
	{
		//tmmsm33["PRINT_NO"] = bcls_rec->Tables["INT_MES_SLAB_REQUEST"].Rows[0]["SLAB_NUMBER"].ToString();

		tmmsm33["MAT_NO"] = bcls_rec->Tables["INT_MES_SLAB_REQUEST"].Rows[0]["SLAB_NUMBER"].ToString();
		tmmsm33.Query("MAT_NO");//STOCK_L2

		bcls_rec->Tables.Add();
		bcls_rec->Tables[1].set_TableName("E2T8M1");
		bcls_rec->Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["E2T8M1"].Rows.Add();
		bcls_rec->Tables["E2T8M1"].Rows[0]["MAT_NO"] = tmmsm33["MAT_NO"];

		doFlag = f_mmsm_e2t8m1_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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


