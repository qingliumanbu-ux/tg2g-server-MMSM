/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56  
Description: 炼钢吹氩站底吹氩维护
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

/***** 外部函数声明 *****/
int f_tmsm01_upd_ar(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsm22_bar)

int f_mmsm22_bar(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	/* 实体类定义 */
	CModel tmmsm22("TMMSM22");

	try
	{
		tmmsm22.Reset();
		tmmsm22.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", __FUNCTION__, "--- tmmsm22.FACTORY_DIV = [{0}] ---", tmmsm22["FACTORY_DIV"].ToString());
		Log::Trace("", __FUNCTION__, "--- tmmsm22.HEAT_NO = [{0}] ---", tmmsm22["HEAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "--- tmmsm22.LADLE_NO = [{0}] ---", tmmsm22["LADLE_NO"].ToString());
		Log::Trace("", __FUNCTION__, "--- tmmsm22.BLOW_AR_RESULT = [{0}] ---", tmmsm22["BLOW_AR_RESULT"].ToString());

		if (tmmsm22["HEAT_NO"].ToString().Trim() == ""){
			strcpy(s.msg, "请输入熔炼号。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm22["LADLE_NO"].ToString().Trim() == ""){
			strcpy(s.msg, "请输入钢包号。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (1 > tmmsm22.QueryCount("HEAT_NO,LADLE_NO,FACTORY_DIV")){
			strcpy(s.msg, "熔炼号：" + tmmsm22["HEAT_NO"].ToString() + ", 钢包号：" + tmmsm22["LADLE_NO"].ToString() + " 吹氩实绩不存在。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm22["REC_REVISE_TIME"] = dateNow;
		tmmsm22["REC_REVISOR"] = s.userid;
		tmmsm22.Update("REC_REVISE_TIME,REC_REVISOR,BLOW_AR_RESULT", "HEAT_NO,LADLE_NO,FACTORY_DIV");

		//调用工器具（钢包）函数
		if (!bcls_rec->Tables[0].Columns.Contains("SM_UNIT_NO")){
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "SM_UNIT_NO");
		}
		bcls_rec->Tables[0].Rows[0]["SM_UNIT_NO"] = tmmsm22["FACTORY_DIV"];
		doFlag = f_tmsm01_upd_ar(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}