/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2018-06-21
Description:批量执行预熔液分配错误的
**************************************************/
//框架用头文件
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82c_batch)
//-EP_SYSTEM_HEAD_END
int f_mmsm_yry(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82c_batch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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

	EIClass bcls_ret4;
	EIClass bcls_ret2;



	CString deal_flag = "";

	begin_time = CDateTime::Now().AddDays(-31).ToString("yyyyMMddHHmmss");
	end_time = CDateTime::Now().AddHours(-1).ToString("yyyyMMddHHmmss");

	Log::Info("", __FUNCTION__, "begin_time =[{0}],end_time=[{1}]", begin_time, end_time);

	try
	{
		//当天预熔液分炉不一致的进行重新核算
		sqlstr = " select t1.HEAT_NO,t2.L2_PROC_NO as HEATNO_PREMELT from "
			" ( SELECT L2_PROC_NO, HEAT_COUNT,HEAT_NO"
			" FROM tmmsmgy06"
			" WHERE HEAT_COUNT > 0"
			" and handle_div = 'Y'"
			" and heat_no in (select heat_no from tmmsmgy05 where start_time<=@end_time and start_time>@begin_time)"
			" ) t1 left join ("
			" SELECT L2_PROC_NO, count(1) SJ_COUNT"
			" FROM tmmsmgy06"
			" WHERE 1=1"
			" and handle_div = 'Y'"
			" and heat_no in (select heat_no from tmmsmgy05 where start_time<=@end_time and start_time>@begin_time)"
			" group by L2_PROC_NO"
			" ) t2 on t1.L2_PROC_NO = t2.L2_PROC_NO"
			" where HEAT_COUNT-nvl(SJ_COUNT,0)!=0"
			" order by t1.HEAT_NO"
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("begin_time", begin_time);
		cmd_inq.Parameters.Set("end_time", end_time);
		cmd_inq.ExecuteQuery(bcls_ret4.Tables[0]);
		cmd_inq.Close();

		doFlag = f_mmsm_yry(&bcls_ret4, &bcls_ret2, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		EIClass bcls_ret1;
		EIClass bcls_rec1;
		bcls_rec1.Tables[0].Columns.Add(tmmsmgy05);
		bcls_rec1.Tables[0].Rows.Clear();
		bcls_rec1.Tables[0].Rows.Add();
		for (int i = 0; i < bcls_ret4.Tables[0].Rows.get_Count(); i++)
		{
			bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = bcls_ret4.Tables[0].Rows[i]["HEAT_NO"].ToString();
			doFlag = f_mmsm_gyupd(&bcls_rec1, &bcls_ret1, conn);
			if (doFlag < 0)
			{
				//throw CApplicationException(-1, s.msg, log.Location);
			}
			tpcommit(0);
			tpbegin(0, 0);
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
