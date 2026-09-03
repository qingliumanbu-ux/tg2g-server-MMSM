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
/// 
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



BM2F_ENTERACE_TELE(cm_e2t8c2_rcv)

int f_cm_e2t8c2_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel tmmsmccm("TMMSMCCM");
	CModel ccm("DA_CCM_ACT_PROD_DATA");

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		//INT_COM_CCM_ACT_PROD_DATA.AGGREGATE_NAME
		//插入L3系统数据
		tmmsmccm.MergeFrom(bcls_rec->Tables["INT_COM_CCM_ACT_PROD_DATA"].Rows[0]);
		tmmsmccm["ID_SJ"] = bcls_rec->Tables["INT_COM_CCM_ACT_PROD_DATA"].Rows[0]["ID"].ToString();
		tmmsmccm["DEV_CODE"] = bcls_rec->Tables["INT_COM_CCM_ACT_PROD_DATA"].Rows[0]["AGGREGATE_NAME"].ToString();
		//INT_COM_CCM_ACT_PROD_DATA.HEAT_NUMBER
		tmmsmccm["HEAT_NO"] = bcls_rec->Tables["INT_COM_CCM_ACT_PROD_DATA"].Rows[0]["HEAT_NUMBER"].ToString();
		//INT_COM_CCM_ACT_PROD_DATA.CASTING_TIME
		tmmsmccm["PUR_TIME"] = bcls_rec->Tables["INT_COM_CCM_ACT_PROD_DATA"].Rows[0]["CASTING_TIME"].ToString();
		tmmsmccm["REC_CREATOR"] = s.userid;
		tmmsmccm["REC_CREATE_TIME"] = datetime;
		tmmsmccm.Insert();
		//插入集控大屏数据
		ccm.MergeFrom(bcls_rec->Tables["INT_COM_CCM_ACT_PROD_DATA"].Rows[0]);
		ccm.Insert();
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

	return doFlag;
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


