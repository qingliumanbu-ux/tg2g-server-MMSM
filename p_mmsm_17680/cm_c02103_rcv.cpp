/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:songwei
Date:2023-11-28
Version:1.0
Description: 接收资源系统
质量数据**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


// service入口
BM2F_ENTERACE_TELE(cm_c02103_rcv)

int f_cm_c02103_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	//程序用变量
	int doFlag = 0;
	CString sqlstr = "";
	CString div_flag = "";
	CString matPlmsCode = "";
	CString matCode = "";
	CString matType = "";
	CDbCommand cmd_inq(conn);
	CModel tmmsm50("TMMSM50");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		div_flag = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();
		matPlmsCode = bcls_rec->Tables[0].Rows[0]["PLMS_MAT_CODE"].ToString();
		matCode = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
		matType = bcls_rec->Tables[0].Rows[0]["PSCS_MAT_TYPE"].ToString();
		Log::Trace("", __FUNCTION__, "matPlmsCode=[{0}] matCode=[{1}]", matPlmsCode, matCode);
		tmmsm50.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (matCode.SubstringNE(0, 1) == "F")
		{
			tmmsm50["MAT_CODE_L2"] = matCode.SubstringNE(0, 3);
			if (matType.Trim() == "")
			{
				tmmsm50["MAT_TYPE"] = "2";
				tmmsm50["BACK_C1"] = "SCRAP";
			}
		}
		else
		{
			tmmsm50["MAT_CODE_L2"] = bcls_rec->Tables[0].Rows[0]["MATERIAL_CODE"].ToString();
			if (matCode.SubstringNE(0, 1) == "A")
			{
				tmmsm50["MAT_TYPE"] = "1";
				tmmsm50["BACK_C1"] = "ALLOY";
			}
		}
		//用于209 废钢进厂临时画面：F01/F02开头物料必须录入批次号
		if (matCode.SubstringNE(0, 3) == "F01" || matCode.SubstringNE(0, 3) == "F02")
		{
			tmmsm50["BACK_C3"] = "1";
		}
		tmmsm50["LOT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
		tmmsm50["PRIMARY_MAT_UNIT"] = bcls_rec->Tables[0].Rows[0]["PRIMARY_MAT_UNIT"].ToString();
		
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
		//用于RCS+RC认证
		if (matCode.SubstringNE(0, 3) == "F00" || matCode.SubstringNE(0, 3) == "F01" || matCode.SubstringNE(0, 3) == "F02")
		{
			tmmsm50["CS_FLAG"] = "3";
		}
		if (matCode.SubstringNE(0, 3) == "F31")
		{
			tmmsm50["CS_FLAG"] = "1";
		}
		if (matCode.SubstringNE(0, 3) == "F03" || matCode.SubstringNE(0, 3) == "F04" || matCode.SubstringNE(0, 3) == "F05" || matCode.SubstringNE(0, 3) == "F06" || matCode.SubstringNE(0, 3) == "F30" || matCode.SubstringNE(0, 3) == "F32")
		{
			tmmsm50["CS_FLAG"] = "4";
		}
		if (0 < tmmsm50.QueryCount("MAT_CODE"))
		{
			tmmsm50["REC_REVISE_TIME"] = datetime;
			tmmsm50["REC_REVISOR"] = s.userid;	
			tmmsm50.TrimOrBlank();
			tmmsm50.Update("MAT_NAME,MAT_CODE_L2,LOT_NO,PRIMARY_MAT_UNIT,REC_REVISE_TIME,REC_REVISOR","MAT_CODE");
		}
		else
		{
			tmmsm50["FACTORY_DIV"] = "S2N";
			tmmsm50["SYSTEM_ID_MAT"] = "C";
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
