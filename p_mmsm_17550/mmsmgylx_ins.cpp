/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 工艺路线确定并发送电文
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsmgylx_ins)

int f_mmsmgylx_ins(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CDbCommand cmd_inq_code(conn);
	CModel tmmsmgy06("TMMSMGY06");
	CString dev_code = "";
	CString sm_plan_no2 = "";//炼钢计划号

	//加入函数的表

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	
	try
	{
		Log::Trace("", __FUNCTION__, "heat_no= [{0}]");
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();
		Log::Trace("", __FUNCTION__, "heat_no= [{0}]", heat_no);
		tmmsmgy06.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE")){
			if (tmmsmgy06["TC_SEND_FLAG"].ToString() == "1"){
				sprintf(s.msg, "该数据已下发");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else
			{
				CDecimal duration_time = tmmsmgy06["END_TIME"].ToDecimal() - tmmsmgy06["START_TIME"].ToDecimal();
				tmmsmgy06["DURATION_TIME"] = duration_time - tmmsmgy06["TOGETHER_TIME"].ToDecimal();
				tmmsmgy06.Update("DURATION_TIME","HEAT_NO,L2_PROC_NO,DEV_CODE");
			}
		}
		else
		{
			tmmsmgy06["TC_SEND_FLAG"] = "1";
			tmmsmgy06["REC_CREATOR"] = s.userid;
			tmmsmgy06["REC_CREATE_TIME"] = datetime;
			CDecimal duration_time = tmmsmgy06["END_TIME"].ToDecimal() - tmmsmgy06["START_TIME"].ToDecimal();
			tmmsmgy06["DURATION_TIME"] = duration_time;
			tmmsmgy06.Insert();
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


