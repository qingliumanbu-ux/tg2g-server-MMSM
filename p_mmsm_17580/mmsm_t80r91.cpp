/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2018-06-21
Description:批量执行抛帐
**************************************************/
//框架用头文件
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm_t80r91)

int f_mmsm_t80r91_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送炉成本
int f_mmsm_t80r91(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	int i_idx = 0;
	CString sqlstr = "";
	CString stat_date = "";
	CString begin_time = "";
	CString end_time = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal i_count = 0;
	int j = 0;
	CString seq_id = "0";
	CString send_flag = "0";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm2a_send("TMMSM2A_SEND");
	CModel tmmsmgy06_sed("TMMSMGY06_SED");



	CString deal_flag = "";

	begin_time = CDateTime::Now().AddHours(-3).ToString("yyyyMMddHHmmss");
	end_time = CDateTime::Now().AddHours(-2).ToString("yyyyMMddHHmmss");

	try
	{
		sqlstr = " select distinct heat_no from tmmsmgy05 where (CAST_DIV_NO,TD_NO_1,CAST_DIV_NO_1) in ("
			" select CAST_DIV_NO,TD_NO_1,CAST_DIV_NO_1 from tmmsmgy05"
			" where 1=1"
			" and TD_NO_1 !=' '"
			" and CAST_DIV_NO !=0"
			/*" and RECV_MAT_TIME<='202411291631'"
			" and RECV_MAT_TIME>='20241126'"*/
			" and RECV_MAT_TIME<=@end_time"
			" and RECV_MAT_TIME>=@begin_time"
			" ) and RECV_MAT_TIME!=' '"
			;
		EIClass bcls_ret3;
		EIClass bcls_rec3;
		bcls_rec3.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_rec3.Tables[0].Rows.Add();
		bcls_rec3.Tables[0].Rows.Clear();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteQuery(bcls_rec3.Tables[0]); 
		doFlag = f_mmsm_t80r91_snd(&bcls_rec3, &bcls_ret3, conn);
		if (doFlag < 0)
		{
			Log::Trace("", __FUNCTION__, "-------调用f_mmsm_t80r91_snd失败-------");
			//throw CApplicationException(-1, s.msg, log.Location);
		}
		cmd_inq.Close();

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
