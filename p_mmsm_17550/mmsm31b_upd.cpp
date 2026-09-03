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
BM2F_ENTERACE(mmsm31b_upd)

int f_mmsm31b_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	/* 实体类定义 */
	CModel tpssm10("TPSSM10");

	try
	{
		tpssm10.Reset();
		tpssm10.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", __FUNCTION__, "--- tpssm10.PONO = [{0}] ---", tpssm10["PONO"].ToString());
		Log::Trace("", __FUNCTION__, "--- tpssm10.INTERMIX_WAY = [{0}] ---", tpssm10["INTERMIX_WAY"].ToString());
		Log::Trace("", __FUNCTION__, "--- tpssm10.INTERMIX_NOEXE_RESULT = [{0}] ---", tpssm10["INTERMIX_NOEXE_RESULT"].ToString());
		Log::Trace("", __FUNCTION__, "--- tpssm10.INTERMIX_CC_WT = [{0}] ---", tpssm10["INTERMIX_CC_WT"].ToString());

		if (tpssm10["INTERMIX_CC_WT"].ToDecimal() <= 0)
		{
			strcpy(s.msg, "请输入开浇吨位。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
	
		tpssm10["REC_REVISE_TIME"] = dateNow;
		tpssm10["REC_REVISOR"] = s.userid;
		tpssm10.Update("REC_REVISE_TIME,REC_REVISOR,INTERMIX_WAY,INTERMIX_NOEXE_RESULT,INTERMIX_CC_WT", "PONO");

		
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