/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      lizhen
Version:     1.0
Date:        2024-2-23
Description:获取板坯数据
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
///
///钢包数据
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



BM2F_ENTERACE_TELE(cm_p3t803_rcv)

int f_cm_p3t803_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel twmsma1("TWMSMA1");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		bcls_rec->Tables[0].Columns["loc_no"].set_ColumnName("SLAB_NO");
		bcls_rec->Tables[0].Columns["batch_no"].set_ColumnName("MAT_NO");
		bcls_rec->Tables[0].Columns["TAID"].set_ColumnName("BACK_N_1");
		bcls_rec->Tables[0].Columns["steel_grade"].set_ColumnName("SG_SIGN");
		bcls_rec->Tables[0].Columns["weight"].set_ColumnName("MAT_WT");
		bcls_rec->Tables[0].Columns["length"].set_ColumnName("MAT_LEN");
		bcls_rec->Tables[0].Columns["width"].set_ColumnName("MAT_WIDTH");
		bcls_rec->Tables[0].Columns["thick"].set_ColumnName("MAT_THICK");
		bcls_rec->Tables[0].Columns["FELDBEZZIEL"].set_ColumnName("BACK_C1");
		bcls_rec->Tables[0].Columns["REIHEBEZZIEL"].set_ColumnName("BACK_C2");
		bcls_rec->Tables[0].Columns["PLATZBEZZIEL"].set_ColumnName("BACK_C3");
		bcls_rec->Tables[0].Columns["programm"].set_ColumnName("BACK_C4");
		bcls_rec->Tables[0].Columns["KZGESPERRT"].set_ColumnName("BACK_C5");
		bcls_rec->Tables[0].Columns["SPERRGRUND"].set_ColumnName("BACK_C6");
		bcls_rec->Tables[0].Columns["FELDBEZ"].set_ColumnName("BACK_C7");
		bcls_rec->Tables[0].Columns["REIHEBEZ"].set_ColumnName("BACK_C8");
		bcls_rec->Tables[0].Columns["PLATZBEZ"].set_ColumnName("BACK_C9");
		bcls_rec->Tables[0].Columns["FA_NR"].set_ColumnName("BACK_CODE_1");
		bcls_rec->Tables[0].Columns["CCUSAGE"].set_ColumnName("BACK_CODE_2");
		bcls_rec->Tables[0].Columns["SURFACEGRINDINGMANNER"].set_ColumnName("BACK_CODE_3");
		bcls_rec->Tables[0].Columns["ORDERID"].set_ColumnName("BACK_CODE_4");

		twmsma1.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twmsma1.Insert();
	
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


