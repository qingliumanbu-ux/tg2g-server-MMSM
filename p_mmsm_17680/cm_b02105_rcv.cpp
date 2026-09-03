/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2023-11-13
Version:1.0
Description: 物料移动实绩
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

#include "tmmsm21.h"


/* ***** 静态函数申明 ***** */



// service入口
BM2F_ENTERACE_TELE(cm_b02105_rcv)

int f_cm_b02105_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString div_flag = "";
	CString matPlmsCode = "";
	CString matCode = "";
	CDbCommand cmd_inq(conn);
	CModel tmmsm50("TMMSM50");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	
	try
	{
		div_flag = bcls_rec->Tables["B02105"].Rows[0]["DEAL_FLAG"].ToString();
		matPlmsCode = bcls_rec->Tables["B02105"].Rows[0]["PLMS_MAT_CODE"].ToString();
		matCode = bcls_rec->Tables["B02105"].Rows[0]["MAT_CODE"].ToString();
		
		
		Log::Trace("", __FUNCTION__, "matPlmsCode=[{0}] matCode=[{1}]", matPlmsCode, matCode);
		tmmsm50.MergeFrom(bcls_rec->Tables["B02105"].Rows[0]);

		tmmsm50["MAT_CODE"] = bcls_rec->Tables["B02105"].Rows[0]["MAT_CODE"].ToString();
		tmmsm50["MAT_NAME"] = bcls_rec->Tables["B02105"].Rows[0]["MAT_CNAME"].ToString();
		if (matCode.SubstringNE(0, 1) == "F")
		{
			tmmsm50["MAT_CODE_L2"] = matCode.SubstringNE(0, 3);
		}
		else
		{
			tmmsm50["MAT_CODE_L2"] = bcls_rec->Tables["B02105"].Rows[0]["COMM_CODE"].ToString();
		}
		
		tmmsm50["LOT_NO"] = bcls_rec->Tables["B02105"].Rows[0]["MAT_CODE"].ToString();
		if ("" == div_flag.Trim())
		{
			strcpy(s.msg, "处理标记不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if ("" == tmmsm50["MAT_CODE"].ToString().Trim())
		{
			strcpy(s.msg, "物料代码不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (0 < tmmsm50.QueryCount("MAT_CODE"))
		{
			tmmsm50["REC_REVISE_TIME"] = datetime;
			tmmsm50["REC_REVISOR"] = s.userid;
			tmmsm50.Update("MAT_NAME,MAT_CODE_L2,LOT_NO,REC_REVISE_TIME,REC_REVISOR","MAT_CODE");
		}
		else
		{
			tmmsm50["FACTORY_DIV"] = "S2N";
			tmmsm50["SYSTEM_ID_MAT"] = "B";
			tmmsm50["REC_CREATE_TIME"] = datetime;
			tmmsm50["REC_CREATOR"] = s.userid;
			tmmsm50.TrimOrBlank();
			tmmsm50.Insert();
		}
		
		
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
