/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:   王佳倩
Version:    1.0
Date:     2022-10-11
Description: 保留炉次判定需要判定项目（智慧质量调用）
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/* ***** 静态函数申明 ***** */
int f_qmts25_get_qx(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	//调用质量查询函数
int f_mmsm_getdata(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	//调用物料查询函数

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>

/// <returns>  </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsmqx_inq)

int f_mmsmqx_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";	

	EIClass inBlock, outBlock;
	try
	{
		/**************************1、调用质量函数 ************************************/
		//bcls_ret->Tables.Add();	//返回两张表

		//doFlag = f_qmts25_get_qx(bcls_rec, &outBlock,conn);
		//if (doFlag < 0)
		//{
		//	Log::Trace("", "", "获取质量数据报错");
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//Log::Trace("", "", "获取质量数据结束，行数 = [{0}]",outBlock.Tables[0].Rows.get_Count());

		////成分数据 - Table[0]
		//if (outBlock.Tables[0].Rows.get_Count() >0)
		//{
		//	bcls_ret->Tables[0].Copy(outBlock.Tables[0]);
		//	bcls_ret->Tables[0].set_TableName("ELM");		//Table0 成分表
		//}

		///**************************2、调用物料函数 ************************************/
		////清空行数
		//outBlock.Tables[0].Columns.Clear();
		//outBlock.Tables[0].Rows.Clear();
		//doFlag = f_mmsm_getdata(bcls_rec, &outBlock, conn);
		//if (doFlag < 0)
		//{
		//	Log::Trace("", "", "获取物料数据报错");
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//Log::Trace("", "", "获取物料数据结束，行数 = [{0}]", outBlock.Tables[0].Rows.get_Count());

		////过程数据 - Table[1]
		//if (outBlock.Tables[0].Rows.get_Count() >0)
		//{
		//	bcls_ret->Tables[1].Copy(outBlock.Tables[0]);
		//	bcls_ret->Tables[1].set_TableName("PRO");		//Table1 过程实绩表
		//}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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

}
